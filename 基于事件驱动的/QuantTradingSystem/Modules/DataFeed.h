#pragma once
#include <string>
#include <functional>
#include "../Events/Event.h"

// 数据源接口类
class DataFeed {
public:
    virtual ~DataFeed() = default;
    

    // 创建数据源实例
    virtual std::shared_ptr<DataFeed> createDataFeed() = 0;
    // 初始化数据源
    virtual void init(const std::string& configPath) = 0;
    
    // 启动数据接收
    virtual void start() = 0;
    
    // 停止数据接收
    virtual void stop() = 0;
    
    // 设置原始数据回调
    void setRawDataCallback(std::function<void(const std::string&)> callback) {
        rawDataCallback = callback;
    }
    
protected:
    std::function<void(const std::string&)> rawDataCallback;
};

