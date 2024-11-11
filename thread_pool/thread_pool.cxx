#include <thread>
#include <functional>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>

using Task = std::function<void()>;

class ThreadPool {
	std::vector<std::thread> workers;
	std::queue<Task> tasks;
	std::mutex mtx;
	std::condition_variable cv;

	public:
	ThreadPool() {
		auto worker = [this]() {
			for (;;) {
				std::unique_lock ul(mtx);
				while (tasks.empty()) {
					cv.wait(ul);
				}

				Task task = tasks.front();
				tasks.pop();
				ul.unlock();
				task();
			}
		};

		for (size_t i=0; i<std::thread::hardware_concurrency(); i++) {
			workers.emplace_back(worker);
		}
	}

	//isti api kao za pravljenje thread-a (lambda, args...)
	void addTask(Task task) {
		std::unique_lock lg(mtx);
		tasks.push(task);
		lg.unlock();
		cv.notify_one();
	}

	~ThreadPool() {
		//sta se desava u destruktoru
		//kako da ugasim thread-pool?
		//ovo je za sada trajno blokirajuce
		for (std::thread &worker : workers) {
			worker.join();
		}
	}
};

void task() {
	std::cout << "hello!\n";
}

int main() {
	ThreadPool tp;
	tp.addTask([](){std::cout << "hello from lambda\n";});
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);
	tp.addTask(task);

	//ovo je main thread, pozvace destrukor kada se ova funkcija izvrsi
}
