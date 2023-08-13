#include "../baselib/Common.h"
#include "../baselib/ConcurrentAlloc.h"
// #include "ConcurrentAlloc.h"
#define SIZE 16

void BenchmarkMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	size_t malloc_costtime = 0;
	size_t free_costtime = 0;
	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&, k]() {
			std::vector<void*> v;
			v.reserve(ntimes);
			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					v.push_back(malloc(SIZE));
					// cout<<clock()-begin1<<" ";
				}
				// cout<<endl<<"-----------------"<<endl;;
				size_t end1 = clock();
				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					free(v[i]);
					// cout<<clock()-begin1<<" ";
				}
				// cout<<endl;
				size_t end2 = clock();
				v.clear();
				malloc_costtime += end1 - begin1;
				free_costtime += end2 - begin2;
			}
		});
	}
	for (auto& t : vthread)
	{
		t.join();
	}
	printf("%zu thread run %zu times, alloc %zu times: spend time: %zu ms\n", 
			nworks, rounds, ntimes, malloc_costtime);
	printf("%zu thread run %zu times, free %zu times: spend time: %zu ms\n",
		nworks, rounds, ntimes, free_costtime);
	printf("%zu threads, alloc&free %zu times, cost: %zu ms\n",
		nworks, nworks*rounds*ntimes, malloc_costtime + free_costtime);}

// 单轮次申请释放次数 线程数 轮次
void BenchmarkConcurrentMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	size_t malloc_costtime = 0;
	size_t free_costtime = 0;
	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&]() {
			std::vector<void*> v;
			v.reserve(ntimes);
			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					v.push_back(ConcurrentAlloc(SIZE));
					// cout<<clock()-begin1<<"  ";
				}
				// cout<<endl<<"--------------------"<<endl;
				size_t end1 = clock();
				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					ConcurrentFree(v[i]);
					// cout<<clock()-begin1<<" ";
				}
				// cout<<endl;
				size_t end2 = clock();
				v.clear();
				malloc_costtime += end1 - begin1;
				free_costtime += end2 - begin2;
			}
		});
	}
	for (auto& t : vthread)
	{
		t.join();
	}
	// printf("%zu个线程并发执行%zu轮次，每轮次concurrent alloc %zu次: 花费：%zu ms\n", 
	// 		nworks, rounds, ntimes, malloc_costtime);
	// printf("%zu个线程并发执行%zu轮次，每轮次concurrent dealloc %zu次: 花费：%zu ms\n",
	// 	nworks, rounds, ntimes, free_costtime);
	// printf("%zu个线程并发concurrent alloc&dealloc %zu次，总计花费：%zu ms\n",
	// 	nworks, nworks*rounds*ntimes, malloc_costtime + free_costtime);
	printf("%zu thread run %zu times, concurrent alloc %zu times: spend time: %zu ms\n", 
			nworks, rounds, ntimes, malloc_costtime);
	printf("%zu thread run %zu times, concurrent dealloc %zu times: spend time: %zu ms\n",
		nworks, rounds, ntimes, free_costtime);
	printf("%zu threads, concurrent alloc&dealloc %zu times, cost: %zu ms\n",
		nworks, nworks*rounds*ntimes, malloc_costtime + free_costtime);
}


int main()
{
	cout << "==========================================================" << endl;

	BenchmarkMalloc(100, 10, 100);
	cout << endl;
	BenchmarkConcurrentMalloc(10, 10, 100);
	cout << endl;

	cout << "==========================================================" << endl;

	return 0;
}
