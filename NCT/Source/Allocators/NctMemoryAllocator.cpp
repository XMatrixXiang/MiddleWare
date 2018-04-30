

#include "Allocators/NctMemoryAllocator.h"

namespace Nct
{
	std::uint64_t NCT_THREADLOCAL MemoryCounter::Allocs = 0;
	std::uint64_t NCT_THREADLOCAL MemoryCounter::Frees = 0;
}