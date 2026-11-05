#pragma once
#include <array>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <Vulkan/Device.hpp>
namespace engine {
	class Texture {
	public:
		Texture( const std::string& filepath);
		Texture( const std::string& filepath, const std::string p_type);
		Texture(const Texture& p_Texture, const std::string p_type);
		Texture(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, bool isDepth = false, uint32_t mipLevels = 1, bool compareEnable = false, bool p_isCubemap = false);

		~Texture();

		inline std::string getPath() { return _path; };
		inline std::string getType() { return _type; };

		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

		

		bool operator==(Texture other)const {
			return _path == other._path;
		}

		VkSampler getSampler() { return _vkSampler; }
		VkImageView getImageView() { return _vkImageView; }
		VkImageLayout getImageLayout() { return _vkImageLayout; }
		VkDescriptorImageInfo getDescriptorImageInfo()const;
        VkImageView getCubemapImageView(uint32_t index) const
        {
            if (index >= 6 && m_isCubemap)
            {
                throw std::out_of_range("Cubemap index must be between 0 and 5.");
            }
            return m_cubemapImageViews[index];
        }

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

        bool m_isCubemap{false};
        std::array<VkImageView, 6> m_cubemapImageViews;

		Device* m_deviceRef;

			
	};
}