#include "PageCache.h"

PageCache PageCache::_inst;
// �����ϵͳ����
Span* PageCache::AllocBigPageObj(size_t size)
{
	assert(size > MAX_BYTES);

	size = SizeClass::_Roundup(size, PAGE_SHIFT); //����
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
	if (npage < NPAGES) //�൱�ڻ���С��128ҳ
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
	// ��������ֹ����߳�ͬʱ��PageCache������span
	// ��������Ǹ�ȫ�ּ��������ܵ����ĸ�ÿ��Ͱ����
	// �����ӦͰû��span,����Ҫ��ϵͳ�����
	// ���ܴ��ڶ���߳�ͬʱ��ϵͳ�����ڴ�Ŀ���
	return _NewSpan(n);
}



Span* PageCache::_NewSpan(size_t n)
{
	assert(n < NPAGES);
	if (!_pagespanlist[n].Empty())
	{
		Span* sp = _pagespanlist[n].PopFront();
		for (size_t i = 0; i < sp->_npage; i++)//�������1:��Span��֮ǰ�Ĵ�Span���и������PageCache�Ĳ���,��δ��Page_Span�Ǽ�,�����Ҫ�Ǽ�
		{									//�������2:��Span��֮ǰ�Ѿ����ǼǵĴ�Span���и������PageCache�Ĳ���,�Ѿ�������ԭ���Ǽǵ�Span��,��˱������µǼ�
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

	// ������˵��SpanList��û�к��ʵ�span,ֻ����ϵͳ����128ҳ���ڴ�
// #ifdef _WIN32
// 	void* ptr = malloc(0, (NPAGES - 1)*(1 << PAGE_SHIFT), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
// #else
// 	//  brk
// #endif

	void* ptr = malloc((NPAGES - 1)*(1 << PAGE_SHIFT));
	span->_pageid = (PageID)ptr >> PAGE_SHIFT;
	span->_npage = NPAGES - 1;

	// ֮�󻹻ᱻ�и���¸��� 
	// for (size_t i = 0; i < span->_npage; ++i)
	// 	_idspanmap.set(span->_pageid + i, span);

	_pagespanlist[span->_npage].PushFront(span);  //������
	// cout<<"_pagespanlist new page "<<span->_npage<<endl;
	return _NewSpan(n);
}

// ��ȡ�Ӷ���span��ӳ��
Span* PageCache::MapObjectToSpan(void* obj)
{
	//����ҳ��
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
	Span* sp =(Span*) _idspanmap.get(id);//��д����,���������TCMalloc_PageMap�е�������λͼ����ǰ���ٺ��˴������ҳ��Ӧ��Span*�Ŀռ䣬�Ͳ�������Ϊд����ı���TCMalloc_PageMap�Ľṹ�����
	assert(sp);
	return sp;
}



void PageCache::ReleaseSpanToPageCache(Span* cur)
{
	// ���ͷŵ��ڴ��Ǵ���128ҳ,ֱ�ӽ��ڴ�黹������ϵͳ,���ܺϲ�
	if (cur->_npage >= NPAGES)
	{
		void* ptr = (void*)(cur->_pageid << PAGE_SHIFT);
		// �黹֮ǰɾ����ҳ��span��ӳ��
		// _idspanmap.erase(cur->_pageid);
		free(ptr);
		// delete cur;
		span_pool_.Delete(cur);
		return;
	}


	// ��ǰ�ϲ�
	while (1)
	{
		////����128ҳ�򲻺ϲ�
		//if (cur->_npage > NPAGES - 1)
		//	break;

		PageID curid = cur->_pageid;
		PageID previd = curid - 1;
		Span* it = (Span*)_idspanmap.get(previd);

		// û���ҵ�
		if (it == nullptr)
			break;

		// ǰһ��span������
		if (it->is_used == true)
			break;

		Span* prev = it;

		//����128ҳ�򲻺ϲ�
		if (cur->_npage + prev->_npage > NPAGES - 1)
			break;

		// cout<<"remove _pagespanlist "<<prev->_npage<<endl;
		// �Ȱ�prev���������Ƴ�
		_pagespanlist[prev->_npage].Erase(prev);

		// �ϲ�
		prev->_npage += cur->_npage;
		//����id->span��ӳ���ϵ
		for (PageID i = 0; i < (PageID)cur->_npage; ++i)
		{
			_idspanmap.set(cur->_pageid + i, prev);
		}
		// delete cur;
		span_pool_.Delete(cur);

		// ������ǰ�ϲ�
		cur = prev;
	}


	//���ϲ�
	while (1)
	{
		////����128ҳ�򲻺ϲ�
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

		//����128ҳ�򲻺ϲ�
		if (cur->_npage + next->_npage >= NPAGES - 1)
			break;

		// cout<<"remove _pagespanlist "<<next->_npage<<endl;
		_pagespanlist[next->_npage].Erase(next);

		cur->_npage += next->_npage;
		//����id->Span��ӳ���ϵ
		for (PageID i = 0; i < (PageID)next->_npage; ++i)
		{
			_idspanmap.set(next->_pageid + i, cur);
		}

		// delete next;
		span_pool_.Delete(next);
	}

	// ��󽫺ϲ��õ�span���뵽span����
	_pagespanlist[cur->_npage].PushFront(cur);
	cur->is_used = false;
}
