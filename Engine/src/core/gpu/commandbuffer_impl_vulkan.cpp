#include "commandbuffer_impl_vulkan.h"
#include "commandpool_impl_vulkan.h"

#include "buffer_impl_vulkan.h"
#include "image_impl_vulkan.h"
#include "pipeline_impl_vulkan.h"
#include "shader_impl_vulkan.h"
#include "../window_impl_glfw_vulkan.h"
#include "../graphicscontext_impl_vulkan.h"
#include "../gpu_detail/vk_initializers.h"
#include "../gpu_detail/vk_images.h"
#include "../gpu_detail/vk_converters.h"

#include <ranges>

using namespace vde::core::gpu;

CommandBuffer::CommandBuffer(util::Badge<core::gpu::CommandPool> badge, bool isSecondary /* = false */)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
{
	auto vkDevice = m_owner->Owner().GetImpl().device;

	auto allocateInfo = gpu_detail::command_buffer_allocate_info(m_owner->GetImpl().pool, isSecondary, 1);
	vkAllocateCommandBuffers(vkDevice, &allocateInfo, &m_pImpl->cmd);

	auto semaphore_info = gpu_detail::semaphore_create_info();
	vkCreateSemaphore(vkDevice, &semaphore_info, nullptr, &m_pImpl->semaphore);

	auto fence_info = gpu_detail::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	vkCreateFence(vkDevice, &fence_info, nullptr, &m_pImpl->fence);

	m_pImpl->isSecondary = isSecondary;
}

CommandBuffer::~CommandBuffer() noexcept
{
	auto vkDevice = m_owner->Owner().GetImpl().device;
	vkFreeCommandBuffers(vkDevice, m_owner->GetImpl().pool, 1, &m_pImpl->cmd);
	vkDestroySemaphore(vkDevice, m_pImpl->semaphore, nullptr);
	vkDestroyFence(vkDevice, m_pImpl->fence, nullptr);
}

CommandBuffer::Impl& CommandBuffer::GetImpl()
{
	return *m_pImpl;
}

CommandPool& CommandBuffer::Owner()
{
	return *m_owner;
}

bool CommandBuffer::IsSignaled() const
{
	auto status = vkGetFenceStatus(m_owner->Owner().GetImpl().device, m_pImpl->fence);
	return (status == VK_SUCCESS);
}

void CommandBuffer::WaitForSignal() const
{
	vkWaitForFences(m_owner->Owner().GetImpl().device, 1, &m_pImpl->fence, true, uint64_t(-1));
}

void CommandBuffer::Reset()
{
	vkResetCommandBuffer(m_pImpl->cmd, 0);
}

std::unique_ptr<CommandEncoder> CommandBuffer::Record(const std::string& name, ECommandBufferRecordType recordType /* = ECommandBufferRecordType::Normal */, const std::vector<ImageFormat>& colorFormats /* = {} */, const ImageFormat& depthFormat /* = {} */)
{
	return std::make_unique<CommandEncoder>(vME, name, recordType, colorFormats, depthFormat);
}

// --------------------------------------------------------------------------------------------------------------------

CommandEncoder::CommandEncoder(util::Badge<CommandBuffer> badge, const std::string& name, ECommandBufferRecordType recordType /* = ECommandBufferRecordType::Normal */, const std::vector<ImageFormat>& colorFormats /* = {} */, const ImageFormat& depthFormat /* = {} */)
	: m_owner(badge.Owner())
{
	VkCommandBufferUsageFlags flags = recordType == ECommandBufferRecordType::OneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	if (!colorFormats.empty())
		flags = flags | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

	auto beginInfo = gpu_detail::command_buffer_begin_info(flags);

	if (!colorFormats.empty())
	{
		auto vkColorFormats = colorFormats | std::ranges::views::transform([](const ImageFormat& f) { return s_ToVk(f); }) | std::ranges::to<std::vector>();

		VkCommandBufferInheritanceRenderingInfo inheritanceRenderingInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO };
		inheritanceRenderingInfo.colorAttachmentCount    = uint32_t(vkColorFormats.size());
		inheritanceRenderingInfo.pColorAttachmentFormats = vkColorFormats.data();
		inheritanceRenderingInfo.depthAttachmentFormat   = s_ToVk(depthFormat);
		inheritanceRenderingInfo.rasterizationSamples    = VK_SAMPLE_COUNT_1_BIT;

		VkCommandBufferInheritanceInfo inheritanceInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
		inheritanceInfo.pNext = &inheritanceRenderingInfo;
		beginInfo.pInheritanceInfo = &inheritanceInfo;

		vkBeginCommandBuffer(m_owner->GetImpl().cmd, &beginInfo);
	}
	else
		vkBeginCommandBuffer(m_owner->GetImpl().cmd, &beginInfo);

	if (ExtensionFunctions::vkCmdBeginDebugUtilsLabelEXT)
	{
		VkDebugUtilsLabelEXT debugLabel{ .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
		debugLabel.pLabelName = name.c_str();
		ExtensionFunctions::vkCmdBeginDebugUtilsLabelEXT(m_owner->GetImpl().cmd, &debugLabel);
	}
}

CommandEncoder::~CommandEncoder()
{
	if (ExtensionFunctions::vkCmdEndDebugUtilsLabelEXT)
		ExtensionFunctions::vkCmdEndDebugUtilsLabelEXT(m_owner->GetImpl().cmd);

	vkEndCommandBuffer(m_owner->GetImpl().cmd);
}

void CommandEncoder::TransitionImage(Image& image, EImageLayout newLayout)
{
	auto& imageImpl = image.GetImpl();

	if (image.Format().first == EImageFormatComponents::Depth || image.Format().first == EImageFormatComponents::DepthStencil)
		gpu_detail::transition_depth(m_owner->GetImpl().cmd, imageImpl.image, imageImpl.currentLayout, s_ToVk(newLayout));
	else
		gpu_detail::transition(m_owner->GetImpl().cmd, imageImpl.image, imageImpl.currentLayout, s_ToVk(newLayout));

	imageImpl.currentLayout = s_ToVk(newLayout);
}

void CommandEncoder::CopyImage(Image& src, Image& dst, const glm::uvec3& srcSize, const glm::uvec3& dstSize)
{
	// FIXME: This command is implemented to only copy from (0,0,0) to size, with no mips and no arrays.

	TransitionImage(src, EImageLayout::TransferSrc);
	TransitionImage(dst, EImageLayout::TransferDst);

	VkImageBlit2 blitRegion { .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2 };
	blitRegion.srcOffsets[1] = { int32_t(srcSize.x), int32_t(srcSize.y), int32_t(srcSize.z) };
	blitRegion.dstOffsets[1] = { int32_t(dstSize.x), int32_t(dstSize.y), int32_t(dstSize.z) };

	blitRegion.srcSubresource = {
		.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
		.mipLevel       = 0,
		.baseArrayLayer = 0,
		.layerCount     = 1
	};
	blitRegion.dstSubresource = blitRegion.srcSubresource;

	VkBlitImageInfo2 blitInfo { .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2 };
	blitInfo.srcImage       = src.GetImpl().image;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.dstImage       = dst.GetImpl().image;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.filter         = VK_FILTER_LINEAR;
	blitInfo.regionCount    = 1;
	blitInfo.pRegions       = &blitRegion;
	vkCmdBlitImage2(m_owner->GetImpl().cmd, &blitInfo);
}

void CommandEncoder::ClearImageColor(Image& image, const glm::vec4& color)
{
	gpu_detail::transition(m_owner->GetImpl().cmd, image.GetImpl().image, image.GetImpl().currentLayout, VK_IMAGE_LAYOUT_GENERAL);
	image.GetImpl().currentLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkClearColorValue clearValue{ { color.r, color.g, color.b, color.a } };
	auto clearRange = gpu_detail::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
	vkCmdClearColorImage(m_owner->GetImpl().cmd, image.GetImpl().image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	gpu_detail::transition(m_owner->GetImpl().cmd, image.GetImpl().image, image.GetImpl().currentLayout, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	image.GetImpl().currentLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

void CommandEncoder::ClearImageDepthStencil(Image& image, float depth, uint32_t stencil /* = 0x0 */)
{
	gpu_detail::transition_depth(m_owner->GetImpl().cmd, image.GetImpl().image, image.GetImpl().currentLayout, VK_IMAGE_LAYOUT_GENERAL);
	image.GetImpl().currentLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkClearDepthStencilValue clearValue { depth, stencil };
	auto clearRange = gpu_detail::image_subresource_range(VK_IMAGE_ASPECT_DEPTH_BIT);
	vkCmdClearDepthStencilImage(m_owner->GetImpl().cmd, image.GetImpl().image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	gpu_detail::transition_depth(m_owner->GetImpl().cmd, image.GetImpl().image, image.GetImpl().currentLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	image.GetImpl().currentLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

std::unique_ptr<RenderingCommandEncoder> CommandEncoder::BeginRendering(const std::vector<Image*>& colorAttachments, Image* depthAttachment)
{
	return std::make_unique<RenderingCommandEncoder>(vME, m_owner, colorAttachments, depthAttachment);
}

std::unique_ptr<RenderingCommandEncoder> CommandEncoder::BeginRendering(Pipeline& pipeline, const std::vector<Image*>& colorAttachments, Image* depthAttachment)
{
	return std::make_unique<RenderingCommandEncoder>(vME, m_owner, pipeline, colorAttachments, depthAttachment);
}

std::unique_ptr<ComputeCommandEncoder> CommandEncoder::BeginCompute()
{
	return std::make_unique<ComputeCommandEncoder>(vME, m_owner);
}

std::unique_ptr<ComputeCommandEncoder> CommandEncoder::BeginCompute(Pipeline& pipeline)
{
	return std::make_unique<ComputeCommandEncoder>(vME, m_owner, pipeline);
}

void CommandEncoder::ExecuteCommands(CommandBuffer& secondary)
{
	vkCmdExecuteCommands(m_owner->GetImpl().cmd, 1, &secondary.GetImpl().cmd);
}

// --------------------------------------------------------------------------------------------------------------------

m_PipelineBasedCommandEncoder::m_PipelineBasedCommandEncoder(CommandBuffer* owner)
	: m_owner(owner)
	, m_pipeline(nullptr)
{
}

m_PipelineBasedCommandEncoder::m_PipelineBasedCommandEncoder(CommandBuffer* owner, Pipeline& pipeline)
	: m_owner(owner)
	, m_pipeline(&pipeline)
{
	VkPipelineBindPoint bindPoint = (pipeline.Family() == EPipelineFamily::Graphics)
		? VK_PIPELINE_BIND_POINT_GRAPHICS
		: VK_PIPELINE_BIND_POINT_COMPUTE;

	vkCmdBindPipeline(m_owner->GetImpl().cmd, bindPoint, m_pipeline->GetImpl().pipeline);
}

m_PipelineBasedCommandEncoder::m_PipelineBasedCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner)
	: m_PipelineBasedCommandEncoder(owner)
{
}

m_PipelineBasedCommandEncoder::m_PipelineBasedCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, Pipeline& pipeline)
	: m_PipelineBasedCommandEncoder(owner, pipeline)
{
}

m_PipelineBasedCommandEncoder::~m_PipelineBasedCommandEncoder() noexcept
{
}

void m_PipelineBasedCommandEncoder::UpdatePushConstants(EShaderStage stage, const ShaderDataStore& pushConstants)
{
	const auto& range = m_pipeline->GetImpl().pushConstantLayouts[stage].range;
	vkCmdPushConstants(m_owner->GetImpl().cmd, m_pipeline->GetImpl().layout, range.stageFlags, range.offset, range.size, pushConstants.Data());
}

void m_PipelineBasedCommandEncoder::BindDescriptorSets(size_t firstSet, const std::vector<DescriptorSet*>& sets)
{
	VkPipelineBindPoint bindPoint = (m_pipeline->Family() == EPipelineFamily::Graphics)
		? VK_PIPELINE_BIND_POINT_GRAPHICS
		: VK_PIPELINE_BIND_POINT_COMPUTE;

	auto setHandles = sets | std::ranges::views::transform([](DescriptorSet* ds) { return ds->GetImpl().descriptorSet; }) | std::ranges::to<std::vector>();
	vkCmdBindDescriptorSets(m_owner->GetImpl().cmd, bindPoint, m_pipeline->GetImpl().layout, uint32_t(firstSet), uint32_t(setHandles.size()), setHandles.data(), 0, nullptr);
}

// --------------------------------------------------------------------------------------------------------------------

RenderingCommandEncoder::RenderingCommandEncoder(CommandBuffer* owner, Pipeline& pipeline)
	: m_PipelineBasedCommandEncoder(owner, pipeline)
	, m_shouldEndRendering(false)
{
}

RenderingCommandEncoder::RenderingCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, const std::vector<Image*>& colorAttachments, Image* depthAttachment)
	: m_PipelineBasedCommandEncoder(badge, owner)
{
	for (auto& a : colorAttachments)
		badge.Owner()->TransitionImage(*a, EImageLayout::ColorAttachment);

	uint32_t layerCount = 1;

	if (depthAttachment)
	{
		badge.Owner()->TransitionImage(*depthAttachment, EImageLayout::DepthStencilAttachment);
		layerCount = std::max(layerCount, depthAttachment->GetImpl().layerCount);
	}

	std::vector<VkRenderingAttachmentInfo> colorInfos;
	for (auto& a : colorAttachments)
	{
		colorInfos.push_back(gpu_detail::attachment_info(
			a->GetImpl().imageView,
			nullptr,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		));

		layerCount = std::max(layerCount, a->GetImpl().layerCount);
	}

	VkRenderingAttachmentInfo depthInfo = gpu_detail::attachment_info(
		depthAttachment ? depthAttachment->GetImpl().imageView : VK_NULL_HANDLE,
		nullptr,
		VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
	);

	VkRenderingInfo renderInfo = gpu_detail::rendering_info({ colorAttachments[0]->GetImpl().extent.width, colorAttachments[0]->GetImpl().extent.height}, colorInfos, depthAttachment ? &depthInfo : nullptr);
	renderInfo.layerCount = layerCount;
	vkCmdBeginRendering(m_owner->GetImpl().cmd, &renderInfo);
}

RenderingCommandEncoder::RenderingCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, Pipeline& pipeline, const std::vector<Image*>& colorAttachments, Image* depthAttachment)
	: m_PipelineBasedCommandEncoder(badge, owner, pipeline)
{
	for (auto& a : colorAttachments)
		badge.Owner()->TransitionImage(*a, EImageLayout::ColorAttachment);

	uint32_t layerCount = 1;

	if (depthAttachment)
	{
		badge.Owner()->TransitionImage(*depthAttachment, EImageLayout::DepthStencilAttachment);
		layerCount = std::max(layerCount, depthAttachment->GetImpl().layerCount);
	}

	std::vector<VkRenderingAttachmentInfo> colorInfos;
	for (auto& a : colorAttachments)
	{
		colorInfos.push_back(gpu_detail::attachment_info(
			a->GetImpl().imageView,
			nullptr,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		));

		layerCount = std::max(layerCount, a->GetImpl().layerCount);
	}

	VkRenderingAttachmentInfo depthInfo = gpu_detail::attachment_info(
		depthAttachment ? depthAttachment->GetImpl().imageView : VK_NULL_HANDLE,
		nullptr,
		VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
	);

	uint32_t renderWidth  = 0;
	uint32_t renderHeight = 0;

	if (!colorAttachments.empty())
	{
		renderWidth  = colorAttachments[0]->GetImpl().extent.width;
		renderHeight = colorAttachments[0]->GetImpl().extent.height;
	}
	else
	{
		if (depthAttachment)
		{
			renderWidth  = depthAttachment->GetImpl().extent.width;
			renderHeight = depthAttachment->GetImpl().extent.height;
		}
	}

	VkRenderingInfo renderInfo = gpu_detail::rendering_info({ renderWidth, renderHeight }, colorInfos, depthAttachment ? &depthInfo : nullptr);
	renderInfo.layerCount = layerCount;
	vkCmdBeginRendering(m_owner->GetImpl().cmd, &renderInfo);
}

RenderingCommandEncoder::~RenderingCommandEncoder() noexcept
{
	if (m_shouldEndRendering)
		vkCmdEndRendering(m_owner->GetImpl().cmd);
}

void RenderingCommandEncoder::SetViewport(const glm::vec2& origin, const glm::vec2& size, const glm::vec2& depthBounds /* = { 0.0f, 1.0f } */, bool setScissorToo /* = true */)
{
	VkViewport vp;
	vp.x        = origin.x;
	vp.y        = origin.y + size.y;
	vp.width    = size.x;
	vp.height   = -size.y;
	vp.minDepth = depthBounds.x;
	vp.maxDepth = depthBounds.y;
	vkCmdSetViewport(m_owner->GetImpl().cmd, 0, 1, &vp);

	if (setScissorToo)
	{
		VkRect2D scissor;
		scissor.offset.x      = int32_t(origin.x);
		scissor.offset.y      = int32_t(origin.y);
		scissor.extent.width  = uint32_t(size.x);
		scissor.extent.height = uint32_t(size.y);
		vkCmdSetScissor(m_owner->GetImpl().cmd, 0, 1, &scissor);
	}
}

void RenderingCommandEncoder::BindPipeline(Pipeline& pipeline)
{
	m_pipeline = &pipeline;
	vkCmdBindPipeline(m_owner->GetImpl().cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetImpl().pipeline);
}

void RenderingCommandEncoder::Draw(const std::vector<Buffer*>& vertexBuffers, uint32_t numVertices, uint32_t numInstances /*= 1*/, uint32_t firstVertex /*= 0*/, uint32_t firstInstance /*= 0*/)
{
	auto cb = m_owner->GetImpl().cmd;

	std::vector<VkBuffer>     buffers;
	std::vector<VkDeviceSize> offsets;
	for (auto b : vertexBuffers)
	{
		if (b)
			buffers.push_back(b->GetImpl().buffer);
		else
			buffers.push_back(VK_NULL_HANDLE);

		offsets.push_back(0);
	}

	vkCmdBindVertexBuffers(cb, 0, uint32_t(buffers.size()), buffers.data(), offsets.data());
	vkCmdDraw(cb, numVertices, numInstances, firstVertex, firstInstance);
}

void RenderingCommandEncoder::DrawIndexed(const std::vector<Buffer*>& vertexBuffers, Buffer* indexBuffer, uint32_t numIndices, uint32_t numInstances /*= 1*/, uint32_t firstIndex /*= 0*/, int32_t vertexOffset /*= 0*/, uint32_t firstInstance /*= 0*/)
{
	auto cb = m_owner->GetImpl().cmd;

	std::vector<VkBuffer>     buffers;
	std::vector<VkDeviceSize> offsets;
	for (auto b : vertexBuffers)
	{
		if (b)
			buffers.push_back(b->GetImpl().buffer);
		else
			buffers.push_back(VK_NULL_HANDLE);

		offsets.push_back(0);
	}

	vkCmdBindVertexBuffers(cb, 0, uint32_t(buffers.size()), buffers.data(), offsets.data());
	vkCmdBindIndexBuffer(cb, indexBuffer->GetImpl().buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cb, numIndices, numInstances, firstIndex, vertexOffset, firstInstance);
}

void RenderingCommandEncoder::DrawIndexedIndirect(const std::vector<Buffer*>& vertexBuffers, Buffer* indexBuffer, Buffer* indirectBuffer, size_t drawCount, size_t offset, size_t stride /* = sizeof(IndirectDrawCommand) */)
{
	auto cb = m_owner->GetImpl().cmd;
	std::vector<VkBuffer>     buffers;
	std::vector<VkDeviceSize> offsets;
	for (auto b : vertexBuffers)
	{
		if (b)
			buffers.push_back(b->GetImpl().buffer);
		else
			buffers.push_back(VK_NULL_HANDLE);

		offsets.push_back(0);
	}

	vkCmdBindVertexBuffers(cb, 0, uint32_t(buffers.size()), buffers.data(), offsets.data());
	vkCmdBindIndexBuffer(cb, indexBuffer->GetImpl().buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexedIndirect(cb, indirectBuffer->GetImpl().buffer, VkDeviceSize(offset), uint32_t(drawCount), uint32_t(stride));
}

void RenderingCommandEncoder::DrawIndexedSubmeshes(const std::vector<Buffer*>& vertexBuffers, Buffer* indexBuffer, const std::vector<std::pair<uint32_t, uint32_t>>& submeshes, uint32_t numInstances /* = 1 */)
{
	auto cb = m_owner->GetImpl().cmd;

	std::vector<VkBuffer>     buffers;
	std::vector<VkDeviceSize> offsets;
	for (auto b : vertexBuffers)
	{
		if (b)
			buffers.push_back(b->GetImpl().buffer);
		else
			buffers.push_back(VK_NULL_HANDLE);

		offsets.push_back(0);
	}

	vkCmdBindVertexBuffers(cb, 0, uint32_t(buffers.size()), buffers.data(), offsets.data());
	vkCmdBindIndexBuffer(cb, indexBuffer->GetImpl().buffer, 0, VK_INDEX_TYPE_UINT32);
	
	for (auto& [firstIndex, indexCount] : submeshes)
		vkCmdDrawIndexed(cb, indexCount, numInstances, firstIndex, 0, 0);
}

// --------------------------------------------------------------------------------------------------------------------

ComputeCommandEncoder::ComputeCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner)
	: m_PipelineBasedCommandEncoder(badge, owner)
{
}

ComputeCommandEncoder::ComputeCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, Pipeline& pipeline)
	: m_PipelineBasedCommandEncoder(badge, owner, pipeline)
{
}

ComputeCommandEncoder::~ComputeCommandEncoder() noexcept
{
}

void ComputeCommandEncoder::Dispatch(const glm::uvec3& groupCount)
{
	vkCmdDispatch(m_owner->GetImpl().cmd, groupCount.x, groupCount.y, groupCount.z);
}
