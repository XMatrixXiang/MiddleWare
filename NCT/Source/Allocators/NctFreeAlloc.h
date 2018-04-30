/**************************************************************************
**  Copyright (C) 2002-2018 北京捷安申谋军工科技有限公司, All Rights Reserved
**
**  文件名：  NctFreeAlloc.h
**  创建者：  闫相伟
**  创建日期：2018年4月27日
**  详细信息：内存释放
**
**
***************************************************************************/
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"

namespace Nct
{

	class FreeAlloc
	{
	public:		
		std::uint8_t * alloc(std::uint32_t amount)
		{
			return (std::uint8_t*)malloc(amount);
		}
		
		void free(void* data)
		{
			::free(data);
		}
		
		void clear()
		{

		}
	};
}
