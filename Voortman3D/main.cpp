#include "main.hpp"

namespace Voortman3D {
	Voortman3D::Voortman3D(HINSTANCE hInstance) : Voortman3DCore(hInstance) {
		this->title = L"Voortman3D";
		this->name = L"Voortman3D";
		this->icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

		camera.type = Camera::CameraType::lookat;
		camera.setPerspective(45.0f, (float)width / (float)height, 0.1f, 512.0f);
		camera.setRotation(glm::vec3(-2.25f, -52.0f, 0.0f));
		camera.setTranslation(glm::vec3(1.9f, -2.05f, -18.0f));
		camera.rotationSpeed *= 0.25f;

		enabledInstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		enabledDeviceExtensions.push_back(VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME);
	}

	void Voortman3D::GetEnabledFeatures() {
		if (deviceFeatures.fillModeNonSolid) {
			enabledFeatures.fillModeNonSolid = VK_TRUE;
		}
	}

	Voortman3D::~Voortman3D() {
		if (device) {
			if (pipelines.solid)
				vkDestroyPipeline(device, pipelines.solid, nullptr);

			if (pipelines.wireframe)
				vkDestroyPipeline(device, pipelines.wireframe, nullptr);

			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			uniformBuffer.destroy();
			conditionalBuffer.destroy();
		}
	}

	void Voortman3D::OnUpdateUIOverlay(UIOverlay* uioverlay) {
		if (uioverlay->header("Visibility")) {

			if (uioverlay->button("All")) {
				for (auto i = 0; i < conditionalVisibility.size(); i++) {
					conditionalVisibility[i] = 1;
				}
				updateConditionalBuffer();
			}
			ImGui::SameLine();
			if (uioverlay->button("None")) {
				for (auto i = 0; i < conditionalVisibility.size(); i++) {
					conditionalVisibility[i] = 0;
				}
				updateConditionalBuffer();
			}

			if (uioverlay->checkBox("Wireframe", &wireframe)) {
				buildCommandBuffers();
			}

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

	void Voortman3D::renderNode(vkglTF::Node* node, VkCommandBuffer commandBuffer) {
		if (node->mesh) {
			for (vkglTF::Primitive* primitive : node->mesh->primitives) {
				const std::vector<VkDescriptorSet> descriptorsets = {
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
		for (auto child : node->children) {
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
		// Pool
		std::vector<VkDescriptorPoolSize> poolSizes = {
			Initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
		};
		VkDescriptorPoolCreateInfo descriptorPoolCI = Initializers::descriptorPoolCreateInfo(poolSizes, 1);
		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolCI, nullptr, &descriptorPool));

		// Layouts
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
			Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
		};
		VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
		descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorLayoutCI.pBindings = setLayoutBindings.data();
		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayoutCI, nullptr, &descriptorSetLayout));

		// Sets
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = Initializers::descriptorSetAllocateInfo(descriptorPool, &descriptorSetLayout, 1);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));
		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			Initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformBuffer.descriptor)
		};
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}

	void Voortman3D::preparePipelines() {
		// Layout
		std::array<VkDescriptorSetLayout, 2> setLayouts = {
			descriptorSetLayout, vkglTF::descriptorSetLayoutUbo
		};
		VkPipelineLayoutCreateInfo pipelineLayoutCI = Initializers::pipelineLayoutCreateInfo(setLayouts.data(), 2);
		VkPushConstantRange pushConstantRange = Initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4), 0);
		pipelineLayoutCI.pushConstantRangeCount = 1;
		pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));

		// Pipeline
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI = Initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
		VkPipelineRasterizationStateCreateInfo rasterizationStateCI = Initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
		VkPipelineColorBlendAttachmentState blendAttachmentState = Initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
		VkPipelineColorBlendStateCreateInfo colorBlendStateCI = Initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCI = Initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
		VkPipelineViewportStateCreateInfo viewportStateCI = Initializers::pipelineViewportStateCreateInfo(1, 1, 0);
		VkPipelineMultisampleStateCreateInfo multisampleStateCI = Initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
		const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicStateCI = Initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables, 0);

		VkGraphicsPipelineCreateInfo pipelineCI = Initializers::pipelineCreateInfo(pipelineLayout, renderPass, 0);
		pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
		pipelineCI.pRasterizationState = &rasterizationStateCI;
		pipelineCI.pColorBlendState = &colorBlendStateCI;
		pipelineCI.pMultisampleState = &multisampleStateCI;
		pipelineCI.pViewportState = &viewportStateCI;
		pipelineCI.pDepthStencilState = &depthStencilStateCI;
		pipelineCI.pDynamicState = &dynamicStateCI;
		pipelineCI.pVertexInputState = vkglTF::Vertex::getPipelineVertexInputState({ vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV });

		const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStagesRender = {
			loadShader("Shaders/model.vert.spv", VK_SHADER_STAGE_VERTEX_BIT),
			loadShader("Shaders/model.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		pipelineCI.stageCount = static_cast<uint32_t>(shaderStagesRender.size());
		pipelineCI.pStages = shaderStagesRender.data();

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.solid));

		rasterizationStateCI.polygonMode = VK_POLYGON_MODE_LINE;
		pipelineCI.renderPass = renderPass;
		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipelines.wireframe));
	}

	void Voortman3D::loadAssets() {
		scene.loadFromFile("C:/Git/Vulkan/assets/models/gltf/glTF-Embedded/Buggy.gltf", vulkanDevice, queue);
	}

	void Voortman3D::buildCommandBuffers() {
		VkCommandBufferBeginInfo cmdBufInfo = Initializers::commandBufferBeginInfo();

		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.812f, 0.063f, 0.063f, 1.0f } };
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

			vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, wireframe ? pipelines.wireframe : pipelines.solid);

			const VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &scene.vertices.buffer, offsets);
			vkCmdBindIndexBuffer(drawCmdBuffers[i], scene.indices.buffer, 0, VK_INDEX_TYPE_UINT32);
			for (auto node : scene.nodes) {
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
		if (!vkCmdBeginConditionalRenderingEXT) {
			std::cerr << "Could not get a valid function pointer for vkCmdBeginConditionalRenderingEXT\n";
		}

		vkCmdEndConditionalRenderingEXT = (PFN_vkCmdEndConditionalRenderingEXT)vkGetDeviceProcAddr(device, "vkCmdEndConditionalRenderingEXT");
		if (!vkCmdEndConditionalRenderingEXT) {
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

	void Voortman3D::prepare() {
		Voortman3DCore::prepare();
		loadAssets();
		prepareConditionalRendering();
		prepareUniformBuffers();
		setupDescriptors();
		preparePipelines();
		buildCommandBuffers();
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