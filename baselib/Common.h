#pragma once
#ifndef __COMMON_H__
#define __COMMON_H__
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>
#include<malloc.h>
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <unistd.h>
#endif
using std::cout;
using std::endl;

// #include <Windows.h>

const size_t MAX_BYTES = 64 * 1024; //ThreadCache ���������ڴ�
const size_t NLISTS = 184; //����Ԫ���ܵ��ж��ٸ����ɶ������������
const size_t PAGE_SHIFT = 12;
const size_t NPAGES = 129;


/**
 * @brief ����ϵͳ�ӿ������ڴ�
 */
inline static void* systemAlloc(size_t npage) {
#ifdef _WIN32
    void* ptr = VirtualAlloc(0, _npage * (1 << kPageShift), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#elif __linux__
    void* ptr = sbrk(npage * (1 << PAGE_SHIFT));
#endif
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    return ptr;
}

/**
 * @brief ����ϵͳ�ӿ��ͷ��ڴ�
 */
inline static void systemFree(void* ptr) {
#ifdef _WIN32
    VirtualFree(_ptr, 0, MEM_RELEASE);
#elif __linux__
    brk(ptr);
#endif
}



inline static void*& NEXT_OBJ(void* obj)//��ȡ����ͷ�ĸ�����ͷ�˸��ֽڣ�void*�ı�������ʡ���ڴ棬ֻ�������Լ�ȡ
{
	return *((void**)obj);   // ��ǿתΪvoid**,Ȼ������þ���һ��void*
}


/**
 * @brief �����
 */
template <class T>
class ObjectPool {
public:
    /// �����ڴ�
    T* New() {
        T* obj = nullptr;

        // �������ù黹���ڴ�����
        if (freelist_ != nullptr) {
            // ����������ͷ��ɾ��һ������
            obj       = (T*)freelist_;
            freelist_ = NEXT_OBJ(freelist_);
        } else {
            // ��֤�����㹻�洢��ַ
            size_t objsize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
            // ʣ���ڴ治��һ�������Сʱ ���¿��ٴ���ڴ�ռ�
            if (remainBytes_ < objsize) {
                // ��������С����Ĭ�ϴ�С����Ҫ���и���
                size_t bytes = sizeof(T) > kMemorySize_ ? sizeof(T) : kMemorySize_;
                memory_      = (char*)systemAlloc(bytes >> PAGE_SHIFT);
                if (memory_ == nullptr) {
                    throw std::bad_alloc();
                }
                remainBytes_ = bytes;
            }
            // �Ӵ���ڴ����г�objsize�ֽڵ��ڴ�
            obj = (T*)memory_;
            memory_ += objsize;
            remainBytes_ -= objsize;
        }
        // ��λnew ��ʽ���ö���Ĺ��캯��
        new (obj) T;

        return obj;
    }

    /// �ͷ��ڴ�
    void Delete(T* obj) {
        // ��ʽ�������������������
        obj->~T();

        // ���ͷŵĶ���ͷ�嵽��������
        NEXT_OBJ(obj) = freelist_;
        freelist_     = obj;
    }

private:
    char*  memory_      = nullptr; // ָ�����ڴ�
    size_t remainBytes_ = 0;       // ����ڴ����зֹ�����ʣ����ֽ���
    void*  freelist_    = nullptr; // �ڴ��黹���γɵ���������

    const size_t kMemorySize_ = 128 * 1024;
};

//����һ��������FreeList��������ÿ�������к��и����ӿڣ���ʱ��ֱ��ʹ�ýӿڽ��в���
//��һ������������������
class Freelist
{
private:
	void* _list = nullptr; // ����ȱʡֵ
	size_t _size = 0;  // ��¼�ж��ٸ�����
	size_t _maxsize = 1;

public:

	void Push(void* obj)
	{
		NEXT_OBJ(obj) = _list;
		_list = obj;
		++_size;
	}

	void PushRange(void* start, void* end, size_t n)
	{
		NEXT_OBJ(end) = _list;
		_list = start;
		_size += n;
	}

	void* Pop() //�Ѷ��󵯳�ȥ
	{
		void* obj = _list;
		_list = NEXT_OBJ(obj);
		--_size;

		return obj;
	}

	void* PopRange()
	{
		_size = 0;
		void* list = _list;
		_list = nullptr;

		return list;
	}
	
	bool Empty()
	{
		return _list == nullptr;
	}

	size_t Size()
	{
		return _size;
	}

	size_t MaxSize()
	{
		return _maxsize;
	}

	void SetMaxSize(size_t maxsize)
	{
		_maxsize = maxsize;
	}
};

//ר�����������Сλ�õ���
class SizeClass
{
public:
	//��ȡFreelist��λ��
	static size_t _Index(size_t size, size_t align)
	{
		size_t alignnum = 1 << align;  //����ʵ�ֵķ���
		return ((size + alignnum - 1) >> align) - 1;
	}
	// ��������ڴ��ֽ�������ȡ�� 
	static size_t _Roundup(size_t size, size_t align)
	{
		size_t alignnum = 1 << align;
		// std::cout<<alignnum<<std::endl;
		return ((size + alignnum - 1)&~(alignnum - 1));
	}

	// ������12%���ҵ�����Ƭ�˷�
	// [1,128]				8byte���� freelist[0,16)  8byte����
	// [129,1024]			16byte���� freelist[16,72) 16byte����
	// [1025,8*1024]		128byte���� freelist[72,128) 128byte����
	// [8*1024+1,64*1024]	1024byte���� freelist[128,184)  1024����
	// ��ö�ӦͰ���±�
	static size_t Index(size_t size)
	{
		assert(size <= MAX_BYTES);

		// ÿ�������ж��ٸ���
		static int group_array[4] = { 16, 56, 56, 56 };
		if (size <= 128)
		{
			return _Index(size, 3);
		}
		else if (size <= 1024)
		{
			return _Index(size - 128, 4) + group_array[0];
		}
		else if (size <= 8192)
		{
			return _Index(size - 1024, 7) + group_array[0] + group_array[1];
		}
		else//if (size <= 65536)
		{
			return _Index(size - 8 * 1024, 10) + group_array[0] + group_array[1] + group_array[2];
		}
	}

	// �����С���㣬����ȡ��
	static size_t Roundup(size_t bytes)
	{
		assert(bytes <= MAX_BYTES);
		if (bytes <= 128){
			return _Roundup(bytes, 3);
		}
		else if (bytes <= 1024){
			return _Roundup(bytes, 4);
		}
		else if (bytes <= 8192){
			return _Roundup(bytes, 7);
		}
		else {//if (bytes <= 65536){
			return _Roundup(bytes, 10);
		}
	}

	//��̬��������Ļ��������ٸ��ڴ����ThreadCache��
	static size_t NumMoveSize(size_t size)
	{
		if (size == 0)
			return 0;

		int num = (int)(MAX_BYTES / size);
		if (num < 2)
			num = 2;

		if (num > 512)
			num = 512;

		return num;
	}

	// ����size�������Ļ���Ҫ��ҳ�����ȡ����span����
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num*size;
		npage >>= PAGE_SHIFT;
		if (npage == 0)
			npage = 1;
		return npage;
	}
};

#ifdef _WIN32
	typedef size_t PageID;
#else
	typedef long long PageID;
#endif //_WIN32

//Span��һ����ȣ��ȿ��Է����ڴ��ȥ��Ҳ�Ǹ����ڴ���ջ�����PageCache�ϲ�
//��һ��ʽ�ṹ������Ϊ�ṹ����У�������Ҫ�ܶ����Ԫ
struct Span
{
	Span()
        : _pageid(0)
        , _npage(0)
        , _prev(nullptr)
		, _next(nullptr)
		, _list(nullptr)
		, _objsize(0)
        , _usecount(0)
        , is_used(false){}
        

	PageID _pageid;//ҳ��
	size_t _npage;//ҳ��  counts 

	Span* _prev; 
	Span* _next;

	void* _list;//���Ӷ�����������������ж���Ͳ�Ϊ�գ�û�ж�����ǿ�
	size_t _objsize;//����Ĵ�С

	size_t _usecount;//����ʹ�ü���, usedBlocks 
	bool is_used;//��־��Span���Ƿ��Ѿ���CentreCache��ȡ
};

//�������Freelistһ���������ӿ��Լ�ʵ�֣�˫���ͷѭ����Span����
class SpanList
{
public:
	SpanList()
	{
		// _head = new Span;
		_head = spanpool_.New();
		_head->_next = _head;
		_head->_prev = _head;
	}

	// ~SpanList()//�ͷ������ÿ���ڵ�
	// {
	// 	Span * cur = _head->_next;
	// 	while (cur != _head)
	// 	{
	// 		Span* next = cur->_next;
	// 		delete cur;
	// 		cur = next;
	// 	}
	// 	delete _head;
	// 	_head = nullptr;
	// }

	// //��ֹ��������͸�ֵ���죬���������û�п����ı�Ҫ����Ȼ���Լ���ʵ��ǳ����
	// SpanList(const SpanList&) = delete;
	// SpanList& operator=(const SpanList&) = delete;

	//����ҿ�
	Span* Begin()//���ص�һ�����ݵ�ָ��
	{
		return _head->_next;
	}

	Span* End()//���һ������һ��ָ��
	{
		return _head;
	}

	bool Empty()
	{
		return _head->_next == _head;
	}

	//��posλ�õ�ǰ�����һ��newspan
	void Insert(Span* cur, Span* newspan)
	{
		Span* prev = cur->_prev;

		//prev newspan cur
		prev->_next = newspan;
		newspan->_next = cur;

		newspan->_prev = prev;
		cur->_prev = newspan;
	}

	//ɾ��posλ�õĽڵ�
	void Erase(Span* cur)//�˴�ֻ�ǵ����İ�pos�ó�������û���ͷŵ������滹���ô�
	{
		Span* prev = cur->_prev;
		Span* next = cur->_next;

		prev->_next = next;
		next->_prev = prev;
	}

	//β��
	void PushBack(Span* newspan)
	{
		Insert(End(), newspan);
	}

	//ͷ��
	void PushFront(Span* newspan)
	{
		Insert(Begin(), newspan);
	}

	//βɾ
	Span* PopBack()//ʵ���ǽ�β��λ�õĽڵ��ó���
	{
		Span* span = _head->_prev;
		Erase(span);

		return span;
	}

	//ͷɾ
	Span* PopFront()//ʵ���ǽ�ͷ��λ�ýڵ��ó���
	{
		Span* span = _head->_next;
		Erase(span);

		return span;
	}
public:
	std::mutex _mutex;
private:
	Span* _head;
	static ObjectPool<Span> spanpool_;
};

#endif