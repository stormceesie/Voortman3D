/*
* Swapchain code was derrived from Sasha Williams
*
* A swap chain is a collection of framebuffers used for rendering and presentation to the windowing system
*
* Copyright (C) 2016-2023 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once
#include "pch.hpp"
#include "Tools.hpp"

namespace Voortman3D {
	typedef struct _SwapChainBuffers {
		VkImage image;
		VkImageView view;
	} SwapChainBuffer;

	class VulkanSwapChain
	{
	private:
		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		VkSurfaceKHR surface;
	public:
		VkFormat colorFormat;
		VkColorSpaceKHR colorSpace;
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		uint32_t imageCount;
		std::vector<VkImage> images;
		std::vector<SwapChainBuffer> buffers;
		uint32_t queueNodeIndex = UINT32_MAX;

		void initSurface(void* platformHandle, void* platformWindow);

		void connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
		void create(uint32_t* width, uint32_t* height, bool fullscreen = false);
		VkResult acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);
		VkResult queuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
		void cleanup();
	};
}
