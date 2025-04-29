#include <iostream>
#include "EventManager.h"
#include "EventHandler.h"
#include <chrono>
#include <thread>

int main() {
    std::cout << "Quant Trading System Initializing..." << std::endl;
    
    // 初始化事件管理器
    EventManager eventManager;
    
    // 初始化并注册事件处理器
    EventHandler marketDataHandler("MarketData");
    EventHandler tradeHandler("Trade");
    EventHandler riskHandler("Risk");
    
    eventManager.registerHandler(marketDataHandler);
    eventManager.registerHandler(tradeHandler);
    eventManager.registerHandler(riskHandler);
    
    // 系统主循环
    while (true) {
        // 处理事件
        eventManager.processEvents();
        
        // 添加休眠避免CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return 0;
}