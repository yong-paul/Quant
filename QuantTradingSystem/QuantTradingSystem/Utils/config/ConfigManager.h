#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <vector>
#include <nlohmann/json.hpp>

namespace QuantTrading {

// 配置变更监听器接口
class IConfigListener {
public:
    virtual ~IConfigListener() = default;
    virtual void onConfigChanged(const std::string& key, const nlohmann::json& newValue) = 0;
};

class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    // 初始化配置管理器
    bool init(const std::string& configDir = "config");

    // 加载配置文件
    bool loadConfig(const std::string& configFile);

    // 重新加载配置文件
    bool reloadConfig(const std::string& configFile);

    // 获取配置值
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T()) const;

    // 设置配置值
    template<typename T>
    void setValue(const std::string& key, const T& value);

    // 注册配置变更监听器
    void registerListener(const std::string& key, std::shared_ptr<IConfigListener> listener);

    // 注销配置变更监听器
    void unregisterListener(const std::string& key, std::shared_ptr<IConfigListener> listener);

    // 保存配置到文件
    bool saveConfig(const std::string& configFile);

    // 获取所有配置
    nlohmann::json getAllConfig() const;

    // 检查配置是否存在
    bool hasConfig(const std::string& key) const;

private:
    ConfigManager();
    ~ConfigManager();

    // 禁止拷贝和赋值
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // 通知配置变更
    void notifyConfigChanged(const std::string& key, const nlohmann::json& newValue);

    // 解析配置键
    std::vector<std::string> parseConfigKey(const std::string& key) const;

    // 成员变量
    std::string configDir_;
    nlohmann::json configData_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IConfigListener>>> listeners_;
    mutable std::mutex mutex_;
};

// 模板函数实现
template<typename T>
T ConfigManager::getValue(const std::string& key, const T& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        auto keys = parseConfigKey(key);
        nlohmann::json current = configData_;
        
        for (const auto& k : keys) {
            if (current.contains(k)) {
                current = current[k];
            } else {
                return defaultValue;
            }
        }
        
        return current.get<T>();
    } catch (const std::exception&) {
        return defaultValue;
    }
}

template<typename T>
void ConfigManager::setValue(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        auto keys = parseConfigKey(key);
        nlohmann::json* current = &configData_;
        
        for (size_t i = 0; i < keys.size() - 1; ++i) {
            if (!current->contains(keys[i])) {
                (*current)[keys[i]] = nlohmann::json::object();
            }
            current = &(*current)[keys[i]];
        }
        
        (*current)[keys.back()] = value;
        notifyConfigChanged(key, value);
    } catch (const std::exception& e) {
        // 记录错误日志
    }
}

} // namespace QuantTrading 