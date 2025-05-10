#pragma once
#include "Event.h"
#include "../MarketData/MarketDataField.h"
#include <string>
#include <vector>
#include <sstream>

// 行情数据事件
class MarketDataEvent : public Event {
public:
    MarketDataEvent(const MarketDataField& data)
        : Event(EventType::MARKET_DATA), data_(data) {}
    
    const MarketDataField& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "MarketDataEvent: " << data_.symbol 
           << " Exchange: " << data_.exchange
           << " Price: " << data_.lastPrice 
           << " Volume: " << data_.volume
           << " Time: " << data_.updateTime;
        return ss.str();
    }
    
private:
    MarketDataField data_;
};