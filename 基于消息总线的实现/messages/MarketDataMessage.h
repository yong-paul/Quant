#pragma once
#include "MessageBus.h"

// 行情数据消息
struct MarketDataMessage : public Message {
    std::string symbol;
    double price;
    int volume;
    
    MarketDataMessage() : Message{"MarketData"} {}
};