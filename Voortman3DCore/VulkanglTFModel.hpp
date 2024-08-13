/*
* Vulkan glTF model and texture loading class based on tinyglTF (https://github.com/syoyo/tinygltf)
*
* Copyright (C) 2018-2023 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

// GLTF loader based of Sacha Willems examples - Florent Kegler

#pragma once
#include "pch.hpp"

#include "VulkanDevice.hpp"
#include "Initializers.inl"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "tiny_gltf.h"

namespace Voortman3D {

	namespace vkglTF
	{
		enum DescriptorBindingFlags {
			ImageBaseColor = 0x00000001,
			ImageNormalMap = 0x00000002
		};

		extern VkDescriptorSetLayout descriptorSetLayoutImage;
		extern VkDescriptorSetLayout descriptorSetLayoutUbo;
		extern VkMemoryPropertyFlags memoryPropertyFlags;
		extern uint32_t descriptorBindingFlags;

		struct Node;

		struct Texture {
			VulkanDevice* device = nullptr;
			VkImage image{};
			VkImageLayout imageLayout{};
			VkDeviceMemory deviceMemory;
			VkImageView view;
			uint32_t width, height{};
			uint32_t mipLevels{};
			uint32_t layerCount{};
			VkDescriptorImageInfo descriptor;
			VkSampler sampler{VK_NULL_HANDLE};
			uint32_t index{};
			void destroy();

			~Texture() { destroy(); }
		};
		/*
			glTF material class
		*/
		struct Material {
			VulkanDevice* device{ nullptr };
			float alphaCutoff{ 1.0f };
			glm::vec4 baseColorFactor = glm::vec4(1.0f);
			vkglTF::Texture* baseColorTexture{ nullptr };

			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

			Material(VulkanDevice* device) : device(device) {};

			~Material() { delete baseColorTexture; }

			void createDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, uint32_t descriptorBindingFlags);
		};

		/*
			glTF primitive
		*/
		struct Primitive {
			uint32_t firstIndex;
			uint32_t indexCount;
			uint32_t firstVertex{};
			uint32_t vertexCount{};
			Material* material;

			struct Dimensions {
				glm::vec3 min = glm::vec3(FLT_MAX);
				glm::vec3 max = glm::vec3(-FLT_MAX);
				glm::vec3 size;
				glm::vec3 center;
				float radius{};
			} dimensions{};

			void setDimensions(glm::vec3 min, glm::vec3 max);
			Primitive(uint32_t firstIndex, uint32_t indexCount, Material* material) : firstIndex(firstIndex), indexCount(indexCount), material(material) {};
		};

		/*
			glTF mesh
		*/
		struct Mesh {
			VulkanDevice* device;

			std::vector<Primitive*> primitives;
			std::string name;

			struct UniformBuffer {
				VkBuffer buffer;
				VkDeviceMemory memory;
				VkDescriptorBufferInfo descriptor;
				VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
				void* mapped;
			} uniformBuffer;

			glm::mat4 matrix;

			Mesh(VulkanDevice* device, glm::mat4 matrix);
			~Mesh();
		};

		/*
			glTF node
		*/
		struct Node {
			// Parent is standard nullptr
			Node* parent{ nullptr };
			uint32_t index;
			std::vector<Node*> children;
			
			// Orientation matrix of node
			glm::mat4 matrix = glm::mat4(1.0f);
			std::string name;
			Mesh* mesh{nullptr};

			inline void Translate(glm::vec3 translation) {
				matrix = glm::translate(matrix, translation);
			}

			inline void Rotate(glm::quat rotation) {
				matrix = matrix * glm::mat4_cast(rotation);
			}

			inline void Scale(glm::vec3 scale) {
				matrix = glm::scale(matrix, scale);
			}

			_NODISCARD inline glm::mat4 getMatrix();
			void update();
			~Node();
		};

		/*
			glTF default vertex layout with easy Vulkan mapping functions
		*/
		enum class VertexComponent { Position, Normal};

		// GLTF supports more but these are the only things we use...
		struct Vertex {
			// Vertex colors are not used only basematerial color is used
			glm::vec3 pos;
			glm::vec3 normal;

			static VkVertexInputBindingDescription vertexInputBindingDescription;
			static std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
			static VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;

			_NODISCARD static constexpr VkVertexInputBindingDescription inputBindingDescription(uint32_t binding) noexcept;
			_NODISCARD static constexpr VkVertexInputAttributeDescription inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component) noexcept;
			_NODISCARD static constexpr std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent>& components) noexcept;

			/** @brief Returns the default pipeline vertex input state create info structure for the requested vertex components */
			_NODISCARD static VkPipelineVertexInputStateCreateInfo* getPipelineVertexInputState(const std::vector<VertexComponent>& components) noexcept;
		};

		enum FileLoadingFlags {
			None = 0x00000000,
			PreTransformVertices = 0x00000001,
			PreMultiplyVertexColors = 0x00000002,
			FlipY = 0x00000004,
			DontLoadImages = 0x00000008
		};

		enum RenderFlags {
			BindImages = 0x00000001,
			RenderOpaqueNodes = 0x00000002,
			RenderAlphaMaskedNodes = 0x00000004,
			RenderAlphaBlendedNodes = 0x00000008
		};

		/*
			glTF model loading and rendering class
		*/
		class Model {
		public:
			VulkanDevice* device{VK_NULL_HANDLE};
			VkDescriptorPool descriptorPool{VK_NULL_HANDLE};

			struct Vertices {
				int count;
				VkBuffer buffer;
				VkDeviceMemory memory;
			} vertices{};

			struct Indices {
				int count;
				VkBuffer buffer;
				VkDeviceMemory memory;
			} indices{};

			std::vector<Node*> nodes;
			std::vector<Node*> linearNodes; // just all the nodes listed in one big vector

			std::vector<Material> materials;
			struct Dimensions {
				glm::vec3 min = glm::vec3(FLT_MAX);
				glm::vec3 max = glm::vec3(-FLT_MAX);
				glm::vec3 size;
				glm::vec3 center;
				float radius;
			} dimensions{};

			bool buffersBound = false;
			std::string path;

			~Model();
			void loadNode(vkglTF::Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale);
			void loadMaterials(tinygltf::Model& gltfModel);
			void loadFromFile(const std::string& filename, VulkanDevice* device, VkQueue transferQueue, uint32_t fileLoadingFlags = vkglTF::FileLoadingFlags::None, float scale = 1.0f);
			void bindBuffers(VkCommandBuffer commandBuffer);
			void drawNode(Node* node, VkCommandBuffer commandBuffer, uint32_t renderFlags = 0, VkPipelineLayout pipelineLayout = VK_NULL_HANDLE, uint32_t bindImageSet = 1);
			void draw(VkCommandBuffer commandBuffer, uint32_t renderFlags = 0, VkPipelineLayout pipelineLayout = VK_NULL_HANDLE, uint32_t bindImageSet = 1);
			void getNodeDimensions(Node* node, glm::vec3& min, glm::vec3& max);
			void getSceneDimensions();

			template <typename T>
			void CopyToIndexBuffer(std::vector<uint32_t>& indexBuffer,
				const tinygltf::BufferView& bufferView,
				const tinygltf::Buffer& buffer,
				const tinygltf::Accessor& accessor,
				uint32_t vertexStart);

			_NODISCARD Node* findNode(Node* parent, uint32_t index);
			_NODISCARD Node* nodeFromIndex(uint32_t index);

			void prepareNodeDescriptor(vkglTF::Node* node, VkDescriptorSetLayout descriptorSetLayout);
		};
	}
}
