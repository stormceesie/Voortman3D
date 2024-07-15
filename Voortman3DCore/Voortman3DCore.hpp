#pragma once
#include "pch.h"
#include "Window.hpp"
#include "Debug.hpp"
#include "Tools.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "Initializers.hpp"
#include "Camera.hpp"

namespace Voortman3D {
	class Voortman3DCore {
	public:
		static std::vector<const char*> args;

		std::wstring title = L"Title Not Set";
		std::wstring name = L"Name Not Set";

		uint32_t width = 1280;
		uint32_t height = 720;

		uint32_t destWidth;
		uint32_t destHeight;
		uint32_t frameCounter = 0;
		uint32_t lastFPS = 0;

		float frameTimer = 1.0f;

		bool paused = false;

		float timer = 0.0f;
		// Multiplier for speeding up (or slowing down) the global timer
		float timerSpeed = 0.25f;

		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;

		bool resizing = false;

		bool prepared = false;
		bool viewUpdated = false;

		void handleMouseMove(int32_t x, int32_t y);
		void nextFrame();
		void updateOverlay() {};
		void createPipelineCache();
		void createCommandPool();
		void createSynchronizationPrimitives();
		void initSwapchain();
		void setupSwapChain();
		void createCommandBuffers();
		void destroyCommandBuffers();

		virtual void setupRenderPass();
		virtual void setupFrameBuffer();

		virtual void render() {};

		struct Settings {
			bool overlay = true;
		} settings;

		/// <summary>
		/// Constructor of the Voortman3DCore app
		/// </summary>
		/// <param name="hInstance">hInstance goed through the constructor of the class so that childs of this class can use it already while initializing</param>
		Voortman3DCore(HINSTANCE hInstance);
		
		/// <summary>
		/// Virtual deconstructor so that childs can tailor the constructor to their exact use
		/// </summary>
		virtual ~Voortman3DCore();

		/// <summary>
		/// Function to initialize Vulkan.
		/// </summary>
		void initVulkan();

		/// <summary>
		/// Function that will setup the WIN32 window
		/// </summary>
		/// <param name="WndProc">WNDPROC function that will be called each frame</param>
		void setupWindow(WNDPROC WndProc);

		/// <summary>
		/// Function that does some preperation like loading 3D models etc
		/// </summary>
		void prepare();

		/// <summary>
		/// Renderloop function. This function will endure the entire duration of the program.
		/// </summary>
		void renderLoop();

		/// <summary>
		/// Function that will handle the WIN32 messages
		/// </summary>
		/// <param name="hWnd">HWND pointer for use</param>
		/// <param name="uMsg">Message ID</param>
		/// <param name="wParam">Parameters</param>
		/// <param name="lParam">Parameters</param>
		void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HICON icon;

		VkInstance instance;

		VkPhysicalDevice physicalDevice;

		VulkanDevice* vulkanDevice;

		VkDevice device;

		VulkanSwapChain swapChain;

		VkSubmitInfo submitInfo;

		Camera camera;

		VkQueue queue{ VK_NULL_HANDLE };

		VkCommandPool cmdPool{ VK_NULL_HANDLE };

		VkPipelineCache pipelineCache{ VK_NULL_HANDLE };

		std::vector<VkFramebuffer>frameBuffers;

		VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkFormat depthFormat;

		std::vector<std::string> supportedInstanceExtensions;

		std::vector<VkCommandBuffer> drawCmdBuffers;

		std::vector<const char*> enabledInstanceExtensions;

		VkPhysicalDeviceFeatures enabledFeatures{};

		VkPhysicalDeviceProperties deviceProperties{};

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkPhysicalDeviceMemoryProperties deviceMemoryProperties{};

		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

		VkRenderPass renderPass{ VK_NULL_HANDLE };

		struct {
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} depthStencil;

		struct {
			VkSemaphore presentComplete;
			VkSemaphore renderComplete;
		} semaphores;

		std::vector<VkFence> waitFences;

		void* deviceCreatepNextChain = nullptr;

		virtual VkResult createInstance();

		virtual void setupDepthStencil();

	private:
		HINSTANCE hInstance;

		std::unique_ptr<Window> window;

		void setupDPIAwareness();

		void setupConsole(const std::wstring& title);

		static char* TO_CHAR(const wchar_t* string);
	};
}

// Some easy macro's for easy reusable code
#pragma region DefineFunctions

// Easy function to start a timer to see how long a process takes.
#define STARTCOUNTER(processName)											\
std::chrono::time_point<std::chrono::high_resolution_clock> start, end;		\
start = std::chrono::high_resolution_clock::now();							\
std::cout << processName << "..." << std::endl;								\
const std::string& name = processName;										\

// Easy function to stop the timer and to show the result in the console.
#define STOPCOUNTER()														\
end = std::chrono::high_resolution_clock::now();							\
std::chrono::duration<double> elapsed_time = end - start;					\
std::cout << name << " endured: " << elapsed_time << std::endl;				\
																			
#define VOORTMAN_3D_MAIN()																		  \
Voortman3D::Voortman3D* voortman3D;																  \
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {					  \
	if (voortman3D != NULL) {																	  \
		voortman3D->handleMessages(hWnd, uMsg, wParam, lParam);									  \
	}																							  \
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));											  \
}																								  \
																								  \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {								  \
	for (int32_t i = 0; i < __argc; i++) { Voortman3D::Voortman3D::args.push_back(__argv[i]); };  \
	voortman3D = new Voortman3D::Voortman3D(hInstance);											  \
	voortman3D->initVulkan();																	  \
	voortman3D->setupWindow(WndProc);															  \
	voortman3D->prepare();																		  \
	voortman3D->renderLoop();																	  \
	delete(voortman3D);																			  \
	return 0;																					  \
}																								

#pragma endregion