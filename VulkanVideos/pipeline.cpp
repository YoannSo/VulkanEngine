#include "pipeline.hpp"
#include "lve_model.hpp"
#include "TriangleMesh.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
namespace lve {
	LvePipeline::LvePipeline(const std::string& vertFilePath, const std::string fragFilePath,const PipelineConfigInfo& configInfo)
	{
		createGraphicsPipeline(vertFilePath, fragFilePath,configInfo);
	}

	LvePipeline::~LvePipeline() {
		vkDestroyShaderModule(LveDevice::getInstance()->getDevice(), vertShaderModule, nullptr);//clean up frag and vertex module
		vkDestroyShaderModule(LveDevice::getInstance()->getDevice(), fragShaderModule, nullptr);

		vkDestroyPipeline(LveDevice::getInstance()->getDevice(), graphicsPipeline, nullptr);//destroy our pipelien
	}
	void LvePipeline::bind(VkCommandBuffer commandBuffer)//bind the pipeline with the command buffer
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);//flag=>signal graphics pipeline 
	}
	void LvePipeline::defaultPipelineConfigInfo(
    PipelineConfigInfo& configInfo) {
 
  configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
 
  ///was not dynamic 
 /* //viewport describe the transformation between pipeline output and target image gl=>output image
  configInfo.viewport.x = 0.0f;
  configInfo.viewport.y = 0.0f;
  configInfo.viewport.width = static_cast<float>(width);
  configInfo.viewport.height = static_cast<float>(height);

  //depth, 
  configInfo.viewport.minDepth = 0.0f;
  configInfo.viewport.maxDepth = 1.0f;
 
  //scissor kind of like the viewport, every pixel outside this, will be discarbed 
  configInfo.scissor.offset = {0, 0};
  configInfo.scissor.extent = {width, height};*/
 

  configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  configInfo.viewportInfo.viewportCount = 1;
  configInfo.viewportInfo.pViewports = nullptr;
  configInfo.viewportInfo.scissorCount = 1;
  configInfo.viewportInfo.pScissors =nullptr;




  //setup the rasterizer state
  configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;//will clamp to 1 max and -1 min, but not good for true, because too far is far not close
  configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;//discard all primitive before tasterization, when you only wnana see the first stage of the pipeline 
  configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;//triangle ? just corner ? filled ? 
  configInfo.rasterizationInfo.lineWidth = 1.0f;
  configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; //discard triangle by their facing ? front to camera ? back ?
  configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // which is facing ? 
  configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE; //alter deapth value, by ading value
  configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
  configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
  configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional


 //multisampling, alising, if we use the pixel cneter to color  probleme of alisiang, multesampling in the pixel for the color
  configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
  configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
  configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
  configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
  configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
 

  //how we combined combine color in the frame buffer line 2 triangle overlapping in the fragment 
  configInfo.colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
      VK_COLOR_COMPONENT_A_BIT;
  configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;   // Optional
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;  // Optional
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
 
  configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
  configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
  configInfo.colorBlendInfo.attachmentCount = 1;
  configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
  configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
  configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
  configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
  configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
 

  //depth buffer aditionnal attanchement to fram buffer , store value of the depth for every pixel, 
  //depth buffer of the fra
  configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
  configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
  configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.front = {};  // Optional
  configInfo.depthStencilInfo.back = {};   // Optional

  //configure the pipeline to expect dynamic pipeleine and viewport
    configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
  configInfo.dynamicStateInfo.dynamicStateCount =
      static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
  configInfo.dynamicStateInfo.flags = 0;



  configInfo.bindingDescriptions = Vertex::getBindingDescriptions();
  configInfo.attributDescriptions = Vertex::getAttributeDescription();
 
}
void LvePipeline::enableAlphaBlinding(PipelineConfigInfo& configInfo)
{

	configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
	configInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;   // Optional
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;  // Optional
	configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
	configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

}
	std::vector<char> LvePipeline::readFile(const std::string& filePath) {

		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file :" + filePath);
		}
		size_t fileSize = static_cast<size_t>(file.tellg());//because ate file, we alreayd at the end, so tellG=last position = size
		std::vector<char> buffer(fileSize);
		file.seekg(0);//start of the file
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
	void LvePipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath,const PipelineConfigInfo& configInfo)
	{
		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "cannot create graphics pipeline: no pipelineLayout provided in configInfo");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "cannot create graphics pipeline: no renderPass provided in configInfo");
		auto vertCode = readFile(vertFilePath);
		auto fragCode = readFile(fragFilePath);

		//first create shader module,
		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2]; // o,ne for vertex and one for frag
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT; //vertex ? or frag ?
		shaderStages[0].module = vertShaderModule; // the moduel associed
		shaderStages[0].pName = "main"; // the function name
		shaderStages[0].flags = 0; // no flag
		shaderStages[0].pNext = nullptr;//
		shaderStages[0].pSpecializationInfo = nullptr;//customize shader functionnality

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT; //vertex ? or frag ?
		shaderStages[1].module = fragShaderModule; // the moduel associed
		shaderStages[1].pName = "main"; // the function name
		shaderStages[1].flags = 0; // no flag
		shaderStages[1].pNext = nullptr;//
		shaderStages[1].pSpecializationInfo = nullptr;//customize shader functionnality

		auto& bindingDecriptions =configInfo.bindingDescriptions;
		auto& attributeDecriptions = configInfo.attributDescriptions;


		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};// struct to describe how to interpret the vertex buffer data,
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; 
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDecriptions.size()); //for now vertex info hard coded in the sahder
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDecriptions.size());
		 vertexInputInfo.pVertexAttributeDescriptions = attributeDecriptions.data();
  vertexInputInfo.pVertexBindingDescriptions = bindingDecriptions.data();
		
  /*		VkPipelineViewportStateCreateInfo viewportInfo{};
		  //combine scissor and viewport  into vewport state create info, can be use multiple viewport and scisor
  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount = 1;
  viewportInfo.pViewports = &configInfo.viewport;
  viewportInfo.scissorCount = 1;
  viewportInfo.pScissors = &configInfo.scissor;*/


		VkGraphicsPipelineCreateInfo pipelineInfo{}; // all the configuration to create the pipeline

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;//how many programmble stage are we using, (2 for use now)
		pipelineInfo.pStages = shaderStages; //
		pipelineInfo.pVertexInputState = &vertexInputInfo; 
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		  pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;

		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;//optionnal to configure pipeline fonctionnalty, viewport dynamiclty ?


		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;//can be optimize performance
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


		if (vkCreateGraphicsPipelines(LveDevice::getInstance()->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)//null handle to pipeline cache => performance, pipeline count =>1, no allocation callback 
			throw	std::runtime_error("Failed to create graphics pipeline");

		std::cout << "Vertex shader code size " << vertCode.size() << std::endl;
		std::cout << "Vertex shader code size " << vertCode.size() << std::endl;
	}
	void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};// common pattern, configure a struct instrad of a function
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;//
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*> (code.data());//vulkan expect uint32 instrad of char, sotre in vector, default allocator already insure

		if (vkCreateShaderModule(LveDevice::getInstance()->getDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module");
	}
}