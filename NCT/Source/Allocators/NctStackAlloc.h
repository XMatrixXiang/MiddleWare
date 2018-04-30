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
#include "Threading/NctThreadDefines.h"
#include "Allocators/NctMemoryAllocator.h"

namespace Nct
{
	
	template <int BlockCapacity = 1024 * 1024>
	class MemStackInternal
	{
	private:
		
		class MemBlock
		{
		public:
			MemBlock(std::uint32_t size) :mSize(size) { }

			~MemBlock() = default;

		
			std::uint8_t* alloc(std::uint32_t amount)
			{
				std::uint8_t* freePtr = &mData[mFreePtr];
				mFreePtr += amount;

				return freePtr;
			}			
			void dealloc(std::uint8_t* data, std::uint32_t amount)
			{
				mFreePtr -= amount;
				assert((&mData[mFreePtr]) == data && "Out of order stack deallocation detected. Deallocations need to happen in order opposite of allocations.");
			}

			std::uint8_t* mData = nullptr;
			std::uint32_t mFreePtr = 0;
			std::uint32_t mSize = 0;
			MemBlock* mNextBlock = nullptr;
			MemBlock* mPrevBlock = nullptr;
		};

	public:
		MemStackInternal()
		{
			mFreeBlock = allocBlock(BlockCapacity);
		}

		~MemStackInternal()
		{
			assert(mFreeBlock->mFreePtr == 0 && "Not all blocks were released before shutting down the stack allocator.");

			MemBlock* curBlock = mFreeBlock;
			while (curBlock != nullptr)
			{
				MemBlock* nextBlock = curBlock->mNextBlock;
				deallocBlock(curBlock);

				curBlock = nextBlock;
			}
		}

		std::uint8_t* alloc(std::uint32_t amount)
		{
			amount += sizeof(std::uint32_t);

			std::uint32_t freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;
			if(amount > freeMem)
				allocBlock(amount);

			std::uint8_t* data = mFreeBlock->alloc(amount);

			std::uint32_t* storedSize = reinterpret_cast<std::uint32_t*>(data);
			*storedSize = amount;

			return data + sizeof(std::uint32_t);
		}		
		void dealloc(std::uint8_t* data)
		{
			data -= sizeof(std::uint32_t);

			std::uint32_t* storedSize = reinterpret_cast<std::uint32_t*>(data);
			mFreeBlock->dealloc(data, *storedSize);

			if (mFreeBlock->mFreePtr == 0)
			{
				MemBlock* emptyBlock = mFreeBlock;

				if (emptyBlock->mPrevBlock != nullptr)
					mFreeBlock = emptyBlock->mPrevBlock;
				
				if (emptyBlock->mNextBlock != nullptr)
				{
					std::uint32_t totalSize = emptyBlock->mSize + emptyBlock->mNextBlock->mSize;

					if (emptyBlock->mPrevBlock != nullptr)
						emptyBlock->mPrevBlock->mNextBlock = nullptr;
					else
						mFreeBlock = nullptr;

					deallocBlock(emptyBlock->mNextBlock);
					deallocBlock(emptyBlock);

					allocBlock(totalSize);
				}
			}
		}

	private:
		MemBlock* mFreeBlock = nullptr;
		
		MemBlock* allocBlock(std::uint32_t wantedSize)
		{
			std::uint32_t blockSize = BlockCapacity;
			if(wantedSize > blockSize)
				blockSize = wantedSize;

			MemBlock* newBlock = nullptr;
			MemBlock* curBlock = mFreeBlock;

			while (curBlock != nullptr)
			{
				MemBlock* nextBlock = curBlock->mNextBlock;
				if (nextBlock != nullptr && nextBlock->mSize >= blockSize)
				{
					newBlock = nextBlock;
					break;
				}

				curBlock = nextBlock;
			}

			if (newBlock == nullptr)
			{
				std::uint8_t* data = (std::uint8_t*)reinterpret_cast<std::uint8_t*>(nct_alloc(blockSize + sizeof(MemBlock)));
				newBlock = new (data)MemBlock(blockSize);
				data += sizeof(MemBlock);

				newBlock->mData = data;
				newBlock->mPrevBlock = mFreeBlock;

				if (mFreeBlock != nullptr)
				{
					if(mFreeBlock->mNextBlock != nullptr)
						mFreeBlock->mNextBlock->mPrevBlock = newBlock;

					newBlock->mNextBlock = mFreeBlock->mNextBlock;
					mFreeBlock->mNextBlock = newBlock;
				}
			}

			mFreeBlock = newBlock;
			return newBlock;
		}		
		void deallocBlock(MemBlock* block)
		{
			block->~MemBlock();
			nct_free(block);
		}
	};

	class MemStack
	{
	public:
		
		static  void beginThread();

	
		static  void endThread();
		
		static  std::uint8_t* alloc(std::uint32_t amount);

		
		static  void deallocLast(std::uint8_t* data);

	private:
		static NCT_THREADLOCAL MemStackInternal<1024 * 1024>* ThreadMemStack;
	};

	inline void* nct_stack_alloc(std::uint32_t amount)
	{
		return (void*)MemStack::alloc(amount);
	}

	template<class T>
	T* nct_stack_alloc()
	{
		return (T*)MemStack::alloc(sizeof(T));
	}


	template<class T>
	T* nct_stack_alloc(std::uint32_t amount)
	{
		return (T*)MemStack::alloc(sizeof(T) * amount);
	}

	template<class T>
	T* nct_stack_new(std::uint32_t count = 0)
	{
		T* data = nct_stack_alloc<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T;

		return data;
	}

	template<class T, class... Args>
	T* nct_stack_new(Args &&...args, std::uint32_t count = 0)
	{
		T* data = nct_stack_alloc<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T(std::forward<Args>(args)...);

		return data;
	}
	template<class T>
	void nct_stack_delete(T* data)
	{
		data->~T();

		MemStack::deallocLast((std::uint8_t*)data);
	}

	template<class T>
	void nct_stack_delete(T* data, std::uint32_t count)
	{
		for(unsigned int i = 0; i < count; i++)
			data[i].~T();

		MemStack::deallocLast((std::uint8_t*)data);
	}
	
	inline void nct_stack_free(void* data)
	{
		return MemStack::deallocLast((std::uint8_t*)data);
	}

	
	class StackAlloc
	{ };

	template<>
	class MemoryAllocator<StackAlloc> : public MemoryAllocatorBase
	{
	public:
		static void* allocate(size_t bytes)
		{
			return nct_stack_alloc((std::uint32_t)bytes);
		}

		static void free(void* ptr)
		{
			nct_stack_free(ptr);
		}
	};
}
