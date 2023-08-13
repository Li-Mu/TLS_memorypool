#pragma once

#include "ThreadCache.h"
#include "PageCache.h"

// static TLS ThreadCache* s_tlsthreadcache = nullptr;
static __thread ThreadCache* tlslist = nullptr;

//被动调用，哪个线程来了之后，需要内存就调用这个接口
static inline void* ConcurrentAlloc(size_t size)
{
	if (size > MAX_BYTES)//超过一个最大值 64k，就自己从系统中获取，否则使用内存池
	{
		Span* span = nullptr;
        {
            PageCache*                  cache = PageCache::GetInstence();
            std::lock_guard<std::mutex> lock(cache->GetPageCacheMUTEX());
            span             = cache->AllocBigPageObj(size);
            span->is_used      = true;
            span->_objsize = size;
        }
		void* ptr = (void*)(span->_pageid << PAGE_SHIFT);
		return ptr;
	}
	else
	{
		if (tlslist == nullptr)//第一次来，自己创建，后面来的，就可以直接使用当前创建好的内存池
		{
			// tlslist = new ThreadCache;
			static ObjectPool<ThreadCache> ThreadCachePool;
			static std::mutex mt;
			{
				std::lock_guard<std::mutex> lock(mt);
				tlslist = ThreadCachePool.New();
			}
		}

		return tlslist->Allocate(size);
	}
}

static inline void ConcurrentFree(void* ptr)//最后释放
{
	Span* span = PageCache::GetInstence()->MapObjectToSpan(ptr);
	size_t size = span->_objsize;
	if (size > MAX_BYTES)
	{
		// PageCache::GetInstence()->FreeBigPageObj(ptr, span);
		PageCache* cache = PageCache::GetInstence();
		std::lock_guard<std::mutex> lock(cache->GetPageCacheMUTEX());
		cache->FreeBigPageObj(ptr, span);
	}
	else
	{
		tlslist->Deallocate(ptr, size);
	}
}