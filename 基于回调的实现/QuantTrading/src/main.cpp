#include <iostream>
#include "DataFeed.h"
#include "StrategyEngine.h"
#include "RiskManager.h"
#include "TradeExecutor.h"

// 数据回调函数
void onMarketData(const MarketData& data) {
    std::cout << "Received market data: " << data.symbol 
              << " price: " << data.price << std::endl;
}

// 信号回调函数
void onSignal(const Signal& signal) {
    std::cout << "Generated signal: " << signal.symbol 
              << " direction: " << signal.direction << std::endl;
}

// 风险检查回调函数
RiskCheckResult onRiskCheck(const Signal& signal) {
    RiskCheckResult result;
    result.approved = true;
    result.reason = "OK";
    return result;
}

// 执行报告回调函数
void onExecution(const ExecutionReport& report) {
    std::cout << "Execution report: " << report.orderId 
              << " status: " << report.status << std::endl;
}

int main() {
    // 初始化各模块
    IDataFeed* dataFeed = nullptr; // 实际应使用具体实现类
    IStrategyEngine* strategyEngine = nullptr;
    IRiskManager* riskManager = nullptr;
    ITradeExecutor* tradeExecutor = nullptr;

    // 注册回调函数
    dataFeed->registerCallback(onMarketData);
    strategyEngine->registerDataCallback(onMarketData);
    strategyEngine->registerSignalCallback(onSignal);
    riskManager->registerSignalCallback(onSignal);
    riskManager->setRiskCheckCallback(onRiskCheck);
    tradeExecutor->registerRiskCheckCallback(onRiskCheck);
    tradeExecutor->registerExecutionCallback(onExecution);

    // 启动系统
    dataFeed->start();
    strategyEngine->start();
    riskManager->start();
    tradeExecutor->connect("config/trade_config.json");

    // 主循环
    while (true) {
        // 系统运行逻辑
    }

    // 停止系统
    tradeExecutor->disconnect();
    riskManager->stop();
    strategyEngine->stop();
    dataFeed->stop();

    return 0;
}