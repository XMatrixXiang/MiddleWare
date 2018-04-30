/**************************************************************************
**  Copyright (C) 20018-2019 北京捷安军工科技有限公司, All Rights Reserved
**
**  文件名：  NctMemoryAllocator.h
**  创建者：  闫相伟
**  创建日期：2018年4月26日
**  详细信息：
**
**
***************************************************************************/

#pragma once
#undef min
#undef max


#include "Prerequisites/NctPrerequisitesUtil.h"
#include "Threading/NctThreadDefines.h"

namespace Nct
{
	 class MemoryAllocatorBase;

	inline void* platformAlignedAlloc16(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	inline void platformAlignedFree16(void* ptr)
	{
		_aligned_free(ptr);
	}

	inline void* platformAlignedAlloc(size_t size, size_t alignment)
	{
		return _aligned_malloc(size, alignment);
	}

	inline void platformAlignedFree(void* ptr)
	{
		_aligned_free(ptr);
	}
	
	class MemoryCounter
	{
	public:
		static  uint64_t getNumAllocs()
		{
			return Allocs;
		}

		static uint64_t getNumFrees()
		{
			return Frees;
		}

	private:
		friend class MemoryAllocatorBase;
		
		static  void incAllocCount() { ++Allocs; }
		static  void incFreeCount() { ++Frees; }

		static NCT_THREADLOCAL std::uint64_t Allocs;
		static NCT_THREADLOCAL std::uint64_t Frees;
	};
	
	class MemoryAllocatorBase
	{
	protected:
		static void incAllocCount() { MemoryCounter::incAllocCount(); }
		static void incFreeCount() { MemoryCounter::incFreeCount(); }
	};
	
	template<class T>
	class MemoryAllocator : public MemoryAllocatorBase
	{
	public:		
		static void* allocate(size_t bytes)
		{
			return malloc(bytes);
		}
		static void* allocateAligned(size_t bytes, size_t alignment)
		{
			return platformAlignedAlloc(bytes, alignment);
		}
		
		static void* allocateAligned16(size_t bytes)
		{
			return platformAlignedAlloc16(bytes);
		}
		
		static void free(void* ptr)
		{
			::free(ptr);
		}

		static void freeAligned(void* ptr)
		{
			platformAlignedFree(ptr);
		}

		static void freeAligned16(void* ptr)
		{
			platformAlignedFree16(ptr);
		}
	};

	class GenAlloc
	{ };
	
	template<class Alloc>
	inline void* nct_alloc(size_t count)
	{
		return MemoryAllocator<Alloc>::allocate(count);
	}

	template<class T, class Alloc>
	inline T* nct_alloc()
	{
		return (T*)MemoryAllocator<Alloc>::allocate(sizeof(T));
	}
	
	template<class T, class Alloc>
	inline T* nct_newN(size_t count)
	{
		T* ptr = (T*)MemoryAllocator<Alloc>::allocate(sizeof(T) * count);

		for(size_t i = 0; i < count; ++i)
			new (&ptr[i]) T;

		return ptr;
	}
	
	template<class Type, class Alloc, class... Args>
	Type* nct_new(Args &&...args)
	{
		return new (nct_alloc<Type, Alloc>()) Type(std::forward<Args>(args)...);
	}
	
	template<class Alloc>
	inline void nct_free(void* ptr)
	{
		MemoryAllocator<Alloc>::free(ptr);
	}
	
	template<class T, class Alloc = GenAlloc>
	inline void nct_delete(T* ptr)
	{
		(ptr)->~T();

		MemoryAllocator<Alloc>::free(ptr);
	}
	
	template<class T, class Alloc = GenAlloc>
	inline void nct_deleteN(T* ptr, size_t count)
	{
		for(size_t i = 0; i < count; ++i)
			ptr[i].~T();

		MemoryAllocator<Alloc>::free(ptr);
	}

	inline void* nct_alloc(size_t count)
	{
		return MemoryAllocator<GenAlloc>::allocate(count);
	}
	
	template<class T>
	inline T* nct_alloc()
	{
		return (T*)MemoryAllocator<GenAlloc>::allocate(sizeof(T));
	}

	inline void* nct_alloc_aligned(size_t count, size_t align)
	{
		return MemoryAllocator<GenAlloc>::allocateAligned(count, align);
	}
	
	inline void* nct_alloc_aligned16(size_t count)
	{
		return MemoryAllocator<GenAlloc>::allocateAligned16(count);
	}
	
	template<class T>
	inline T* nct_allocN(size_t count)
	{
		return (T*)MemoryAllocator<GenAlloc>::allocate(count * sizeof(T));
	}
	
	template<class T>
	inline T* nct_newN(size_t count)
	{
		T* ptr = (T*)MemoryAllocator<GenAlloc>::allocate(count * sizeof(T));

		for(size_t i = 0; i < count; ++i)
			new (&ptr[i]) T;

		return ptr;
	}
	
	template<class Type, class... Args>
	Type* nct_new(Args &&...args)
	{
		return new (nct_alloc<Type, GenAlloc>()) Type(std::forward<Args>(args)...);
	}
	
	inline void nct_free(void* ptr)
	{
		MemoryAllocator<GenAlloc>::free(ptr);
	}
	
	inline void nct_free_aligned(void* ptr)
	{
		MemoryAllocator<GenAlloc>::freeAligned(ptr);
	}
	
	inline void nct_free_aligned16(void* ptr)
	{
		MemoryAllocator<GenAlloc>::freeAligned16(ptr);
	}


#define NCT_PVT_DELETE(T, ptr) \
	(ptr)->~T(); \
	MemoryAllocator<GenAlloc>::free(ptr);

#define NCT_PVT_DELETE_A(T, ptr, Alloc) \
	(ptr)->~T(); \
	MemoryAllocator<Alloc>::free(ptr);

	
	template <class T, class Alloc = GenAlloc>
	class StdAlloc
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr StdAlloc() = default;
		constexpr StdAlloc(StdAlloc&&) = default;
		constexpr StdAlloc(const StdAlloc&) = default;
		template<class U, class Alloc2> constexpr StdAlloc(const StdAlloc<U, Alloc2>&) { };
		template<class U, class Alloc2> constexpr bool operator==(const StdAlloc<U, Alloc2>&) const noexcept { return true; }
		template<class U, class Alloc2> constexpr bool operator!=(const StdAlloc<U, Alloc2>&) const noexcept { return false; }

		template<class U> class rebind { public: using other = StdAlloc<U, Alloc>; };
		
		static T* allocate(const size_t num)
		{
			if (num == 0)
				return nullptr;

			if (num > std::numeric_limits<size_t>::max() / sizeof(T))
				return nullptr; 

			void* const pv = bs_alloc<Alloc>(num * sizeof(T));
			if (!pv)
				return nullptr; 

			return static_cast<T*>(pv);
		}
		
		static void deallocate(pointer p, size_type)
		{
			bs_free<Alloc>(p);
		}

		static constexpr size_t max_size() { return std::numeric_limits<size_type>::max() / sizeof(T); }
		static constexpr void destroy(pointer p) { p->~T(); }

		template<class... Args>
		static void construct(pointer p, Args&&... args) { new(p) T(std::forward<Args>(args)...); }

	};
}

#include "Allocators/NctStackAlloc.h"
#include "Allocators/NctFreeAlloc.h"
#include "Allocators/NctFrameAlloc.h"
#include "Allocators/NctStaticAlloc.h"
#include "Allocators/NctMemAllocProfiler.h"
