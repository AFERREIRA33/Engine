#include "shader_impl_vulkan__reflect.h"

#ifndef NDEBUG
# pragma comment(lib, "SPIRV-Reflect-d.lib")
#else /* !NDEBUG */
# pragma comment(lib, "SPIRV-Reflect.lib")
#endif /* !NDEBUG */

vde::core::gpu::EShaderVariableType s_ToVde(SpvOp op, SpvReflectTypeFlags flags, const SpvReflectTypeDescription::Traits& traits)
{
	using namespace vde::core::gpu;

	if (op == SpvOpTypeArray || op == SpvOpTypeRuntimeArray)
	{
		     if (flags & SPV_REFLECT_TYPE_FLAG_VECTOR) op = SpvOpTypeVector;
		else if (flags & SPV_REFLECT_TYPE_FLAG_MATRIX) op = SpvOpTypeMatrix;
		else if (flags & SPV_REFLECT_TYPE_FLAG_BOOL)   op = SpvOpTypeBool;
		else if (flags & SPV_REFLECT_TYPE_FLAG_INT)    op = SpvOpTypeInt;
		else if (flags & SPV_REFLECT_TYPE_FLAG_FLOAT)  op = SpvOpTypeFloat;
		else if (flags & SPV_REFLECT_TYPE_FLAG_STRUCT) op = SpvOpTypeStruct;
	}

	switch (op)
	{
	case SpvOpTypeBool:   return EShaderVariableType::Bool;
	case SpvOpTypeInt:    return traits.numeric.scalar.signedness ? EShaderVariableType::Int : EShaderVariableType::UnsignedInt;
	case SpvOpTypeFloat:  return EShaderVariableType::Float;
	case SpvOpTypeVector: {
		EShaderVariableType base = EShaderVariableType(-1);

		if (flags & SPV_REFLECT_TYPE_FLAG_BOOL)  base = EShaderVariableType::Bool;
		if (flags & SPV_REFLECT_TYPE_FLAG_INT)   base = traits.numeric.scalar.signedness ? EShaderVariableType::Int : EShaderVariableType::UnsignedInt;
		if (flags & SPV_REFLECT_TYPE_FLAG_FLOAT) base = EShaderVariableType::Float;

		return EShaderVariableType(uint32_t(base) + traits.numeric.vector.component_count - 1);
	} break;
	case SpvOpTypeMatrix: {
		if (traits.numeric.matrix.column_count == traits.numeric.matrix.row_count)
			return EShaderVariableType(uint32_t(EShaderVariableType::Mat2) + traits.numeric.matrix.column_count - 2);

		if (traits.numeric.matrix.column_count == 2)
		{
			if (traits.numeric.matrix.row_count == 3) return EShaderVariableType::Mat2x3;
			if (traits.numeric.matrix.row_count == 4) return EShaderVariableType::Mat2x4;
		}
		if (traits.numeric.matrix.column_count == 3)
		{
			if (traits.numeric.matrix.row_count == 2) return EShaderVariableType::Mat3x2;
			if (traits.numeric.matrix.row_count == 4) return EShaderVariableType::Mat3x4;
		}
		if (traits.numeric.matrix.column_count == 4)
		{
			if (traits.numeric.matrix.row_count == 2) return EShaderVariableType::Mat4x2;
			if (traits.numeric.matrix.row_count == 4) return EShaderVariableType::Mat4x3;
		}
	} break;
	case SpvOpTypeStruct: return EShaderVariableType::Struct;
	}

	return EShaderVariableType(-1);
}

vde::core::gpu::ShaderDataMember s_processBlockMember(const SpvReflectBlockVariable& memberReflect)
{
	using namespace vde::core::gpu;

	ShaderDataMember member;
	member.name   = memberReflect.name;
	member.offset = memberReflect.offset;
	member.size   = memberReflect.size;
	member.type   = s_ToVde(memberReflect.type_description->op, memberReflect.type_description->type_flags, memberReflect.type_description->traits);

	if (memberReflect.array.dims_count > 0)
	{
		member.arraySize   = memberReflect.array.dims[0];
		member.arrayStride = memberReflect.array.stride;
	}

	if (memberReflect.type_description->op == SpvOpTypeRuntimeArray)
	{
		member.isRuntimeArray = true;
		member.arrayStride = memberReflect.type_description->traits.array.stride;
	}

	for (uint32_t i = 0; i < memberReflect.member_count; i++)
	{
		const auto& nestedMember = memberReflect.members[i];
		member.members.push_back(s_processBlockMember(nestedMember));
	}

	return member;
}

vde::core::gpu::ShaderDataType s_createRuntimeTypeFromSpirv(const SpvReflectBlockVariable& block)
{
	using namespace vde::core::gpu;

	ShaderDataType type;
	type.size = block.size;

	// Process each member in the block
	for (uint32_t i = 0; i < block.member_count; i++)
	{
		const auto& memberReflect = block.members[i];
		ShaderDataMember member = s_processBlockMember(memberReflect);
		type.members.push_back(member);
	}

	return type;
}

vde::core::gpu::ImageDataType s_createImageBindingDataFromSpirv(const SpvReflectImageTraits& image)
{
	using namespace vde::core::gpu;

	//Shader::DescriptorSetLayout::ImageBindingData result {};
	ImageDataType result {};
	result.type = ([] (SpvDim dim) {
		switch (dim)
		{
		case SpvDim1D:   return EImageType::Image1D;
		case SpvDim2D:   return EImageType::Image2D;
		case SpvDim3D:   return EImageType::Image3D;
		case SpvDimCube: return EImageType::ImageCube;
		}
		return EImageType::Undefined;
	})(image.dim);
	return result;
}

std::vector<vde::core::gpu::DescriptorSetLayout> vde::core::gpu::ReflectDescriptorSetLayouts(VkDevice device, SpvReflectShaderModule* module)
{
	using namespace vde::core::gpu;

	uint32_t numDescriptorSets = 0;
	spvReflectEnumerateDescriptorSets(module, &numDescriptorSets, nullptr);

	std::vector<SpvReflectDescriptorSet*> descriptorSets(numDescriptorSets);
	spvReflectEnumerateDescriptorSets(module, &numDescriptorSets, descriptorSets.data());

	std::vector<vde::core::gpu::DescriptorSetLayout> layouts(numDescriptorSets);

	for (uint32_t i = 0; i < numDescriptorSets; ++i)
	{
		const SpvReflectDescriptorSet& reflSet = *descriptorSets[i];
		layouts[i].setId = reflSet.set;

		std::vector<VkDescriptorSetLayoutBinding> vkBindings(reflSet.binding_count);
		std::vector<DescriptorSetLayout::Binding> bindings(reflSet.binding_count);

		for (uint32_t j = 0; j < reflSet.binding_count; ++j)
		{
			const SpvReflectDescriptorBinding& reflBinding = *reflSet.bindings[j];
			bindings[j].bindingId = reflBinding.binding;
			bindings[j].name      = reflBinding.name;

			VkDescriptorSetLayoutBinding& vkBindingLayout = vkBindings[j];
			vkBindingLayout.binding         = reflBinding.binding;
			vkBindingLayout.descriptorType  = static_cast<VkDescriptorType>(reflBinding.descriptor_type);
			vkBindingLayout.descriptorCount = 1;

			switch (vkBindingLayout.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				bindings[j].data = s_createRuntimeTypeFromSpirv(reflBinding.block);
				break;

			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				bindings[j].data = s_createImageBindingDataFromSpirv(reflBinding.image);
				break;
			}

			for (uint32_t dim = 0; dim < reflBinding.array.dims_count; dim++)
				vkBindingLayout.descriptorCount *= reflBinding.array.dims[dim];

			vkBindingLayout.stageFlags = static_cast<VkShaderStageFlagBits>(module->shader_stage);
		}

		layouts[i].bindings = bindings;

		VkDescriptorSetLayoutCreateInfo layoutInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = static_cast<uint32_t>(vkBindings.size());
		layoutInfo.pBindings    = vkBindings.data();

		vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layouts[i].GetImpl().layout);
	}

	return layouts;
}

std::vector<VkVertexInputAttributeDescription> vde::core::gpu::ReflectVertexInput(SpvReflectShaderModule* module)
{
	std::vector<VkVertexInputAttributeDescription> result;

	uint32_t inputVarCount = 0;
	spvReflectEnumerateInputVariables(module, &inputVarCount, nullptr);

	std::vector<SpvReflectInterfaceVariable*> inputVars(inputVarCount);
	spvReflectEnumerateInputVariables(module, &inputVarCount, inputVars.data());

	for (const auto& inputVar : inputVars)
	{
		if (inputVar->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
			continue;

		result.push_back({
			.location = inputVar->location,
			.binding  = inputVar->location,
			.format   = VkFormat(inputVar->format),
			.offset   = 0
		});
	}

	return result;
}

vde::core::gpu::Shader::PushConstantsLayout vde::core::gpu::ReflectPushConstants(SpvReflectShaderModule* module)
{
	vde::core::gpu::Shader::PushConstantsLayout result;

	uint32_t pushConstantBlockCount = 0;
	spvReflectEnumeratePushConstantBlocks(module, &pushConstantBlockCount, nullptr);

	if (pushConstantBlockCount == 0)
		return result;

	result.isValid = true;

	std::vector<SpvReflectBlockVariable*> pushConstantBlocks(pushConstantBlockCount);
	spvReflectEnumeratePushConstantBlocks(module, &pushConstantBlockCount, pushConstantBlocks.data());

	result.range.stageFlags = static_cast<VkShaderStageFlagBits>(module->shader_stage);
	result.range.offset     = pushConstantBlocks[0]->offset;
	result.range.size       = pushConstantBlocks[0]->size;
	result.type             = s_createRuntimeTypeFromSpirv(*pushConstantBlocks[0]);

	return result;
}
