#pragma once

#include <string>
#include <memory>
class IQuoteDataApi
{
public:
	virtual ~IQuoteDataApi() {}

public:
	/*
	 *	��ʼ������ģ��
	 *	@config	ģ������
	 *	����ֵ	�Ƿ��ʼ���ɹ�
	 */
	virtual bool Init(const std::string config) { return false; }

	/*
	 *	�ͷŽ���ģ��
	 *	�����˳�ʱ
	 */
	virtual void Release() {}

	/*
	 *	��ʼ���ӷ�����
	 *	@����ֵ	���������Ƿ��ͳɹ�
	 */
	virtual bool Connect() { return false; }

	/*
	 *	�Ͽ�����
	 *	@����ֵ	�����Ƿ��ͳɹ�
	 */
	virtual bool Disconnect() { return false; }

	/*
	 *	�Ƿ�������
	 *	@����ֵ	�Ƿ�������
	 */
	virtual bool IsConnected() { return false; }

	///*
	// *	���ĺ�Լ�б�
	// */
	//virtual void subscribe(const & setCodes) {}

	///*
	// *	�˶���Լ�б�
	// */
	//virtual void unsubscribe(const CodeSet& setCodes) {}
};

