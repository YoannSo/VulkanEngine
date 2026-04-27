#include <Engine/App/App.hpp>
namespace engine {


	Device* Device::s_device = nullptr;


	FirstApp::FirstApp() {

		system("C:/prog/git/VulkanEngine/VulkanVideos/compile_shader.bat");

		_globalPool =
			DescriptorPool::Builder()
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();


		/*m_guiManager = std::make_unique<GuiManager>();
		m_guiManager->init(
			_window.getGLFWWindow(),
			Device::getInstance()->getVkInstance(),
			Device::getInstance()->getDevice(),
			Device::getInstance()->getPhysicalDevice(),
			0,
			Device::getInstance()->getGraphicsQueue(),
			Renderer.getRenderPass(),
			nullptr,
			SwapChain::MAX_FRAMES_IN_FLIGHT
		);*/
		loadGameObjects();

	}
	FirstApp::~FirstApp() {
	}

	void FirstApp::run() {


		//changer le traitement des descriptorset 



		Camera* viewerObject = SceneManager::getInstance()->createCameraObject();//store camera state
		viewerObject->transform.translation.z = -2.5f;
		viewerObject->setViewTarget(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.0f));
		viewerObject->setPerspectiveProjection(glm::radians(50.f),  Renderer.getAspectRatio(), 0.5f, 30.f);

		Camera* lightCamera = SceneManager::getInstance()->createCameraObject();
		lightCamera->transform.translation = { -1.f, 2.f, 2.f };
		lightCamera->setViewTarget(glm::vec3(10.f, 10.f, 0.f), glm::vec3(0.f, 1.f, 0.0f));
		lightCamera->setOrthographicProjection(-10.f, 10.f, -10.f, 10.f, 0.5f, 30.f);


		KeyBoardMovementController cameraController{};
		auto currentTime = std::chrono::high_resolution_clock::now();


		while (!_window.shouldClose()) {//tant que la window doit pas close 
			glfwPollEvents();//process any window event 
		//	m_guiManager->newFrame();


			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;




			// frameTime = glm::min(frameTime, MAX_FRAME_TIME);
			cameraController.moveInPLaneXZ(_window.getGLFWWindow(), frameTime, viewerObject);
			viewerObject->setViewYXZ(viewerObject->transform.translation, viewerObject->transform.rotation);



			//std::cout << "Camera position: " << viewerObject->transform.translation.x << ", "<< viewerObject->transform.translation.y << ", "<< viewerObject->transform.translation.z << std::endl;

			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);

			SceneManager::getInstance()->updateAllGameObject(frameTime);
			// measure update CPU time
			auto updateEndTime = std::chrono::high_resolution_clock::now();
			static std::chrono::high_resolution_clock::time_point updateStartTime = updateEndTime; // placeholder

			// record start of update was measured earlier (frameTime calc), but measure actual update cost
			// For simplicity, record updateStartTime before update call and compute here
			// (we approximate here by using previous points)

			if (auto commandBuffer = Renderer.beginFrame()) {//begin fram return nullptr if swapchain need to be recreated


				int frameIndex = Renderer.getFrameIndex();

				// Reset queries for this frame and write GPU timestamp start

				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, *viewerObject };
				cameraController.handleAction(_window.getGLFWWindow());

				SceneManager::GlobalUbo ubo{};
				ubo.projection = viewerObject->getProjection();
				ubo.view = viewerObject->getView();
				ubo.lightSpace = lightCamera->getProjection() * lightCamera->getView();
				//Renderer.updateRenderSystems(frameInfo, ubo);
				ubo.inverseView = viewerObject->getInverseView();

				SceneManager::getInstance()->writeInGlobalUbo(ubo, frameIndex);

				Renderer.render(frameInfo);
			//	m_guiManager->render(commandBuffer);

				// write GPU timestamp end for this frame



				Renderer.endFrame();

			}
		}
		vkDeviceWaitIdle(Device::getInstance()->getDevice());//block the stop, cpu will block until gpu is completed
	}
	void FirstApp::loadGameObjects() {
	//	SceneManager::getInstance()->addTextureElement("C:/prog/git/VulkanEngine/VulkanVideos/models/debugTex.png", new Texture("C:/prog/git/VulkanEngine/VulkanVideos/models/debugTex.png"));


		/* std::shared_ptr<Model> lveModel = std::make_shared<Model>(Device, "Seahawk", "models/Helicopter/Seahawk.obj");


		 auto flatVase = LveGameObject::createGameObject();
		 flatVase._model = lveModel;
		 flatVase.transform.translation = {-.5f, .5f, 0.f};
		 flatVase.transform.scale = {3.f, 1.5f, 3.f};
		 gameObjects.emplace(flatVase.getId(), std::move(flatVase));*/

		 /* std::shared_ptr<Model>  lveModel = std::make_shared<Model>(Device, "bunny", "models/bunny/bunny_2.obj");
		  auto smoothVase = LveGameObject::createGameObject();
		  smoothVase._model = lveModel;
		  smoothVase.transform.translation = {.5f, .5f, 0.f};
		  smoothVase.transform.scale = {3.f, 1.5f, 3.f};
		  gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));*/

		  //for (std::string test : smoothVase._model->m_allTexturesName)
			//  std::cout << test << std::endl;

		 // Texture* myText = new Texture(Device, "E:/Prog/VulkanEngine/VulkanVideos/models/debugTex.png");
			/*auto bunny = SceneManager::getInstance()->createMeshObject("bunny", "models/bunny/bunny_2.obj");
			bunny->transform.translation = { 0.f, .5f, 0.f };
			bunny->transform.scale = { 3.f, 1.f, 3.f };*/

		
		
		/*Model* floor = SceneManager::getInstance()->createModelObject("sponza", "models/sponza/sponza.obj");
		floor->transform.translation = { 2.f, 0.f, 0.f };
		floor->transform.rotation = { 0.f, 0.f, glm::pi<float>() };
		floor->transform.scale = { 0.01f,  0.01f, 0.01f };*/

		



		SceneManager::getInstance()->getMaterialManager().createBasicMaterial("red", glm::vec4(1.0f, 0.f, 0.f, 1.f));
		SceneManager::getInstance()->getMaterialManager().createBasicMaterial("green", glm::vec4(0.f, 1.f, 0.f, 1.f));
		SceneManager::getInstance()->getMaterialManager().createBasicMaterial("blue", glm::vec4(0.f, 0.f, 1.f, 1.f));


		Model* icosphere = SceneManager::getInstance()->createModelObject("icosphere", "models/basics/models/icosphere/icosphere.obj", true);
		icosphere->transform.translation = { -3.f, 2.f, 0.f };
		icosphere->transform.scale = { 1.f, 1.f,1.f };
		icosphere->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("red"));

		Model* sphere = SceneManager::getInstance()->createModelObject("sphere", "models/basics/models/uvsphere/uvsphere.obj", true);
		sphere->transform.translation = { 0.f, 2.f, 0.f };
		sphere->transform.scale = { 1.f, 1.f,1.f };
		sphere->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("red"));


		Model* cube = SceneManager::getInstance()->createModelObject("cube", "models/basics/models/cube/cube.obj", true);
		cube->transform.translation = { 3.f, 2.f, 0.f };
		cube->transform.scale = { 1.f, 1.f,1.f };
		cube->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("box"));



		Model* cubeX = SceneManager::getInstance()->createModelObject("cubeX", "models/basics/models/cube/cube.obj", true);
		cubeX->transform.translation = { 1.f, 0.f, 0.f };
		cubeX->transform.scale = { 1.f, 0.1f,0.1f };
		cubeX->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("red"));

		Model* cubeY = SceneManager::getInstance()->createModelObject("cubeX", "models/basics/models/cube/cube.obj", true);
		cubeY->transform.translation = { 0.f, 0.f, 1.f};
		cubeY->transform.scale = { 0.1f, 0.1f,1.f };
		cubeY->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("blue"));

		Model* cubeZ = SceneManager::getInstance()->createModelObject("cubeX", "models/basics/models/cube/cube.obj", true);
		cubeZ->transform.translation = { 0.f, 1.f, 0.f };
		cubeZ->transform.scale = { 0.1f, 1.f,0.1f };
		cubeZ->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("green"));



		Model* plane = SceneManager::getInstance()->createModelObject("plane", "models/basics/models/plane/plane.obj", true);
		//plane->transform.rotation = { glm::pi<float>(), 0.f, 0.f };
		plane->transform.translation = { 3.f, -0.f, 0.f };
		plane->transform.scale = { 10.f, 1.0,10.f };
		plane->overrideMaterial(SceneManager::getInstance()->getMaterialManager().getIDOfMaterial("bricks1"));


		//cube->transform.rotation = { glm::half_pi<float>(), 0.f, 0.f };



			/*Model * bunny2 = SceneManager::getInstance()->createModelObject("Helicopter", "models/Helicopter/Seahawk.obj");
			bunny2->transform.translation = { 0.f,0.0f, 0.f };
			bunny2->transform.scale = { 0.1f, 0.1f, 0.1f };
			bunny2->transform.rotation = { 0.f,  glm::pi<float>(),  glm::pi<float>() };
			*/
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


		/*for (int i = 0; i < lightColors.size(); i++) {
			PointLight* light = SceneManager::getInstance()->createLightObject();
			light->setColor(lightColors[i]);
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, 1.f, 0.f });

			// Increase the Y component (height) to make the light higher  
			light->transform.translation = glm::vec3(rotateLight * glm::vec4(4.f, -3.f, -2.f, 1.f));
			light->transform.scale = { 0.2f, 0.2f, 0.2f };

			light->setUpdateFunction([](GameObject* gameObject, float deltaTime) {
				gameObject->transform.rotate(glm::vec3(0.f, 1.f, 0.f), deltaTime);
				});
		}*/

		//SceneManager::getInstance()->getLightManager().addDirectionalLight({ 0.0,-1.f,0.f }, { 1.f,1.f,1.f }, 1.f);


		SceneManager::getInstance()->getLightManager().addPointLight({ 3.f, 3.0f, 3.f }, { 1.f,1.f,1.f }, 20.f);

		Model* cubeLight = SceneManager::getInstance()->createModelObject("light1", "models/basics/models/cube/cube.obj", true);
		cubeLight->transform.translation = { 3.f, 3.0f, 3.f };
		cubeLight->transform.scale = { 0.2f, 0.2f,0.2f };

		/*
		SceneManager::getInstance()->getLightManager().addPointLight({-3.f, 3.0f, -3.f}, {0.f,0.f,1.f}, 20.f);
		Model* cubeLight2 = SceneManager::getInstance()->createModelObject("light2", "models/basics/models/cube/cube.obj", true);
		cubeLight2->transform.translation = { -3.f, 3.0f, -3.f };
		cubeLight2->transform.scale = { 0.2f, 0.2f,0.2f };

		*/
		//SceneManager::getInstance()->getBillboardManager().addBillboard({ 0.f, 0.f, 0.f }, 0.5f, { 1.f, 0.f, 0.f, 1.f }, 0);


		SceneManager::getInstance()->initializeDescriptor();

		Renderer.createRenderStages();

	}


}