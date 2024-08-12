#include "main.hpp"

#define LOAD_MODEL = 0xDF;

namespace Voortman3D {
	Voortman3D::Voortman3D(HINSTANCE hInstance) : Voortman3DCore(hInstance) {
		this->title = L"Voortman3D";
		this->name = L"Voortman3D";
		this->icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

		camera.setPerspective(45.0f, (float)width / (float)height, 0.1f, 512.0f);
		camera.setRotation(glm::vec3(-26.5f, -37.0f, 0.0f));
		camera.setTranslation(glm::vec3(0.07f, -0.06f, -0.6f));
		camera.setRotationSpeed(0.25f);
		camera.setMovementSpeed(0.5f);

		enabledInstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		enabledDeviceExtensions.push_back(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME);

		// Allocate TCconnection on heap memory as it is long time use
		TCconnection = new TwinCATConnection();
	}

	void Voortman3D::GetEnabledFeatures() {
		if (deviceFeatures.fillModeNonSolid) _LIKELY {
			enabledFeatures.fillModeNonSolid = VK_TRUE;
		}
	}

	Voortman3D::~Voortman3D() {
		if (device) {
			if (pipelines.solid) _LIKELY
				vkDestroyPipeline(device, pipelines.solid, nullptr);

			if (pipelines.wireframe) _LIKELY
				vkDestroyPipeline(device, pipelines.wireframe, nullptr);

			if (TCconnection)
				delete TCconnection;

			if (pipelineLayout) _LIKELY
				vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

			if (descriptorSetLayout)
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

			uniformBuffer.destroy();
			conditionalBuffer.destroy();
		}
	}

	void Voortman3D::OnUpdateUIOverlay(UIOverlay* uioverlay) {
		if (uioverlay->header("Visibility")) {

			if (uioverlay->button("All")) _UNLIKELY { // Wont be true very often
				for (auto i = 0; i < conditionalVisibility.size(); i++) {
					conditionalVisibility[i] = 1;
				}
				updateConditionalBuffer();
			}
			ImGui::SameLine();
			if (uioverlay->button("None")) _UNLIKELY {
				for (auto i = 0; i < conditionalVisibility.size(); i++) {
					conditionalVisibility[i] = 0;
				}
				updateConditionalBuffer();
			}

			if (uioverlay->checkBox("Wireframe", &wireframe)) _UNLIKELY {
				buildCommandBuffers();
			}

			if (uioverlay->button("Load Model")) {
				OpenFileDialog();
			}

			// Read 10 times per second
//			if (std::chrono::high_resolution_clock::now() - lastPLCRead >= std::chrono::milliseconds(100))
//				TCconnection->ReadValue<float>(randomVariableKey, &sawHeight);
			
			uioverlay->inputFloat("Saw Height", &sawHeight);

			ImGui::NewLine();

			ImGui::BeginChild("InnerRegion", ImVec2(200.0f * uioverlay->scale, 400.0f * uioverlay->scale), false);
			for (auto node : scene.linearNodes) {
				// Add visibility toggle checkboxes for all model nodes with a mesh
				if (node->mesh) {
					if (uioverlay->checkBox(("[" + std::to_string(node->index) + "] " + node->mesh->name).c_str(), &conditionalVisibility[node->index])) {
						updateConditionalBuffer();
					}
				}
			}
			ImGui::EndChild();
		}
	}

	void Voortman3D::OpenFileDialog() {
		OPENFILENAME ofn;

		// 520 bytes and short time usage use alloca instead of malloc or new
#pragma warning(disable:6255)
		LPWSTR szFile = (LPWSTR)_alloca(260*sizeof(wchar_t));
#pragma warning(default:6255)

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = window->window();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn)) {

		}
	}

	void Voortman3D::renderNode(const vkglTF::Node* node, const VkCommandBuffer commandBuffer) {
		if (node->mesh) {
			for (vkglTF::Primitive* primitive : node->mesh->primitives) {
				const std::array<VkDescriptorSet, 2> descriptorsets = {
					descriptorSet,
					node->mesh->uniformBuffer.descriptorSet
				};
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>(descriptorsets.size()), descriptorsets.data(), 0, NULL);

				vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(primitive->material.baseColorFactor), &primitive->material.baseColorFactor);

				/*
					[POI] Setup the conditional rendering
				*/
				VkConditionalRenderingBeginInfoEXT conditionalRenderingBeginInfo{};
				conditionalRenderingBeginInfo.sType = VK_STRUCTURE_TYPE_CONDITIONAL_RENDERING_BEGIN_INFO_EXT;
				conditionalRenderingBeginInfo.buffer = conditionalBuffer.buffer;
				conditionalRenderingBeginInfo.offset = sizeof(int32_t) * node->index;

				/*
					[POI] Begin conditionally rendered section

					If the value from the conditional rendering buffer at the given offset is != 0, the draw commands will be executed
				*/
				vkCmdBeginConditionalRenderingEXT(commandBuffer, &conditionalRenderingBeginInfo);

				vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);

				vkCmdEndConditionalRenderingEXT(commandBuffer);
			}

		};
		for (const auto child : node->children) {
			renderNode(child, commandBuffer);
		}
	}

	void Voortman3D::prepareUniformBuffers() {
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&uniformBuffer,
			sizeof(UniformData)));
		VK_CHECK_RESULT(uniformBuffer.map());
		updateUniformBuffers();
	}

	void Voortman3D::setupDescriptors() {

		// Compile time initialization
		static constexpr std::array<VkDescriptorPoolSize, 1> poolSizes = {
			Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
		};

		constexpr VkDescriptorPoolCreateInfo descriptorInfo = Initializers::descriptorPoolCreateInfo(poolSizes, 1);

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorInfo, nullptr, &descriptorPool));

		static constexpr std::array<VkDescriptorSetLayoutBinding, 1> setLayoutBindings = {
			Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0)
		};

		constexpr VkDescriptorSetLayoutCreateInfo descriptorLayoutCI = Initializers::descriptorLayoutCI(setLayoutBindings);

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayoutCI, nullptr, &descriptorSetLayout));

		// Sets cannot be set at compile time
		const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = Initializers::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));

		const std::array<VkWriteDescriptorSet, 1> writeDescriptorSets = {
			Initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformBuffer.descriptor)
		};

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}

	void Voortman3D::preparePipelines() {
		// Layout
		const std::array<VkDescriptorSetLayout, 2> setLayouts = {
			descriptorSetLayout, vkglTF::descriptorSetLayoutUbo
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCI = Initializers::pipelineLayoutCreateInfo(setLayouts.data(), 2);
		const VkPushConstantRange pushConstantRange = Initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4), 0);

		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));

		// Pipeline most of the info can be set at compile time
		constexpr VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI = Initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
		VkPipelineRasterizationStateCreateInfo rasterizationStateCI = Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);

		static constexpr VkPipelineColorBlendAttachmentState blendAttachmentState = Initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

		constexpr VkPipelineColorBlendStateCreateInfo colorBlendStateCI = Initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

		constexpr VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = Initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
		constexpr VkPipelineViewportStateCreateInfo viewportStateCI = Initializers::pipelineViewportStateCreateInfo(1, 1, 0);
		constexpr VkPipelineMultisampleStateCreateInfo multisampleStateCI = Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);

		static constexpr std::array<VkDynamicState, 2> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		constexpr VkPipelineDynamicStateCreateInfo dynamicStateCI = Initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables, 0);

		VkGraphicsPipelineCreateInfo pipelineCI = Initializers::pipelineCreateInfo(pipelineLayout, renderPass, 0);

		pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
		pipelineCI.pRasterizationState = &rasterizationStateCI;
		pipelineCI.pColorBlendState = &colorBlendStateCI;
		pipelineCI.pMultisampleState = &multisampleStateCI;
		pipelineCI.pViewportState = &viewportStateCI;
		pipelineCI.pDepthStencilState = &depthStencilStateCI;
		pipelineCI.pDynamicState = &dynamicStateCI;
		pipelineCI.pVertexInputState = vkglTF::Vertex::getPipelineVertexInputState({ vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::Color });

		// Load shader from resource
		const HRSRC FragmentResource = FindResource(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_MODEL_FRAGMENT), L"Shader");
		const HRSRC VertexResource = FindResource(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_MODEL_VERTEX), L"Shader");

		const HGLOBAL fragmentData = LoadResource(GetModuleHandle(nullptr), FragmentResource);
		const HGLOBAL vertexData = LoadResource(GetModuleHandle(nullptr), VertexResource);

		const size_t fragmentDataSize = SizeofResource(GetModuleHandle(nullptr), FragmentResource);
		const size_t VertexDataSize = SizeofResource(GetModuleHandle(nullptr), VertexResource);

		const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStagesRender = {
			loadShader(VK_SHADER_STAGE_FRAGMENT_BIT, LockResource(fragmentData), fragmentDataSize),
			loadShader(VK_SHADER_STAGE_VERTEX_BIT, LockResource(vertexData), VertexDataSize)
		};

		pipelineCI.stageCount = static_cast<uint32_t>(shaderStagesRender.size());
		pipelineCI.pStages = shaderStagesRender.data();

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.solid));

		rasterizationStateCI.polygonMode = VK_POLYGON_MODE_LINE;
		pipelineCI.renderPass = renderPass;
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.wireframe));
	}

	void Voortman3D::loadAssets(const std::string& FilePath) {
		scene.loadFromFile(FilePath, vulkanDevice, queue);
	}

	void Voortman3D::buildCommandBuffers() {
		constexpr VkCommandBufferBeginInfo cmdBufInfo = Initializers::commandBufferBeginInfo();

		VkClearValue clearValues[2];
		clearValues[0].color = backgroundColor;
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = Initializers::renderPassBeginInfo();
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		for (int32_t i = 0; i < drawCmdBuffers.size(); ++i) {
			renderPassBeginInfo.framebuffer = frameBuffers[i];

			VK_CHECK_RESULT(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo));

			vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport = Initializers::viewport((float)width, (float)height, 0.0f, 1.0f);

			vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

			VkRect2D scissor = Initializers::rect2D(width, height, 0, 0);

			vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

			vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

			// Choose wether we bind the wireframe pipeline or the solid pipeline
			vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, wireframe ? pipelines.wireframe : pipelines.solid);

			constexpr VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &scene.vertices.buffer, offsets);
			vkCmdBindIndexBuffer(drawCmdBuffers[i], scene.indices.buffer, 0, VK_INDEX_TYPE_UINT32);
			for (const auto node : scene.nodes) {
				renderNode(node, drawCmdBuffers[i]);
			}

			drawUI(drawCmdBuffers[i]);

			vkCmdEndRenderPass(drawCmdBuffers[i]);

			VK_CHECK_RESULT(vkEndCommandBuffer(drawCmdBuffers[i]));
		}
	}

	void Voortman3D::prepareConditionalRendering()
	{
		/*
			The conditional rendering functions are part of an extension so they have to be loaded manually
		*/
		vkCmdBeginConditionalRenderingEXT = (PFN_vkCmdBeginConditionalRenderingEXT)vkGetDeviceProcAddr(device, "vkCmdBeginConditionalRenderingEXT");
		if (!vkCmdBeginConditionalRenderingEXT) _UNLIKELY {
			std::cerr << "Could not get a valid function pointer for vkCmdBeginConditionalRenderingEXT\n";
		}

		vkCmdEndConditionalRenderingEXT = (PFN_vkCmdEndConditionalRenderingEXT)vkGetDeviceProcAddr(device, "vkCmdEndConditionalRenderingEXT");
		if (!vkCmdEndConditionalRenderingEXT) _UNLIKELY {
			std::cerr << "Could not get a valid function pointer for vkCmdEndConditionalRenderingEXT\n";
		}

		/*
			Create the buffer that contains the conditional rendering information

			A single conditional value is 32 bits and if it's zero the rendering commands are discarded
			This sample renders multiple rows of objects conditionally, so we setup a buffer with one value per row
		*/
		conditionalVisibility.resize(scene.linearNodes.size());
		VK_CHECK_RESULT(vulkanDevice->createBuffer(
			VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&conditionalBuffer,
			sizeof(int32_t) * conditionalVisibility.size(),
			conditionalVisibility.data()));
		VK_CHECK_RESULT(conditionalBuffer.map());

		// By default, all parts of the glTF are visible
		for (auto i = 0; i < conditionalVisibility.size(); i++) {
			conditionalVisibility[i] = 1;
		}

		/*
			Copy visibility data
		*/
		updateConditionalBuffer();
	}

	void Voortman3D::updateConditionalBuffer() {
		memcpy(conditionalBuffer.mapped, conditionalVisibility.data(), sizeof(int32_t) * conditionalVisibility.size());
	}

	void Voortman3D::updateUniformBuffers()
	{
		uniformData.projection = camera.matrices.perspective;
		uniformData.view = glm::scale(camera.matrices.view, glm::vec3(0.1f, -0.1f, 0.1f));
		uniformData.model = glm::translate(glm::mat4(1.0f), scene.dimensions.min);
		memcpy(uniformBuffer.mapped, &uniformData, sizeof(UniformData));
	}

	void Voortman3D::renderFrame() {
		Voortman3DCore::prepareFrame();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];
		VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
		Voortman3DCore::submitFrame();
	}

	void Voortman3D::TwinCATPreperation() {
		// Connect to TwinCAT (local port 851)
		TCconnection->ConnectToTwinCAT();

		TCconnection->CreateVariableHandle(randomVariableKey);
	}

	void Voortman3D::prepare() {
		Voortman3DCore::prepare();
		loadAssets("C:/Git/Vulkan/assets/models/chinesedragon.gltf");
		prepareConditionalRendering();
		prepareUniformBuffers();
		setupDescriptors();
		preparePipelines();
		buildCommandBuffers();
		TwinCATPreperation();

		prepared = true;
	}

	void Voortman3D::draw() {
		Voortman3DCore::prepareFrame();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];
		VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
		Voortman3DCore::submitFrame();
	}

	void Voortman3D::render() {
		if (!prepared)
			return;
		updateUniformBuffers();
		draw();
	}
}