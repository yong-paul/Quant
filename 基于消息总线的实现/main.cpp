#include "MessageBus.h"
#include "IModule.h"
#include <iostream>
#include <vector>
#include <windows.h>

// 模块加载器
class ModuleLoader {
    std::vector<std::unique_ptr<IModule>> modules;
    
public:
    void loadModule(const std::string& dllPath) {
        HMODULE handle = LoadLibraryA(dllPath.c_str());
        if (!handle) {
            std::cerr << "无法加载模块: " << dllPath << std::endl;
            return;
        }
        
        auto createFunc = (IModule*(*)())GetProcAddress(handle, "createModule");
        if (!createFunc) {
            std::cerr << "无效的模块: " << dllPath << std::endl;
            FreeLibrary(handle);
            return;
        }
        
        modules.emplace_back(createFunc());
    }
    
    void initializeAll(std::shared_ptr<MessageBus> bus) {
        for (auto& module : modules) {
            module->initialize(bus);
        }
    }
    
    void startAll() {
        for (auto& module : modules) {
            module->start();
        }
    }
    
    void stopAll() {
        for (auto& module : modules) {
            module->stop();
        }
    }
};

int main() {
    auto bus = MessageBus::create();
    ModuleLoader loader;
    
    // 加载模块
    loader.loadModule("MarketDataModule.dll");
    loader.loadModule("StrategyEngine.dll");
    loader.loadModule("RiskControlModule.dll");
    loader.loadModule("TradeModule.dll");
    
    // 初始化和启动
    loader.initializeAll(bus);
    loader.startAll();
    
    // 创建事件对象用于系统退出
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    // 订阅系统关闭消息
    bus->subscribe("SystemControl", [&](const Message& msg) {
        const auto& scMsg = static_cast<const SystemControlMessage&>(msg);
        if (scMsg.controlType == SystemControlMessage::SHUTDOWN) {
            SetEvent(hEvent);
        }
    });
    
    std::cout << "系统运行中...等待关闭信号" << std::endl;
    
    // 事件驱动主循环
    WaitForSingleObject(hEvent, INFINITE);
    
    // 停止
    loader.stopAll();
    CloseHandle(hEvent);
    return 0;
}