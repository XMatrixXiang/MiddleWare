/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctGroupAlloc.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：
**
**
***************************************************************************/
#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Allocators/NctMemoryAllocator.h"
#include "Allocators/NctNonCopyable.h"

namespace Nct
{
	
	class GroupAlloc : INonCopyable
	{
	public:
		GroupAlloc() = default;

		GroupAlloc(GroupAlloc&& other) noexcept
			: mData(other.mData), mDataPtr(other.mDataPtr), mNumBytes(other.mNumBytes)
		{
			other.mData = nullptr;
			other.mDataPtr = nullptr;
			other.mNumBytes = 0;
		}

		~GroupAlloc()
		{
			if (mNumBytes > 0)
				nct_free(mData);
		}

		GroupAlloc& operator=(GroupAlloc&& other) noexcept
		{
			if (this == &other)
				return *this;

			if (mNumBytes > 0)
				nct_free(mData);

			mData = other.mData;
			mDataPtr = other.mDataPtr;
			mNumBytes = other.mNumBytes;

			other.mData = nullptr;
			other.mDataPtr = nullptr;
			other.mNumBytes = 0;

			return *this;
		}

		void init()
		{
			assert(mData == nullptr);

			if (mNumBytes > 0)
				mData = (std::uint8_t*)nct_alloc(mNumBytes);

			mDataPtr = mData;
		}

		GroupAlloc& reserve(std::uint32_t amount)
		{
			assert(mData == nullptr);

			mNumBytes += amount;
			return *this;
		}

		template<class T>
		GroupAlloc& reserve(UINT32 count = 1)
		{
			assert(mData == nullptr);

			mNumBytes += sizeof(T) * count;
			return *this;
		}

		std::uint8_t* alloc(std::uint32_t amount)
		{
			assert(mDataPtr + amount <= (mData + mNumBytes));

			std::uint8_t* output = mDataPtr;
			mDataPtr += amount;

			return output;
		}

		template<class T>
		T* alloc(std::uint32_t count = 1)
		{
			return (T*)alloc(sizeof(T) * count);
		}
		
		void free(void* data)
		{
			
		}

		template<class T>
		T* construct(std::uint32_t count = 1)
		{
			T* data = (T*)alloc(sizeof(T) * count);

			for(unsigned int i = 0; i < count; i++)
				new ((void*)&data[i]) T;

			return data;
		}

		template<class T, class... Args>
		T* construct(Args &&...args, std::uint32_t count = 1)
		{
			T* data = (T*)alloc(sizeof(T) * count);

			for(unsigned int i = 0; i < count; i++)
				new ((void*)&data[i]) T(std::forward<Args>(args)...);

			return data;
		}
		
		template<class T>
		void destruct(T* data)
		{
			data->~T();

			free(data);
		}
		
		template<class T>
		void destruct(T* data, std::uint32_t count)
		{
			for(unsigned int i = 0; i < count; i++)
				data[i].~T();

			free(data);
		}

	private:
		std::uint8_t* mData = nullptr;
		std::uint8_t* mDataPtr = nullptr;
		std::uint32_t mNumBytes = 0;
	};
}
