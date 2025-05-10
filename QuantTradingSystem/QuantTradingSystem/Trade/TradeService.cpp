#include "TradeService.h"
#include "ITradeFeed.h"
#include "../Events/AllEvents.h"
#include "../Events/StrategySignalEvent.h"
#include <memory>
#include <stdexcept>
#include <functional>

TradeService::TradeService(std::shared_ptr<EventManager> eventManager)
    : eventManager_(eventManager), 
      tradeFeed_(nullptr),
      running_(false) {
}

TradeService::~TradeService() {
    Stop();
}

bool TradeService::Init(const std::string& provider, const std::string& config) {
    try {
        // 使用工厂创建交易接口
        tradeFeed_ = TradeFeedFactory::CreateTradeFeed(provider);
        providerName_ = provider;
        
        if (!tradeFeed_) {
            return false;
        }
        
        // 初始化交易接口
        if (!tradeFeed_->Init(config)) {
            return false;
        }
        
        // 设置回调函数
        tradeFeed_->SetOrderCallback(std::bind(&TradeService::OnOrder, this, std::placeholders::_1));
        tradeFeed_->SetTradeCallback(std::bind(&TradeService::OnTrade, this, std::placeholders::_1));
        tradeFeed_->SetPositionCallback(std::bind(&TradeService::OnPosition, this, std::placeholders::_1));
        tradeFeed_->SetAccountCallback(std::bind(&TradeService::OnAccount, this, std::placeholders::_1));
        
        return true;
    }
    catch (const std::exception& e) {
        // 处理异常
        return false;
    }
}

bool TradeService::Start() {
    if (!tradeFeed_) {
        return false;
    }
    
    // 连接交易接口
    if (!tradeFeed_->Connect()) {
        return false;
    }
    
    running_ = true;
    return true;
}

void TradeService::Stop() {
    if (tradeFeed_ && running_) {
        tradeFeed_->Disconnect();
        tradeFeed_->Release();
        running_ = false;
    }
}

bool TradeService::Login(const std::string& userId, const std::string& password) {
    if (!tradeFeed_ || !running_) {
        return false;
    }
    
    return tradeFeed_->Login(userId, password);
}

bool TradeService::Logout() {
    if (!tradeFeed_ || !running_) {
        return false;
    }
    
    return tradeFeed_->Logout();
}

bool TradeService::ProcessSignal(const std::shared_ptr<StrategySignalEvent>& signal) {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        return false;
    }
    
    try {
        // 创建订单
        trade::OrderData orderData = CreateOrderFromSignal(signal->getData());
        
        // 发送订单
        std::string orderId = tradeFeed_->PlaceOrder(orderData);
        
        return !orderId.empty();
    }
    catch (const std::exception& e) {
        // 处理异常
        return false;
    }
}

std::string TradeService::PlaceOrder(const trade::OrderData& orderData) {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        return "";
    }
    
    return tradeFeed_->PlaceOrder(orderData);
}

bool TradeService::CancelOrder(const std::string& orderId) {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        return false;
    }
    
    return tradeFeed_->CancelOrder(orderId);
}

bool TradeService::RefreshData() {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        return false;
    }
    
    // 查询并更新持仓
    auto positions = tradeFeed_->QueryPositions();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        positions_.clear();
        for (const auto& pos : positions) {
            std::string key = pos.symbol + ":" + (pos.direction == trade::OrderDirection::Buy ? "Long" : "Short");
            positions_[key] = pos;
        }
    }
    
    // 查询并更新账户
    accountData_ = tradeFeed_->QueryAccount();
    
    return true;
}

std::vector<trade::OrderData> TradeService::QueryPendingOrders() const {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        return {};
    }
    
    return tradeFeed_->QueryPendingOrders();
}

trade::OrderData TradeService::QueryOrder(const std::string& orderId) const {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        return {};
    }
    
    return tradeFeed_->QueryOrder(orderId);
}

std::vector<trade::PositionData> TradeService::QueryPositions() const {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        std::vector<trade::PositionData> result;
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : positions_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    return tradeFeed_->QueryPositions();
}

trade::AccountData TradeService::QueryAccount() const {
    if (!tradeFeed_ || !running_ || !tradeFeed_->IsLoggedIn()) {
        std::lock_guard<std::mutex> lock(mutex_);
        return accountData_;
    }
    
    return tradeFeed_->QueryAccount();
}

bool TradeService::IsConnected() const {
    return tradeFeed_ && tradeFeed_->IsConnected();
}

bool TradeService::IsLoggedIn() const {
    return tradeFeed_ && tradeFeed_->IsLoggedIn();
}

std::string TradeService::GetProviderName() const {
    return providerName_;
}

void TradeService::OnOrder(const trade::OrderData& data) {
    if (!eventManager_) {
        return;
    }
    
    // 转换为订单事件并发布
    auto event = ConvertToOrderEvent(data);
    eventManager_->addEvent(event);
}

void TradeService::OnTrade(const trade::TradeData& data) {
    if (!eventManager_) {
        return;
    }
    
    // 转换为成交事件并发布
    auto event = ConvertToTradeEvent(data);
    eventManager_->addEvent(event);
}

void TradeService::OnPosition(const trade::PositionData& data) {
    if (!eventManager_) {
        return;
    }
    
    // 更新本地持仓缓存
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = data.symbol + ":" + (data.direction == trade::OrderDirection::Buy ? "Long" : "Short");
    positions_[key] = data;
}

void TradeService::OnAccount(const trade::AccountData& data) {
    if (!eventManager_) {
        return;
    }
    
    // 更新本地账户缓存
    std::lock_guard<std::mutex> lock(mutex_);
    accountData_ = data;
}

trade::OrderData TradeService::CreateOrderFromSignal(const StrategySignalData& signalData) {
    trade::OrderData orderData;
    
    orderData.symbol = signalData.symbol;
    orderData.volume = signalData.volume;
    
    // 根据信号类型设置买卖方向和开平标志
    if (signalData.signalType == SignalType::OPEN_LONG) {
        orderData.direction = trade::OrderDirection::Buy;
        orderData.offset = trade::OrderOffset::Open;
    }
    else if (signalData.signalType == SignalType::CLOSE_LONG) {
        orderData.direction = trade::OrderDirection::Sell;
        orderData.offset = trade::OrderOffset::Close;
    }
    else if (signalData.signalType == SignalType::OPEN_SHORT) {
        orderData.direction = trade::OrderDirection::Sell;
        orderData.offset = trade::OrderOffset::Open;
    }
    else if (signalData.signalType == SignalType::CLOSE_SHORT) {
        orderData.direction = trade::OrderDirection::Buy;
        orderData.offset = trade::OrderOffset::Close;
    }
    
    // 设置价格类型和价格
    orderData.priceType = trade::OrderPriceType::Limit;  // 默认使用限价单
    orderData.price = signalData.price;
    
    // 设置策略ID
    orderData.strategyId = signalData.strategyId;
    
    return orderData;
}

std::shared_ptr<OrderEvent> TradeService::ConvertToOrderEvent(const trade::OrderData& data) {
    // 创建Events/OrderEvent.h中定义的OrderData结构
    ::OrderData eventData;  // 使用全局命名空间的OrderData
    
    eventData.orderId = data.orderId;
    eventData.symbol = data.symbol;
    eventData.strategyId = data.strategyId;
    
    // 转换方向
    eventData.direction = (data.direction == trade::OrderDirection::Buy) ? 
                           ::OrderDirection::BUY : ::OrderDirection::SELL;
    
    // 转换类型
    eventData.type = (data.priceType == trade::OrderPriceType::Limit) ? 
                      ::OrderType::LIMIT : ::OrderType::MARKET;
    
    // 转换状态
    switch (data.status) {
        case trade::OrderStatus::Submitting:
            eventData.status = ::OrderStatus::SUBMITTED;
            break;
        case trade::OrderStatus::Accepted:
            eventData.status = ::OrderStatus::ACCEPTED;
            break;
        case trade::OrderStatus::PartialFilled:
            eventData.status = ::OrderStatus::PARTIAL;
            break;
        case trade::OrderStatus::Filled:
            eventData.status = ::OrderStatus::FILLED;
            break;
        case trade::OrderStatus::Canceled:
            eventData.status = ::OrderStatus::CANCELLED;
            break;
        case trade::OrderStatus::Rejected:
            eventData.status = ::OrderStatus::REJECTED;
            break;
        default:
            eventData.status = ::OrderStatus::SUBMITTED;
            break;
    }
    
    eventData.price = data.price;
    eventData.volume = data.volume;
    eventData.tradedVolume = data.tradedVolume;
    eventData.createTime = data.insertTime;
    eventData.updateTime = data.updateTime;
    
    return std::make_shared<OrderEvent>(eventData);
}

std::shared_ptr<TradeEvent> TradeService::ConvertToTradeEvent(const trade::TradeData& data) {
    // 创建Events/TradeEvent.h中定义的TradeData结构
    ::TradeData eventData;  // 使用全局命名空间的TradeData
    
    eventData.tradeId = data.tradeId;
    eventData.orderId = data.orderId;
    eventData.symbol = data.symbol;
    eventData.strategyId = ""; // 在trade::TradeData中可能没有策略ID
    
    // 转换方向
    eventData.direction = (data.direction == trade::OrderDirection::Buy) ? 
                           ::OrderDirection::BUY : ::OrderDirection::SELL;
    
    eventData.price = data.price;
    eventData.volume = data.volume;
    eventData.commission = 0.0;  // 假设没有佣金数据
    eventData.tradeTime = data.tradeTime;
    
    return std::make_shared<TradeEvent>(eventData);
} 