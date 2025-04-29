#pragma once
#include "MessageBus.h"
#include <memory>

// 模块接口
class IModule {
public:
    virtual ~IModule() = default;
    
    // 初始化模块
    virtual void initialize(std::shared_ptr<MessageBus> bus) = 0;
    
    // 启动模块
    virtual void start() = 0;
    
    // 停止模块
    virtual void stop() = 0;
    
    // 获取模块名称
    virtual std::string name() const = 0;
};

// 模块注册宏
#define REGISTER_MODULE(ModuleClass) \
    extern "C" __declspec(dllexport) IModule* createModule() { \
        return new ModuleClass(); \
    }