#pragma once
#include "MessageBus.h"

// 策略信号消息
struct StrategySignal : public Message {
    std::string symbol;
    std::string signal; // BUY/SELL/HOLD
    double price;
    int quantity;
    
    StrategySignal() : Message{"StrategySignal"} {}
};