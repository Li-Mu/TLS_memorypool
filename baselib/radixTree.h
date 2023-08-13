#pragma once
#include "cstring"
#include "Common.h"
// 单层基数树
template <int BITS>//BITS表示最大页号所占的二进制位，32-PAGE_SHIFT 或64-PAGE_SHIFT
class TCMalloc_PageMap1 {
private:
	static const int LENGTH = 1 << BITS;
	void** array_;
public:
	typedef uintptr_t Number;
	explicit TCMalloc_PageMap1()
	{
		size_t requiredBytes = sizeof(void*) << BITS;
		size_t alignedSize = SizeClass::_Roundup(requiredBytes, PAGE_SHIFT);
		array_ = (void**)systemAlloc(alignedSize >> PAGE_SHIFT);
		memset(array_, 0, sizeof(void*) << BITS);
	}
	// 返回KEY的当前值。如果还没有设置或者k超出范围，返回NULL。
	void* get(Number k) const {
		if ((k >> BITS) > 0) {
			return NULL;
		}
		return array_[k];
	}
	// 要求 "k" 在范围 "[0,2^BITS-1]" 内。
	// 要求 "k" 已经在之前确保过。
	//
	// 设置键 'k' 的值为 'v'。
	void set(Number k, void* v) {
		array_[k] = v;
	}
};
// 两层基数树
template <int BITS>
class TCMalloc_PageMap2 {
private:
	// 在根节点放32个条目，每个叶节点放(2^BITS)/32个条目。
	static const int ROOT_BITS = 5;
	static const int ROOT_LENGTH = 1 << ROOT_BITS;
	static const int LEAF_BITS = BITS - ROOT_BITS;
	static const int LEAF_LENGTH = 1 << LEAF_BITS;
	// 叶节点
	struct Leaf {
		void* values[LEAF_LENGTH];
	};
	Leaf* root_[ROOT_LENGTH];// 指向32个子节点的指针
public:
	typedef uintptr_t Number;
	explicit TCMalloc_PageMap2()
	{
		//allocator_ = allocator;
		memset(root_, 0, sizeof(root_));
		PreallocateMoreMemory();
	}
	void* get(Number k) const {
		const Number i1 = k >> LEAF_BITS;
		const Number i2 = k & (LEAF_LENGTH - 1);
		if ((k >> BITS) > 0 || root_[i1] == NULL) {
			return NULL;
		}
		return root_[i1]->values[i2];
	}
	void set(Number k, void* v) {
		const Number i1 = k >> LEAF_BITS;
		const Number i2 = k & (LEAF_LENGTH - 1);
		assert(i1 < ROOT_LENGTH);
		root_[i1]->values[i2] = v;
	}
	bool Ensure(Number start, size_t n) {
		for (Number key = start; key <= start + n - 1;) {
			const Number i1 = key >> LEAF_BITS;
			// 检查溢出
			if (i1 >= ROOT_LENGTH)
				return false;
			// 如果必要，创建第二层节点
			if (root_[i1] == NULL) {
				static ObjectPool<Leaf> LeafPool;
				Leaf* leaf = LeafPool.New();
				if (leaf == NULL) return false;
				memset(leaf, 0, sizeof(*leaf));
				root_[i1] = leaf;
			}
			// 将键提升到这个叶节点覆盖的下一个区域
			key = ((key >> LEAF_BITS) + 1) << LEAF_BITS;
		}
		return true;
	}
	void PreallocateMoreMemory() {
		// 分配足够的内存来跟踪所有可能的页面
		Ensure(0, 1 << BITS);
	}
};
// 三层基数树结构
template <int BITS>
class TCMalloc_PageMap3 {
private:
	// 每个内部层消耗多少位
	static const int INTERIOR_BITS = (BITS + 2) / 3; // 向上取整
	static const int INTERIOR_LENGTH = 1 << INTERIOR_BITS;
	// 叶子层消耗多少位
	static const int LEAF_BITS = BITS - 2 * INTERIOR_BITS;
	static const int LEAF_LENGTH = 1 << LEAF_BITS;
	// 内部节点
	struct Node {
		Node* ptrs[INTERIOR_LENGTH]; // 指向下一层节点的指针
	};
	// 叶子节点
	struct Leaf {
		void* values[LEAF_LENGTH]; // 实际的数据值
	};
	Node* root_; // 基数树的根节点
	Node* NewNode() {
		static ObjectPool<Node> NodePool;
		Node* result = NodePool.New();
		if (result != NULL) {
			memset(result, 0, sizeof(*result));
		}
		return result;
	}
public:
	typedef uintptr_t Number; // 页号的类型
	explicit TCMalloc_PageMap3()
	{
		//allocator_ = allocator;
		root_ = NewNode(); // 创建根节点
	}
	void* get(Number k) const { // 根据页号获取值
		const Number i1 = k >> (LEAF_BITS + INTERIOR_BITS); // 第一层索引
		const Number i2 = (k >> LEAF_BITS) & (INTERIOR_LENGTH - 1); // 第二层索引
		const Number i3 = k & (LEAF_LENGTH - 1); // 第三层索引
		if ((k >> BITS) > 0 || // 检查页号是否超出范围
			root_->ptrs[i1] == NULL || root_->ptrs[i1]->ptrs[i2] == NULL) { // 检查节点是否存在
			return NULL; // 如果没有找到或者超出范围，返回NULL
		}
		return reinterpret_cast<Leaf*>(root_->ptrs[i1]->ptrs[i2])->values[i3]; // 如果找到，返回值
	}
	void set(Number k, void* v) { // 根据页号设置值
		assert(k >> BITS == 0); // 断言页号在范围内
		Ensure(k, 1); //确保第k页对应节点存在,不存在则开辟节点空间
		const Number i1 = k >> (LEAF_BITS + INTERIOR_BITS); // 第一层索引
		const Number i2 = (k >> LEAF_BITS) & (INTERIOR_LENGTH - 1); // 第二层索引
		const Number i3 = k & (LEAF_LENGTH - 1); // 第三层索引
		reinterpret_cast<Leaf*>(root_->ptrs[i1]->ptrs[i2])->values[i3] = v; // 设置值
	}
	bool Ensure(Number start, size_t n) { // 确保一段页号范围内的节点都存在
		for (Number key = start; key <= start + n - 1;) { // 遍历页号范围
			const Number i1 = key >> (LEAF_BITS + INTERIOR_BITS); // 第一层索引
			const Number i2 = (key >> LEAF_BITS) & (INTERIOR_LENGTH - 1); // 第二层索引
			// 检查溢出情况
			if (i1 >= INTERIOR_LENGTH || i2 >= INTERIOR_LENGTH)
				return false; // 如果超出范围，返回false
			// 如果需要，创建第二层节点
			if (root_->ptrs[i1] == NULL) {
				Node* n = NewNode(); // 创建新节点
				if (n == NULL) return false; // 如果内存分配失败，返回false
				root_->ptrs[i1] = n; // 设置指向新节点的指针
			}
			// 如果需要，创建叶子节点
			if (root_->ptrs[i1]->ptrs[i2] == NULL) {
				static ObjectPool<Leaf> LeafPool_3;
				Leaf* leaf = reinterpret_cast<Leaf*>(LeafPool_3.New());
				if (leaf == NULL) return false; // 如果内存分配失败，返回false
				memset(leaf, 0, sizeof(*leaf)); // 初始化叶子为零
				root_->ptrs[i1]->ptrs[i2] = reinterpret_cast<Node*>(leaf); // 设置指向新叶子的指针
			}
			// 将页号推进到下一个叶子节点的范围
			key = ((key >> LEAF_BITS) + 1) << LEAF_BITS;
		}
		return true; // 如果成功，返回true
	}
	void PreallocateMoreMemory() {
		// 提前开辟某段空间以提升效率，如果需要，实现这个函数
	}
};