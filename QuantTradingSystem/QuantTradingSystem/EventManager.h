#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <functional>
#include "Handlers/EventHandler.h"
#include "Events/AllEvents.h"
#include "Utils/LockFreeQueue.h"

class EventManager {
public:
    EventManager();
    ~EventManager();

    // 启动事件处理线程
    void start();
    
    // 停止事件处理线程
    void stop();
    
    // 注册事件处理器
    void registerHandler(std::shared_ptr<EventHandler> handler);
    
    // 注销事件处理器
    void unregisterHandler(std::shared_ptr<EventHandler> handler);
    
    // 处理所有事件（单次调用）
    void processEvents();
    
    // 添加事件
    void addEvent(std::shared_ptr<Event> event);

    // 按事件类型注册处理器
    void registerHandlerForType(EventType type, std::shared_ptr<EventHandler> handler);
    
    // 按事件类型注销处理器
    void unregisterHandlerForType(EventType type, std::shared_ptr<EventHandler> handler);
    
    // 检查事件队列是否为空
    bool isEventQueueEmpty() const;
    
    // 获取事件队列大小
    size_t getEventQueueSize() const;
    
private:
    // 所有事件处理器
    std::vector<std::shared_ptr<EventHandler>> handlers_;
    
    // 按事件类型分类的处理器
    std::unordered_map<EventType, std::vector<std::shared_ptr<EventHandler>>> typeHandlers_;
    
    // 使用无锁队列
    Utils::LockFreeQueue<std::shared_ptr<Event>, 10000> eventQueue_;
    
    // 互斥锁和条件变量，用于同步事件处理
    mutable std::mutex eventMutex_;
    std::condition_variable eventCondition_;
    
    // 控制事件处理线程的标志
    std::atomic<bool> running_;
    
    // 事件处理线程
    std::thread eventThread_;
    
    // 事件处理线程函数
    void eventProcessingThread();
    
    // 临时事件缓冲区，用于批量处理事件
    std::vector<std::shared_ptr<Event>> eventBuffer_;
    static const size_t MAX_BUFFER_SIZE = 1000;
};