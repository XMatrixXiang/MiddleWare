
/**************************************************************************
**  Copyright (C) 2002-2018 北京捷安申谋军工科技有限公司, All Rights Reserved
**
**  文件名：  NctFrameAlloc.h
**  创建者：  闫相伟
**  创建日期：2018年4月27日
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
	
	class  FrameAlloc
	{
	private:
		
		class MemBlock
		{
		public:
			MemBlock(std::uint32_t size) :mSize(size) { }

			~MemBlock() = default;

			
			std::uint8_t * alloc(std::uint32_t amount);
			
			void clear();

			std::uint8_t* mData = nullptr;
			std::uint32_t mFreePtr = 0;
			std::uint32_t mSize;
		};

	public:
		FrameAlloc(std::uint32_t blockSize = 1024 * 1024);
		~FrameAlloc();

		
		std::uint8_t* alloc(std::uint32_t amount);

		
		std::uint8_t* allocAligned(std::uint32_t amount, std::uint32_t alignment);

		
		template<class T, class... Args>
		T* construct(Args &&...args)
		{
			return new ((T*)alloc(sizeof(T))) T(std::forward<Args>(args)...);
		}

	
		void free(std::uint8_t* data);

		
		template<class T>
		void free(T* obj)
		{
			if (obj != nullptr)
				obj->~T();

			free((UINT8*)obj);
		}		
		void markFrame();

		void clear();
		
		void setOwnerThread(NctThreadId thread);

	private:
		std::uint32_t mBlockSize;
		std::vector<MemBlock*> mBlocks;
		MemBlock* mFreeBlock;
		std::uint32_t mNextBlockIdx;
		std::atomic<std::uint32_t> mTotalAllocBytes;
		void* mLastFrame;

		MemBlock* allocBlock(std::uint32_t wantedSize);
		
		void deallocBlock(MemBlock* block);
	};


	template<int BlockSize>
	class TFrameAlloc : public FrameAlloc
	{
	public:
		TFrameAlloc()
			:FrameAlloc(BlockSize)
		{ }
	};

	
	template <class T>
	class StdFrameAlloc
	{
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		StdFrameAlloc() noexcept = default;

		StdFrameAlloc(FrameAlloc* alloc) noexcept
			:mFrameAlloc(alloc)
		{ }

		template<class U> StdFrameAlloc(const StdFrameAlloc<U>& alloc) noexcept
			:mFrameAlloc(alloc.mFrameAlloc)
		{ }

		template<class U> bool operator==(const StdFrameAlloc<U>&) const noexcept { return true; }
		template<class U> bool operator!=(const StdFrameAlloc<U>&) const noexcept { return false; }
		template<class U> class rebind { public: typedef StdFrameAlloc<U> other; };
		
		T* allocate(const size_t num) const
		{
			if (num == 0)
				return nullptr;

			if (num > static_cast<size_t>(-1) / sizeof(T))
				return nullptr; 

			void* const pv = mFrameAlloc->alloc((UINT32)(num * sizeof(T)));
			if (!pv)
				return nullptr; 

			return static_cast<T*>(pv);
		}

		void deallocate(T* p, size_t num) const noexcept
		{
			mFrameAlloc->free((UINT8*)p);
		}

		FrameAlloc* mFrameAlloc = nullptr;

		size_t max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }
		void construct(pointer p, const_reference t) { new (p) T(t); }
		void destroy(pointer p) { p->~T(); }
		template<class U, class... Args>
		void construct(U* p, Args&&... args) { new(p) U(std::forward<Args>(args)...); }
	};
	
	template <class T1, class T2>
	bool operator== (const StdFrameAlloc<T1>&,
		const StdFrameAlloc<T2>&) throw() {
		return true;
	}
	
	template <class T1, class T2>
	bool operator!= (const StdFrameAlloc<T1>&,
		const StdFrameAlloc<T2>&) throw() {
		return false;
	}

	
	 FrameAlloc& gFrameAlloc();

	
	 std::uint8_t* nct_frame_alloc(std::uint32_t numBytes);

	
	 std::uint8_t* nct_frame_alloc_aligned(std::uint32_t count, std::uint32_t align);


	 void nct_frame_free(void* data);


	 void nct_frame_free_aligned(void* data);

	template<class T>
	T* nct_frame_alloc()
	{
		return (T*)nct_frame_alloc(sizeof(T));
	}

	template<class T>
	T* nct_frame_alloc(std::uint32_t count)
	{
		return (T*)nct_frame_alloc(sizeof(T) * count);
	}


	template<class T>
	T* nct_frame_new(std::uint32_t count = 0)
	{
		T* data = nct_frame_alloc<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T;

		return data;
	}
	
	template<class T, class... Args>
	T* nct_frame_new(Args &&...args, std::uint32_t count = 0)
	{
		T* data = nct_frame_alloc<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T(std::forward<Args>(args)...);

		return data;
	}

	template<class T>
	void nct_frame_delete(T* data)
	{
		data->~T();

		nct_frame_free((std::uint8_t*)data);
	}

	template<class T>
	void nct_frame_delete(T* data, std::uint32_t count)
	{
		for(unsigned int i = 0; i < count; i++)
			data[i].~T();

		nct_frame_free((std::uint8_t*)data);
	}
	
	void nct_frame_mark();

	
	void nct_frame_clear();
	
	typedef std::basic_string<char, std::char_traits<char>, StdAlloc<char, FrameAlloc>> FrameString;

	
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, StdAlloc<wchar_t, FrameAlloc>> FrameWString;

	
	template <typename T, typename A = StdAlloc<T, FrameAlloc>>
	using FrameVector = std::vector < T, A > ;

	
	template <typename T, typename A = StdAlloc<T, FrameAlloc>>
	using FrameStack = std::stack < T, std::deque<T, A> > ;

	
	template <typename T, typename A = StdAlloc<T, FrameAlloc>>
	using FrameQueue = std::queue<T, std::deque<T, A>>;

	
	template <typename T, typename P = std::less<T>, typename A = StdAlloc<T, FrameAlloc>>
	using FrameSet = std::set < T, P, A > ;

	
	template <typename K, typename V, typename P = std::less<K>, typename A = StdAlloc<std::pair<const K, V>, FrameAlloc>>
	using FrameMap = std::map < K, V, P, A >;

	
	template <typename T, typename H = std::hash<T>, typename C = std::equal_to<T>, typename A = StdAlloc<T, FrameAlloc>>
	using FrameUnorderedSet = std::unordered_set < T, H, C, A >;

	
	template <typename K, typename V, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = StdAlloc<std::pair<const K, V>, FrameAlloc>>
	using FrameUnorderedMap = std::unordered_map < K, V, H, C, A >;

	extern  FrameAlloc* _GlobalFrameAlloc;

	template<>
	class MemoryAllocator<FrameAlloc> : public MemoryAllocatorBase
	{
	public:
		
		static void* allocate(size_t bytes)
		{
			return nct_frame_alloc((std::uint32_t)bytes);
		}
		
		static void* allocateAligned(size_t bytes, size_t alignment)
		{
			return nct_frame_alloc_aligned((std::uint32_t)bytes, (std::uint32_t)alignment);
		}
		
		static void* allocateAligned16(size_t bytes)
		{
			return nct_frame_alloc_aligned((std::uint32_t)bytes, 16);
		}
		
		static void free(void* ptr)
		{
			nct_frame_free(ptr);
		}		
		static void freeAligned(void* ptr)
		{
			nct_frame_free_aligned(ptr);
		}		
		static void freeAligned16(void* ptr)
		{
			nct_frame_free_aligned(ptr);
		}
	};
}
