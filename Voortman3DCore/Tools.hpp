#pragma once
#include "pch.hpp"

#define VK_FLAGS_NONE 0

#define DEFAULT_FENCE_TIMEOUT 100000000000

#define VK_CHECK_RESULT(f)																											 \
{																																	 \
	VkResult res = (f);																												 \
	if (res != VK_SUCCESS) _UNLIKELY																								 \
	{																																 \
		std::cerr << "Fatal : VkResult is \"" << Tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
		assert(res == VK_SUCCESS);																									 \
	}																																 \
}

namespace Voortman3D {
	namespace Tools {
		/** @brief Disable message boxes on fatal errors */
		extern bool errorModeSilent;

		/** @brief Returns an error code as a string */
		_NODISCARD std::string errorString(VkResult errorCode);

		/** @brief Returns the device type as a string */
		_NODISCARD std::string physicalDeviceTypeString(VkPhysicalDeviceType type);

		// Selected a suitable supported depth format starting with 32 bit down to 16 bit
		// Returns false if none of the depth formats in the list is supported by the device
		_NODISCARD VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat);
		// Same as getSupportedDepthFormat but will only select formats that also have stencil
		_NODISCARD VkBool32 getSupportedDepthStencilFormat(VkPhysicalDevice physicalDevice, VkFormat* depthStencilFormat);

		// Returns tru a given format support LINEAR filtering
		VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling);
		// Returns true if a given format has a stencil part
		VkBool32 formatHasStencil(VkFormat format);

		// Put an image memory barrier for setting an image layout on the sub resource into the given command buffer
		void setImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkImageSubresourceRange subresourceRange,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		// Uses a fixed sub resource layout with first mip level and layer
		void setImageLayout(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkImageAspectFlags aspectMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		/** @brief Insert an image memory barrier into the command buffer */
		void insertImageMemoryBarrier(
			VkCommandBuffer cmdbuffer,
			VkImage image,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageLayout oldImageLayout,
			VkImageLayout newImageLayout,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkImageSubresourceRange subresourceRange);

		_NODISCARD VkShaderModule loadShader(const char* fileName, VkDevice device);

		/** @brief Checks if a file exists */
		_NODISCARD bool fileExists(const std::string& filename);

		/// <summary>
		/// Function to get the aligned size in any kind of integer.
		/// </summary>
		template<typename T>
		_NODISCARD inline static 
			typename std::enable_if<std::is_same<T, int16_t>::value ||
			std::is_same<T, int32_t>::value ||
			std::is_same<T, int64_t>::value, T>::type
		alignedSize(const T value, const T alignment) noexcept {
			return (value + alignment - 1) & ~(alignment - 1);
		}
	}
}