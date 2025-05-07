#pragma once
#include "../Modules/DataFeed.h"
#include "ThostFtdcMdApi.h"

class CTPMarketDataFeed : public DataFeed {
public:
    CTPMarketDataFeed();
    ~CTPMarketDataFeed();
    
    // 初始化CTP API
    void init(const std::string& configPath) override;
    
    // 启动行情接收
    void start() override;
    
    // 停止行情接收
    void stop() override;
    
private:
    CThostFtdcMdApi* mdApi;
    std::string frontAddress;
    std::string brokerID;
    std::string userID;
    std::string password;
    
    // CTP API回调函数
    static void OnFrontConnected();
    static void OnFrontDisconnected(int nReason);
    static void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    static void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
};