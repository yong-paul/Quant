#pragma once
#include "Event.h"
#include <string>
#include <vector>

// 行情数据事件
class MarketDataEvent : public Event {
public:
    MarketDataEvent(const std::string& symbol, double price, int volume)
        : Event(EventType::MARKET_DATA), symbol_(symbol), price_(price), volume_(volume) {}
    
    std::string getSymbol() const { return symbol_; }
    double getPrice() const { return price_; }
    int getVolume() const { return volume_; }
    
    std::string toString() const override {
        return "MarketDataEvent: " + symbol_ + " Price: " + std::to_string(price_) + 
               " Volume: " + std::to_string(volume_);
    }
    
private:
    std::string symbol_;
    double price_;
    int volume_;
};