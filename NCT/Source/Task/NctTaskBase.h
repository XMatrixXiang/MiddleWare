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
#include "Threading/NctTaskScheduler.h"


namespace Nct
{

	class TaskBase
	{
	public:
		TaskBase() : mTerminate(false) {}
	public:
		virtual std::function<void(void)> getRunTaskFunction() = 0;
		virtual void terminate();
		virtual void reserveTask(std::shared_ptr<Task> task) { mTask = task; }
	protected:
		bool mTerminate;
		std::shared_ptr<Task> mTask;
	};
}