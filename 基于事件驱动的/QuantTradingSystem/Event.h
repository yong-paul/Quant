#pragma once
#include <string>
#include <memory>

// 事件类型枚举
enum class EventType {
    MARKET_DATA,    // 行情数据事件
    ORDER,          // 订单事件
    TRADE,          // 成交事件
    RISK,           // 风控事件
    STRATEGY        // 策略事件
};

// 事件基类
class Event {
public:
    Event(EventType type) : type_(type) {}
    virtual ~Event() = default;
    
    EventType getType() const { return type_; }
    
    // 获取事件描述
    virtual std::string toString() const = 0;
    
private:
    EventType type_;
};

// 事件指针类型
typedef std::shared_ptr<Event> EventPtr;