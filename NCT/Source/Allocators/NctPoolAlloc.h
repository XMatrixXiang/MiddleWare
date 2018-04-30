/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctPoolAlloc.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：
**
**
***************************************************************************/

#pragma once

#include "Prerequisites/NctPrerequisitesUtil.h"
#include <climits>

namespace Nct
{
	
	template <int ElemSize, int ElemsPerBlock = 512, int Alignment = 4>
	class PoolAlloc
	{
	private:		
		class MemBlock
		{
		public:
			MemBlock(std::uint8_t * blockData)
				:blockData(blockData), freePtr(0), freeElems(ElemsPerBlock), nextBlock(nullptr)
			{
				std::uint32_t offset = 0;
				for(std::uint32_t i = 0; i < ElemsPerBlock; i++)
				{
					std::uint32_t* entryPtr = (std::uint32_t*)&blockData[offset];

					offset += ActualElemSize;
					*entryPtr = offset;
				}
			}

			~MemBlock()
			{
				assert(freeElems == ElemsPerBlock && "Not all elements were deallocated from a block.");
			}

		
			std::uint8_t* alloc()
			{
				std::uint8_t* freeEntry = &blockData[freePtr];
				freePtr = *(UINT32*)freeEntry;
				--freeElems;

				return freeEntry;
			}
			
			void dealloc(void* data)
			{
				std::uint32_t* entryPtr = (std::uint32_t*)data;
				*entryPtr = freePtr;
				++freeElems;

				freePtr = (std::uint32_t)(((std::uint8_t*)data) - blockData);
			}

			std::uint8_t* blockData;
			std::uint32_t freePtr;
			std::uint32_t freeElems;
			MemBlock* nextBlock;
		};

	public:
		PoolAlloc()
		{
			static_assert(ElemSize >= 4, "Pool allocator minimum allowed element size is 4 bytes.");
			static_assert(ElemsPerBlock > 0, "Number of elements per block must be at least 1.");
			static_assert(ElemsPerBlock * ActualElemSize <= UINT_MAX, "Pool allocator block size too large.");
		}

		~PoolAlloc()
		{
			MemBlock* curBlock = mFreeBlock;
			while (curBlock != nullptr)
			{
				MemBlock* nextBlock = curBlock->nextBlock;
				deallocBlock(curBlock);

				curBlock = nextBlock;
			}
		}
		
		std::uint8_t* alloc()
		{
			if(mFreeBlock == nullptr || mFreeBlock->freeElems == 0)
				allocBlock();

			mTotalNumElems++;
			return mFreeBlock->alloc();
		}
		
		void free(void* data)
		{
			MemBlock* curBlock = mFreeBlock;
			while(curBlock)
			{
				constexpr UINT32 blockDataSize = ActualElemSize * ElemsPerBlock;
				if(data >= curBlock->blockData && data < (curBlock->blockData + blockDataSize))
				{
					curBlock->dealloc(data);
					mTotalNumElems--;

					if(curBlock->freeElems == 0 && curBlock->nextBlock)
					{						
						const UINT32 totalSpace = (mNumBlocks - 1) * ElemsPerBlock;
						const UINT32 freeSpace = totalSpace - mTotalNumElems;

						if(freeSpace > ElemsPerBlock / 2)
						{
							mFreeBlock = curBlock->nextBlock;
							deallocBlock(curBlock);
						}
					}

					return;
				}

				curBlock = curBlock->nextBlock;
			}

			assert(false);
		}
		
		template<class T, class... Args>
		T* construct(Args &&...args)
		{
			T* data = (T*)alloc();
			new ((void*)data) T(std::forward<Args>(args)...);

			return data;
		}
		
		template<class T>
		void destruct(T* data)
		{
			data->~T();
			free(data);
		}

	private:		
		MemBlock* allocBlock()
		{
			MemBlock* newBlock = nullptr;
			MemBlock* curBlock = mFreeBlock;

			while (curBlock != nullptr)
			{
				MemBlock* nextBlock = curBlock->nextBlock;
				if (nextBlock != nullptr && nextBlock->freeElems > 0)
				{					
					newBlock = nextBlock;

					curBlock->nextBlock = newBlock->nextBlock;
					newBlock->nextBlock = mFreeBlock;

					break;
				}

				curBlock = nextBlock;
			}

			if (newBlock == nullptr)
			{
				constexpr std::uint32_t blockDataSize = ActualElemSize * ElemsPerBlock;
				size_t paddedBlockDataSize = blockDataSize + (Alignment - 1); 

				std::uint8_t* data = (std::uint8_t*)bs_alloc(sizeof(MemBlock) + (std::uint32_t)paddedBlockDataSize);

				void* blockData = data + sizeof(MemBlock);
				blockData = std::align(Alignment, blockDataSize, blockData, paddedBlockDataSize);

				newBlock = new (data) MemBlock((std::uint8_t*)blockData);
				mNumBlocks++;

				newBlock->nextBlock = mFreeBlock;
			}

			mFreeBlock = newBlock;
			return newBlock;
		}
		
		void deallocBlock(MemBlock* block)
		{
			block->~MemBlock();
			bs_free(block);

			mNumBlocks--;
		}

		static constexpr int ActualElemSize = ((ElemSize + Alignment - 1) / Alignment) * Alignment;

		MemBlock* mFreeBlock = nullptr;
		std::uint32_t mTotalNumElems = 0;
		std::uint32_t mNumBlocks = 0;
	};
}
