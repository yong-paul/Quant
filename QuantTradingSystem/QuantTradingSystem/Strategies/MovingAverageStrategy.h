#pragma once
#include "../Handlers/StrategyHandler.h"
#include "../Events/AllEvents.h"
#include "../MarketData/MarketDataField.h"
#include <memory>
#include <string>
#include <deque>
#include <unordered_map>

// 移动平均线策略参数
class MAStrategyParam : public StrategyParam {
public:
    std::string symbol;       // 交易合约
    int shortPeriod;          // 短周期
    int longPeriod;           // 长周期
    int volume;               // 交易数量
    double stopLossPercent;   // 止损百分比
    double takeProfitPercent; // 止盈百分比
    
    MAStrategyParam() 
        : shortPeriod(5), longPeriod(20), volume(1),
          stopLossPercent(0.02), takeProfitPercent(0.05) {}
};

// 移动平均线策略
class MovingAverageStrategy : public Strategy {
public:
    MovingAverageStrategy(const std::string& id)
        : Strategy(id, "MovingAverageStrategy"), 
          initialized_(false), lastSignal_(0.0) {}
    
    ~MovingAverageStrategy() override = default;
    
    // 初始化策略
    bool init(std::shared_ptr<StrategyParam> param) override {
        auto maParam = std::dynamic_pointer_cast<MAStrategyParam>(param);
        if (!maParam) {
            return false;
        }
        
        param_ = *maParam;
        
        // 清空历史数据
        priceSeries_.clear();
        shortMA_.clear();
        longMA_.clear();
        lastSignal_ = 0.0;
        initialized_ = true;
        
        return true;
    }
    
    // 处理行情数据
    void onMarketData(const MarketDataField& data) override {
        if (!initialized_ || status_ != StrategyStatus::RUNNING) {
            return;
        }
        
        // 只处理关注的合约
        if (data.symbol != param_.symbol) {
            return;
        }
        
        // 更新价格序列
        priceSeries_.push_back(data.lastPrice);
        if (priceSeries_.size() > static_cast<size_t>(param_.longPeriod)) {
            priceSeries_.pop_front();
        }
        
        // 计算移动平均线
        if (priceSeries_.size() >= static_cast<size_t>(param_.longPeriod)) {
            double shortSum = 0.0;
            double longSum = 0.0;
            
            // 计算短周期MA
            for (int i = priceSeries_.size() - param_.shortPeriod; i < priceSeries_.size(); ++i) {
                shortSum += priceSeries_[i];
            }
            double shortMA = shortSum / param_.shortPeriod;
            shortMA_.push_back(shortMA);
            if (shortMA_.size() > 2) {
                shortMA_.pop_front();
            }
            
            // 计算长周期MA
            for (int i = 0; i < priceSeries_.size(); ++i) {
                longSum += priceSeries_[i];
            }
            double longMA = longSum / param_.longPeriod;
            longMA_.push_back(longMA);
            if (longMA_.size() > 2) {
                longMA_.pop_front();
            }
            
            // 生成交易信号
            if (shortMA_.size() >= 2 && longMA_.size() >= 2) {
                // 金叉信号 (短周期上穿长周期)
                if (shortMA_[0] <= longMA_[0] && shortMA_[1] > longMA_[1]) {
                    // 策略发出开多信号
                    if (lastSignal_ <= 0.0) {  // 如果之前是空仓或空头，才开多
                        generateOpenLongSignal(data);
                        lastSignal_ = 1.0;  // 标记为多头
                    }
                }
                // 死叉信号 (短周期下穿长周期)
                else if (shortMA_[0] >= longMA_[0] && shortMA_[1] < longMA_[1]) {
                    // 策略发出开空信号
                    if (lastSignal_ >= 0.0) {  // 如果之前是空仓或多头，才开空
                        generateOpenShortSignal(data);
                        lastSignal_ = -1.0;  // 标记为空头
                    }
                }
            }
        }
    }
    
    // 处理订单状态
    void onOrder(const OrderData& data) override {
        // 处理订单回报，更新策略内部状态
    }
    
    // 处理成交回报
    void onTrade(const TradeData& data) override {
        // 处理成交回报，更新策略内部状态
    }
    
private:
    // 生成开多信号
    void generateOpenLongSignal(const MarketDataField& data) {
        StrategySignalData signal;
        signal.strategyId = id_;
        signal.symbol = data.symbol;
        signal.signalType = SignalType::OPEN_LONG;
        signal.price = data.lastPrice;
        signal.volume = param_.volume;
        signal.stopLoss = data.lastPrice * (1.0 - param_.stopLossPercent);
        signal.takeProfit = data.lastPrice * (1.0 + param_.takeProfitPercent);
        signal.signalTime = data.updateTime;
        signal.comment = "MA CrossOver: Short MA crosses above Long MA";
        
        // 创建并发送信号事件
        auto event = std::make_shared<StrategySignalEvent>(signal);
        // 事件发送到事件管理器
        // 需要持有EventManager引用才能发送事件
    }
    
    // 生成开空信号
    void generateOpenShortSignal(const MarketDataField& data) {
        StrategySignalData signal;
        signal.strategyId = id_;
        signal.symbol = data.symbol;
        signal.signalType = SignalType::OPEN_SHORT;
        signal.price = data.lastPrice;
        signal.volume = param_.volume;
        signal.stopLoss = data.lastPrice * (1.0 + param_.stopLossPercent);
        signal.takeProfit = data.lastPrice * (1.0 - param_.takeProfitPercent);
        signal.signalTime = data.updateTime;
        signal.comment = "MA CrossUnder: Short MA crosses below Long MA";
        
        // 创建并发送信号事件
        auto event = std::make_shared<StrategySignalEvent>(signal);
        // 事件发送到事件管理器
        // 需要持有EventManager引用才能发送事件
    }
    
private:
    bool initialized_;
    MAStrategyParam param_;
    std::deque<double> priceSeries_;  // 价格序列
    std::deque<double> shortMA_;      // 短周期MA
    std::deque<double> longMA_;       // 长周期MA
    double lastSignal_;               // 上一次信号
}; 