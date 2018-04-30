
/**************************************************************************
**  Copyright (C) 2002-2018 北京捷安申谋军工科技有限公司, All Rights Reserved
**
**  文件名：  NctStaticAlloc.h
**  创建者：  闫相伟
**  创建日期：2018年4月27日
**  详细信息：内存开辟 
**
**
***************************************************************************/

#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Allocators/NctFreeAlloc.h"
#include "Allocators/NctFrameAlloc.h"

namespace Nct
{
	
	template<int BlockSize = 512, class DynamicAllocator = TFrameAlloc<BlockSize>>
	class StaticAlloc
	{
	private:		
		class MemBlock
		{
		public:
			MemBlock( std::uint8_t * data, std::uint32_t size) 
				:mData(data), mSize(size)
			{ }
			
			std::uint8_t* alloc(std::uint32_t amount)
			{
				std::uint8_t* freePtr = &mData[mFreePtr];
				mFreePtr += amount;

				return freePtr;
			}


			void free(std::uint8_t* data, std::uint32_t allocSize)
			{
				if((data + allocSize) == (mData + mFreePtr))
					mFreePtr -= allocSize;
			}
			
			void clear()
			{
				mFreePtr = 0;
			}

			std::uint8_t* mData = nullptr;
			std::uint32_t mFreePtr = 0;
			std::uint32_t mSize = 0;
			MemBlock* mNextBlock = nullptr;
		};

	public:
		StaticAlloc() = default;
		~StaticAlloc() = default;

		std::uint8_t* alloc(std::uint32_t amount)
		{
			if (amount == 0)
				return nullptr;


			std::uint32_t freeMem = BlockSize - mFreePtr;
			
			std::uint8_t* data;
			if (amount > freeMem)
				data = mDynamicAlloc.alloc(amount);
			else
			{
				data = &mStaticData[mFreePtr];
				mFreePtr += amount;
			}

			return data;
		}
		
		void free(void* data, std::uint32_t allocSize)
		{
			if (data == nullptr)
				return;

			std::uint8_t* dataPtr = (std::uint8_t*)data;

			if(data > mStaticData && data < (mStaticData + BlockSize))
			{
				if((((std::uint8_t*)data) + allocSize) == (mStaticData + mFreePtr))
					mFreePtr -= allocSize;
			}
			else
				mDynamicAlloc.free(dataPtr);
		}
		
		void free(void* data)
		{
			if (data == nullptr)
				return;

			std::uint8_t* dataPtr = (std::uint8_t*)data;

			if(data < mStaticData || data >= (mStaticData + BlockSize))
				mDynamicAlloc.free(dataPtr);
		}
				
		 
		template<class T>
		T* construct(std::uint32_t count = 0)
		{
			T* data = (T*)alloc(sizeof(T) * count);

			for(unsigned int i = 0; i < count; i++)
				new ((void*)&data[i]) T;

			return data;
		}


		template<class T, class... Args>
		T* construct(Args &&...args, std::uint32_t count = 0)
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

			free(data, sizeof(T));
		}
		
		template<class T>
		void destruct(T* data, std::uint32_t count)
		{
			for(unsigned int i = 0; i < count; i++)
				data[i].~T();

			free(data, sizeof(T) * count);
		}

		
		void clear()
		{
			assert(mTotalAllocBytes == 0);

			mFreePtr = 0;
			mDynamicAlloc.clear();
		}

	private:
		std::uint8_t mStaticData[BlockSize];
		std::uint32_t mFreePtr = 0;
		DynamicAllocator mDynamicAlloc;

		std::uint32_t mTotalAllocBytes = 0;
	};

	
	template <int BlockSize, class T>
	class StdStaticAlloc
	{
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		StdStaticAlloc() = default; 

		StdStaticAlloc(StaticAlloc<BlockSize, FreeAlloc>* alloc) noexcept
			:mStaticAlloc(alloc)
		{ }

		template<class U> StdStaticAlloc(const StdStaticAlloc<BlockSize, U>& alloc) noexcept
			:mStaticAlloc(alloc.mStaticAlloc)
		{ }

		template<class U> class rebind { public: typedef StdStaticAlloc<BlockSize, U> other; };

		
		T* allocate(const size_t num) const
		{
			if (num == 0)
				return nullptr;

			if (num > static_cast<size_t>(-1) / sizeof(T))
				return nullptr; 

			void* const pv = mStaticAlloc->alloc((std::uint32_t)(num * sizeof(T)));
			if (!pv)
				return nullptr; 

			return static_cast<T*>(pv);
		}

		
		void deallocate(T* p, size_t num) const noexcept
		{
			mStaticAlloc->free((std::uint8_t*)p, (std::uint32_t)num);
		}

		StaticAlloc<BlockSize, FreeAlloc>* mStaticAlloc = nullptr;

		size_t max_size() const { return std::numeric_limits<std::uint32_t>::max() / sizeof(T); }
		void construct(pointer p, const_reference t) { new (p) T(t); }
		void destroy(pointer p) { p->~T(); }
		template<class U, class... Args>
		void construct(U* p, Args&&... args) { new(p) U(std::forward<Args>(args)...); }

		template <class T1, int N1, class T2, int N2>
		friend bool operator== (const StdStaticAlloc<N1, T1>& a, const StdStaticAlloc<N2, T2>& b) throw();
	
	};

	template <class T1, int N1, class T2, int N2>
	bool operator== (const StdStaticAlloc<N1, T1>& a, const StdStaticAlloc<N2, T2>& b) throw() 
	{
		return N1 == N2 && a.mStaticAlloc == b.mStaticAlloc;
	}
	
	template <class T1, int N1, class T2, int N2>
	bool operator!= (const StdStaticAlloc<N1, T1>& a, const StdStaticAlloc<N2, T2>& b) throw() 
	{
		return !(a == b);
	}

	template <typename T, int Count> 
	using StaticVector = std::vector<T, StdStaticAlloc<sizeof(T) * Count, T>>;
	
}
