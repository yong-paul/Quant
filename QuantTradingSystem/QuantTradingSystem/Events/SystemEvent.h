#pragma once
#include "Event.h"
#include <string>
#include <sstream>

// 系统事件类型
enum class SystemEventType {
    STARTUP,         // 系统启动
    SHUTDOWN,        // 系统关闭
    CONNECT,         // 连接成功
    DISCONNECT,      // 断开连接
    LOGIN,           // 登录成功
    LOGOUT,          // 登出成功
    ERROR,           // 错误
    INFO,            // 信息
    WARNING          // 警告
};

// 系统事件数据结构
struct SystemEventData {
    SystemEventType type;     // 事件类型
    std::string source;       // 事件来源
    std::string message;      // 事件信息
    std::string eventTime;    // 事件时间
};

// 系统事件
class SystemEvent : public Event {
public:
    SystemEvent(const SystemEventData& data)
        : Event(EventType::SYSTEM), data_(data) {}
    
    const SystemEventData& getData() const { return data_; }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << "SystemEvent: ";
        
        switch (data_.type) {
            case SystemEventType::STARTUP: ss << "Startup"; break;
            case SystemEventType::SHUTDOWN: ss << "Shutdown"; break;
            case SystemEventType::CONNECT: ss << "Connect"; break;
            case SystemEventType::DISCONNECT: ss << "Disconnect"; break;
            case SystemEventType::LOGIN: ss << "Login"; break;
            case SystemEventType::LOGOUT: ss << "Logout"; break;
            case SystemEventType::ERROR: ss << "Error"; break;
            case SystemEventType::INFO: ss << "Info"; break;
            case SystemEventType::WARNING: ss << "Warning"; break;
        }
        
        ss << " Source: " << data_.source
           << " Message: " << data_.message
           << " Time: " << data_.eventTime;
        
        return ss.str();
    }
    
private:
    SystemEventData data_;
}; 