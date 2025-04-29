#pragma once
#include "../IModule.h"
#include "../MessageBus.h"
#include <memory>

// 交易模块
class TradeModule : public IModule {
    std::shared_ptr<MessageBus> bus;
    
public:
    void initialize(std::shared_ptr<MessageBus> bus) override {
        this->bus = bus;
    }
    
    void start() override {
        // 启动订单管理
    }
    
    void stop() override {
        // 停止订单管理
    }
    
    std::string name() const override {
        return "TradeModule";
    }
    
    // 订单管理
    void manageOrder(const std::string& symbol, double price, int quantity);
};

REGISTER_MODULE(TradeModule)