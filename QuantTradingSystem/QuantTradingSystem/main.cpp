#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <string>
#include <csignal>

#include "EventManager.h"
#include "MarketData/MarketDataService.h"
#include "Trade/TradeService.h"
#include "Handlers/MarketDataHandler.h"
#include "Handlers/StrategyHandler.h"
#include "Handlers/RiskHandler.h"
#include "Handlers/SignalHandler.h"
#include "Strategies/MovingAverageStrategy.h"

// 全局变量用于信号处理
std::atomic<bool> g_running(true);

// 信号处理函数
void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    g_running = false;
}

int main() {
    std::cout << "Quant Trading System Initializing..." << std::endl;
    
    // 设置信号处理
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // 初始化事件管理器
    auto eventManager = std::make_shared<EventManager>();
    eventManager->start();
    
    // 初始化行情数据服务
    auto marketDataService = std::make_shared<MarketDataService>(eventManager);
    if (!marketDataService->init("CTP", "config/ctp_md.json")) {
        std::cerr << "Failed to initialize market data service" << std::endl;
        return 1;
    }
    
    // 初始化交易服务
    auto tradingService = std::make_shared<TradeService>(eventManager);
    if (!tradingService->Init("CTP", "config/ctp_td.json")) {
        std::cerr << "Failed to initialize trading service" << std::endl;
        return 1;
    }
    
    // 创建行情数据缓存处理器
    auto marketDataCache = std::make_shared<MarketDataCache>();
    eventManager->registerHandlerForType(EventType::MARKET_DATA, marketDataCache);
    
    // 创建策略管理器
    auto strategyManager = std::make_shared<StrategyManager>(eventManager);
    eventManager->registerHandler(strategyManager);
    
    // 创建风控管理器并添加风控规则
    auto riskManager = std::make_shared<RiskManager>(eventManager);
    riskManager->addRule(std::make_shared<OrderFrequencyRule>(10, 5));  // 限制为5秒内最多10单
    riskManager->addRule(std::make_shared<PositionLimitRule>(10, 50));  // 限制单个合约最多10手，总持仓最多50手
    eventManager->registerHandlerForType(EventType::ORDER, riskManager);
    
    // 创建信号处理器
    auto signalHandler = std::make_shared<SignalHandler>(eventManager, tradingService);
    eventManager->registerHandlerForType(EventType::STRATEGY_SIGNAL, signalHandler);
    
    // 创建移动平均线策略实例
    auto maStrategy = std::make_shared<MovingAverageStrategy>("MA_01");
    auto maParam = std::make_shared<MAStrategyParam>();
    maParam->symbol = "rb2405";  // 指定关注的合约
    maParam->shortPeriod = 5;
    maParam->longPeriod = 20;
    maParam->volume = 1;
    maStrategy->init(maParam);
    
    // 注册策略
    strategyManager->registerStrategy(maStrategy);
    
    // 启动服务
    if (!marketDataService->start()) {
        std::cerr << "Failed to start market data service" << std::endl;
        return 1;
    }
    
    if (!tradingService->Start()) {
        std::cerr << "Failed to start trading service" << std::endl;
        return 1;
    }
    
    // 登录交易服务
    if (!tradingService->Login("123456", "password")) {
        std::cerr << "Failed to login trading service" << std::endl;
        return 1;
    }
    
    // 订阅合约
    std::vector<std::string> symbols = {"rb2405"};
    if (!marketDataService->subscribeSymbols(symbols)) {
        std::cerr << "Failed to subscribe symbols" << std::endl;
        return 1;
    }
    
    // 启动策略
    strategyManager->startStrategy("MA_01");
    
    std::cout << "Quant Trading System Started." << std::endl;
    
    // 系统主循环
    while (g_running) {
        // 事件处理已经在事件管理器线程中完成，这里只需等待信号
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Shutting down..." << std::endl;
    
    // 停止策略
    strategyManager->stopStrategy("MA_01");
    
    // 停止服务
    tradingService->Stop();
    marketDataService->stop();
    
    // 停止事件管理器
    eventManager->stop();
    
    std::cout << "Quant Trading System Shutdown Complete." << std::endl;
    
    return 0;
}