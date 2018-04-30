/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctNonCopyable.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：
**
**
***************************************************************************/

#pragma once

namespace Nct
{

	class INonCopyable
	{
	protected:
		INonCopyable() = default;
		~INonCopyable() = default;

	private:
		INonCopyable(const INonCopyable&) = delete;
		INonCopyable& operator=(const INonCopyable&) = delete;

	};	
}
