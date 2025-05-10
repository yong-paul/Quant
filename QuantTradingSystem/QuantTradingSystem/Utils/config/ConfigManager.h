#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include "../jsonparser/json.hpp"

class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    // 加载配置文件
    bool loadConfig(const std::string& configPath);
    
    // 获取配置项
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T()) const;
    
    // 设置配置项
    template<typename T>
    void setValue(const std::string& key, const T& value);
    
    // 保存配置到文件
    bool saveConfig(const std::string& configPath = "");
    
    // 重新加载配置
    bool reloadConfig();
    
private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    nlohmann::json config_;
    std::string currentConfigPath_;
}; 