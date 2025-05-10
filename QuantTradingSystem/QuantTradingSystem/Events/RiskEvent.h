#pragma once
#include "Event.h"
#include <string>
#include <sstream>

// 风控级别
enum class RiskLevel {
    INFO,     // 信息
    WARNING,  // 警告
    ERROR,    // 错误
    CRITICAL  // 严重错误
};

// 风控类型
enum class RiskType {
    POSITION_LIMIT,        // 持仓限制
    ORDER_FREQUENCY_LIMIT, // 下单频率限制
    LOSS_LIMIT,            // 亏损限制
    MARGIN_LIMIT,          // 保证金限制
    PRICE_LIMIT,           // 价格限制
    SYSTEM_ERROR           // 系统错误
};

// 风控数据结构
struct RiskData {
    RiskLevel level;         // 风控级别
    RiskType type;           // 风控类型
    std::string strategyId;  // 策略ID
    std::string symbol;      // 相关合约代码
    std::string message;     // 风控信息
    std::string triggerTime; // 触发时间
};

// 风控事件
class RiskEvent : public Event {
public:
    RiskEvent(const RiskData& data)
        : Event(EventType::RISK_CONTROL), data_(data) {}
    
    const RiskData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "RiskEvent: ";
        
        switch (data_.level) {
            case RiskLevel::INFO: ss << "INFO"; break;
            case RiskLevel::WARNING: ss << "WARNING"; break;
            case RiskLevel::ERROR: ss << "ERROR"; break;
            case RiskLevel::CRITICAL: ss << "CRITICAL"; break;
        }
        
        ss << " Type: ";
        switch (data_.type) {
            case RiskType::POSITION_LIMIT: ss << "PositionLimit"; break;
            case RiskType::ORDER_FREQUENCY_LIMIT: ss << "OrderFrequencyLimit"; break;
            case RiskType::LOSS_LIMIT: ss << "LossLimit"; break;
            case RiskType::MARGIN_LIMIT: ss << "MarginLimit"; break;
            case RiskType::PRICE_LIMIT: ss << "PriceLimit"; break;
            case RiskType::SYSTEM_ERROR: ss << "SystemError"; break;
        }
        
        ss << " Strategy: " << data_.strategyId
           << " Symbol: " << data_.symbol
           << " Message: " << data_.message;
        
        return ss.str();
    }
    
private:
    RiskData data_;
}; 