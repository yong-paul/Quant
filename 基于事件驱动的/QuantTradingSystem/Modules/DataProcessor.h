#pragma once
#include <string>
#include <memory>
#include "../Events/Event.h"
#include "../EventManager.h"

// 市场数据处理类
class DataProcessor {
public:
    DataProcessor(std::shared_ptr<EventManager> eventManager);
    
    // 处理原始市场数据
    void processRawData(const std::string& rawData);
    
private:
    std::shared_ptr<EventManager> eventManager;
    
    // 解析原始数据
    std::shared_ptr<Event> parseMarketData(const std::string& rawData);
};