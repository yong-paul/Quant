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
#include "Utils/logger/AsyncLogger.h"

// 全局变量用于信号处理
std::atomic<bool> g_running(true);

// 信号处理函数
void signalHandler(int signal) {
    LOG_INFO("Received signal " + std::to_string(signal) + ", shutting down...");
    g_running = false;
}

int main() {
    try {
        // 初始化日志系统
        QuantTrading::AsyncLogger::getInstance().init(
            "logs",                    // 日志目录
            "quant_trading",           // 日志文件前缀
            QuantTrading::LogLevel::DEBUG,  // 最小日志级别
            10 * 1024 * 1024,         // 单个日志文件最大大小（10MB）
            5                         // 最大日志文件数
        );

        LOG_INFO("Quant Trading System Initializing...");
        
        // 设置信号处理
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // 初始化事件管理器
        auto eventManager = std::make_shared<EventManager>();
        eventManager->start();
        LOG_INFO("Event Manager started");
        
        // 初始化行情数据服务
        auto marketDataService = std::make_shared<MarketDataService>(eventManager);
        if (!marketDataService->init("CTP", "config/ctp_md.json")) {
            LOG_ERROR("Failed to initialize market data service");
            return 1;
        }
        LOG_INFO("Market Data Service initialized");
        
        // 初始化交易服务
        auto tradingService = std::make_shared<TradeService>(eventManager);
        if (!tradingService->Init("CTP", "config/ctp_td.json")) {
            LOG_ERROR("Failed to initialize trading service");
            return 1;
        }
        LOG_INFO("Trading Service initialized");
        
        // 创建行情数据缓存处理器
        auto marketDataCache = std::make_shared<MarketDataCache>();
        eventManager->registerHandlerForType(EventType::MARKET_DATA, marketDataCache);
        LOG_INFO("Market Data Cache Handler registered");
        
        // 创建策略管理器
        auto strategyManager = std::make_shared<StrategyManager>(eventManager);
        eventManager->registerHandler(strategyManager);
        LOG_INFO("Strategy Manager registered");
        
        // 创建风控管理器并添加风控规则
        auto riskManager = std::make_shared<RiskManager>(eventManager);
        riskManager->addRule(std::make_shared<OrderFrequencyRule>(10, 5));
        riskManager->addRule(std::make_shared<PositionLimitRule>(10, 50));
        eventManager->registerHandlerForType(EventType::ORDER, riskManager);
        LOG_INFO("Risk Manager registered with rules");
        
        // 创建信号处理器
        auto signalHandler = std::make_shared<SignalHandler>(eventManager, tradingService);
        eventManager->registerHandlerForType(EventType::STRATEGY_SIGNAL, signalHandler);
        LOG_INFO("Signal Handler registered");
        
        // 创建移动平均线策略实例
        auto maStrategy = std::make_shared<MovingAverageStrategy>("MA_01");
        auto maParam = std::make_shared<MAStrategyParam>();
        maParam->symbol = "rb2405";
        maParam->shortPeriod = 5;
        maParam->longPeriod = 20;
        maParam->volume = 1;
        maStrategy->init(maParam);
        LOG_INFO("Moving Average Strategy initialized");
        
        // 注册策略
        strategyManager->registerStrategy(maStrategy);
        LOG_INFO("Strategy registered with manager");
        
        // 启动服务
        if (!marketDataService->start()) {
            LOG_ERROR("Failed to start market data service");
            return 1;
        }
        LOG_INFO("Market Data Service started");
        
        if (!tradingService->Start()) {
            LOG_ERROR("Failed to start trading service");
            return 1;
        }
        LOG_INFO("Trading Service started");
        
        // 登录交易服务
        if (!tradingService->Login("123456", "password")) {
            LOG_ERROR("Failed to login trading service");
            return 1;
        }
        LOG_INFO("Trading Service logged in");
        
        // 订阅合约
        std::vector<std::string> symbols = {"rb2405"};
        if (!marketDataService->subscribeSymbols(symbols)) {
            LOG_ERROR("Failed to subscribe symbols");
            return 1;
        }
        LOG_INFO("Symbols subscribed: " + symbols[0]);
        
        // 启动策略
        strategyManager->startStrategy("MA_01");
        LOG_INFO("Strategy started: MA_01");
        
        LOG_INFO("Quant Trading System Started Successfully");
        
        // 系统主循环
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        LOG_INFO("Shutting down...");
        
        // 停止策略
        strategyManager->stopStrategy("MA_01");
        LOG_INFO("Strategy stopped: MA_01");
        
        // 停止服务
        tradingService->Stop();
        marketDataService->stop();
        LOG_INFO("Services stopped");
        
        // 停止事件管理器
        eventManager->stop();
        LOG_INFO("Event Manager stopped");
        
        // 停止日志系统
        QuantTrading::AsyncLogger::getInstance().stop();
        
        LOG_INFO("Quant Trading System Shutdown Complete");
        
    } catch (const std::exception& e) {
        LOG_FATAL("Fatal error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}