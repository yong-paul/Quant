#include "ConfigManager.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>

namespace QuantTrading {

ConfigManager::ConfigManager() {
}

ConfigManager::~ConfigManager() {
}

bool ConfigManager::init(const std::string& configDir) {
    std::lock_guard<std::mutex> lock(mutex_);
    configDir_ = configDir;
    
    // 创建配置目录（如果不存在）
    if (!std::filesystem::exists(configDir_)) {
        try {
            std::filesystem::create_directories(configDir_);
        } catch (const std::exception&) {
            return false;
        }
    }
    
    return true;
}

bool ConfigManager::loadConfig(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string fullPath = configDir_ + "/" + configFile;
    
    try {
        std::ifstream file(fullPath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json newConfig;
        file >> newConfig;
        
        // 合并新配置
        configData_.merge_patch(newConfig);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigManager::reloadConfig(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string fullPath = configDir_ + "/" + configFile;
    
    try {
        std::ifstream file(fullPath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json newConfig;
        file >> newConfig;
        
        // 更新配置并通知变更
        for (const auto& [key, value] : newConfig.items()) {
            if (configData_[key] != value) {
                configData_[key] = value;
                notifyConfigChanged(key, value);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void ConfigManager::registerListener(const std::string& key, 
                                   std::shared_ptr<IConfigListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_[key].push_back(listener);
}

void ConfigManager::unregisterListener(const std::string& key, 
                                     std::shared_ptr<IConfigListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = listeners_.find(key);
    if (it != listeners_.end()) {
        auto& listeners = it->second;
        listeners.erase(
            std::remove(listeners.begin(), listeners.end(), listener),
            listeners.end()
        );
    }
}

bool ConfigManager::saveConfig(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string fullPath = configDir_ + "/" + configFile;
    
    try {
        std::ofstream file(fullPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(4) << configData_;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

nlohmann::json ConfigManager::getAllConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return configData_;
}

bool ConfigManager::hasConfig(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto keys = parseConfigKey(key);
    nlohmann::json current = configData_;
    
    for (const auto& k : keys) {
        if (!current.contains(k)) {
            return false;
        }
        current = current[k];
    }
    
    return true;
}

void ConfigManager::notifyConfigChanged(const std::string& key, 
                                      const nlohmann::json& newValue) {
    auto it = listeners_.find(key);
    if (it != listeners_.end()) {
        for (const auto& listener : it->second) {
            listener->onConfigChanged(key, newValue);
        }
    }
}

std::vector<std::string> ConfigManager::parseConfigKey(const std::string& key) const {
    std::vector<std::string> result;
    std::stringstream ss(key);
    std::string item;
    
    while (std::getline(ss, item, '.')) {
        result.push_back(item);
    }
    
    return result;
}

} // namespace QuantTrading