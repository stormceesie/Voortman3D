// Code was derrived from Sasha Williams.

// constexprinitializers (standards)
#pragma once
#include "pch.h"

/// <summary>
/// constexpr function will be executed (if possible) at compile time this can increase the speed of de code by many times!
/// These functions should not be discarded compiler should give a warning in this case
/// </summary>
namespace Voortman3D {
	namespace Initializers {
		_NODISCARD constexpr VkMemoryAllocateInfo memoryAllocateInfo() noexcept
		{
			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return memAllocInfo;
		}

		_NODISCARD constexpr VkMappedMemoryRange mappedMemoryRange() noexcept
		{
			VkMappedMemoryRange mappedMemoryRange{};
			mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			return mappedMemoryRange;
		}

		_NODISCARD constexpr VkCommandBufferAllocateInfo commandBufferAllocateInfo(
			VkCommandPool commandPool,
			VkCommandBufferLevel level,
			uint32_t bufferCount) noexcept
		{
			VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = commandPool;
			commandBufferAllocateInfo.level = level;
			commandBufferAllocateInfo.commandBufferCount = bufferCount;
			return commandBufferAllocateInfo;
		}

		_NODISCARD constexpr VkCommandPoolCreateInfo commandPoolCreateInfo() noexcept
		{
			VkCommandPoolCreateInfo cmdPoolCreateInfo{};
			cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return cmdPoolCreateInfo;
		}

		_NODISCARD constexpr VkCommandBufferBeginInfo commandBufferBeginInfo() noexcept
		{
			VkCommandBufferBeginInfo cmdBufferBeginInfo{};
			cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return cmdBufferBeginInfo;
		}

		_NODISCARD constexpr VkCommandBufferInheritanceInfo commandBufferInheritanceInfo() noexcept
		{
			VkCommandBufferInheritanceInfo cmdBufferInheritanceInfo{};
			cmdBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			return cmdBufferInheritanceInfo;
		}

		_NODISCARD constexpr VkRenderPassBeginInfo renderPassBeginInfo() noexcept
		{
			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			return renderPassBeginInfo;
		}

		_NODISCARD constexpr VkRenderPassCreateInfo renderPassCreateInfo() noexcept
		{
			VkRenderPassCreateInfo renderPassCreateInfo{};
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return renderPassCreateInfo;
		}

		/** @brief Initialize an image memory barrier with no image transfer ownership */
		_NODISCARD constexpr VkImageMemoryBarrier imageMemoryBarrier() noexcept
		{
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			return imageMemoryBarrier;
		}

		/** @brief Initialize a buffer memory barrier with no image transfer ownership */
		_NODISCARD constexpr VkBufferMemoryBarrier bufferMemoryBarrier() noexcept
		{
			VkBufferMemoryBarrier bufferMemoryBarrier{};
			bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			return bufferMemoryBarrier;
		}

		_NODISCARD constexpr VkMemoryBarrier memoryBarrier() noexcept
		{
			VkMemoryBarrier memoryBarrier{};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			return memoryBarrier;
		}

		_NODISCARD constexpr VkImageCreateInfo imageCreateInfo() noexcept
		{
			VkImageCreateInfo imageCreateInfo{};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			return imageCreateInfo;
		}

		_NODISCARD constexpr VkSamplerCreateInfo samplerCreateInfo() noexcept
		{
			VkSamplerCreateInfo samplerCreateInfo{};
			samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerCreateInfo.maxAnisotropy = 1.0f;
			return samplerCreateInfo;
		}

		_NODISCARD constexpr VkImageViewCreateInfo imageViewCreateInfo() noexcept
		{
			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			return imageViewCreateInfo;
		}

		_NODISCARD constexpr VkFramebufferCreateInfo framebufferCreateInfo() noexcept
		{
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			return framebufferCreateInfo;
		}

		_NODISCARD constexpr VkSemaphoreCreateInfo semaphoreCreateInfo() noexcept
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return semaphoreCreateInfo;
		}

		_NODISCARD constexpr VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0) noexcept
		{
			VkFenceCreateInfo fenceCreateInfo{};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = flags;
			return fenceCreateInfo;
		}

		_NODISCARD constexpr VkEventCreateInfo eventCreateInfo() noexcept
		{
			VkEventCreateInfo eventCreateInfo{};
			eventCreateInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
			return eventCreateInfo;
		}

		_NODISCARD constexpr VkSubmitInfo submitInfo() noexcept
		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submitInfo;
		}

		_NODISCARD constexpr VkViewport viewport(
			float width,
			float height,
			float minDepth,
			float maxDepth) noexcept
		{
			VkViewport viewport{};
			viewport.width = width;
			viewport.height = height;
			viewport.minDepth = minDepth;
			viewport.maxDepth = maxDepth;
			return viewport;
		}

		_NODISCARD constexpr VkRect2D rect2D(
			int32_t width,
			int32_t height,
			int32_t offsetX,
			int32_t offsetY) noexcept
		{
			VkRect2D rect2D{};
			rect2D.extent.width = width;
			rect2D.extent.height = height;
			rect2D.offset.x = offsetX;
			rect2D.offset.y = offsetY;
			return rect2D;
		}

		_NODISCARD constexpr VkBufferCreateInfo bufferCreateInfo() noexcept
		{
			VkBufferCreateInfo bufCreateInfo{};
			bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			return bufCreateInfo;
		}

		_NODISCARD constexpr VkBufferCreateInfo bufferCreateInfo(
			VkBufferUsageFlags usage,
			VkDeviceSize size) noexcept
		{
			VkBufferCreateInfo bufCreateInfo{};
			bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufCreateInfo.usage = usage;
			bufCreateInfo.size = size;
			return bufCreateInfo;
		}

		_NODISCARD constexpr VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(
			uint32_t poolSizeCount,
			VkDescriptorPoolSize* pPoolSizes,
			uint32_t maxSets) noexcept
		{
			VkDescriptorPoolCreateInfo descriptorPoolInfo{};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = poolSizeCount;
			descriptorPoolInfo.pPoolSizes = pPoolSizes;
			descriptorPoolInfo.maxSets = maxSets;
			return descriptorPoolInfo;
		}

		_NODISCARD constexpr VkDescriptorPoolCreateInfo descriptorPoolCreateInfo(
			const std::vector<VkDescriptorPoolSize>& poolSizes,
			uint32_t maxSets) noexcept
		{
			VkDescriptorPoolCreateInfo descriptorPoolInfo{};
			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			descriptorPoolInfo.pPoolSizes = poolSizes.data();
			descriptorPoolInfo.maxSets = maxSets;
			return descriptorPoolInfo;
		}

		_NODISCARD constexpr VkDescriptorPoolSize descriptorPoolSize(
			VkDescriptorType type,
			uint32_t descriptorCount) noexcept
		{
			VkDescriptorPoolSize descriptorPoolSize{};
			descriptorPoolSize.type = type;
			descriptorPoolSize.descriptorCount = descriptorCount;
			return descriptorPoolSize;
		}

		_NODISCARD constexpr VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(
			VkDescriptorType type,
			VkShaderStageFlags stageFlags,
			uint32_t binding,
			uint32_t descriptorCount = 1) noexcept
		{
			VkDescriptorSetLayoutBinding setLayoutBinding{};
			setLayoutBinding.descriptorType = type;
			setLayoutBinding.stageFlags = stageFlags;
			setLayoutBinding.binding = binding;
			setLayoutBinding.descriptorCount = descriptorCount;
			return setLayoutBinding;
		}

		_NODISCARD constexpr VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
			const VkDescriptorSetLayoutBinding* pBindings,
			uint32_t bindingCount) noexcept
		{
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = pBindings;
			descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
			return descriptorSetLayoutCreateInfo;
		}

		_NODISCARD constexpr VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
			const std::vector<VkDescriptorSetLayoutBinding>& bindings) noexcept
		{
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = bindings.data();
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			return descriptorSetLayoutCreateInfo;
		}

		_NODISCARD constexpr VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
			const VkDescriptorSetLayout* pSetLayouts,
			uint32_t setLayoutCount = 1) noexcept
		{
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
			pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
			return pipelineLayoutCreateInfo;
		}

		_NODISCARD constexpr VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
			uint32_t setLayoutCount = 1) noexcept
		{
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
			return pipelineLayoutCreateInfo;
		}

		_NODISCARD constexpr VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(
			VkDescriptorPool descriptorPool,
			const VkDescriptorSetLayout* pSetLayouts,
			uint32_t descriptorSetCount) noexcept
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = descriptorPool;
			descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;
			descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
			return descriptorSetAllocateInfo;
		}

		_NODISCARD constexpr VkDescriptorImageInfo descriptorImageInfo(
			VkSampler sampler,
			VkImageView imageView,
			VkImageLayout imageLayout) noexcept
		{
			VkDescriptorImageInfo descriptorImageInfo{};
			descriptorImageInfo.sampler = sampler;
			descriptorImageInfo.imageView = imageView;
			descriptorImageInfo.imageLayout = imageLayout;
			return descriptorImageInfo;
		}

		_NODISCARD constexpr VkWriteDescriptorSet writeDescriptorSet(
			VkDescriptorSet dstSet,
			VkDescriptorType type,
			uint32_t binding,
			VkDescriptorBufferInfo* bufferInfo,
			uint32_t descriptorCount = 1) noexcept
		{
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = dstSet;
			writeDescriptorSet.descriptorType = type;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.pBufferInfo = bufferInfo;
			writeDescriptorSet.descriptorCount = descriptorCount;
			return writeDescriptorSet;
		}

		_NODISCARD constexpr VkWriteDescriptorSet writeDescriptorSet(
			VkDescriptorSet dstSet,
			VkDescriptorType type,
			uint32_t binding,
			VkDescriptorImageInfo* imageInfo,
			uint32_t descriptorCount = 1) noexcept
		{
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = dstSet;
			writeDescriptorSet.descriptorType = type;
			writeDescriptorSet.dstBinding = binding;
			writeDescriptorSet.pImageInfo = imageInfo;
			writeDescriptorSet.descriptorCount = descriptorCount;
			return writeDescriptorSet;
		}

		_NODISCARD constexpr VkVertexInputBindingDescription vertexInputBindingDescription(
			uint32_t binding,
			uint32_t stride,
			VkVertexInputRate inputRate) noexcept
		{
			VkVertexInputBindingDescription vInputBindDescription{};
			vInputBindDescription.binding = binding;
			vInputBindDescription.stride = stride;
			vInputBindDescription.inputRate = inputRate;
			return vInputBindDescription;
		}

		_NODISCARD constexpr VkVertexInputAttributeDescription vertexInputAttributeDescription(
			uint32_t binding,
			uint32_t location,
			VkFormat format,
			uint32_t offset) noexcept
		{
			VkVertexInputAttributeDescription vInputAttribDescription{};
			vInputAttribDescription.location = location;
			vInputAttribDescription.binding = binding;
			vInputAttribDescription.format = format;
			vInputAttribDescription.offset = offset;
			return vInputAttribDescription;
		}

		_NODISCARD constexpr VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
		{
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return pipelineVertexInputStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
			const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions
		) noexcept
		{
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
			pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
			return pipelineVertexInputStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
			VkPrimitiveTopology topology,
			VkPipelineInputAssemblyStateCreateFlags flags,
			VkBool32 primitiveRestartEnable)
		{
			VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
			pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipelineInputAssemblyStateCreateInfo.topology = topology;
			pipelineInputAssemblyStateCreateInfo.flags = flags;
			pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
			return pipelineInputAssemblyStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
			VkPolygonMode polygonMode,
			VkCullModeFlags cullMode,
			VkFrontFace frontFace,
			VkPipelineRasterizationStateCreateFlags flags = 0) noexcept
		{
			VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
			pipelineRasterizationStateCreateInfo.cullMode = cullMode;
			pipelineRasterizationStateCreateInfo.frontFace = frontFace;
			pipelineRasterizationStateCreateInfo.flags = flags;
			pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
			return pipelineRasterizationStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
			VkColorComponentFlags colorWriteMask,
			VkBool32 blendEnable) noexcept
		{
			VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
			pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
			pipelineColorBlendAttachmentState.blendEnable = blendEnable;
			return pipelineColorBlendAttachmentState;
		}

		_NODISCARD constexpr VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
			uint32_t attachmentCount,
			const VkPipelineColorBlendAttachmentState* pAttachments) noexcept
		{
			VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
			pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
			pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
			return pipelineColorBlendStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
			VkBool32 depthTestEnable,
			VkBool32 depthWriteEnable,
			VkCompareOp depthCompareOp) noexcept
		{
			VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
			pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
			pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
			pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
			pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			return pipelineDepthStencilStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
			uint32_t viewportCount,
			uint32_t scissorCount,
			VkPipelineViewportStateCreateFlags flags = 0) noexcept
		{
			VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
			pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipelineViewportStateCreateInfo.viewportCount = viewportCount;
			pipelineViewportStateCreateInfo.scissorCount = scissorCount;
			pipelineViewportStateCreateInfo.flags = flags;
			return pipelineViewportStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
			VkSampleCountFlagBits rasterizationSamples,
			VkPipelineMultisampleStateCreateFlags flags = 0) noexcept
		{
			VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
			pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
			pipelineMultisampleStateCreateInfo.flags = flags;
			return pipelineMultisampleStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
			const VkDynamicState* pDynamicStates,
			uint32_t dynamicStateCount,
			VkPipelineDynamicStateCreateFlags flags = 0) noexcept
		{
			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
			pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
			pipelineDynamicStateCreateInfo.flags = flags;
			return pipelineDynamicStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
			const std::vector<VkDynamicState>& pDynamicStates,
			VkPipelineDynamicStateCreateFlags flags = 0) noexcept
		{
			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
			pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
			pipelineDynamicStateCreateInfo.flags = flags;
			return pipelineDynamicStateCreateInfo;
		}

		_NODISCARD constexpr VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo(uint32_t patchControlPoints) noexcept
		{
			VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo{};
			pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			pipelineTessellationStateCreateInfo.patchControlPoints = patchControlPoints;
			return pipelineTessellationStateCreateInfo;
		}

		_NODISCARD constexpr VkGraphicsPipelineCreateInfo pipelineCreateInfo(
			VkPipelineLayout layout,
			VkRenderPass renderPass,
			VkPipelineCreateFlags flags = 0) noexcept
		{
			VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.layout = layout;
			pipelineCreateInfo.renderPass = renderPass;
			pipelineCreateInfo.flags = flags;
			pipelineCreateInfo.basePipelineIndex = -1;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			return pipelineCreateInfo;
		}

		_NODISCARD constexpr VkGraphicsPipelineCreateInfo pipelineCreateInfo() noexcept
		{
			VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
			pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineCreateInfo.basePipelineIndex = -1;
			pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			return pipelineCreateInfo;
		}

		_NODISCARD constexpr VkComputePipelineCreateInfo computePipelineCreateInfo(
			VkPipelineLayout layout,
			VkPipelineCreateFlags flags = 0) noexcept
		{
			VkComputePipelineCreateInfo computePipelineCreateInfo{};
			computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			computePipelineCreateInfo.layout = layout;
			computePipelineCreateInfo.flags = flags;
			return computePipelineCreateInfo;
		}

		_NODISCARD constexpr VkPushConstantRange pushConstantRange(
			VkShaderStageFlags stageFlags,
			uint32_t size,
			uint32_t offset) noexcept
		{
			VkPushConstantRange pushConstantRange{};
			pushConstantRange.stageFlags = stageFlags;
			pushConstantRange.offset = offset;
			pushConstantRange.size = size;
			return pushConstantRange;
		}

		_NODISCARD constexpr VkBindSparseInfo bindSparseInfo() noexcept
		{
			VkBindSparseInfo bindSparseInfo{};
			bindSparseInfo.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
			return bindSparseInfo;
		}

		/** @brief Initialize a map entry for a shader specialization constant */
		_NODISCARD constexpr VkSpecializationMapEntry specializationMapEntry(uint32_t constantID, uint32_t offset, size_t size) noexcept
		{
			VkSpecializationMapEntry specializationMapEntry{};
			specializationMapEntry.constantID = constantID;
			specializationMapEntry.offset = offset;
			specializationMapEntry.size = size;
			return specializationMapEntry;
		}

		/** @brief Initialize a specialization constant info structure to pass to a shader stage */
		_NODISCARD constexpr VkSpecializationInfo specializationInfo(uint32_t mapEntryCount, const VkSpecializationMapEntry* mapEntries, size_t dataSize, const void* data) noexcept
		{
			VkSpecializationInfo specializationInfo{};
			specializationInfo.mapEntryCount = mapEntryCount;
			specializationInfo.pMapEntries = mapEntries;
			specializationInfo.dataSize = dataSize;
			specializationInfo.pData = data;
			return specializationInfo;
		}

		/** @brief Initialize a specialization constant info structure to pass to a shader stage */
		_NODISCARD constexpr VkSpecializationInfo specializationInfo(const std::vector<VkSpecializationMapEntry>& mapEntries, size_t dataSize, const void* data) noexcept
		{
			VkSpecializationInfo specializationInfo{};
			specializationInfo.mapEntryCount = static_cast<uint32_t>(mapEntries.size());
			specializationInfo.pMapEntries = mapEntries.data();
			specializationInfo.dataSize = dataSize;
			specializationInfo.pData = data;
			return specializationInfo;
		}

		// Ray tracing related
		_NODISCARD constexpr VkAccelerationStructureGeometryKHR accelerationStructureGeometryKHR() noexcept
		{
			VkAccelerationStructureGeometryKHR accelerationStructureGeometryKHR{};
			accelerationStructureGeometryKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			return accelerationStructureGeometryKHR;
		}

		_NODISCARD constexpr VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR() noexcept
		{
			VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfoKHR{};
			accelerationStructureBuildGeometryInfoKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			return accelerationStructureBuildGeometryInfoKHR;
		}

		_NODISCARD constexpr VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR() noexcept
		{
			VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfoKHR{};
			accelerationStructureBuildSizesInfoKHR.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
			return accelerationStructureBuildSizesInfoKHR;
		}

		_NODISCARD constexpr VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR() noexcept
		{
			VkRayTracingShaderGroupCreateInfoKHR rayTracingShaderGroupCreateInfoKHR{};
			rayTracingShaderGroupCreateInfoKHR.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			return rayTracingShaderGroupCreateInfoKHR;
		}

		_NODISCARD constexpr VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR() noexcept
		{
			VkRayTracingPipelineCreateInfoKHR rayTracingPipelineCreateInfoKHR{};
			rayTracingPipelineCreateInfoKHR.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
			return rayTracingPipelineCreateInfoKHR;
		}

		_NODISCARD constexpr VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructureKHR() noexcept
		{
			VkWriteDescriptorSetAccelerationStructureKHR writeDescriptorSetAccelerationStructureKHR{};
			writeDescriptorSetAccelerationStructureKHR.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			return writeDescriptorSetAccelerationStructureKHR;
		}
	}
}