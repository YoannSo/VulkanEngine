#pragma once

#include <string>
#include <vector>
#include "lve_device.hpp"
namespace lve {

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
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = nullptr;// validation layer wil ug if nullptr to layout
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};
 
	class LvePipeline {

	public:
		LvePipeline(LveDevice &device,const std::string& vertFilePath, const std::string fragFilePath,const PipelineConfigInfo& configInfo);
		~LvePipeline();

		LvePipeline(const LvePipeline&) = delete; // delete copy constructor
		LvePipeline& operator=(const LvePipeline&) = delete; // and copy operator, make sure to not have 2 pointer to window, then if we destroy one, the second isnt destroyed
		void bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(
      PipelineConfigInfo& configInfo);

		static void enableAlphaBlinding(
			PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filePath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath,const PipelineConfigInfo& configInfo);
		
		void createShaderModule(const std::vector<char> &code, VkShaderModule* shaderModule);


		LveDevice& lveDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}