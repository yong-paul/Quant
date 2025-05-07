#include "MarketDataModule.h"
#include "EventManager.h"
#include "Events/MarketDataEvent.h"

void MarketDataModule::onNewMarketData(const std::string& symbol, double price, int volume) {
    // 创建行情数据事件
    auto event = std::make_shared<MarketDataEvent>(symbol, price, volume);
    
    // 获取事件管理器实例
    extern EventManager eventManager;
    
    // 将事件添加到事件队列
    eventManager.addEvent(event);
}