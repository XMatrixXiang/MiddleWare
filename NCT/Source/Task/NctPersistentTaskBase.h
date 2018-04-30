/**************************************************************************
**  Copyright (C) 2002-2018 北京捷安申谋军工科技有限公司, All Rights Reserved
**
**  文件名：  NctPersistentTaskBase.h
**  创建者：  闫相伟
**  创建日期：2018年4月27日
**  详细信息：持久化任务基类
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