#include "EventManager.h"
#include <algorithm>
#include <iostream>
#include <ctime>

EventManager::EventManager() : running_(false) {
}

EventManager::~EventManager() {
    stop();
}

void EventManager::start() {
    if (!running_) {
        running_ = true;
        eventThread_ = std::thread(&EventManager::eventProcessingThread, this);
        
        // 添加系统启动事件
        SystemEventData startupData;
        startupData.type = SystemEventType::STARTUP;
        startupData.source = "EventManager";
        startupData.message = "Event processing system started";
        
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        char timeStr[26];
#ifdef _WIN32
        ctime_s(timeStr, sizeof(timeStr), &now_time_t);
#else
        // 在非Windows系统上使用线程安全的ctime_r
        ctime_r(&now_time_t, timeStr);
#endif
        startupData.eventTime = timeStr;
        
        addEvent(std::make_shared<SystemEvent>(startupData));
    }
}

void EventManager::stop() {
    if (running_) {
        running_ = false;
        eventCondition_.notify_all();
        if (eventThread_.joinable()) {
            eventThread_.join();
        }
        
        // 清空事件队列
        std::lock_guard<std::mutex> lock(eventMutex_);
        while (!eventQueue_.empty()) {
            eventQueue_.pop();
        }
    }
}

void EventManager::registerHandler(std::shared_ptr<EventHandler> handler) {
    if (!handler) return;
    
    std::lock_guard<std::mutex> lock(eventMutex_);
    handlers_.push_back(handler);
}

void EventManager::unregisterHandler(std::shared_ptr<EventHandler> handler) {
    if (!handler) return;
    
    std::lock_guard<std::mutex> lock(eventMutex_);
    handlers_.erase(
        std::remove_if(
            handlers_.begin(), 
            handlers_.end(),
            [&handler](const std::shared_ptr<EventHandler>& h) { 
                return h == handler; 
            }
        ),
        handlers_.end()
    );
    
    // 同时从类型处理器映射中删除
    for (auto& pair : typeHandlers_) {
        auto& typeHandlerList = pair.second;
        typeHandlerList.erase(
            std::remove_if(
                typeHandlerList.begin(), 
                typeHandlerList.end(),
                [&handler](const std::shared_ptr<EventHandler>& h) { 
                    return h == handler; 
                }
            ),
            typeHandlerList.end()
        );
    }
}

void EventManager::processEvents() {
    std::vector<std::shared_ptr<Event>> currentEvents;
    
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        while (!eventQueue_.empty()) {
            currentEvents.push_back(eventQueue_.front());
            eventQueue_.pop();
        }
    }
    
    for (auto& event : currentEvents) {
        // 检查是否有针对该事件类型的处理器
        EventType type = event->getType();
        
        // 首先使用特定类型的处理器
        {
            std::lock_guard<std::mutex> lock(eventMutex_);
            auto it = typeHandlers_.find(type);
            if (it != typeHandlers_.end()) {
                for (auto& handler : it->second) {
                    try {
                        handler->handleEvent(event);
                    } catch (const std::exception& e) {
                        std::cerr << "Error handling event in " << handler->getName() 
                                 << ": " << e.what() << std::endl;
                    }
                }
            }
        }
        
        // 然后使用通用处理器
        {
            std::lock_guard<std::mutex> lock(eventMutex_);
            for (auto& handler : handlers_) {
                try {
                    handler->handleEvent(event);
                } catch (const std::exception& e) {
                    std::cerr << "Error handling event in " << handler->getName() 
                             << ": " << e.what() << std::endl;
                }
            }
        }
    }
}

void EventManager::addEvent(std::shared_ptr<Event> event) {
    if (!event) return;
    
    {
        std::lock_guard<std::mutex> lock(eventMutex_);
        eventQueue_.push(event);
    }
    
    eventCondition_.notify_one();
}

void EventManager::registerHandlerForType(EventType type, std::shared_ptr<EventHandler> handler) {
    if (!handler) return;
    
    std::lock_guard<std::mutex> lock(eventMutex_);
    typeHandlers_[type].push_back(handler);
}

void EventManager::unregisterHandlerForType(EventType type, std::shared_ptr<EventHandler> handler) {
    if (!handler) return;
    
    std::lock_guard<std::mutex> lock(eventMutex_);
    auto it = typeHandlers_.find(type);
    if (it != typeHandlers_.end()) {
        auto& typeHandlerList = it->second;
        typeHandlerList.erase(
            std::remove_if(
                typeHandlerList.begin(), 
                typeHandlerList.end(),
                [&handler](const std::shared_ptr<EventHandler>& h) { 
                    return h == handler; 
                }
            ),
            typeHandlerList.end()
        );
    }
}

bool EventManager::isEventQueueEmpty() const {
    std::lock_guard<std::mutex> lock(eventMutex_);
    return eventQueue_.empty();
}

size_t EventManager::getEventQueueSize() const {
    std::lock_guard<std::mutex> lock(eventMutex_);
    return eventQueue_.size();
}

void EventManager::eventProcessingThread() {
    while (running_) {
        std::unique_lock<std::mutex> lock(eventMutex_);
        eventCondition_.wait(lock, [this]() { 
            return !eventQueue_.empty() || !running_; 
        });
        
        if (!running_) break;
        
        // 处理事件
        processEvents();
    }
}