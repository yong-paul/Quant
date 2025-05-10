#pragma once
#include <string>
#include <memory>
#include <chrono>

// 事件类型枚举
enum class EventType {
    MARKET_DATA,    // 行情数据事件
    ORDER,          // 订单事件
    TRADE,          // 成交事件
    POSITION,       // 持仓事件
    ACCOUNT,        // 账户事件
    RISK_CONTROL,   // 风控事件
    STRATEGY_SIGNAL,// 策略信号事件
    SYSTEM          // 系统事件
};

// 事件基类
class Event {
public:
    Event(EventType type) 
        : type_(type), 
          timestamp_(std::chrono::system_clock::now()) {}
    
    virtual ~Event() = default;
    
    EventType getType() const { return type_; }
    
    // 获取事件时间戳
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    // 获取事件描述
    virtual std::string toString() const = 0;
    
private:
    EventType type_;
    std::chrono::system_clock::time_point timestamp_;
};

// 事件指针类型
typedef std::shared_ptr<Event> EventPtr;