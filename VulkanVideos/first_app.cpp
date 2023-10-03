#include "first_app.hpp"
#include <stdexcept>
#include "lve_camera.hpp"
#include "lve_gui.hpp"
#include "model.hpp"
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

//#include "lve_texture.hpp"


// General settings.




namespace lve {
	


	FirstApp::FirstApp() {
		_globalPool = LveDescriptorPool::Builder(lveDevice).setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT).build();
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
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<LveBuffer>(
				lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}
		auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).addBinding(1,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT).build();
		//this global set, will be the master globalset, for all our shader, Single Master Render System, we can derive to


		LveTexture tex = LveTexture(lveDevice, "models/bunny/bunny_diffuse.png");

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.sampler = tex.getSampler();
		imageInfo.imageView = tex.getImageView();
		imageInfo.imageLayout = tex.getImageLayout();


		std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LveDescriptorWriter(*globalSetLayout, *_globalPool)
				.writeBuffer(0, &bufferInfo).writeImage(1,&imageInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ lveDevice,lveRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout() };
		PointLighRenderSystem pointLightSystem{ lveDevice,lveRenderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout() };
        LveCamera camera{};
        //camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f,-2.f,2.f), glm::vec3(0.f, 0.f, 2.5f));
        //        camera.setPerspectiveProjection(glm::radians(50.f),aspect,0.1f,10.f)
        auto viewerObject = LveGameObject::createGameObject();//store camera state
		viewerObject.transform.translation.z = -2.5f;


        KeyBoardMovementController cameraController{};
        auto currentTime = std::chrono::high_resolution_clock::now();

		LveImgui lveImgui{
	  _window,
	  lveDevice,
	  lveRenderer.getSwapChainRenderPass(),
	  lveRenderer.getImageCount() };


		while (!_window.shouldClose()) {//tant que la window doit pas close 
			glfwPollEvents();//process any window event 

		



            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;
           // frameTime = glm::min(frameTime, MAX_FRAME_TIME);
            cameraController.moveInPLaneXZ(_window.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
             camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
               if (auto commandBuffer = lveRenderer.beginFrame()) {//begin fram return nullptr if swapchain need to be recreated
				   
				   lveImgui.newFrame();



				   int frameIndex = lveRenderer.getFrameIndex();
				   FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera,globalDescriptorSets[frameIndex],gameObjects };




				   GlobalUbo ubo{};
				   ubo.projection = camera.getProjection();
					ubo.view=camera.getView();
					pointLightSystem.update(frameInfo, ubo);
					ubo.inverseView = camera.getInverseView();
				   uboBuffers[frameIndex]->writeToBuffer(&ubo);
				   uboBuffers[frameIndex]->flush();



			//begin frame and begin swap chain,  are differe,t -> want app dont main control the fonctionnality, multiple render pass for reflection shadow etc ... 
				   lveRenderer.beginSwapChainRenderPass(commandBuffer);

				   //render first solid object 
				   simpleRenderSystem.renderGameObjects(frameInfo);

				   pointLightSystem.render(frameInfo);


				   lveImgui.runExample();

				   // as last step in render pass, record the imgui draw commands
				   lveImgui.render(commandBuffer);



				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();

			}
		}
		vkDeviceWaitIdle(lveDevice.device());//block the stop, cpu will block until gpu is completed
	}

/*	void FirstApp::loadGameOjects()
	{
		std::vector<LveModel::Vertex> vertices{
			{{0.0f,-0.5f},{1.0f,0.0f,0.0f}},
			{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
			{{-0.5f,0.5f},{0.0f,0.0f,1.0f}}
		};

		auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);//one model instance use by mutiple game object, will stay in memory while using


		auto triangle = LveGameObject::createGameObject();
		triangle.model = lveModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform.translation.x = .2f;
		triangle.transform.scale = { 2.f, .5f };
		triangle.transform.rotation = .25f * glm::two_pi<float>();


		gameObjects.push_back(std::move(triangle));//move => move emory instaed of copy
	}*/

    void FirstApp::loadGameObjects() {
	

		/*std::shared_ptr<LveModel> lveModel =
			LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
		auto flatVase = LveGameObject::createGameObject();
		flatVase.model = lveModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.getId(),std::move(flatVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
		auto smoothVase = LveGameObject::createGameObject();
		smoothVase.model = lveModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(smoothVase.getId(),std::move(smoothVase));

		lveModel = LveModel::createModelFromFile(lveDevice, "models/sponza/sponza.obj");
		auto floor = LveGameObject::createGameObject();
		floor.model = lveModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = { 0.02f, 0.02f, 0.02f };
		gameObjects.emplace(floor.getId(),std::move(floor));*/

		auto _myModel = Model(lveDevice, "bunny", "models/bunny/bunny_2.obj");
		auto test = LveGameObject::createGameObject();
		test._model = std::make_shared<Model>(_myModel);
		test.transform.translation = { .5f, .5f, 0.f };
		test.transform.scale = { 2.f, 2.f, 2.f };
		test.transform.rotation = { 0.f,0.f,glm::pi<float>() };
		gameObjects.emplace(test.getId(), std::move(test));

		{
			auto pointLight = LveGameObject::makePointLight(0.2f);
			gameObjects.emplace(pointLight.getId(), std::move(pointLight)); // can't use pointLight again bc move, so we can limit the scope by nested block => limiting the scope
		}


		std::vector<glm::vec3> lightColors{
	 {1.f, .1f, .1f},
	 {.1f, .1f, 1.f},
	 {.1f, 1.f, .1f},
	 {1.f, 1.f, .1f},
	 {.1f, 1.f, 1.f},
	 {1.f, 1.f, 1.f}  
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = LveGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate( //
				glm::mat4(1.f),//indeity matrix
				(i * glm::two_pi<float>()) / lightColors.size(),// circle equal size of slice, rotate each point incremently to create a ring of light
				{ 0.f, -1.f, 0.f });//up vector
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
    }

	
}