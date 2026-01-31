#pragma once 
namespace engine {
#ifdef _DEBUG
	const bool VERBOSE = true;
#else
	const bool VERBOSE = false;
#endif

#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

}