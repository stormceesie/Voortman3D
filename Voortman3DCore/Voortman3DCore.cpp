// Voortman3DCore.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "Voortman3DCore.hpp"

namespace Voortman3D {
	std::vector<const char*> Voortman3DCore::args;

	Voortman3DCore::Voortman3DCore(HINSTANCE hInstance) : hInstance{hInstance} {

		// Setup console if we are in debug mode
#ifdef _DEBUG
		setupConsole(L"Debug Console");
#endif
		setupDPIAwareness();
	}

	void Voortman3DCore::setupConsole(const std::wstring& title) {
		AllocConsole();
		AttachConsole(GetCurrentProcessId());

		FILE* stream;
		freopen_s(&stream, "CONIN$", "r", stdin);
		freopen_s(&stream, "CONOUT$", "w+", stdout);
		freopen_s(&stream, "CONOUT$", "w+", stderr);
		// Enable flags so we can color the output
		HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(consoleHandle, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(consoleHandle, dwMode);
		SetConsoleTitle(title.c_str());
	}

	void Voortman3DCore::setupDPIAwareness() {
		typedef HRESULT* (__stdcall* SetProcessDpiAwarenessFunc)(PROCESS_DPI_AWARENESS);

		HMODULE shCore = LoadLibraryA("Shcore.dll");
		if (shCore)
		{
			SetProcessDpiAwarenessFunc setProcessDpiAwareness =
				(SetProcessDpiAwarenessFunc)GetProcAddress(shCore, "SetProcessDpiAwareness");

			if (setProcessDpiAwareness != nullptr)
			{
				setProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			}

			FreeLibrary(shCore);
		}
	}

	char* Voortman3DCore::TO_CHAR(const wchar_t* string) {
		size_t len = wcslen(string) + 1;
		char* c_string = new char[len];
		size_t numCharsRead;
		wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
		return c_string;
	}

	VkResult Voortman3DCore::createInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = TO_CHAR(name.c_str());
		appInfo.pEngineName = TO_CHAR(name.c_str());
		appInfo.apiVersion = VK_VERSION_1_3;

		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

		instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

		uint32_t extCount = 0;

		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
		if (extCount > 0) {
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == VK_SUCCESS) {
				for (const VkExtensionProperties& extension : extensions) {
					supportedInstanceExtensions.push_back(extension.extensionName);
				}
			}
		}

		if (enabledInstanceExtensions.size() > 0)
		{
			for (const char* enabledExtension : enabledInstanceExtensions)
			{
				// Output message if requested extension is not available
				if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), enabledExtension) == supportedInstanceExtensions.end())
				{
					std::cerr << "Enabled instance extension \"" << enabledExtension << "\" is not present at instance level\n";
				}
				instanceExtensions.push_back(enabledExtension);
			}
		}

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.pApplicationInfo = &appInfo;

#ifdef _DEBUG
		if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != supportedInstanceExtensions.end()) {
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			std::cout << "Found Debug utils extension !" << std::endl;
		}
		else {
			std::cerr << "Could not find 'VK_EXT_DEBUG_UTILS_EXTENSION_NAME' in supported extensions. Because of this the validation layers will be disables!" << std::endl;
		}
#endif

		if (instanceExtensions.size() > 0) {
			instanceCreateInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
			instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
		}

#ifdef _DEBUG
		const char* validationLayerName = "VK_LAYER_KHRONOS_validation";

		// Check if this layer is available at instance level
		uint32_t instanceLayerCount;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
		std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
		bool validationLayerPresent = false;
		for (VkLayerProperties& layer : instanceLayerProperties) {
			if (strcmp(layer.layerName, validationLayerName) == 0) {
				validationLayerPresent = true;
				break;
			}
		}
		if (validationLayerPresent) {
			instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
			instanceCreateInfo.enabledLayerCount = 1;

			std::cout << "Validation was enabled !" << std::endl;
		}
		else {
			std::cerr << "Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled";
		}
#endif

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

		if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != supportedInstanceExtensions.end()) {
			DebugUtils::setup(instance);
		}

		return result;
	}

	Voortman3DCore::~Voortman3DCore() {
		swapChain.cleanup();
		
		if (descriptorPool)
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		destroyCommandBuffers();

		if (renderPass)
			vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto& frameBuffer : frameBuffers) {
			vkDestroyFramebuffer(device, frameBuffer, nullptr);
		}

		vkDestroyImageView(device, depthStencil.view, nullptr);
		vkDestroyImage(device, depthStencil.image, nullptr);
		vkFreeMemory(device, depthStencil.mem, nullptr);

		vkDestroyPipelineCache(device, pipelineCache, nullptr);

		vkDestroyCommandPool(device, cmdPool, nullptr);

		vkDestroySemaphore(device, semaphores.presentComplete, nullptr);
		vkDestroySemaphore(device, semaphores.renderComplete, nullptr);

		for (auto& fence : waitFences) {
			vkDestroyFence(device, fence, nullptr);
		}

		if (vulkanDevice)
			delete vulkanDevice;

#ifdef _DEBUG
		Debug::freeDebugCallback(instance);
#endif

		if (instance)
			vkDestroyInstance(instance, nullptr);
	}

	void Voortman3DCore::destroyCommandBuffers() {
		vkFreeCommandBuffers(device, cmdPool, static_cast<uint32_t>(drawCmdBuffers.size()), drawCmdBuffers.data());
	}

	void Voortman3DCore::initVulkan() {
#ifdef _DEBUG
		STARTCOUNTER("Initializing Vulkan");
#endif
		VkResult err;

		err = createInstance();

		if (err) {
			std::cerr << "Could not create Vulkan instance : " << Tools::errorString(err) << std::endl;
			return;
		}

#ifdef _DEBUG
		Debug::setupDebugging(instance);
#endif

		uint32_t gpuCount = 0;

		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr));
		if (gpuCount == 0) {
			std::cerr << "No device with Vulkan support found" << std::endl;
			return;
		}

		std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
		err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());

		if (err) {
			std::cerr << "Could not enumerate physical devices : \n" << Tools::errorString(err) << std::endl;
			return;
		}

#ifdef _DEBUG
		std::cout << "Available Vulkan devices" << "\n";
		for (uint32_t i = 0; i < gpuCount; i++) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
			std::cout << "Device [" << i << "] : " << deviceProperties.deviceName << std::endl;
			std::cout << " Type: " << Tools::physicalDeviceTypeString(deviceProperties.deviceType) << "\n";
			std::cout << " API: " << (deviceProperties.apiVersion >> 22) << "." << ((deviceProperties.apiVersion >> 12) & 0x3ff) << "." << (deviceProperties.apiVersion & 0xfff) << "\n";
		}

		std::cout << "Selected device " << gpuCount - 1 << std::endl;
#endif

		// Select last GPU for now it is also possible to query for memory, speed etc
		physicalDevice = physicalDevices[gpuCount - 1];

		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

		vulkanDevice = new VulkanDevice(physicalDevice);

		VkResult res = vulkanDevice->createLogicalDevice(enabledFeatures, enabledInstanceExtensions, deviceCreatepNextChain);

		if (res != VK_SUCCESS) {
			std::cerr << "Could not create Vulkan Device : \n" + Tools::errorString(res) << std::endl;
			return;
		}

		device = vulkanDevice->logicalDevice;

		vkGetDeviceQueue(device, vulkanDevice->queueFamilyIndices.graphics, 0, &queue);

		VkBool32 validFormat{ false };

		validFormat = Tools::getSupportedDepthFormat(physicalDevice, &depthFormat);

		assert(validFormat);

		swapChain.connect(instance, physicalDevice, device);

#ifdef _DEBUG
		std::cout << "Swapchain is connected to the physical device!" << std::endl;
#endif

		// Create synchronization objects
		VkSemaphoreCreateInfo semaphoreCreateInfo = Initializers::semaphoreCreateInfo();
		// Create a semaphore used to synchronize image presentation
		// Ensures that the image is displayed before we start submitting new commands to the queue
		VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.presentComplete));
		// Create a semaphore used to synchronize command submission
		// Ensures that the image is not presented until all commands have been submitted and executed
		VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.renderComplete));

		// Set up submit info structure
		// Semaphores will stay the same during application lifetime
		// Command buffer submission info is set by each example
		submitInfo = Initializers::submitInfo();
		submitInfo.pWaitDstStageMask = &submitPipelineStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &semaphores.presentComplete;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &semaphores.renderComplete;

#ifdef _DEBUG
		STOPCOUNTER();
#endif
	}

	void Voortman3DCore::setupWindow(WNDPROC WndProc) {
		// If window doesn't exist create a new unique ptr
		if (!window)
			window = std::make_unique<Window>(hInstance, WndProc, icon, name, title, height, width);
	}

	void Voortman3DCore::initSwapchain() {
		swapChain.initSurface(hInstance, window->window());
	}

	void Voortman3DCore::createCommandPool() {
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = swapChain.queueNodeIndex;
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmdPool));
	}

	void Voortman3DCore::setupSwapChain() {
		swapChain.create(&width, &height, true, false);
	}

	void Voortman3DCore::createCommandBuffers() {
		// Create one command buffer for each swap chain image and reuse for rendering
		drawCmdBuffers.resize(swapChain.imageCount);

		VkCommandBufferAllocateInfo cmdBufAllocateInfo =
			Initializers::commandBufferAllocateInfo(
				cmdPool,
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				static_cast<uint32_t>(drawCmdBuffers.size()));

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, drawCmdBuffers.data()));
	}

	void Voortman3DCore::createSynchronizationPrimitives() {
		// Wait fences to sync command buffer access
		VkFenceCreateInfo fenceCreateInfo = Initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		waitFences.resize(drawCmdBuffers.size());
		for (auto& fence : waitFences) {
			VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
		}
	}

	void Voortman3DCore::setupDepthStencil() {
		VkImageCreateInfo imageCI{};
		imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCI.imageType = VK_IMAGE_TYPE_2D;
		imageCI.format = depthFormat;
		imageCI.extent = { width, height, 1 };
		imageCI.mipLevels = 1;
		imageCI.arrayLayers = 1;
		imageCI.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCI.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		VK_CHECK_RESULT(vkCreateImage(device, &imageCI, nullptr, &depthStencil.image));
		VkMemoryRequirements memReqs{};
		vkGetImageMemoryRequirements(device, depthStencil.image, &memReqs);

		VkMemoryAllocateInfo memAllloc{};
		memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllloc.allocationSize = memReqs.size;
		memAllloc.memoryTypeIndex = vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &memAllloc, nullptr, &depthStencil.mem));
		VK_CHECK_RESULT(vkBindImageMemory(device, depthStencil.image, depthStencil.mem, 0));

		VkImageViewCreateInfo imageViewCI{};
		imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCI.image = depthStencil.image;
		imageViewCI.format = depthFormat;
		imageViewCI.subresourceRange.baseMipLevel = 0;
		imageViewCI.subresourceRange.levelCount = 1;
		imageViewCI.subresourceRange.baseArrayLayer = 0;
		imageViewCI.subresourceRange.layerCount = 1;
		imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
		if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
			imageViewCI.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		VK_CHECK_RESULT(vkCreateImageView(device, &imageViewCI, nullptr, &depthStencil.view));
	}

	void Voortman3DCore::setupRenderPass() {
#ifdef _DEBUG
		STARTCOUNTER("Setting up render pass");
#endif

		std::array<VkAttachmentDescription, 2> attachments = {};
		// Color attachment
		attachments[0].format = swapChain.colorFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		attachments[1].format = depthFormat;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		// Subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		dependencies[0].dependencyFlags = 0;

		dependencies[1].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].dstSubpass = 0;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].srcAccessMask = 0;
		dependencies[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependencies[1].dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));

#ifdef _DEBUG
		STOPCOUNTER();
#endif
	}

	void Voortman3DCore::createPipelineCache() {
#ifdef _DEBUG
		STARTCOUNTER("Creating Pipeline Cache");
#endif

		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache));

#ifdef _DEBUG
		STOPCOUNTER();
#endif
	}

	void Voortman3DCore::setupFrameBuffer() {
		VkImageView attachments[2];

		// Depth/Stencil attachment is the same for all frame buffers
		attachments[1] = depthStencil.view;

		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.pNext = NULL;
		frameBufferCreateInfo.renderPass = renderPass;
		frameBufferCreateInfo.attachmentCount = 2;
		frameBufferCreateInfo.pAttachments = attachments;
		frameBufferCreateInfo.width = width;
		frameBufferCreateInfo.height = height;
		frameBufferCreateInfo.layers = 1;

		// Create frame buffers for every swap chain image
		frameBuffers.resize(swapChain.imageCount);
		for (uint32_t i = 0; i < frameBuffers.size(); i++)
		{
			attachments[0] = swapChain.buffers[i].view;
			VK_CHECK_RESULT(vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &frameBuffers[i]));
		}
	}


	void Voortman3DCore::prepare() {
#ifdef _DEBUG
		STARTCOUNTER("Final Preperations");
#endif

		initSwapchain();
		createCommandPool();
		setupSwapChain();
		createCommandBuffers();
		createSynchronizationPrimitives();
		setupDepthStencil();
		setupRenderPass();
		createPipelineCache();
		setupFrameBuffer();

#ifdef _DEBUG
		STOPCOUNTER();
#endif
	}

	void Voortman3DCore::nextFrame() {
		auto tStart = std::chrono::high_resolution_clock::now();
		if (viewUpdated)
		{
			viewUpdated = false;
		}

		render();
		frameCounter++;
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();

		frameTimer = (float)tDiff / 1000.0f;
		camera.update(frameTimer);
		if (camera.moving())
		{
			viewUpdated = true;
		}
		// Convert to clamped timer value
		if (!paused)
		{
			timer += timerSpeed * frameTimer;
			if (timer > 1.0)
			{
				timer -= 1.0f;
			}
		}
		float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count());
		if (fpsTimer > 1000.0f)
		{
			lastFPS = static_cast<uint32_t>((float)frameCounter * (1000.0f / fpsTimer));
#if defined(_WIN32)
			if (!settings.overlay) {
				std::wstring windowTitle = title;
				SetWindowText(window->window(), windowTitle.c_str());
			}
#endif
			frameCounter = 0;
			lastTimestamp = tEnd;
		}
		tPrevEnd = tEnd;

		// TODO: Cap UI overlay update rates
		updateOverlay();
	}

	void Voortman3DCore::renderLoop() {
		MSG msg;
		bool quitMessageReceived = false;
		while (!quitMessageReceived) {
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					quitMessageReceived = true;
					break;
				}
			}
			if (prepared && !IsIconic(window->window())) {
				nextFrame();
			}
		}

		// Flush device
		if (device != VK_NULL_HANDLE) {
			vkDeviceWaitIdle(device);
		}
	}

	void Voortman3DCore::handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_CLOSE:
			DestroyWindow(window->window());
			PostQuitMessage(0);
			break;

		case WM_PAINT:
			ValidateRect(window->window(), NULL);
			break;
		}
	}
}
