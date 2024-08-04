#pragma once
#include "Voortman3DCore.hpp"
#include "resource.h"
#include "VulkanglTFModel.hpp"
#include "TwinCATConnection.hpp"
#include "commdlg.h"

namespace Voortman3D {

	constexpr uint32_t randomVariableKey = 10;

	class Voortman3D final : public Voortman3DCore {
	public:
		Voortman3D(HINSTANCE hInstance);

		~Voortman3D();

		PFN_vkCmdBeginConditionalRenderingEXT vkCmdBeginConditionalRenderingEXT{};
		PFN_vkCmdEndConditionalRenderingEXT vkCmdEndConditionalRenderingEXT{};

		vkglTF::Model scene;

		bool wireframe = false;

		// Value that will be read from TwinCAT
		float sawHeight{};

		std::chrono::time_point<std::chrono::high_resolution_clock> lastPLCRead;

		const VkClearColorValue backgroundColor = { 1.f, 1.f, 1.f, 1.f };

		struct UniformData {
			glm::mat4 projection;
			glm::mat4 view;
			glm::mat4 model;
		} uniformData{};
		Buffer uniformBuffer{};

		std::vector<int32_t> conditionalVisibility{};
		Buffer conditionalBuffer{};

		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

		TwinCATConnection* TCconnection{};

		struct Pipelines {
			VkPipeline solid{ VK_NULL_HANDLE };
			VkPipeline wireframe{ VK_NULL_HANDLE };
		} pipelines;
		
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
		VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };

		void renderNode(const vkglTF::Node* node, const VkCommandBuffer commandBuffer);

		void loadAssets(const std::string& FilePath);
		void prepareUniformBuffers();
		void setupDescriptors();
		void preparePipelines();
		void buildCommandBuffers();
		void updateUniformBuffers();
		void renderFrame();
		void updateConditionalBuffer();
		void prepareConditionalRendering();
		void TwinCATPreperation();
		void draw();
		void OpenFileDialog();

		void OnUpdateUIOverlay(UIOverlay* uioverlay) override;
		void prepare()			                     override;
		void GetEnabledFeatures()                    override;
		void render()                                override;
	};
}

// Easy defined main for reuse in other projects
#pragma warning(disable : 28251)
VOORTMAN_3D_MAIN()
#pragma warning(default : 28251)