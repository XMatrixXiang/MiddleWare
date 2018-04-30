/**************************************************************************
**  Copyright (C) 2002-2018 �����ݰ���ı�����Ƽ����޹�˾, All Rights Reserved
**
**  �ļ�����  NctPersistentTaskBase.h
**  �����ߣ�  ����ΰ
**  �������ڣ�2018��4��27��
**  ��ϸ��Ϣ���־û��������
**
**
***************************************************************************/

#pragma once


#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Error/NctException.h"
#include "Task/NctTaskBase.h"
#include "Message/NctMsgBuffer.h"

namespace Nct
{

	class PresistentBaseException : public Exception
	{
	public:
		PresistentBaseException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("PresistentBaseException", inDescription, inSource, inFile, inLine) {}
	};


	class PersistentTaskBase :  public TaskBase
	{	
		
	public:
		PersistentTaskBase(const std::uint32_t threadSleep = 15)		    
		    : mThreadSleep(threadSleep){}
		
		virtual std::function<void(void)> getRunTaskFunction() override
		{
			std::function<void(void)> retFun = std::bind(&PersistentTaskBase::run, this);
			return retFun;
		}

		virtual void run() = 0;
			
	protected:								
		std::uint32_t mThreadSleep;
	};

#define PERSISTENT_TASK_BASE_ASSERT(condition,expectionDes ) \
	 if(!(condition)) \
	{ \
      NCT_EXCEPT(PresistentBaseException, expectionDes)); \
	}
}