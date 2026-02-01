#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

#include <Vulkan/Device.hpp>
#include <Engine/Objects/Model.hpp>
#include <Engine/Objects/TriangleMesh.hpp>

namespace engine {

	struct PipelineConfigInfo {
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		PipelineConfigInfo() = default;

		VkPipelineViewportStateCreateInfo viewportInfo;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributDescriptions{};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;// validation layer wil ug if nullptr to layout
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline {

	public:
		Pipeline(const std::string& vertFilePath, const std::string fragFilePath, const PipelineConfigInfo& configInfo);
		~Pipeline();

		Pipeline(const Pipeline&) = delete; // delete copy constructor
		Pipeline& operator=(const Pipeline&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed
		void bind(VkCommandBuffer commandBuffer);
		static void defaultFowardPipelineConfigInfo(PipelineConfigInfo& configInfo);
		static void defaultDeferredPipelineConfigInfo(PipelineConfigInfo& configInfo);

		static void enableAlphaBlinding(
			PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filePath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);


		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}