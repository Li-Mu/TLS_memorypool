#pragma once

#include "ThreadCache.h"
#include "PageCache.h"

// static TLS ThreadCache* s_tlsthreadcache = nullptr;
static __thread ThreadCache* tlslist = nullptr;

//�������ã��ĸ��߳�����֮����Ҫ�ڴ�͵�������ӿ�
static inline void* ConcurrentAlloc(size_t size)
{
	if (size > MAX_BYTES)//����һ�����ֵ 64k�����Լ���ϵͳ�л�ȡ������ʹ���ڴ��
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
		if (tlslist == nullptr)//��һ�������Լ��������������ģ��Ϳ���ֱ��ʹ�õ�ǰ�����õ��ڴ��
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

static inline void ConcurrentFree(void* ptr)//����ͷ�
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