#pragma once
#include "Event.h"
#include "OrderEvent.h"
#include <string>
#include <sstream>

// 持仓方向
enum class PositionDirection {
    LONG,   // 多头
    SHORT   // 空头
};

// 持仓数据结构
struct PositionData {
    std::string symbol;           // 合约代码
    PositionDirection direction;  // 持仓方向
    int position;                 // 持仓量
    int ydPosition;               // 昨仓
    int tdPosition;               // 今仓
    double avgPrice;              // 持仓均价
    double openCost;              // 开仓成本
    double positionProfit;        // 持仓盈亏
    std::string strategyId;       // 策略ID
    std::string updateTime;       // 更新时间
};

// 持仓事件
class PositionEvent : public Event {
public:
    PositionEvent(const PositionData& data)
        : Event(EventType::POSITION), data_(data) {}
    
    const PositionData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "PositionEvent: " << data_.symbol 
           << " Direction: " << (data_.direction == PositionDirection::LONG ? "Long" : "Short")
           << " Position: " << data_.position
           << " AvgPrice: " << data_.avgPrice
           << " Profit: " << data_.positionProfit;
        return ss.str();
    }
    
private:
    PositionData data_;
}; 