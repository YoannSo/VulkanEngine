#include "GuiManager.h"
#if LVF_USE_IMGUI
// Loader helper for ImGui Vulkan to resolve function pointers through vkGetInstanceProcAddr
static PFN_vkVoidFunction ImGuiVulkanLoader(const char* function_name, void* user_data) {
  VkInstance instance = reinterpret_cast<VkInstance>(user_data);
  return vkGetInstanceProcAddr(instance, function_name);
}
#endif

GuiManager::~GuiManager() { cleanup(); }

void GuiManager::init(GLFWwindow* window,
                      VkInstance instance,
                      VkDevice device,
                      VkPhysicalDevice physicalDevice,
                      uint32_t graphicsQueueFamily,
                      VkQueue graphicsQueue,
                      VkRenderPass renderPass,
                      VkDescriptorPool descriptorPool,
                      uint32_t minImageCount) {
  if (m_initialized) {
    std::cerr << "GuiManager already initialized\n";
    return;
  }
  m_window = window;
  m_instance = instance;
  m_device = device;
  m_physicalDevice = physicalDevice;
  m_graphicsQueue = graphicsQueue;
  m_graphicsQueueFamily = graphicsQueueFamily;
  m_renderPass = renderPass;
  m_descriptorPool = descriptorPool;
  m_minImageCount = minImageCount;


  // 1: create descriptor pool for IMGUI
	//  the size of the pool is very oversize, but it's copied from imgui demo
	//  itself.
  VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
	  { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	  { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
	  { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
	  { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
	  { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
	  { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
	  { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
	  { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
	  { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
	  { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000;
  pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

  VkDescriptorPool imguiPool;

  if (vkCreateDescriptorPool(m_device, &pool_info, nullptr, &imguiPool) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create descriptor pool for imgui");
  }
  // 2: initialize imgui library

  // this initializes the core structures of imgui
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // this initializes imgui for GLFW
  ImGui_ImplGlfw_InitForVulkan(m_window, true);
  // this initializes imgui for Vulkan
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = m_instance;
  init_info.PhysicalDevice = m_physicalDevice;
  init_info.Device = m_device;
  init_info.Queue = m_graphicsQueue;
  init_info.DescriptorPool = imguiPool;
  init_info.MinImageCount = m_minImageCount > 0 ? m_minImageCount : 2;
  init_info.ImageCount = m_minImageCount > 0 ? m_minImageCount : 2;
  // Do not use dynamic rendering unless the instance/device expose VK_KHR_dynamic_rendering
  // and function pointers are loaded. Keep dynamic rendering disabled for broad compatibility.
  init_info.UseDynamicRendering = false;

  init_info.PipelineInfoMain.RenderPass = m_renderPass;
  init_info.PipelineInfoMain.Subpass = 0;
  init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  //dynamic rendering parameters for imgui to use
 // init_info.PipelineRenderingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
  //init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
  //init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &m_swap;


  //init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

#if LVF_USE_IMGUI
  // Ensure ImGui backend has Vulkan function pointers (esp. for optional KHR functions)
  ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_2, ImGuiVulkanLoader, reinterpret_cast<void*>(m_instance));
  init_info.ApiVersion = VK_API_VERSION_1_2;
  ImGui_ImplVulkan_Init(&init_info);
  // store pool for cleanup
  m_imguiPool = imguiPool;
#else
  (void)imguiPool; // suppress unused
  ImGui_ImplVulkan_Init(&init_info);
#endif

  // NOTE: font texture upload requires recording a command buffer and calling
  // ImGui_ImplVulkan_CreateFontsTexture(command_buffer) followed by
  // ImGui_ImplVulkan_DestroyFontUploadObjects(). Do this in your setup where a
  // temp command buffer is available. Skipping here to keep this init safe.

  // add the destroy the imgui created structures
 // _mainDeletionQueue.push_function([=]() {
//	  ImGui_ImplVulkan_Shutdown();
//	  vkDestroyDescriptorPool(_device, imguiPool, nullptr);
//	  });

  // Currently this is a stub. Real ImGui initialization will go here.
  std::cout << "GuiManager initialized (stub).\n";
  m_initialized = true;
}

void GuiManager::newFrame() {
  if (!m_initialized) return;
#if LVF_USE_IMGUI
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
#endif
}
void GuiManager::setupTopMenuBar() {

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit"))
            {
                // TODO: signaler à ton app de quitter
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            //    ImGui::MenuItem("Wireframe", nullptr, &m_showWireframe);
             //   ImGui::MenuItem("Debug Window", nullptr, &m_showDebugWindow);
            ImGui::EndMenu();
        }

        // Spacer vers la droite
        ImGui::SameLine(ImGui::GetWindowWidth() - 150.0f);

        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::EndMainMenuBar();
    }
}

void GuiManager::render(VkCommandBuffer commandBuffer) {
  if (!m_initialized) return;
#if LVF_USE_IMGUI
  setupTopMenuBar();
  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();
  if (draw_data) {
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
  }
#endif
}

void GuiManager::cleanup() {
  if (!m_initialized) return;
#if LVF_USE_IMGUI
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  if (m_imguiPool != VK_NULL_HANDLE) {
    vkDestroyDescriptorPool(m_device, m_imguiPool, nullptr);
    m_imguiPool = VK_NULL_HANDLE;
  }
#endif
  std::cout << "GuiManager cleaned up.\n";
  m_initialized = false;
}

void GuiManager::recreate(VkRenderPass renderPass) {
  if (!m_initialized) return;
  m_renderPass = renderPass;
  // Stub: handle recreating any ImGui resources that depend on render pass
}

