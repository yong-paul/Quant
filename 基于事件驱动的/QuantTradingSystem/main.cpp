#include <iostream>
#include "EventManager.h"
#include "Handlers/StrategyEngineHandler.h"
#include "DataFeed.h"
//#include "Modules/StrategyEngine.h"
#include <chrono>
#include <thread>

int main() {
    std::cout << "Quant Trading System Initializing..." << std::endl;
    
    // 初始化事件管理器
    EventManager eventManager;
    
    // 初始化数据源和处理器
    auto dataFeed = createDataFeed();
    auto dataProcessor = std::make_shared<DataProcessor>(std::make_shared<EventManager>(eventManager));
    
    // 设置数据回调
    dataFeed->setRawDataCallback([&](const std::string& rawData) {
        dataProcessor->processRawData(rawData);
    });
    
    // 初始化并注册事件处理器
    auto marketDataHandler = std::make_shared<StrategyEngine>();
    auto tradeHandler = std::make_shared<StrategyEngine>();
    auto riskHandler = std::make_shared<StrategyEngine>();
    
    eventManager.registerHandler(marketDataHandler);
    eventManager.registerHandler(tradeHandler);
    eventManager.registerHandler(riskHandler);
    
    // 启动数据源
    dataFeed->start();
    
    // 系统主循环
    while (true) {
        // 处理事件
        eventManager.processEvents();
        
        // 添加休眠避免CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return 0;
}