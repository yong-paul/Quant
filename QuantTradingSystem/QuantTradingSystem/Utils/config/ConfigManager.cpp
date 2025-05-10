#include "ConfigManager.h"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
//#include "../logger/Logger.h"

namespace {
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& configPath) {
    try {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            //Logger::error("ConfigManager", "Failed to open config file: {}", configPath);
            return false;
        }
        
        config_ = nlohmann::json::parse(file);
        currentConfigPath_ = configPath;
        return true;
    } catch (const std::exception& e) {
        //Logger::error("ConfigManager", "Failed to parse config file: {} - {}", configPath, e.what());
        return false;
    }
}

template<typename T>
T ConfigManager::getValue(const std::string& key, const T& defaultValue) const {
    try {
        auto paths = split(key, '.');
        auto current = config_;
        
        for (const auto& path : paths) {
            if (current.contains(path)) {
                current = current[path];
            } else {
                return defaultValue;
            }
        }
        
        return current.get<T>();
    } catch (const std::exception& e) {
        //Logger::error("ConfigManager", "Failed to get config value for key: {} - {}", key, e.what());
        return defaultValue;
    }
}

template<typename T>
void ConfigManager::setValue(const std::string& key, const T& value) {
    try {
        auto paths = split(key, '.');
        auto& current = config_;
        
        for (size_t i = 0; i < paths.size() - 1; ++i) {
            if (!current.contains(paths[i])) {
                current[paths[i]] = nlohmann::json::object();
            }
            current = current[paths[i]];
        }
        
        current[paths.back()] = value;
    } catch (const std::exception& e) {
        //Logger::error("ConfigManager", "Failed to set config value for key: {} - {}", key, e.what());
    }
}

bool ConfigManager::saveConfig(const std::string& configPath) {
    try {
        std::string savePath = configPath.empty() ? currentConfigPath_ : configPath;
        std::ofstream file(savePath);
        if (!file.is_open()) {
            //Logger::error("ConfigManager", "Failed to open config file for writing: {}", savePath);
            return false;
        }
        
        file << std::setw(4) << config_;
        return true;
    } catch (const std::exception& e) {
        //Logger::error("ConfigManager", "Failed to save config: {}", e.what());
        return false;
    }
}

bool ConfigManager::reloadConfig() {
    if (currentConfigPath_.empty()) {
        //Logger::error("ConfigManager", "No config file has been loaded yet");
        return false;
    }
    return loadConfig(currentConfigPath_);
}

// 显式实例化常用类型
template int ConfigManager::getValue<int>(const std::string&, const int&) const;
template double ConfigManager::getValue<double>(const std::string&, const double&) const;
template std::string ConfigManager::getValue<std::string>(const std::string&, const std::string&) const;
template bool ConfigManager::getValue<bool>(const std::string&, const bool&) const;

template void ConfigManager::setValue<int>(const std::string&, const int&);
template void ConfigManager::setValue<double>(const std::string&, const double&);
template void ConfigManager::setValue<std::string>(const std::string&, const std::string&);
template void ConfigManager::setValue<bool>(const std::string&, const bool&);