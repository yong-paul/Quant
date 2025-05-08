#pragma once
#include "IQuoteDataApi.h"
#include "include/CTP/ThostFtdcMdApi.h"

class CTPQuoteDataApi : public IQuoteDataApi, public CThostFtdcMdSpi
{
	
public:
	bool Init(const std::string configfile) override;


	void Release() override;


	bool Connect() override;


	bool Disconnect() override;


	bool IsConnected() override;

};



