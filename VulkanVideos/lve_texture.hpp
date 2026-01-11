#pragma once

#include <vulkan/vulkan.h>
#include "lve_device.hpp"
namespace lve {
	class LveTexture {


	public:
		LveTexture( const std::string& filepath);
		LveTexture( const std::string& filepath, const std::string p_type);
		LveTexture(const LveTexture& p_lveTexture, const std::string p_type);
		~LveTexture();


		inline std::string getPath() { return _path; };
		inline std::string getType() { return _type; };

		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

		

		bool operator==(LveTexture other)const {
			return _path == other._path;
		}

		VkSampler getSampler() { return _vkSampler; }
		VkImageView getImageView() { return _vkImageView; }
		VkImageLayout getImageLayout() { return _vkImageLayout; }
		VkDescriptorImageInfo getDescriptorImageInfo()const;

	private:
		void generateMipMaps();

		uint32_t _id;
		static uint32_t  _nextAvailbleId;
		std::string _path;

		std::string _type;
		int _width, _height,_mipLevels;
		VkImage _vkImage;
		VkDeviceMemory _vkDeviceMem;
		VkImageView _vkImageView;
		VkSampler _vkSampler;
		VkFormat _vkFormat;
		VkImageLayout _vkImageLayout;

		LveDevice* m_deviceRef;

			
	};
}