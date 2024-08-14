/*
* Vulkan glTF model and texture loading class based on tinyglTF (https://github.com/syoyo/tinygltf)
*
* Copyright (C) 2018-2023 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

/*
 * Note that this isn't a complete glTF loader and not all features of the glTF 2.0 spec are supported
 * For details on how glTF 2.0 works, see the official spec at https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * If you are looking for a complete glTF implementation, check out https://github.com/SaschaWillems/Vulkan-glTF-PBR/
 */

#include "Tools.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "VulkanglTFModel.hpp"
#include <new>
#include <iostream>

namespace Voortman3D {

	VkDescriptorSetLayout vkglTF::descriptorSetLayoutImage = VK_NULL_HANDLE;
	VkDescriptorSetLayout vkglTF::descriptorSetLayoutUbo = VK_NULL_HANDLE;
	VkMemoryPropertyFlags vkglTF::memoryPropertyFlags = 0;

	uint32_t vkglTF::descriptorBindingFlags = vkglTF::DescriptorBindingFlags::ImageBaseColor;

	/*
		glTF material
	*/
	void vkglTF::Material::createDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorBindingFlags)
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = descriptorPool;
		descriptorSetAllocInfo.pSetLayouts = &descriptorSetLayout;
		descriptorSetAllocInfo.descriptorSetCount = 1;
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device->logicalDevice, &descriptorSetAllocInfo, &descriptorSet));
		std::vector<VkDescriptorImageInfo> imageDescriptors{};
		std::vector<VkWriteDescriptorSet> writeDescriptorSets{};
		if (descriptorBindingFlags & DescriptorBindingFlags::ImageBaseColor) {
			imageDescriptors.push_back(baseColorTexture->descriptor);
			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.dstSet = descriptorSet;
			writeDescriptorSet.dstBinding = static_cast<uint32_t>(writeDescriptorSets.size());
			writeDescriptorSet.pImageInfo = &baseColorTexture->descriptor;
			writeDescriptorSets.push_back(writeDescriptorSet);
		}
		vkUpdateDescriptorSets(device->logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}

	/*
		glTF primitive
	*/
	void vkglTF::Primitive::setDimensions(glm::vec3 min, glm::vec3 max) {
		dimensions.min = min;
		dimensions.max = max;
		dimensions.size = max - min;
		dimensions.center = (min + max) / 2.0f;
		dimensions.radius = glm::distance(min, max) / 2.0f;
	}

	/*
		glTF mesh
	*/
	vkglTF::Mesh::Mesh(VulkanDevice* device, glm::mat4 matrix) {
		this->device = device;
		this->matrix = matrix;
		VK_CHECK_RESULT(device->createBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			sizeof(matrix),
			&uniformBuffer.buffer,
			&uniformBuffer.memory,
			&matrix));
		VK_CHECK_RESULT(vkMapMemory(device->logicalDevice, uniformBuffer.memory, 0, sizeof(matrix), 0, &uniformBuffer.mapped));
		uniformBuffer.descriptor = { uniformBuffer.buffer, 0, sizeof(matrix) };
	};

	vkglTF::Mesh::~Mesh() {
		vkDestroyBuffer(device->logicalDevice, uniformBuffer.buffer, nullptr);
		vkFreeMemory(device->logicalDevice, uniformBuffer.memory, nullptr);
		for (auto primitive : primitives)
		{
			delete primitive;
		}
	}

	// Multiply yourself with your parent till there is no parent anymore
	// This will result in the orientation matrix in the world where this node needs to be
	glm::mat4 vkglTF::Node::getMatrix() {
		// Use _LIKELY [[likely]] or _UNLIKELY [[unlikely]] as often as possible as it helps the compiler to optimize code
		if (parent) _LIKELY // Most objects are going to have a parent
			return this->matrix * parent->getMatrix();

		return this->matrix;
	}

	void vkglTF::Node::update() {
		if (mesh) _LIKELY {
			glm::mat4x4 matrix = getMatrix();
			memcpy(mesh->uniformBuffer.mapped, &matrix, sizeof(glm::mat4));
		}

		// Also update all children
		for (auto& child : children) _LIKELY {
			child->update();
		}
	}

	vkglTF::Node::~Node() {
		if (mesh) _LIKELY {
			delete mesh;
		}
		for (auto& child : children) _LIKELY {
			delete child;
		}
	}

	/*
		glTF default vertex layout with easy Vulkan mapping functions
	*/
	VkVertexInputBindingDescription vkglTF::Vertex::vertexInputBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vkglTF::Vertex::vertexInputAttributeDescriptions;
	VkPipelineVertexInputStateCreateInfo vkglTF::Vertex::pipelineVertexInputStateCreateInfo;

	constexpr VkVertexInputBindingDescription vkglTF::Vertex::inputBindingDescription(uint32_t binding) noexcept {
		return VkVertexInputBindingDescription({ binding, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX });
	}

	constexpr VkVertexInputAttributeDescription vkglTF::Vertex::inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component) noexcept {
		switch (component) {
		case VertexComponent::Position: _LIKELY
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) });
		case VertexComponent::Normal: _LIKELY
			return VkVertexInputAttributeDescription({ location, binding, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		default: _UNLIKELY
			return VkVertexInputAttributeDescription({});
		}
	}

	constexpr std::vector<VkVertexInputAttributeDescription> vkglTF::Vertex::inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent>& components) noexcept {
		std::vector<VkVertexInputAttributeDescription> result;
		uint32_t location = 0;
		for (VertexComponent component : components) _LIKELY {
			result.push_back(Vertex::inputAttributeDescription(binding, location, component));
			location++;
		}
		return result;
	}

	/** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
	VkPipelineVertexInputStateCreateInfo* vkglTF::Vertex::getPipelineVertexInputState(const std::vector<VertexComponent>& components) noexcept{
		vertexInputBindingDescription = Vertex::inputBindingDescription(0);
		Vertex::vertexInputAttributeDescriptions = Vertex::inputAttributeDescriptions(0, components);
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &Vertex::vertexInputBindingDescription;
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(Vertex::vertexInputAttributeDescriptions.size());
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = Vertex::vertexInputAttributeDescriptions.data();
		return &pipelineVertexInputStateCreateInfo;
	}

	/*
		glTF model loading and rendering class
	*/
	vkglTF::Model::~Model()
	{
		if (vertices.buffer)
			vkDestroyBuffer(device->logicalDevice, vertices.buffer, nullptr);

		if (vertices.memory)
			vkFreeMemory(device->logicalDevice, vertices.memory, nullptr);

		if (indices.buffer)
			vkDestroyBuffer(device->logicalDevice, indices.buffer, nullptr);

		if (indices.memory)
			vkFreeMemory(device->logicalDevice, indices.memory, nullptr);

		for (auto node : nodes) _LIKELY{
			delete node;
		}

		if (descriptorSetLayoutUbo != VK_NULL_HANDLE) _LIKELY {
			vkDestroyDescriptorSetLayout(device->logicalDevice, descriptorSetLayoutUbo, nullptr);
			descriptorSetLayoutUbo = VK_NULL_HANDLE;
		}
		if (descriptorSetLayoutImage != VK_NULL_HANDLE) _LIKELY {
			vkDestroyDescriptorSetLayout(device->logicalDevice, descriptorSetLayoutImage, nullptr);
			descriptorSetLayoutImage = VK_NULL_HANDLE;
		}
		vkDestroyDescriptorPool(device->logicalDevice, descriptorPool, nullptr);
	}

	template <typename T>
	void vkglTF::Model::CopyToIndexBuffer(std::vector<uint32_t>& indexBuffer,
		const tinygltf::BufferView& bufferView,
		const tinygltf::Buffer& buffer,
		const tinygltf::Accessor& accessor,
		uint32_t vertexStart) {

		constexpr size_t kMaxStackAllocSize = 8192; // Define a reasonable maximum stack allocation size

		size_t numElements = accessor.count;
		size_t elementSize = sizeof(T);
		size_t allocSize = numElements * elementSize;

		// Determine if allocation size exceeds the maximum stack allocation size
		T* buf = nullptr;
		if (allocSize <= kMaxStackAllocSize) {
			// Allocate on stack
#pragma warning(disable : 6255) // Ignore stack overflow warning
			buf = (T*)_alloca(allocSize);
#pragma warning(default : 6255)
		}
		else {
			// Allocate on heap
			buf = (T*)malloc(allocSize);
			if (!buf) {
				throw std::bad_alloc(); // Handle malloc failure
			}
		}

		// Copy data into buffer
		memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], allocSize);

		// Process data
		for (size_t index = 0; index < numElements; ++index) {
			_LIKELY
				indexBuffer.push_back(buf[index] + vertexStart);
		}

		// Free heap-allocated memory if necessary
		if (allocSize > kMaxStackAllocSize) {
			free(buf);
		}
	}

	void vkglTF::Model::loadNode(vkglTF::Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale)
	{
		vkglTF::Node* newNode = new Node;

		newNode->index = nodeIndex;
		newNode->parent = parent;
		newNode->name = node.name;
		newNode->matrix = glm::mat4(1.0f);

		if (node.translation.size() == 3) {
			newNode->Translate(glm::make_vec3(node.translation.data()));
		}

		if (node.rotation.size() == 4) {
			glm::quat q = glm::make_quat(node.rotation.data());
			newNode->Rotate(q);
		}

		if (node.scale.size() == 3) {
			newNode->Scale(glm::make_vec3(node.scale.data()));
		}

		if (node.matrix.size() == 16) {
			newNode->matrix = glm::make_mat4x4((float*)node.matrix.data()) * newNode->matrix;
		};

		// Node with children
		if (node.children.size() > 0) {
			for (auto i = 0; i < node.children.size(); i++) {
				loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer, globalscale);
			}
		}

		// Node contains mesh data
		if (node.mesh > -1) {
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			Mesh* newMesh = new Mesh(device, newNode->matrix);
			newMesh->name = mesh.name;
			for (size_t j = 0; j < mesh.primitives.size(); j++) {
				const tinygltf::Primitive& primitive = mesh.primitives[j];
				if (primitive.indices < 0) {
					continue;
				}
				uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
				uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
				uint32_t indexCount = 0;
				uint32_t vertexCount = 0;
				glm::vec3 posMin{};
				glm::vec3 posMax{};
				// Vertices
				{
					const float* bufferPos = nullptr;
					const float* bufferNormals = nullptr;
					const float* bufferColors = nullptr;
					uint32_t numColorComponents;

					// Position attribute is required
					assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

					const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
					const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
					bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
					posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
					posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

					if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
						const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
						const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
						bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					}
					if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
					{
						const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
						const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
						// Color buffer are either of type vec3 or vec4
						numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
						bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
					}

					vertexCount = static_cast<uint32_t>(posAccessor.count);

					for (size_t v = 0; v < posAccessor.count; v++) {
						Vertex vert{};
						vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
						vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
						vertexBuffer.push_back(vert);
					}
				}
				// Indices
				{
					const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

					indexCount = static_cast<uint32_t>(accessor.count);

					switch (accessor.componentType) {
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: _LIKELY { // Uint32_t will be the most likely type
						CopyToIndexBuffer<uint32_t>(indexBuffer, bufferView, buffer, accessor, vertexStart);
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: _UNLIKELY {
						CopyToIndexBuffer<uint16_t>(indexBuffer, bufferView, buffer, accessor, vertexStart);
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: _UNLIKELY {
						CopyToIndexBuffer<uint8_t>(indexBuffer, bufferView, buffer, accessor, vertexStart);
						break;
					}
					default: _UNLIKELY // Very unlikely that it will be from another type
						std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
						return;
					}
				}
				Primitive* newPrimitive = new Primitive(indexStart, indexCount, primitive.material > -1 ? &materials[primitive.material] : &materials.back());

				newPrimitive->firstVertex = vertexStart;
				newPrimitive->vertexCount = vertexCount;
				newPrimitive->setDimensions(posMin, posMax);
				newMesh->primitives.push_back(newPrimitive);
			}
			newNode->mesh = newMesh;
		}
		if (parent) {
			parent->children.push_back(newNode);
		}
		else {
			nodes.push_back(newNode);
		}
		linearNodes.push_back(newNode);
	}

	void vkglTF::Model::loadMaterials(tinygltf::Model& gltfModel)
	{
		for (tinygltf::Material& mat : gltfModel.materials) {
			vkglTF::Material material(device);
			if (mat.values.find("baseColorFactor") != mat.values.end()) {
				material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
			}

			materials.push_back(material);
		}
		// Push a default material at the end of the list for meshes with no material assigned
		materials.push_back(Material(device));
	}

	void vkglTF::Model::loadFromFile(const std::string& filename, VulkanDevice* device, VkQueue transferQueue, uint32_t fileLoadingFlags, float scale)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF gltfContext;

		size_t pos = filename.find_last_of('/');
		path = filename.substr(0, pos);

		std::string error, warning;

		this->device = device;

		bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);

		std::vector<uint32_t> indexBuffer;
		std::vector<Vertex> vertexBuffer;

		if (fileLoaded) {
			loadMaterials(gltfModel);
			const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
			for (size_t i = 0; i < scene.nodes.size(); i++) {
				const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
				loadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer, scale);
			}

			for (auto node : linearNodes) {
				// Initial pose
				if (node->mesh) {
					node->update();
				}
			}
		}
		else {
			std::cerr << "Could not load glTF file \"" + filename + "\": " + error + "\n";
			return;
		}

		// Pre-Calculations for requested features
		if ((fileLoadingFlags & FileLoadingFlags::PreTransformVertices) || (fileLoadingFlags & FileLoadingFlags::PreMultiplyVertexColors) || (fileLoadingFlags & FileLoadingFlags::FlipY)) {
			const bool preTransform = fileLoadingFlags & FileLoadingFlags::PreTransformVertices;
			const bool flipY = fileLoadingFlags & FileLoadingFlags::FlipY;
			for (Node* node : linearNodes) {
				if (node->mesh) {
					const glm::mat4 localMatrix = node->getMatrix();
					for (Primitive* primitive : node->mesh->primitives) {
						for (uint32_t i = 0; i < primitive->vertexCount; i++) {
							Vertex& vertex = vertexBuffer[primitive->firstVertex + i];
							// Pre-transform vertex positions by node-hierarchy
							if (preTransform) {
								vertex.pos = glm::vec3(localMatrix * glm::vec4(vertex.pos, 1.0f));
								vertex.normal = glm::normalize(glm::mat3(localMatrix) * vertex.normal);
							}
							// Flip Y-Axis of vertex positions
							if (flipY) {
								vertex.pos.y *= -1.0f;
								vertex.normal.y *= -1.0f;
							}
						}
					}
				}
			}
		}

		size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
		size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
		indices.count = static_cast<uint32_t>(indexBuffer.size());
		vertices.count = static_cast<uint32_t>(vertexBuffer.size());

		assert((vertexBufferSize > 0) && (indexBufferSize > 0));

		struct StagingBuffer {
			VkBuffer buffer;
			VkDeviceMemory memory;
		} vertexStaging, indexStaging;

		// Create staging buffers
		// Vertex data
		VK_CHECK_RESULT(device->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBufferSize,
			&vertexStaging.buffer,
			&vertexStaging.memory,
			vertexBuffer.data()));
		// Index data
		VK_CHECK_RESULT(device->createBuffer(
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			indexBufferSize,
			&indexStaging.buffer,
			&indexStaging.memory,
			indexBuffer.data()));

		// Create device local buffers
		// Vertex buffer
		VK_CHECK_RESULT(device->createBuffer(
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | memoryPropertyFlags,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBufferSize,
			&vertices.buffer,
			&vertices.memory));
		// Index buffer
		VK_CHECK_RESULT(device->createBuffer(
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | memoryPropertyFlags,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBufferSize,
			&indices.buffer,
			&indices.memory));

		// Copy from staging buffers
		VkCommandBuffer copyCmd = device->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkBufferCopy copyRegion = {};

		copyRegion.size = vertexBufferSize;
		vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, vertices.buffer, 1, &copyRegion);

		copyRegion.size = indexBufferSize;
		vkCmdCopyBuffer(copyCmd, indexStaging.buffer, indices.buffer, 1, &copyRegion);

		device->flushCommandBuffer(copyCmd, transferQueue, true);

		vkDestroyBuffer(device->logicalDevice, vertexStaging.buffer, nullptr);
		vkFreeMemory(device->logicalDevice, vertexStaging.memory, nullptr);
		vkDestroyBuffer(device->logicalDevice, indexStaging.buffer, nullptr);
		vkFreeMemory(device->logicalDevice, indexStaging.memory, nullptr);

		getSceneDimensions();

		// Setup descriptors
		uint32_t uboCount{ 0 };
		uint32_t imageCount{ 0 };
		for (auto node : linearNodes) {
			if (node->mesh) {
				uboCount++;
			}
		}
		for (const auto& material : materials) {
			if (material.baseColorTexture != nullptr) {
				imageCount++;
			}
		}
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uboCount },
		};
		if (imageCount > 0) {
			if (descriptorBindingFlags & DescriptorBindingFlags::ImageBaseColor) {
				poolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageCount });
			}
			if (descriptorBindingFlags & DescriptorBindingFlags::ImageNormalMap) {
				poolSizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, imageCount });
			}
		}
		VkDescriptorPoolCreateInfo descriptorPoolCI{};
		descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCI.pPoolSizes = poolSizes.data();
		descriptorPoolCI.maxSets = uboCount + imageCount;
		VK_CHECK_RESULT(vkCreateDescriptorPool(device->logicalDevice, &descriptorPoolCI, nullptr, &descriptorPool));

		// Descriptors for per-node uniform buffers
		{
			// Layout is global, so only create if it hasn't already been created before
			if (descriptorSetLayoutUbo == VK_NULL_HANDLE) {
				std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
					Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
				};
				VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
				descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
				descriptorLayoutCI.pBindings = setLayoutBindings.data();
				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->logicalDevice, &descriptorLayoutCI, nullptr, &descriptorSetLayoutUbo));
			}
			for (auto node : nodes) {
				prepareNodeDescriptor(node, descriptorSetLayoutUbo);
			}
		}

		// Descriptors for per-material images
		{
			// Layout is global, so only create if it hasn't already been created before
			if (descriptorSetLayoutImage == VK_NULL_HANDLE) {
				std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
				if (descriptorBindingFlags & DescriptorBindingFlags::ImageBaseColor) {
					setLayoutBindings.push_back(Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<uint32_t>(setLayoutBindings.size())));
				}
				if (descriptorBindingFlags & DescriptorBindingFlags::ImageNormalMap) {
					setLayoutBindings.push_back(Initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, static_cast<uint32_t>(setLayoutBindings.size())));
				}
				VkDescriptorSetLayoutCreateInfo descriptorLayoutCI{};
				descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorLayoutCI.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
				descriptorLayoutCI.pBindings = setLayoutBindings.data();
				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device->logicalDevice, &descriptorLayoutCI, nullptr, &descriptorSetLayoutImage));
			}
			for (auto& material : materials) {
				if (material.baseColorTexture != nullptr) {
					material.createDescriptorSet(descriptorPool, vkglTF::descriptorSetLayoutImage, descriptorBindingFlags);
				}
			}
		}
	}

	void vkglTF::Texture::destroy()
	{
		if (device)
		{
			vkDestroyImageView(device->logicalDevice, view, nullptr);
			vkDestroyImage(device->logicalDevice, image, nullptr);
			vkFreeMemory(device->logicalDevice, deviceMemory, nullptr);
			vkDestroySampler(device->logicalDevice, sampler, nullptr);
		}
	}


	void vkglTF::Model::bindBuffers(VkCommandBuffer commandBuffer)
	{
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
		buffersBound = true;
	}

	void vkglTF::Model::drawNode(Node* node, VkCommandBuffer commandBuffer, uint32_t renderFlags, VkPipelineLayout pipelineLayout, uint32_t bindImageSet)
	{
		if (node->mesh) {
			for (Primitive* primitive : node->mesh->primitives) {
				if (renderFlags & RenderFlags::BindImages) {
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, bindImageSet, 1, &primitive->material->descriptorSet, 0, nullptr);
				}
				vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
			}
		}
		for (const auto& child : node->children) {
			drawNode(child, commandBuffer, renderFlags, pipelineLayout, bindImageSet);
		}
	}

	void vkglTF::Model::draw(VkCommandBuffer commandBuffer, uint32_t renderFlags, VkPipelineLayout pipelineLayout, uint32_t bindImageSet)
	{
		if (!buffersBound) {
			const VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
		}
		for (auto& node : nodes) {
			drawNode(node, commandBuffer, renderFlags, pipelineLayout, bindImageSet);
		}
	}

	void vkglTF::Model::getNodeDimensions(Node* node, glm::vec3& min, glm::vec3& max)
	{
		if (node->mesh) {
			for (Primitive* primitive : node->mesh->primitives) {
				glm::vec4 locMin = glm::vec4(primitive->dimensions.min, 1.0f) * node->getMatrix();
				glm::vec4 locMax = glm::vec4(primitive->dimensions.max, 1.0f) * node->getMatrix();
				if (locMin.x < min.x) { min.x = locMin.x; }
				if (locMin.y < min.y) { min.y = locMin.y; }
				if (locMin.z < min.z) { min.z = locMin.z; }
				if (locMax.x > max.x) { max.x = locMax.x; }
				if (locMax.y > max.y) { max.y = locMax.y; }
				if (locMax.z > max.z) { max.z = locMax.z; }
			}
		}
		for (auto child : node->children) {
			getNodeDimensions(child, min, max);
		}
	}

	void vkglTF::Model::getSceneDimensions()
	{
		dimensions.min = glm::vec3(FLT_MAX);
		dimensions.max = glm::vec3(-FLT_MAX);
		for (auto node : nodes) {
			getNodeDimensions(node, dimensions.min, dimensions.max);
		}
		dimensions.size = dimensions.max - dimensions.min;
		dimensions.center = (dimensions.min + dimensions.max) / 2.0f;
		dimensions.radius = glm::distance(dimensions.min, dimensions.max) / 2.0f;
	}

	/*
		Helper functions
	*/
	vkglTF::Node* vkglTF::Model::findNode(Node* parent, uint32_t index) {
		Node* nodeFound = nullptr;
		if (parent->index == index) {
			return parent;
		}
		for (auto& child : parent->children) {
			nodeFound = findNode(child, index);
			if (nodeFound) {
				break;
			}
		}
		return nodeFound;
	}

	vkglTF::Node* vkglTF::Model::nodeFromIndex(uint32_t index) {
		Node* nodeFound = nullptr;
		for (auto& node : nodes) {
			nodeFound = findNode(node, index);
			if (nodeFound) {
				break;
			}
		}
		return nodeFound;
	}

	void vkglTF::Model::prepareNodeDescriptor(vkglTF::Node* node, VkDescriptorSetLayout descriptorSetLayout) {
		if (node->mesh) {
			VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
			descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocInfo.descriptorPool = descriptorPool;
			descriptorSetAllocInfo.pSetLayouts = &descriptorSetLayout;
			descriptorSetAllocInfo.descriptorSetCount = 1;
			VK_CHECK_RESULT(vkAllocateDescriptorSets(device->logicalDevice, &descriptorSetAllocInfo, &node->mesh->uniformBuffer.descriptorSet));

			VkWriteDescriptorSet writeDescriptorSet{};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSet.descriptorCount = 1;
			writeDescriptorSet.dstSet = node->mesh->uniformBuffer.descriptorSet;
			writeDescriptorSet.dstBinding = 0;
			writeDescriptorSet.pBufferInfo = &node->mesh->uniformBuffer.descriptor;

			vkUpdateDescriptorSets(device->logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
		}
		for (auto& child : node->children) {
			prepareNodeDescriptor(child, descriptorSetLayout);
		}
	}
}
