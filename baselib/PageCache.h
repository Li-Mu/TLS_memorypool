#pragma once

#include "Common.h"
#include "radixTree.h"

//����Page CacheҲҪ����Ϊ����������Central Cache��ȡspan��ʱ��
//ÿ�ζ��Ǵ�ͬһ��page�����л�ȡspan
//����ģʽ
class PageCache
{
public:
	static PageCache* GetInstence()
	{
		return &_inst;
	}

	Span* AllocBigPageObj(size_t size);
	void FreeBigPageObj(void* ptr, Span* span);

	Span* _NewSpan(size_t n);
	Span* NewSpan(size_t n);//��ȡ������ҳΪ��λ

	//��ȡ�Ӷ���span��ӳ��
	Span* MapObjectToSpan(void* obj);

	//�ͷſռ�span�ص�PageCache�����ϲ����ڵ�span
	void ReleaseSpanToPageCache(Span* span);

	std::mutex& GetPageCacheMUTEX() {return _mutex; }

	ObjectPool<Span>& GetSpanPool() 
	{
		return span_pool_;
	}
private:
	SpanList _pagespanlist[NPAGES];
	//std::map<PageID, Span*> _idspanmap;
	// std::unordered_map<PageID, Span*> _idspanmap;
	
	#if (__x86_64__)
		TCMalloc_PageMap3<64 - PAGE_SHIFT> _idspanmap;
	#else
		TCMalloc_PageMap2<32 - PAGE_SHIFT> _idspanmap;
	#endif

	std::mutex _mutex;
private:
	PageCache(){}

	PageCache(const PageCache&) = delete;
	static PageCache _inst;
	ObjectPool<Span> span_pool_; //�ö����ڴ�ش��Span�ṹ��
};