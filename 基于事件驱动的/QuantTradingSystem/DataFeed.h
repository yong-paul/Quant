#pragma once
#include <memory>
#include <functional>

class DataFeed {
public:
    virtual ~DataFeed() = default;
    
    // 设置原始数据回调
    void setRawDataCallback(std::function<void(const std::string&)> callback) {
        rawDataCallback = callback;
    }
    
    // 启动数据源
    virtual void start() = 0;
    
protected:
    std::function<void(const std::string&)> rawDataCallback;
};

// 创建数据源实例
std::shared_ptr<DataFeed> createDataFeed();