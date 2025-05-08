#include "CTPQuoteDataApi.h"
#include "jsonparser/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// 配置文件结构
struct AppConfig {
	std::string appName;
	int windowWidth;
	int windowHeight;
	bool fullscreen;
	std::vector<std::string> recentFiles;
	double volumeLevel;
};

// 从JSON文件加载配置
AppConfig loadConfig(const std::string& filePath) {
	AppConfig config;

	try {
		// 读取JSON文件
		std::ifstream configFile(filePath);
		if (!configFile.is_open()) {
			throw std::runtime_error("Could not open config file: " + filePath);
		}

		// 解析JSON
		json j;
		configFile >> j;

		// 提取配置项
		config.appName = j.value("appName", "MyApp"); // 带默认值
		config.windowWidth = j["window"]["width"];
		config.windowHeight = j["window"]["height"];
		config.fullscreen = j.value("fullscreen", false);
		config.recentFiles = j["recentFiles"].get<std::vector<std::string>>();
		config.volumeLevel = j.value("volume", 0.8); // 默认音量0.8

	}
	catch (const json::exception& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
		// 可以在这里设置默认值或抛出异常
	}
	catch (const std::exception& e) {
		std::cerr << "Config loading error: " << e.what() << std::endl;
	}

	return config;
}

bool CTPQuoteDataApi::Init(const std::string configfile)
{
	if (configfile.empty())
	{
		return false;
	}
	else
	{
		auto xxx = configfile;

	}
	CThostFtdcMdApi* m_ctoapi = CThostFtdcMdApi::CreateFtdcMdApi();
	m_ctoapi->RegisterSpi(this);
	//throw std::logic_error("The method or operation is not implemented.");
}

void CTPQuoteDataApi::Release()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool CTPQuoteDataApi::Connect()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool CTPQuoteDataApi::Disconnect()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool CTPQuoteDataApi::IsConnected()
{
	throw std::logic_error("The method or operation is not implemented.");
}
