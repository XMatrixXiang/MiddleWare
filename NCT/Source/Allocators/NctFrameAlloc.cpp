
#include "Allocators/NctMemoryAllocator.h"
#include "Error/NctException.h"

namespace Nct
{
	std::uint8_t* FrameAlloc::MemBlock::alloc(std::uint32_t amount)
	{
		std::uint8_t* freePtr = &mData[mFreePtr];
		mFreePtr += amount;

		return freePtr;
	}

	void FrameAlloc::MemBlock::clear()
	{
		mFreePtr = 0;
	}


	FrameAlloc::FrameAlloc(std::uint32_t blockSize)
		: mBlockSize(blockSize), mFreeBlock(nullptr), mNextBlockIdx(0), mTotalAllocBytes(0), mLastFrame(nullptr)
	{
	}

	FrameAlloc::~FrameAlloc()
	{
		for(auto& block : mBlocks)
			deallocBlock(block);
	}

	std::uint8_t* FrameAlloc::alloc(std::uint32_t amount)
	{

		std::uint32_t freeMem = 0;
		if(mFreeBlock != nullptr)
			freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;

		if(amount > freeMem)
			allocBlock(amount);

		std::uint8_t* data = mFreeBlock->alloc(amount);

		return data;
	}

	std::uint8_t* FrameAlloc::allocAligned(std::uint32_t amount, std::uint32_t alignment)
	{
		std::uint32_t freeMem = 0;
		std::uint32_t freePtr = 0;
		if(mFreeBlock != nullptr)
		{
			freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;
			freePtr = mFreeBlock->mFreePtr;
		}

		std::uint32_t alignOffset = (alignment - (freePtr & (alignment - 1))) & (alignment - 1);
		if ((amount + alignOffset) > freeMem)
		{

			if (alignment > 16)
				alignOffset = alignment - 16;
			else
				alignOffset = 0;

			allocBlock(amount + alignOffset);
		}

		amount += alignOffset;
		std::uint8_t* data = mFreeBlock->alloc(amount);

		return data + alignOffset;
	}

	void FrameAlloc::free(std::uint8_t* data)
	{
	}

	void FrameAlloc::markFrame()
	{
		void** framePtr = (void**)alloc(sizeof(void*));
		*framePtr = mLastFrame;
		mLastFrame = framePtr;
	}

	void FrameAlloc::clear()
	{
		if(mLastFrame != nullptr)
		{
			assert(mBlocks.size() > 0 && mNextBlockIdx > 0);

			free((std::uint8_t*)mLastFrame);

			std::uint8_t* framePtr = (std::uint8_t*)mLastFrame;
			mLastFrame = *(void**)mLastFrame;


			std::uint32_t startBlockIdx = mNextBlockIdx - 1;
			std::uint32_t numFreedBlocks = 0;
			for (std::uint32_t i = startBlockIdx; i >= 0; i--)
			{
				MemBlock* curBlock = mBlocks[i];
				std::uint8_t* blockEnd = curBlock->mData + curBlock->mSize;
				if (framePtr >= curBlock->mData && framePtr < blockEnd)
				{
					std::uint8_t* dataEnd = curBlock->mData + curBlock->mFreePtr;
					std::uint32_t sizeInBlock = (std::uint32_t)(dataEnd - framePtr);
					assert(sizeInBlock <= curBlock->mFreePtr);

					curBlock->mFreePtr -= sizeInBlock;
					if (curBlock->mFreePtr == 0)
					{
						numFreedBlocks++;
						
						if (numFreedBlocks > 1)
							mNextBlockIdx = (std::uint32_t)i;
					}

					break;
				}
				else
				{
					curBlock->mFreePtr = 0;
					mNextBlockIdx = (std::uint32_t)i;
					numFreedBlocks++;
				}
			}

			if (numFreedBlocks > 1)
			{
				std::uint32_t totalBytes = 0;
				for (std::uint32_t i = 0; i < numFreedBlocks; i++)
				{
					MemBlock* curBlock = mBlocks[mNextBlockIdx];
					totalBytes += curBlock->mSize;

					deallocBlock(curBlock);
					mBlocks.erase(mBlocks.begin() + mNextBlockIdx);
				}
				
				std::uint32_t oldNextBlockIdx = mNextBlockIdx;
				allocBlock(totalBytes);
				
				if (oldNextBlockIdx > 0)
					mFreeBlock = mBlocks[oldNextBlockIdx - 1];
			}
			else
			{
				mFreeBlock = mBlocks[mNextBlockIdx - 1];
			}
		}
		else
		{

			if (mBlocks.size() > 1)
			{				
				std::uint32_t totalBytes = 0;
				for (auto& block : mBlocks)
				{
					totalBytes += block->mSize;
					deallocBlock(block);
				}

				mBlocks.clear();
				mNextBlockIdx = 0;

				allocBlock(totalBytes);
			}
			else if(mBlocks.size() > 0)
				mBlocks[0]->mFreePtr = 0;
		}
	}

	FrameAlloc::MemBlock* FrameAlloc::allocBlock(std::uint32_t wantedSize)
	{
		std::uint32_t blockSize = mBlockSize;
		if(wantedSize > blockSize)
			blockSize = wantedSize;

		MemBlock* newBlock = nullptr;
		while (mNextBlockIdx < mBlocks.size())
		{
			MemBlock* curBlock = mBlocks[mNextBlockIdx];
			if (blockSize <= curBlock->mSize)
			{
				newBlock = curBlock;
				mNextBlockIdx++;
				break;
			}
			else
			{				
				deallocBlock(curBlock);
				mBlocks.erase(mBlocks.begin() + mNextBlockIdx);
			}
		}

		if (newBlock == nullptr)
		{
			std::uint32_t alignOffset = 16 - (sizeof(MemBlock) & (16 - 1));

			std::uint8_t* data = (std::uint8_t*)reinterpret_cast<std::uint8_t*>(nct_alloc_aligned16(blockSize + sizeof(MemBlock) + alignOffset));
			newBlock = new (data) MemBlock(blockSize);
			data += sizeof(MemBlock) + alignOffset;
			newBlock->mData = data;

			mBlocks.push_back(newBlock);
			mNextBlockIdx++;
		}

		mFreeBlock = newBlock; 

		return newBlock;
	}

	void FrameAlloc::deallocBlock(MemBlock* block)
	{
		block->~MemBlock();
		nct_free_aligned16(block);
	}

	void FrameAlloc::setOwnerThread(NctThreadId thread)
	{
	}

	FrameAlloc* _GlobalFrameAlloc = nullptr;

	FrameAlloc& gFrameAlloc()
	{
		if (_GlobalFrameAlloc == nullptr)
		{			
			_GlobalFrameAlloc = new FrameAlloc();
		}

		return *_GlobalFrameAlloc;
	}

	std::uint8_t* nct_frame_alloc(std::uint32_t numBytes)
	{
		return gFrameAlloc().alloc(numBytes);
	}

	std::uint8_t* nct_frame_alloc_aligned(std::uint32_t count, std::uint32_t align)
	{
		return gFrameAlloc().allocAligned(count, align);
	}

	void nct_frame_free(void* data)
	{
		gFrameAlloc().free((std::uint8_t*)data);
	}

	void nct_frame_free_aligned(void* data)
	{
		gFrameAlloc().free((std::uint8_t*)data);
	}

	void nct_frame_mark()
	{
		gFrameAlloc().markFrame();
	}

	void nct_frame_clear()
	{
		gFrameAlloc().clear();
	}
}
