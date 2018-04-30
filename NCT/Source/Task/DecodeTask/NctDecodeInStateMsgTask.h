
/**************************************************************************
**  Copyright (C) 2002-2018 北京捷安申谋军工科技有限公司, All Rights Reserved
**
**  文件名：  NctDecodeTaskBase.h
**  创建者：  闫相伟
**  创建日期：2018年4月27日
**  详细信息：解码实时收到的状态信息
**
**
***************************************************************************/


#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Task/DecodeTask/NctDecodeMsgTask.h"

namespace Nct
{

	class DecodeInStateMsgTask : public DecodeMsgTask
	{
	public:
		virtual bool getMsg() override;

		virtual void decodeMsg() override;

	};

}