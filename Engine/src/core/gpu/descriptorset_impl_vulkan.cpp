#include "descriptorset_impl_vulkan.h"
#include "descriptorpool_impl_vulkan.h"
#include "shader_impl_vulkan.h"

#include "../graphicscontext_impl_vulkan.h"
#include "buffer_impl_vulkan.h"
#include "image_impl_vulkan.h"

using namespace vde::core::gpu;

DescriptorSetLayout::DescriptorSetLayout()
	: m_pImpl(new Impl)
{
}

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayout& other)
	: m_pImpl(new Impl)
{
	m_pImpl->layout = other.m_pImpl->layout;
	setId = other.setId;
	bindings = other.bindings;
}

DescriptorSetLayout& DescriptorSetLayout::operator=(const DescriptorSetLayout& other)
{
	m_pImpl->layout = other.m_pImpl->layout;
	setId = other.setId;
	bindings = other.bindings;
	return *this;
}

DescriptorSetLayout::Impl& DescriptorSetLayout::GetImpl() const
{
	return *m_pImpl;
}

template<>
const ShaderDataType& DescriptorSetLayout::GetBinding(size_t binding)
{
	std::vector<DescriptorSetLayout::Binding>::iterator bindingIt = std::find_if(bindings.begin(), bindings.end(), [binding](Binding& b) { return b.bindingId == binding; });
	if (bindingIt == bindings.end())
		throw std::runtime_error("Invalid descriptor set binding index");

	if (!std::holds_alternative<ShaderDataType>(bindingIt->data))
		throw std::runtime_error("Invalid descriptor set binding type");

	return std::get<ShaderDataType>(bindingIt->data);
}

const std::string& DescriptorSetLayout::GetBindingName(size_t binding)
{
	static std::string s_emptyName = "";
	if (binding < bindings.size())
		return bindings[binding].name;

	return s_emptyName;
}

size_t DescriptorSetLayout::GetBindingCount() const
{
	return bindings.size();
}

// ----------------------------------------------------------------------------

DescriptorSet::DescriptorSet(util::Badge<DescriptorPool> badge, const DescriptorSetLayout& layout)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
{
	VkDescriptorSetAllocateInfo allocInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorSetCount = 1;
	allocInfo.descriptorPool     = m_owner->GetImpl().pool;
	allocInfo.pSetLayouts        = &(layout.GetImpl().layout);

	vkAllocateDescriptorSets(m_owner->Owner().GetImpl().device, &allocInfo, &m_pImpl->descriptorSet);
}

DescriptorSet::~DescriptorSet()
{
	vkFreeDescriptorSets(m_owner->Owner().GetImpl().device, m_owner->GetImpl().pool, 1, &m_pImpl->descriptorSet);
}

DescriptorSet::Impl& DescriptorSet::GetImpl()
{
	return *m_pImpl;
}

template<>
void DescriptorSet::Bind<Buffer>(uint32_t binding, Buffer& target)
{
	VkDescriptorBufferInfo buffer_info {};
	buffer_info.buffer = target.GetImpl().buffer;
	buffer_info.range  = target.Size();

	VkWriteDescriptorSet descriptor_write { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptor_write.dstSet          = m_pImpl->descriptorSet;
	descriptor_write.dstBinding      = binding;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorType  = uint32_t(target.GetImpl().usage & EBufferUsageBits::UniformBuffer) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptor_write.descriptorCount = 1;
	descriptor_write.pBufferInfo     = &buffer_info;
	vkUpdateDescriptorSets(m_owner->Owner().GetImpl().device, 1, &descriptor_write, 0, nullptr);
}

template<>
void DescriptorSet::Bind<Image>(uint32_t binding, Image& target)
{
	bool isStorage = bool(target.GetImpl().usage & EImageUsageBits::Storage);

	VkDescriptorImageInfo image_info {};
	image_info.sampler     = target.GetImpl().imageSampler;
	image_info.imageView   = target.GetImpl().imageView;
	image_info.imageLayout = isStorage ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet descriptor_write { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptor_write.dstSet          = m_pImpl->descriptorSet;
	descriptor_write.dstBinding      = binding;
	descriptor_write.dstArrayElement = 0;
	descriptor_write.descriptorType  = isStorage ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptor_write.descriptorCount = 1;
	descriptor_write.pImageInfo      = &image_info;
	vkUpdateDescriptorSets(m_owner->Owner().GetImpl().device, 1, &descriptor_write, 0, nullptr);
}
