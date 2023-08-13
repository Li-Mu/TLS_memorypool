#include "CentralCache.h"
#include "PageCache.h"

CentralCache CentralCache::_inst;

Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte_size, std::unique_lock<std::mutex>& bucket_lock)
{
	Span* span = spanlist.Begin();
	while (span != spanlist.End())//当前找到一个span
	{
		if (span->_list != nullptr)
			return span;
		else
			span = span->_next;
	}

	bucket_lock.unlock(); // 解桶锁

	// 走到这儿，说明前面没有获取到span,都是空的，到下一层pagecache获取span
	Span* newspan = nullptr;
	{
		std::lock_guard<std::mutex> lock(PageCache::GetInstence()->GetPageCacheMUTEX());
		// newspan = PageCache::GetInstence()->NewSpan(SizeClass::NumMovePage(byte_size));
		// 如果_list中没有非空的span，只能向PageCache申请
        newspan             = PageCache::GetInstence()->NewSpan(SizeClass::NumMovePage(byte_size));
        newspan->is_used      = true;
        newspan->_objsize = byte_size;
	}
	// 将span页切分成需要的对象并链接起来
	char* cur = (char*)(newspan->_pageid << PAGE_SHIFT);
	char* end = cur + (newspan->_npage << PAGE_SHIFT);
	newspan->_list = cur;
	// 尾插
	while (cur + byte_size < end)
	{
		char* next = cur + byte_size;
		NEXT_OBJ(cur) = next;
		cur = next;
	}
	NEXT_OBJ(cur) = nullptr;

	bucket_lock.lock(); // 加桶锁

	spanlist.PushFront(newspan);
	return newspan;
}


//获取一个批量的内存对象
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t byte_size)
{
	size_t index = SizeClass::Index(byte_size);
	SpanList& spanlist = _spanlist[index];//赋值->拷贝构造

	std::unique_lock<std::mutex> lock(spanlist._mutex);

	Span* span = GetOneSpan(spanlist, byte_size, lock);
	//到这儿已经获取到一个newspan

	//从span中获取range对象
	size_t batchsize = 0;
	void* prev = nullptr;//提前保存前一个
	void* cur = span->_list;//用cur来遍历，往后走
	for (size_t i = 0; i < n; ++i)
	{
		prev = cur;
		cur = NEXT_OBJ(cur);
		++batchsize;
		if (cur == nullptr)//随时判断cur是否为空，为空的话，提前停止
			break;
	}

	start = span->_list;
	end = prev;

	span->_list = cur;
	span->_usecount += batchsize;

	// //将空的span移到最后，保持非空的span在前面
	// if (span->_list == nullptr)
	// {
	// 	spanlist.Erase(span);
	// 	spanlist.PushBack(span);
	// }

	return batchsize;
}

void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::Index(size);
	SpanList& spanlist = _spanlist[index];
	// CentralCache:对当前桶进行加锁(桶锁)，减小锁的粒度
	// PageCache:必须对整个SpanList全局加锁
	// 因为可能存在多个线程同时去系统申请内存的情况
	std::unique_lock<std::mutex> bucket_lock(spanlist._mutex);

	while (start)
	{
		void* next = NEXT_OBJ(start);

		Span* span = PageCache::GetInstence()->MapObjectToSpan(start);
		NEXT_OBJ(start) = span->_list;
		span->_list = start;
		//当一个span的对象全部释放回来的时候，将span还给pagecache,并且做页合并
		if (--span->_usecount == 0)
		{
			spanlist.Erase(span);
			span->_list = nullptr; // 自由链表置空
            span->_next     = nullptr;
            span->_prev     = nullptr;
			bucket_lock.unlock();
			// 必须上全局锁,可能多个线程一起从ThreadCache中归还数据
			{
				PageCache* cache = PageCache::GetInstence();
                std::lock_guard<std::mutex> lock(cache->GetPageCacheMUTEX());
                cache->ReleaseSpanToPageCache(span);
			}
			
			bucket_lock.lock();

		}
		start = next;
	}
}