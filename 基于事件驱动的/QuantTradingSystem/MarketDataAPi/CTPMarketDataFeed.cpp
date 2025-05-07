#include "CTPMarketDataFeed.h"
#include <iostream>

CTPMarketDataFeed::CTPMarketDataFeed() : mdApi(nullptr) {}

CTPMarketDataFeed::~CTPMarketDataFeed() {
    stop();
}

void CTPMarketDataFeed::init(const std::string& configPath) {
    // 从配置文件读取CTP连接参数
    // 这里需要实现配置文件解析逻辑
    
    // 创建CTP行情API实例
    mdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    
    // 注册回调接口
    mdApi->RegisterSpi(this);
    
    // 注册前置机地址
    mdApi->RegisterFront(const_cast<char*>(frontAddress.c_str()));
    
    // 初始化连接
    mdApi->Init();
}

void CTPMarketDataFeed::start() {
    if (mdApi) {
        // 发起连接请求
        mdApi->Join();
    }
}

void CTPMarketDataFeed::stop() {
    if (mdApi) {
        // 登出并释放API
        mdApi->Release();
        mdApi = nullptr;
    }
}

void CTPMarketDataFeed::OnFrontConnected() {
    // 连接成功后发起登录请求
    CThostFtdcReqUserLoginField loginReq;
    memset(&loginReq, 0, sizeof(loginReq));
    strcpy(loginReq.BrokerID, brokerID.c_str());
    strcpy(loginReq.UserID, userID.c_str());
    strcpy(loginReq.Password, password.c_str());
    
    mdApi->ReqUserLogin(&loginReq, 0);
}

void CTPMarketDataFeed::OnFrontDisconnected(int nReason) {
    std::cerr << "CTP行情连接断开，原因: " << nReason << std::endl;
}

void CTPMarketDataFeed::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
                                      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        std::cerr << "CTP登录失败，错误代码: " << pRspInfo->ErrorID 
                  << "，错误信息: " << pRspInfo->ErrorMsg << std::endl;
        return;
    }
    
    // 登录成功后订阅行情
    const char* instruments[] = {"rb2405", "hc2405"}; // 示例合约
    mdApi->SubscribeMarketData(const_cast<char**>(instruments), sizeof(instruments)/sizeof(char*));
}

void CTPMarketDataFeed::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    if (!pDepthMarketData) return;
    
    // 将行情数据转换为字符串格式
    std::string marketData = std::string(pDepthMarketData->InstrumentID) + "," +
                             std::to_string(pDepthMarketData->LastPrice) + "," +
                             std::to_string(pDepthMarketData->Volume);
    
    // 触发原始数据回调
    if (rawDataCallback) {
        rawDataCallback(marketData);
    }
}