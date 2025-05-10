#pragma once
#include "Event.h"
#include <string>
#include <sstream>

// 账户数据结构
struct AccountData {
    std::string accountId;      // 账户ID
    double balance;             // 账户余额
    double available;           // 可用资金
    double margin;              // 保证金
    double commission;          // 手续费
    double closePnL;            // 平仓盈亏
    double positionPnL;         // 持仓盈亏
    double totalPnL;            // 总盈亏
    std::string updateTime;     // 更新时间
};

// 账户事件
class AccountEvent : public Event {
public:
    AccountEvent(const AccountData& data)
        : Event(EventType::ACCOUNT), data_(data) {}
    
    const AccountData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "AccountEvent: " << data_.accountId 
           << " Balance: " << data_.balance
           << " Available: " << data_.available
           << " Margin: " << data_.margin
           << " TotalPnL: " << data_.totalPnL;
        return ss.str();
    }
    
private:
    AccountData data_;
}; 