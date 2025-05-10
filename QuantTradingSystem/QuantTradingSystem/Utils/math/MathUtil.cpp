#include "MathUtil.h"
#include <algorithm>
#include <numeric>

double MathUtil::mean(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double MathUtil::variance(const std::vector<double>& data) {
    if (data.size() < 2) return 0.0;
    
    double m = mean(data);
    double sum = 0.0;
    
    for (const auto& value : data) {
        sum += (value - m) * (value - m);
    }
    
    return sum / (data.size() - 1);
}

double MathUtil::stddev(const std::vector<double>& data) {
    return std::sqrt(variance(data));
}

double MathUtil::median(std::vector<double> data) {
    if (data.empty()) return 0.0;
    
    std::sort(data.begin(), data.end());
    size_t size = data.size();
    
    if (size % 2 == 0) {
        return (data[size/2 - 1] + data[size/2]) / 2.0;
    } else {
        return data[size/2];
    }
}

std::vector<double> MathUtil::SMA(const std::vector<double>& data, int period) {
    if (period <= 0 || data.empty()) return std::vector<double>();
    
    std::vector<double> result;
    result.reserve(data.size());
    
    double sum = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
        if (i >= period) {
            sum -= data[i - period];
        }
        if (i >= period - 1) {
            result.push_back(sum / period);
        } else {
            result.push_back(sum / (i + 1));
        }
    }
    
    return result;
}

std::vector<double> MathUtil::EMA(const std::vector<double>& data, int period) {
    if (period <= 0 || data.empty()) return std::vector<double>();
    
    std::vector<double> result;
    result.reserve(data.size());
    
    double alpha = 2.0 / (period + 1);
    double ema = data[0];
    result.push_back(ema);
    
    for (size_t i = 1; i < data.size(); ++i) {
        ema = data[i] * alpha + ema * (1 - alpha);
        result.push_back(ema);
    }
    
    return result;
}

std::vector<double> MathUtil::MACD(const std::vector<double>& data, 
                                  int fastPeriod, int slowPeriod, int signalPeriod) {
    auto fastEMA = EMA(data, fastPeriod);
    auto slowEMA = EMA(data, slowPeriod);
    
    std::vector<double> diff;
    diff.reserve(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        diff.push_back(fastEMA[i] - slowEMA[i]);
    }
    
    auto signal = EMA(diff, signalPeriod);
    
    std::vector<double> result;
    result.reserve(data.size() * 3);
    result.insert(result.end(), diff.begin(), diff.end());
    result.insert(result.end(), signal.begin(), signal.end());
    
    for (size_t i = 0; i < data.size(); ++i) {
        result.push_back(diff[i] - signal[i]);  // MACD柱状图
    }
    
    return result;
}

std::vector<double> MathUtil::RSI(const std::vector<double>& data, int period) {
    if (period <= 0 || data.size() < period + 1) return std::vector<double>();
    
    std::vector<double> result;
    result.reserve(data.size());
    
    std::vector<double> gains, losses;
    gains.reserve(data.size() - 1);
    losses.reserve(data.size() - 1);
    
    // 计算涨跌
    for (size_t i = 1; i < data.size(); ++i) {
        double diff = data[i] - data[i-1];
        gains.push_back(std::max(diff, 0.0));
        losses.push_back(std::max(-diff, 0.0));
    }
    
    // 计算初始平均涨跌
    double avgGain = std::accumulate(gains.begin(), gains.begin() + period, 0.0) / period;
    double avgLoss = std::accumulate(losses.begin(), losses.begin() + period, 0.0) / period;
    
    // 填充前period个数据
    for (int i = 0; i < period; ++i) {
        result.push_back(0);
    }
    
    // 计算RSI
    for (size_t i = period; i < data.size() - 1; ++i) {
        avgGain = (avgGain * (period - 1) + gains[i]) / period;
        avgLoss = (avgLoss * (period - 1) + losses[i]) / period;
        
        if (avgLoss < 1e-10) {
            result.push_back(100);
        } else {
            double rs = avgGain / avgLoss;
            result.push_back(100 - 100 / (1 + rs));
        }
    }
    
    return result;
}

std::vector<double> MathUtil::Bollinger(const std::vector<double>& data, 
                                      int period, double multiplier) {
    if (period <= 0 || data.size() < period) return std::vector<double>();
    
    auto sma = SMA(data, period);
    std::vector<double> result;
    result.reserve(data.size() * 3);  // 存储中轨、上轨、下轨
    
    // 计算标准差
    for (size_t i = period - 1; i < data.size(); ++i) {
        std::vector<double> window(data.begin() + (i - period + 1), data.begin() + i + 1);
        double std = stddev(window);
        
        result.push_back(sma[i]);  // 中轨
        result.push_back(sma[i] + multiplier * std);  // 上轨
        result.push_back(sma[i] - multiplier * std);  // 下轨
    }
    
    return result;
}

double MathUtil::sharpeRatio(const std::vector<double>& returns, double riskFreeRate) {
    if (returns.empty()) return 0.0;
    
    double meanReturn = mean(returns);
    double std = stddev(returns);
    
    if (std < 1e-10) return 0.0;
    return (meanReturn - riskFreeRate) / std;
}

double MathUtil::maxDrawdown(const std::vector<double>& equity) {
    if (equity.empty()) return 0.0;
    
    double maxDrawdown = 0.0;
    double peak = equity[0];
    
    for (const auto& value : equity) {
        if (value > peak) {
            peak = value;
        } else {
            double drawdown = (peak - value) / peak;
            maxDrawdown = std::max(maxDrawdown, drawdown);
        }
    }
    
    return maxDrawdown;
}

double MathUtil::winRate(const std::vector<double>& trades) {
    if (trades.empty()) return 0.0;
    
    int wins = std::count_if(trades.begin(), trades.end(), 
                            [](double x) { return x > 0; });
    return static_cast<double>(wins) / trades.size();
}

double MathUtil::profitFactor(const std::vector<double>& trades) {
    double totalProfit = 0.0;
    double totalLoss = 0.0;
    
    for (const auto& trade : trades) {
        if (trade > 0) {
            totalProfit += trade;
        } else {
            totalLoss -= trade;
        }
    }
    
    if (totalLoss < 1e-10) return totalProfit > 0 ? INFINITY : 0.0;
    return totalProfit / totalLoss;
}

bool MathUtil::isNearZero(double value, double epsilon) {
    return std::abs(value) < epsilon;
}

double MathUtil::round(double value, int decimals) {
    double multiplier = std::pow(10.0, decimals);
    return std::round(value * multiplier) / multiplier;
} 