#pragma once
#include "../IModule.h"
#include "../MessageBus.h"
#include <memory>

// 策略信号消息
struct StrategySignal : public Message {
    std::string strategyName;
    std::string symbol;
    std::string action; // BUY/SELL
    double price;
    int quantity;
    
    StrategySignal() : Message{"StrategySignal"} {}
};

// 策略引擎模块
class StrategyEngine : public IModule {
    std::shared_ptr<MessageBus> bus;
    
public:
    void initialize(std::shared_ptr<MessageBus> bus) override {
        this->bus = bus;
    }
    
    void start() override {
        // 策略引擎实现
    }
    
    void stop() override {
        // 停止策略引擎
    }
    
    std::string name() const override {
        return "StrategyEngine";
    }
};

REGISTER_MODULE(StrategyEngine)