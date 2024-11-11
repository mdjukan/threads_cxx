#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

class WaitGroup {
	private:
		size_t m_counter;
		std::mutex m_mtx;
		std::condition_variable m_cv;
	public:
		WaitGroup(size_t count) : m_counter(count) {}
		void wait() {
			std::unique_lock ul(m_mtx);
			while (m_counter!=0) {
				m_cv.wait(ul);
			}
		}

		void done() {
			std::unique_lock ul(m_mtx);
			m_counter -= 1;
			bool notify = (m_counter==0);
			ul.unlock();

			if (notify) {
				m_cv.notify_all();
			}
		}
};

#define THREAD_COUNT 10
int main() {
	WaitGroup wg(THREAD_COUNT);
	std::vector<std::jthread> threads;
	for (int i=0; i<THREAD_COUNT; i++) {
		threads.emplace_back([&wg](){
				std::cout << "hello!\n";
				wg.done();
				});
	}
	wg.wait();
	std::cout << "all threads done\n";
	//RAII joins
}
