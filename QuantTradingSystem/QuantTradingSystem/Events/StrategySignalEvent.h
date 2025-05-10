#pragma once
#include "Event.h"
#include "OrderEvent.h"
#include <string>
#include <sstream>

// 信号类型
enum class SignalType {
    OPEN_LONG,     // 开多
    CLOSE_LONG,    // 平多
    OPEN_SHORT,    // 开空
    CLOSE_SHORT    // 平空
};

// 策略信号数据结构
struct StrategySignalData {
    std::string strategyId;     // 策略ID
    std::string symbol;         // 合约代码
    SignalType signalType;      // 信号类型
    double price;               // 信号价格
    int volume;                 // 信号数量
    double stopLoss;            // 止损价
    double takeProfit;          // 止盈价
    std::string signalTime;     // 信号时间
    std::string comment;        // 信号备注
};

// 策略信号事件
class StrategySignalEvent : public Event {
public:
    StrategySignalEvent(const StrategySignalData& data)
        : Event(EventType::STRATEGY_SIGNAL), data_(data) {}
    
    const StrategySignalData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "StrategySignalEvent: " << data_.strategyId 
           << " Symbol: " << data_.symbol
           << " Signal: ";
        
        switch (data_.signalType) {
            case SignalType::OPEN_LONG: ss << "OpenLong"; break;
            case SignalType::CLOSE_LONG: ss << "CloseLong"; break;
            case SignalType::OPEN_SHORT: ss << "OpenShort"; break;
            case SignalType::CLOSE_SHORT: ss << "CloseShort"; break;
        }
        
        ss << " Price: " << data_.price
           << " Volume: " << data_.volume
           << " Time: " << data_.signalTime;
        
        return ss.str();
    }
    
private:
    StrategySignalData data_;
}; 