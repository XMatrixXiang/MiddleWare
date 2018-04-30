


#include "Allocators/NctStackAlloc.h"
#include "Allocators/NctMemoryAllocator.h"

namespace Nct
{
	NCT_THREADLOCAL MemStackInternal<1024 * 1024>* MemStack::ThreadMemStack = nullptr;

	void MemStack::beginThread()
	{
		if(ThreadMemStack != nullptr)
			endThread();

		ThreadMemStack = nct_new<MemStackInternal<1024 * 1024>>();
	}

	void MemStack::endThread()
	{
		if(ThreadMemStack != nullptr)
		{
			nct_delete(ThreadMemStack);
			ThreadMemStack = nullptr;
		}
	}

	std::uint8_t* MemStack::alloc(std::uint32_t numBytes)
	{
		assert(ThreadMemStack != nullptr && "Stack allocation failed. Did you call beginThread?");

		return ThreadMemStack->alloc(numBytes);
	}

	void MemStack::deallocLast(std::uint8_t* data)
	{
		assert(ThreadMemStack != nullptr && "Stack deallocation failed. Did you call beginThread?");

		ThreadMemStack->dealloc(data);
	}
}
