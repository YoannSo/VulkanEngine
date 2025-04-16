#include "first_app.hpp"
#include <stdexcept>
#include "lve_camera.hpp"
#include "model.hpp"
#include "PointLight.h"
#include <array>
#include "simple_render_system.hpp"
#include "point_light_system.hpp"
#include <glm/gtc/constants.hpp>
#include "keyboard_movement_controller.hpp"
#include <chrono>
#define GLM_FORCE_RADIANS//force use radian 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //depth value to 0 to 1
#include <glm/glm.hpp>
#include "lve_buffer.hpp"

#include <fstream>
#include <stdexcept>
#include "lve_frame_info.hpp"
#include "simple_render_system.hpp"
#include <iostream>
#include <cstdlib>

//#include "lve_texture.hpp"


// General settings.




namespace lve {
	

	LveDevice* LveDevice::s_device = nullptr;


	FirstApp::FirstApp() {

		system("E:/Prog/VulkanEngine/VulkanVideos/compile_shader.bat");

				_globalPool =
			LveDescriptorPool::Builder()
			.setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		SceneManager::getInstance();

		loadGameObjects();

	}
	FirstApp::~FirstApp() {
	}
#ifdef IMGUI_VULKAN_DEBUG_REPORT
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
	{
		(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
		fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
		return VK_FALSE;
	}
#endif // IMGUI_VULKAN_DEBUG_REPORT

	void FirstApp::run() {

	

		std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (auto& uboBuffer : uboBuffers)
		{
			uboBuffer = std::make_unique<LveBuffer>(
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffer->map();
		}

		auto globalSetLayout = LveDescriptorSetLayout::Builder().addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();
		//this global set, will be the master globalset, for all our shader, Single Master Render System, we can derive to

		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *_globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}
	

        LveCamera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f,-2.f,2.f), glm::vec3(0.f, 0.f, 2.5f));
        //       camera.setPerspectiveProjection(glm::radians(50.f),aspect,0.1f,10.f)


        Camera* viewerObject = SceneManager::getInstance()->createCameraObject();//store camera state
		viewerObject->transform.translation.z = -2.5f;

		SimpleRenderSystem simpleRenderSystem{ lveRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout() };
		PointLighRenderSystem pointLightSystem{ lveRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout() };


        KeyBoardMovementController cameraController{};
        auto currentTime = std::chrono::high_resolution_clock::now();


		while (!_window.shouldClose()) {//tant que la window doit pas close 
			glfwPollEvents();//process any window event 


            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;


           // frameTime = glm::min(frameTime, MAX_FRAME_TIME);
            cameraController.moveInPLaneXZ(_window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject->transform.translation, viewerObject->transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
             camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			 SceneManager::getInstance()->updateAllGameObject(frameTime);
               if (auto commandBuffer = lveRenderer.beginFrame()) {//begin fram return nullptr if swapchain need to be recreated
				  

				   int frameIndex = lveRenderer.getFrameIndex();

				   FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera,globalDescriptorSets[frameIndex] };

				   GlobalUbo ubo{};
				   ubo.projection = camera.getProjection();
					ubo.view=camera.getView();
					pointLightSystem.update(frameInfo, ubo);
					ubo.inverseView = camera.getInverseView();

					ubo.numLights = SceneManager::getInstance()->getLightMap().size();
					for (size_t i = 0; i < SceneManager::getInstance()->getLightMap().size() && i < MAX_LIGHTS; ++i) {
						ubo.pointsLights[i].position = glm::vec4(SceneManager::getInstance()->getLightMap()[i]->transform.translation, 1.0f);
						ubo.pointsLights[i].color = glm::vec4(SceneManager::getInstance()->getLightMap()[i]->getColor(), SceneManager::getInstance()->getLightMap()[i]->getIntensity());
					}

				   uboBuffers[frameIndex]->writeToBuffer(&ubo);
				   uboBuffers[frameIndex]->flush();



			//begin frame and begin swap chain,  are differe,t -> want app dont main control the fonctionnality, multiple render pass for reflection shadow etc ... 
				   lveRenderer.beginSwapChainRenderPass(commandBuffer);

				   //render first solid object 
				   simpleRenderSystem.renderGameObjects(frameInfo);

				   pointLightSystem.render(frameInfo);



				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();

			}
		}
		vkDeviceWaitIdle(LveDevice::getInstance()->getDevice());//block the stop, cpu will block until gpu is completed
	}

	void FirstApp::loadGameObjects() {
		SceneManager::getInstance()->addTextureElement("E:/Prog/VulkanEngine/VulkanVideos/models/debugTex.png", new LveTexture("E:/Prog/VulkanEngine/VulkanVideos/models/debugTex.png"));



 /* std::shared_ptr<Model> lveModel = std::make_shared<Model>(lveDevice, "Seahawk", "models/Helicopter/Seahawk.obj");


  auto flatVase = LveGameObject::createGameObject();
  flatVase._model = lveModel;
  flatVase.transform.translation = {-.5f, .5f, 0.f};
  flatVase.transform.scale = {3.f, 1.5f, 3.f};
  gameObjects.emplace(flatVase.getId(), std::move(flatVase));*/

 /* std::shared_ptr<Model>  lveModel = std::make_shared<Model>(lveDevice, "bunny", "models/bunny/bunny_2.obj");
  auto smoothVase = LveGameObject::createGameObject();
  smoothVase._model = lveModel;
  smoothVase.transform.translation = {.5f, .5f, 0.f};
  smoothVase.transform.scale = {3.f, 1.5f, 3.f};
  gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));*/

  //for (std::string test : smoothVase._model->m_allTexturesName)
	//  std::cout << test << std::endl;

 // LveTexture* myText = new LveTexture(lveDevice, "E:/Prog/VulkanEngine/VulkanVideos/models/debugTex.png");
	/*auto bunny = SceneManager::getInstance()->createMeshObject("bunny", "models/bunny/bunny_2.obj");
	bunny->transform.translation = { 0.f, .5f, 0.f };
	bunny->transform.scale = { 3.f, 1.f, 3.f };*/

		Model* floor = SceneManager::getInstance()->createModelObject("bunny", "models/quad.obj");
		floor->transform.translation = { 0.f, 1.5f, 0.f };
		floor->transform.scale = { 3.f,  3.f, 3.f }; 

	

		Model* bunny2 = SceneManager::getInstance()->createModelObject("Helicopter", "models/Helicopter/Seahawk.obj");
		bunny2->transform.translation = { 0.f,0.0f, 0.f };
		bunny2->transform.scale = { 0.1f, 0.1f, 0.1f };
		bunny2->transform.rotation = { 0.f,  glm::pi<float>(),  glm::pi<float>() };

		//bunny2->setUpdateFunction([](GameObject* gameObject,float deltaTime) {
		//	gameObject->transform.rotate(glm::vec3(0.f,1.f,0.f),deltaTime);
		//	});




		/*LveGameObject* bunny = SceneManager::getInstance()->createMeshObject("bunny", "models/bunny/bunny_2.obj");
	bunny->transform.translation = { 0.f, 0.f, 0.f };
	bunny->transform.scale = { 3.f, 1.f, 3.f };*/
	std::vector<glm::vec3> lightColors{
		  {1.f, .1f, .1f},
		{.1f, .1f, 1.f},
		{.1f, 1.f, .1f},
		{1.f, 1.f, .1f},
		{.1f, 1.f, 1.f},
		 {1.f, 1.f, 1.f}  //
	};

	;


  for (int i = 0; i < lightColors.size(); i++) {
	 PointLight* light = SceneManager::getInstance()->createLightObject();
	 light->setColor(lightColors[i]);
		auto rotateLight = glm::rotate(
        glm::mat4(1.f),
        (i * glm::two_pi<float>()) / lightColors.size(),
        {0.f, -1.f, 0.f});
		light->transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, 1.f, -1.f, 1.f));
		light->transform.scale = { 0.2f, 0.2f, 0.2f };

		light->setUpdateFunction([](GameObject* gameObject, float deltaTime) {
			gameObject->transform.rotate(glm::vec3(0.f, 1.f, 0.f), deltaTime);
			});
	
  }

  SceneManager::getInstance()->setMaterialDescriptorSet();
  SceneManager::getInstance()->setupRenderingBatch();

}

	
}