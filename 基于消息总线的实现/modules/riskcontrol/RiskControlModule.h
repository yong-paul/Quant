#pragma once
#include "../IModule.h"
#include "../MessageBus.h"
#include <memory>

// 风险控制模块
class RiskControlModule : public IModule {
    std::shared_ptr<MessageBus> bus;
    
public:
    void initialize(std::shared_ptr<MessageBus> bus) override {
        this->bus = bus;
    }
    
    void start() override {
        // 启动风控检查
    }
    
    void stop() override {
        // 停止风控检查
    }
    
    std::string name() const override {
        return "RiskControlModule";
    }
    
    // 风控规则检查
    bool checkRisk(const std::string& symbol, double price, int quantity);
};

REGISTER_MODULE(RiskControlModule)