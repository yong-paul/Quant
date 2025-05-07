#include "DataProcessor.h"
#include "../Events/Event.h"
#include <iostream>
//#include <json/json.h>

DataProcessor::DataProcessor(std::shared_ptr<EventManager> eventManager) 
    : eventManager(eventManager) {}

void DataProcessor::processRawData(const std::string& rawData) {
    try {
        // 解析原始数据并生成事件
        auto event = parseMarketData(rawData);
        if (event) {
            eventManager->addEvent(event);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing market data: " << e.what() << std::endl;
    }
}

std::shared_ptr<Event> DataProcessor::parseMarketData(const std::string& rawData) {
    // 这里简化处理，实际应用中需要根据交易所API返回的数据结构进行解析
    //Json::Value root;
    //Json::Reader reader;
    //
    //if (!reader.parse(rawData, root)) {
    //    throw std::runtime_error("Failed to parse market data");
    //}
    //
    //// 创建市场数据事件
    auto event = std::make_shared<Event>();
    //event->setType(EventType::MARKET_DATA);
    //
    //// 设置事件数据
    //event->setData("symbol", root["symbol"].asString());
    //event->setData("price", root["price"].asDouble());
    
    return event;
}