#pragma once

#include <string>
namespace lve {

	class LvePipeline {

	public:
		LvePipeline(const std::string& vertFilePath, const std::string fragPilePath);
	private:
		static std::vector<char> readFile(const std::string& filePath);
		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
	};
}