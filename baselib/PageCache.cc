#include "PageCache.h"

PageCache PageCache::_inst;
// 大对象系统申请
Span* PageCache::AllocBigPageObj(size_t size)
{
	assert(size > MAX_BYTES);

	size = SizeClass::_Roundup(size, PAGE_SHIFT); //对齐
	size_t npage = size >> PAGE_SHIFT;
	if (npage < NPAGES)
	{
		Span* span = NewSpan(npage);
		span->_objsize = size;
		return span;
	}
	else
	{
		void* ptr = malloc( npage << PAGE_SHIFT);

		if (ptr == nullptr)
			throw std::bad_alloc();

		Span* span = new Span;
		span->_npage = npage;
		span->_pageid = (PageID)ptr >> PAGE_SHIFT;
		span->_objsize = npage << PAGE_SHIFT;

		_idspanmap.set(span->_pageid, span);

		return span;
	}
}

void PageCache::FreeBigPageObj(void* ptr, Span* span)
{
	size_t npage = span->_objsize >> PAGE_SHIFT;
	if (npage < NPAGES) //相当于还是小于128页
	{
		span->_objsize = 0;
		span->_usecount = 0;
		ReleaseSpanToPageCache(span);
	}
	else
	{
		_idspanmap.set(span->_pageid, nullptr);
		delete span;
		// free(ptr);
		systemFree(ptr);
	}
}

Span* PageCache::NewSpan(size_t n)
{
	// 加锁，防止多个线程同时到PageCache中申请span
	// 这里必须是给全局加锁，不能单独的给每个桶加锁
	// 如果对应桶没有span,是需要向系统申请的
	// 可能存在多个线程同时向系统申请内存的可能
	return _NewSpan(n);
}



Span* PageCache::_NewSpan(size_t n)
{
	assert(n < NPAGES);
	if (!_pagespanlist[n].Empty())
	{
		Span* sp = _pagespanlist[n].PopFront();
		for (size_t i = 0; i < sp->_npage; i++)//特殊情况1:该Span是之前的大Span被切割后插入回PageCache的部分,还未被Page_Span登记,因此需要登记
		{									//特殊情况2:该Span是之前已经被登记的大Span块切割后插入回PageCache的部分,已经不属于原本登记的Span块,因此必须重新登记
			_idspanmap.set(sp->_pageid + i, sp);
		}
		return sp;
	}

	for (size_t i = n + 1; i < NPAGES; ++i)
	{
		if (!_pagespanlist[i].Empty())
		{
			Span* span = _pagespanlist[i].PopFront();
			// Span* splist = new Span;
			Span* splist = span_pool_.New();

			// splist->_pageid = span->_pageid;
			// splist->_npage = n;
			// span->_pageid = span->_pageid + n;
			// span->_npage = span->_npage - n;

			span->_npage = n;
			splist->_pageid = span->_pageid + n;
			splist->_npage = i - n;

			_idspanmap.set(splist->_pageid, splist);
			_idspanmap.set(splist->_pageid + span->_npage-1, splist);
			for (size_t i = 0; i < span->_npage; ++i)
				_idspanmap.set(span->_pageid + i, span);

			//_spanlist[splist->_npage].PushFront(splist);
			//return span;

			_pagespanlist[splist->_npage].PushFront(splist);
			// cout<<"_pagespanlist split "<<splist->_npage<<endl;
			return span;
		}
	}

	// Span* span = new Span;
	Span* span = span_pool_.New();

	// 到这里说明SpanList中没有合适的span,只能向系统申请128页的内存
// #ifdef _WIN32
// 	void* ptr = malloc(0, (NPAGES - 1)*(1 << PAGE_SHIFT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
// #else
// 	//  brk
// #endif

	void* ptr = malloc((NPAGES - 1)*(1 << PAGE_SHIFT));
	span->_pageid = (PageID)ptr >> PAGE_SHIFT;
	span->_npage = NPAGES - 1;

	// 之后还会被切割，重新更新 
	// for (size_t i = 0; i < span->_npage; ++i)
	// 	_idspanmap.set(span->_pageid + i, span);

	_pagespanlist[span->_npage].PushFront(span);  //方括号
	// cout<<"_pagespanlist new page "<<span->_npage<<endl;
	return _NewSpan(n);
}

// 获取从对象到span的映射
Span* PageCache::MapObjectToSpan(void* obj)
{
	//计算页号
	PageID id = (PageID)obj >> PAGE_SHIFT;
	// auto it = _idspanmap.find(id);
	// if (it != _idspanmap.end())
	// {
	// 	return it->second;
	// }
	// else
	// {
	// 	assert(false);
	// 	return nullptr;
	// }
	Span* sp =(Span*) _idspanmap.get(id);//读写分离,无需加锁。TCMalloc_PageMap有点类似于位图，提前开辟好了存放所有页对应的Span*的空间，就不存在因为写入而改变了TCMalloc_PageMap的结构的情况
	assert(sp);
	return sp;
}



void PageCache::ReleaseSpanToPageCache(Span* cur)
{
	// 当释放的内存是大于128页,直接将内存归还给操作系统,不能合并
	if (cur->_npage >= NPAGES)
	{
		void* ptr = (void*)(cur->_pageid << PAGE_SHIFT);
		// 归还之前删除掉页到span的映射
		// _idspanmap.erase(cur->_pageid);
		free(ptr);
		// delete cur;
		span_pool_.Delete(cur);
		return;
	}


	// 向前合并
	while (1)
	{
		////超过128页则不合并
		//if (cur->_npage > NPAGES - 1)
		//	break;

		PageID curid = cur->_pageid;
		PageID previd = curid - 1;
		Span* it = (Span*)_idspanmap.get(previd);

		// 没有找到
		if (it == nullptr)
			break;

		// 前一个span不空闲
		if (it->is_used == true)
			break;

		Span* prev = it;

		//超过128页则不合并
		if (cur->_npage + prev->_npage > NPAGES - 1)
			break;

		// cout<<"remove _pagespanlist "<<prev->_npage<<endl;
		// 先把prev从链表中移除
		_pagespanlist[prev->_npage].Erase(prev);

		// 合并
		prev->_npage += cur->_npage;
		//修正id->span的映射关系
		for (PageID i = 0; i < (PageID)cur->_npage; ++i)
		{
			_idspanmap.set(cur->_pageid + i, prev);
		}
		// delete cur;
		span_pool_.Delete(cur);

		// 继续向前合并
		cur = prev;
	}


	//向后合并
	while (1)
	{
		////超过128页则不合并
		//if (cur->_npage > NPAGES - 1)
		//	break;

		PageID curid = cur->_pageid;
		PageID nextid = curid + cur->_npage;
		//std::map<PageID, Span*>::iterator it = _idspanmap.find(nextid);
		Span* nextSpan = nullptr;
		nextSpan  = (Span*)_idspanmap.get(nextid);

		if (nextSpan  == nullptr)
			break;

		if (nextSpan->is_used == true)
			break;

		Span* next = nextSpan;

		//超过128页则不合并
		if (cur->_npage + next->_npage >= NPAGES - 1)
			break;

		// cout<<"remove _pagespanlist "<<next->_npage<<endl;
		_pagespanlist[next->_npage].Erase(next);

		cur->_npage += next->_npage;
		//修正id->Span的映射关系
		for (PageID i = 0; i < (PageID)next->_npage; ++i)
		{
			_idspanmap.set(next->_pageid + i, cur);
		}

		// delete next;
		span_pool_.Delete(next);
	}

	// 最后将合并好的span插入到span链中
	_pagespanlist[cur->_npage].PushFront(cur);
	cur->is_used = false;
}
