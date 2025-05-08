#include "CTPQuoteDataApi.h"
#include "jsonparser/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// �����ļ��ṹ
struct AppConfig {
	std::string appName;
	int windowWidth;
	int windowHeight;
	bool fullscreen;
	std::vector<std::string> recentFiles;
	double volumeLevel;
};

// ��JSON�ļ���������
AppConfig loadConfig(const std::string& filePath) {
	AppConfig config;

	try {
		// ��ȡJSON�ļ�
		std::ifstream configFile(filePath);
		if (!configFile.is_open()) {
			throw std::runtime_error("Could not open config file: " + filePath);
		}

		// ����JSON
		json j;
		configFile >> j;

		// ��ȡ������
		config.appName = j.value("appName", "MyApp"); // ��Ĭ��ֵ
		config.windowWidth = j["window"]["width"];
		config.windowHeight = j["window"]["height"];
		config.fullscreen = j.value("fullscreen", false);
		config.recentFiles = j["recentFiles"].get<std::vector<std::string>>();
		config.volumeLevel = j.value("volume", 0.8); // Ĭ������0.8

	}
	catch (const json::exception& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
		// ��������������Ĭ��ֵ���׳��쳣
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
