#pragma once
#include "Event.h"
#include <string>
#include <sstream>

// 订单方向
enum class OrderDirection {
    BUY,     // 买入
    SELL     // 卖出
};

// 订单类型
enum class OrderType {
    LIMIT,   // 限价单
    MARKET,  // 市价单
    FAK,     // FAK单
    FOK      // FOK单
};

// 订单状态
enum class OrderStatus {
    SUBMITTED,    // 已提交
    ACCEPTED,     // 已接受
    REJECTED,     // 已拒绝
    PARTIAL,      // 部分成交
    FILLED,       // 全部成交
    CANCELLED,    // 已撤销
    EXPIRED       // 已过期
};

// 订单结构
struct OrderData {
    std::string orderId;        // 订单编号
    std::string symbol;         // 合约代码
    OrderDirection direction;   // 方向
    OrderType type;             // 类型
    OrderStatus status;         // 状态
    double price;               // 价格
    int volume;                 // 数量
    int tradedVolume;           // 已成交数量
    std::string strategyId;     // 策略ID
    std::string createTime;     // 创建时间
    std::string updateTime;     // 更新时间
};

// 订单事件
class OrderEvent : public Event {
public:
    OrderEvent(const OrderData& data)
        : Event(EventType::ORDER), data_(data) {}
    
    const OrderData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "OrderEvent: " << data_.orderId 
           << " Symbol: " << data_.symbol
           << " Direction: " << (data_.direction == OrderDirection::BUY ? "Buy" : "Sell")
           << " Price: " << data_.price
           << " Volume: " << data_.volume
           << " Status: ";
        
        switch (data_.status) {
            case OrderStatus::SUBMITTED: ss << "Submitted"; break;
            case OrderStatus::ACCEPTED: ss << "Accepted"; break;
            case OrderStatus::REJECTED: ss << "Rejected"; break;
            case OrderStatus::PARTIAL: ss << "Partial"; break;
            case OrderStatus::FILLED: ss << "Filled"; break;
            case OrderStatus::CANCELLED: ss << "Cancelled"; break;
            case OrderStatus::EXPIRED: ss << "Expired"; break;
        }
        
        return ss.str();
    }
    
private:
    OrderData data_;
};