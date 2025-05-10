#pragma once
#include "Event.h"
#include "OrderEvent.h"
#include <string>
#include <sstream>

// 成交结构
struct TradeData {
    std::string tradeId;        // 成交编号
    std::string orderId;        // 关联订单ID
    std::string symbol;         // 合约代码
    OrderDirection direction;   // 方向
    double price;               // 成交价格
    int volume;                 // 成交数量
    double commission;          // 手续费
    std::string strategyId;     // 策略ID
    std::string tradeTime;      // 成交时间
};

// 成交事件
class TradeEvent : public Event {
public:
    TradeEvent(const TradeData& data)
        : Event(EventType::TRADE), data_(data) {}
    
    const TradeData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "TradeEvent: " << data_.tradeId 
           << " OrderId: " << data_.orderId
           << " Symbol: " << data_.symbol
           << " Direction: " << (data_.direction == OrderDirection::BUY ? "Buy" : "Sell")
           << " Price: " << data_.price
           << " Volume: " << data_.volume
           << " Time: " << data_.tradeTime;
        return ss.str();
    }
    
private:
    TradeData data_;
};