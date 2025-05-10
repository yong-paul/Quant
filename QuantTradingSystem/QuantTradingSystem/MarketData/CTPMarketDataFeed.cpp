#include "CTPMarketDataFeed.h"
#include <iostream>
#include "config/ConfigManager.h"
//#include "logger/spdlog/spdlog.h"
#include "MarketDataEvent.h"
#include <algorithm>
#include <ctime>

#ifdef _WIN32
#define SAFE_STRNCPY(dest, destsize, src, count) strncpy_s(dest, destsize, src, count)
#else
#define SAFE_STRNCPY(dest, destsize, src, count) strncpy(dest, src, count)
#endif

#ifdef _WIN32
#define SAFE_CTIME(result, time, size) ctime_s(result, size, time)
#else
#define SAFE_CTIME(result, time, size) ctime_r(time, result)
#endif

CTPMarketDataFeed::CTPMarketDataFeed() 
    : mdApi_(nullptr)
    , connected_(false)
    , running_(false) {
}

CTPMarketDataFeed::~CTPMarketDataFeed() {
    Disconnect();
}

bool CTPMarketDataFeed::Init(const std::string& config) {
    try {
        auto& configManager = ConfigManager::getInstance();
        if (!configManager.loadConfig(config)) {
            //spdlog::error("Failed to load config file: {}", config);
            return false;
        }

        // 读取CTP连接参数
        frontAddress_ = configManager.getValue<std::string>("ctp.mdFront", "");
        brokerID_ = configManager.getValue<std::string>("ctp.brokerID", "");
        userID_ = configManager.getValue<std::string>("ctp.userID", "");
        password_ = configManager.getValue<std::string>("ctp.password", "");

        if (frontAddress_.empty() || brokerID_.empty() || userID_.empty() || password_.empty()) {
            //spdlog::error("CTP配置参数不完整");
            return false;
        }

        // 创建行情API实例
        mdApi_ = CThostFtdcMdApi::CreateFtdcMdApi("./mdflow/");
        if (!mdApi_) {
            //spdlog::error("创建CTP行情API实例失败");
            return false;
        }

        // 注册回调接口
        mdApi_->RegisterSpi(this);
        
        // 注册前置机地址
        mdApi_->RegisterFront(const_cast<char*>(frontAddress_.c_str()));
        
        // 初始化连接
        mdApi_->Init();
        
        //spdlog::info("CTP行情接口初始化成功");
        return true;
    }
    catch (const std::exception& e) {
        //spdlog::error("CTP行情接口初始化异常: {}", e.what());
        return false;
    }
}

bool CTPMarketDataFeed::Connect() {
    if (!mdApi_) {
        //spdlog::error("CTP行情API未初始化");
        return false;
    }

    running_ = true;
    //spdlog::info("启动CTP行情接口...");
    return true;
}

void CTPMarketDataFeed::Disconnect() {
    running_ = false;
    connected_ = false;

    if (mdApi_) {
        mdApi_->RegisterSpi(nullptr);
        mdApi_->Release();
        mdApi_ = nullptr;
    }

    //spdlog::info("CTP行情接口已停止");
}

void CTPMarketDataFeed::Release() {
    Disconnect();
}

bool CTPMarketDataFeed::Subscribe(const std::vector<std::string>& symbols) {
    if (!IsConnected()) {
        //spdlog::error("CTP行情未连接");
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<char*> charSymbols;
    for (const auto& symbol : symbols) {
        if (subscribedSymbols_.find(symbol) == subscribedSymbols_.end()) {
            charSymbols.push_back(const_cast<char*>(symbol.c_str()));
            subscribedSymbols_.insert(symbol);
        }
    }

    if (!charSymbols.empty()) {
        int result = mdApi_->SubscribeMarketData(charSymbols.data(), charSymbols.size());
        if (result != 0) {
            //spdlog::error("订阅行情失败，错误码: {}", result);
            return false;
        }
    }

    return true;
}

bool CTPMarketDataFeed::Unsubscribe(const std::vector<std::string>& symbols) {
    if (!IsConnected()) {
        //spdlog::error("CTP行情未连接");
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<char*> charSymbols;
    for (const auto& symbol : symbols) {
        if (subscribedSymbols_.find(symbol) != subscribedSymbols_.end()) {
            charSymbols.push_back(const_cast<char*>(symbol.c_str()));
            subscribedSymbols_.erase(symbol);
        }
    }

    if (!charSymbols.empty()) {
        int result = mdApi_->UnSubscribeMarketData(charSymbols.data(), charSymbols.size());
        if (result != 0) {
            //spdlog::error("取消订阅行情失败，错误码: {}", result);
            return false;
        }
    }

    return true;
}

bool CTPMarketDataFeed::IsConnected() const {
    return connected_;
}

void CTPMarketDataFeed::SetMarketDataCallback(MarketDataCallback callback) {
    marketDataCallback_ = callback;
}

void CTPMarketDataFeed::OnFrontConnected() {
    //spdlog::info("CTP行情服务器连接成功");
    
    CThostFtdcReqUserLoginField loginReq = {0};
    SAFE_STRNCPY(loginReq.BrokerID, sizeof(loginReq.BrokerID), brokerID_.c_str(), sizeof(loginReq.BrokerID) - 1);
    SAFE_STRNCPY(loginReq.UserID, sizeof(loginReq.UserID), userID_.c_str(), sizeof(loginReq.UserID) - 1);
    SAFE_STRNCPY(loginReq.Password, sizeof(loginReq.Password), password_.c_str(), sizeof(loginReq.Password) - 1);
    
    int result = mdApi_->ReqUserLogin(&loginReq, 0);
    if (result != 0) {
        //spdlog::error("发送登录请求失败，错误码: {}", result);
    }
}

void CTPMarketDataFeed::OnFrontDisconnected(int nReason) {
    connected_ = false;
    //spdlog::error("CTP行情服务器连接断开，原因代码: {}", nReason);
}

void CTPMarketDataFeed::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
                                      CThostFtdcRspInfoField *pRspInfo, 
                                      int nRequestID, 
                                      bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID == 0) {
        connected_ = true;
        //spdlog::info("CTP行情服务器登录成功");
        
        // 重新订阅之前的合约
        std::lock_guard<std::mutex> lock(mutex_);
        if (!subscribedSymbols_.empty()) {
            std::vector<std::string> symbols(subscribedSymbols_.begin(), subscribedSymbols_.end());
            Subscribe(symbols);
        }
    } else {
        std::string errorMsg = pRspInfo ? pRspInfo->ErrorMsg : "Unknown error";
        int errorId = pRspInfo ? pRspInfo->ErrorID : -1;
        //spdlog::error("CTP行情服务器登录失败，错误码: {}，错误信息: {}", errorId, errorMsg);
    }
}

void CTPMarketDataFeed::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                                          CThostFtdcRspInfoField *pRspInfo,
                                          int nRequestID,
                                          bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        //spdlog::error("订阅行情失败，合约: {}，错误码: {}，错误信息: {}", 
        //             (pSpecificInstrument ? pSpecificInstrument->InstrumentID : "unknown"),
        //             pRspInfo->ErrorID,
        //             pRspInfo->ErrorMsg);
    } else if (pSpecificInstrument) {
        //spdlog::info("成功订阅合约: {}", pSpecificInstrument->InstrumentID);
    }
}

void CTPMarketDataFeed::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    if (!pDepthMarketData || !marketDataCallback_) {
        return;
    }

    try {
        MarketDataField data = ConvertMarketData(pDepthMarketData);
        marketDataCallback_(data);
    }
    catch (const std::exception& e) {
        //spdlog::error("处理行情数据异常: {}", e.what());
    }
}

MarketDataField CTPMarketDataFeed::ConvertMarketData(const CThostFtdcDepthMarketDataField* pData) {
    MarketDataField data;
    
    // 基本信息
    data.symbol = pData->InstrumentID;
    data.exchange = pData->ExchangeID;
    data.tradingDay = pData->TradingDay;
    
    // 时间戳处理
    char updateTime[9];
    SAFE_STRNCPY(updateTime, sizeof(updateTime), pData->UpdateTime, sizeof(updateTime) - 1);
    updateTime[8] = '\0';
    data.updateTime = updateTime;
    data.updateMillisec = pData->UpdateMillisec;
    
    // 价格信息
    data.lastPrice = pData->LastPrice;
    data.openPrice = pData->OpenPrice;
    data.highPrice = pData->HighestPrice;
    data.lowPrice = pData->LowestPrice;
    data.closePrice = pData->ClosePrice;
    data.preClosePrice = pData->PreClosePrice;
    data.upperLimit = pData->UpperLimitPrice;
    data.lowerLimit = pData->LowerLimitPrice;
    
    // 成交量和持仓量
    data.volume = pData->Volume;
    data.turnover = pData->Turnover;
    data.openInterest = pData->OpenInterest;
    data.preOpenInterest = pData->PreOpenInterest;
    
    // 深度行情数据
    const double* bidPrices[] = {&pData->BidPrice1, &pData->BidPrice2, &pData->BidPrice3, &pData->BidPrice4, &pData->BidPrice5};
    const int* bidVolumes[] = {&pData->BidVolume1, &pData->BidVolume2, &pData->BidVolume3, &pData->BidVolume4, &pData->BidVolume5};
    const double* askPrices[] = {&pData->AskPrice1, &pData->AskPrice2, &pData->AskPrice3, &pData->AskPrice4, &pData->AskPrice5};
    const int* askVolumes[] = {&pData->AskVolume1, &pData->AskVolume2, &pData->AskVolume3, &pData->AskVolume4, &pData->AskVolume5};
    
    for (int i = 0; i < 5; ++i) {
        data.bidPrice[i] = *bidPrices[i];
        data.bidVolume[i] = *bidVolumes[i];
        data.askPrice[i] = *askPrices[i];
        data.askVolume[i] = *askVolumes[i];
    }
    
    return data;
}