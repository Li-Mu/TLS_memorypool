#include "CentralCache.h"
#include "PageCache.h"

CentralCache CentralCache::_inst;

Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte_size, std::unique_lock<std::mutex>& bucket_lock)
{
	Span* span = spanlist.Begin();
	while (span != spanlist.End())//��ǰ�ҵ�һ��span
	{
		if (span->_list != nullptr)
			return span;
		else
			span = span->_next;
	}

	bucket_lock.unlock(); // ��Ͱ��

	// �ߵ������˵��ǰ��û�л�ȡ��span,���ǿյģ�����һ��pagecache��ȡspan
	Span* newspan = nullptr;
	{
		std::lock_guard<std::mutex> lock(PageCache::GetInstence()->GetPageCacheMUTEX());
		// newspan = PageCache::GetInstence()->NewSpan(SizeClass::NumMovePage(byte_size));
		// ���_list��û�зǿյ�span��ֻ����PageCache����
        newspan             = PageCache::GetInstence()->NewSpan(SizeClass::NumMovePage(byte_size));
        newspan->is_used      = true;
        newspan->_objsize = byte_size;
	}
	// ��spanҳ�зֳ���Ҫ�Ķ�����������
	char* cur = (char*)(newspan->_pageid << PAGE_SHIFT);
	char* end = cur + (newspan->_npage << PAGE_SHIFT);
	newspan->_list = cur;
	// β��
	while (cur + byte_size < end)
	{
		char* next = cur + byte_size;
		NEXT_OBJ(cur) = next;
		cur = next;
	}
	NEXT_OBJ(cur) = nullptr;

	bucket_lock.lock(); // ��Ͱ��

	spanlist.PushFront(newspan);
	return newspan;
}


//��ȡһ���������ڴ����
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t n, size_t byte_size)
{
	size_t index = SizeClass::Index(byte_size);
	SpanList& spanlist = _spanlist[index];//��ֵ->��������

	std::unique_lock<std::mutex> lock(spanlist._mutex);

	Span* span = GetOneSpan(spanlist, byte_size, lock);
	//������Ѿ���ȡ��һ��newspan

	//��span�л�ȡrange����
	size_t batchsize = 0;
	void* prev = nullptr;//��ǰ����ǰһ��
	void* cur = span->_list;//��cur��������������
	for (size_t i = 0; i < n; ++i)
	{
		prev = cur;
		cur = NEXT_OBJ(cur);
		++batchsize;
		if (cur == nullptr)//��ʱ�ж�cur�Ƿ�Ϊ�գ�Ϊ�յĻ�����ǰֹͣ
			break;
	}

	start = span->_list;
	end = prev;

	span->_list = cur;
	span->_usecount += batchsize;

	// //���յ�span�Ƶ���󣬱��ַǿյ�span��ǰ��
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
	// CentralCache:�Ե�ǰͰ���м���(Ͱ��)����С��������
	// PageCache:���������SpanListȫ�ּ���
	// ��Ϊ���ܴ��ڶ���߳�ͬʱȥϵͳ�����ڴ�����
	std::unique_lock<std::mutex> bucket_lock(spanlist._mutex);

	while (start)
	{
		void* next = NEXT_OBJ(start);

		Span* span = PageCache::GetInstence()->MapObjectToSpan(start);
		NEXT_OBJ(start) = span->_list;
		span->_list = start;
		//��һ��span�Ķ���ȫ���ͷŻ�����ʱ�򣬽�span����pagecache,������ҳ�ϲ�
		if (--span->_usecount == 0)
		{
			spanlist.Erase(span);
			span->_list = nullptr; // ���������ÿ�
            span->_next     = nullptr;
            span->_prev     = nullptr;
			bucket_lock.unlock();
			// ������ȫ����,���ܶ���߳�һ���ThreadCache�й黹����
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