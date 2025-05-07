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
	virtual bool init(const std::string config) { return false; }

	/*
	 *	�ͷŽ���ģ��
	 *	�����˳�ʱ
	 */
	virtual void release() {}

	/*
	 *	��ʼ���ӷ�����
	 *	@����ֵ	���������Ƿ��ͳɹ�
	 */
	virtual bool connect() { return false; }

	/*
	 *	�Ͽ�����
	 *	@����ֵ	�����Ƿ��ͳɹ�
	 */
	virtual bool disconnect() { return false; }

	/*
	 *	�Ƿ�������
	 *	@����ֵ	�Ƿ�������
	 */
	virtual bool isConnected() { return false; }

	///*
	// *	���ĺ�Լ�б�
	// */
	//virtual void subscribe(const & setCodes) {}

	///*
	// *	�˶���Լ�б�
	// */
	//virtual void unsubscribe(const CodeSet& setCodes) {}
};

