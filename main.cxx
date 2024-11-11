#include <thread>
#include <iostream>
#include <latch>
#include <vector>
#include <mutex>
#define NUM_THREAD 10

//[1] consumer-producer
//[2] thread_pool
//[3] consumer-producer koriscenjem thread-pool-a
//problem -> consumer producer threadovi nemaju kraj, zauzece po jedan thread iz
//thread pool-a
//Taskovi proizvodnja jednog elementa i konzimacija jednog elementa
//ConsumerTaskeMaker, ProducerTaskMaker -> za svakog od njih moram da napravim po thread??

int main() {
	std::vector<size_t> idxs;
	std::mutex mtx;

	std::vector<std::thread> threads;
	for (size_t i=0; i<NUM_THREAD; i++) {
		threads.emplace_back([i, &mtx, &idxs]() {
				std::lock_guard lg(mtx);
				idxs.push_back(i); 
		});
	}

	for (std::thread &t : threads) {
		t.join();
	}

	for (size_t &idx : idxs) {
		std::cout << idx << std::endl;
	}
}
