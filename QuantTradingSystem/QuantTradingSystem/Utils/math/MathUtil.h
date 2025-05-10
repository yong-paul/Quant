#pragma once
#include <vector>
#include <cmath>

class MathUtil {
public:
    // 基础统计函数
    static double mean(const std::vector<double>& data);
    static double variance(const std::vector<double>& data);
    static double stddev(const std::vector<double>& data);
    static double median(std::vector<double> data);
    
    // 技术指标计算
    static std::vector<double> SMA(const std::vector<double>& data, int period);
    static std::vector<double> EMA(const std::vector<double>& data, int period);
    static std::vector<double> MACD(const std::vector<double>& data, 
                                   int fastPeriod = 12, 
                                   int slowPeriod = 26, 
                                   int signalPeriod = 9);
    static std::vector<double> RSI(const std::vector<double>& data, int period = 14);
    static std::vector<double> Bollinger(const std::vector<double>& data, 
                                       int period = 20, 
                                       double multiplier = 2.0);
    
    // 回测指标计算
    static double sharpeRatio(const std::vector<double>& returns, double riskFreeRate = 0.0);
    static double maxDrawdown(const std::vector<double>& equity);
    static double winRate(const std::vector<double>& trades);
    static double profitFactor(const std::vector<double>& trades);
    
    // 其他工具函数
    static bool isNearZero(double value, double epsilon = 1e-10);
    static double round(double value, int decimals = 2);
}; 