/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctStackAlloc.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：
**
**
***************************************************************************/

#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{
	
	class ProfilerAlloc
	{};

	
	template<>
	class MemoryAllocator<ProfilerAlloc> : public MemoryAllocatorBase
	{
	public:
		
		static void* allocate(size_t bytes)
		{
			return malloc(bytes);
		}

		
		static void free(void* ptr)
		{
			::free(ptr);
		}
	};
}