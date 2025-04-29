#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// 基础消息类型
struct Message {
    std::string type;
    virtual ~Message() = default;
};

// 系统控制消息
struct SystemControlMessage : public Message {
    enum ControlType {
        SHUTDOWN
    } controlType;
    SystemControlMessage(ControlType type) : controlType(type) { this->type = "SystemControl"; }
};

// 消息处理器类型
using MessageHandler = std::function<void(const Message&)>;

// 消息总线接口
class MessageBus {
public:
    virtual ~MessageBus() = default;
    
    // 订阅消息
    virtual void subscribe(const std::string& messageType, MessageHandler handler) = 0;
    
    // 发布消息
    virtual void publish(const Message& message) = 0;
    
    // 创建总线实例
    static std::shared_ptr<MessageBus> create();
};