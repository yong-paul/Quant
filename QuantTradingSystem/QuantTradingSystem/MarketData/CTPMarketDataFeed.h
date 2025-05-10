#pragma once
#include "IMarketDataFeed.h"
#include "API/CTP/ThostFtdcMdApi.h"
#include <atomic>
#include <mutex>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <ctime>
#include "../MarketData/MarketDataField.h"

#ifdef _WIN32
#define SAFE_CTIME(result, time, size) ctime_s(result, size, time)
#define SAFE_STRNCPY(dest, destsize, src, count) strncpy_s(dest, destsize, src, count)
#else
#define SAFE_CTIME(result, time, size) ctime_r(time, result)
#define SAFE_STRNCPY(dest, destsize, src, count) strncpy(dest, src, count)
#endif

using MarketDataCallback = std::function<void(const MarketDataField&)>;

class CTPMarketDataFeed : public IMarketDataFeed, public CThostFtdcMdSpi {
public:
    CTPMarketDataFeed();
    ~CTPMarketDataFeed();
    
    // IMarketDataFeed接口实现
    bool Init(const std::string& config) override;
    bool Connect() override;
    void Disconnect() override;
    bool IsConnected() const override;
    bool Subscribe(const std::vector<std::string>& symbols) override;
    bool Unsubscribe(const std::vector<std::string>& symbols) override;
    void Release() override;
    void SetMarketDataCallback(MarketDataCallback callback) override;
    
protected:
    // CTP SPI回调函数
    void OnFrontConnected() override;
    void OnFrontDisconnected(int nReason) override;
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
                       CThostFtdcRspInfoField *pRspInfo, 
                       int nRequestID, bool bIsLast) override;
    void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                           CThostFtdcRspInfoField *pRspInfo,
                           int nRequestID, bool bIsLast) override;
    void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) override;
    
private:
    // 将CTP行情数据转换为统一的MarketData格式
    MarketDataField ConvertMarketData(const CThostFtdcDepthMarketDataField* pData);
    
private:
    CThostFtdcMdApi* mdApi_;
    std::string frontAddress_;
    std::string brokerID_;
    std::string userID_;
    std::string password_;
    
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    MarketDataCallback marketDataCallback_;
    
    std::unordered_set<std::string> subscribedSymbols_;
    mutable std::mutex mutex_;
};