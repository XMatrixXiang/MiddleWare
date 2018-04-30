
/**************************************************************************
**  Copyright (C) 2002-2018 北京捷安申谋军工科技有限公司, All Rights Reserved
**
**  文件名：  NctDecodeTaskBase.h
**  创建者：  闫相伟
**  创建日期：2018年4月27日
**  详细信息：解码任务基础类
**
**
***************************************************************************/


#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Task/NctPersistentTaskBase.h"
#include "Message/NctMsgBuffer.h"

namespace Nct
{

	class DecodeMsgTask : public PersistentTaskBase
	{
	public:
		DecodeMsgTask();

		virtual ~ DecodeMsgTask();
	public:
		virtual bool getMsg() = 0;

		virtual void  decodeMsg() = 0;

	private:
		void run();

	protected:		
		std::shared_ptr<MsgBuffer> mMsgBuf;		
	};

}