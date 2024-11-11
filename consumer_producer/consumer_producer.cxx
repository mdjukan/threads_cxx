#include <thread>
#include <queue>
#include <format>
#include <vector>
#include <string>
#include <iostream>
#include <condition_variable>

using Message = std::string;
using ID = int;

std::queue<Message> msg_queue;
std::mutex mtx;
std::condition_variable cv;

#define NOW std::chrono::system_clock::now()

#define CONSUMER_SLEEP std::chrono::milliseconds(1000)
#define CONSUMER_DURATION std::chrono::milliseconds(10000)

//consumeri su zaglavljeni na wait-u
//kako mogu da im signaliziram da je zavrseno
//kako mogu da signaliziram
void message_printer(ID id) {
	for (;;) {
		std::unique_lock ul(mtx);
		while (not msg_queue.empty()) {
			std::cout << std::format("[consumer {}] sleeping!\n", id);
			//ako nema porizvodjaca koji rade izlaz iz petlje
			//kako mogu da znam da li ima proizvodjaca koji rade?
			cv.wait(ul);
		}

		std::string msg = msg_queue.front();
		msg_queue.pop();
		ul.unlock();
		std::cout << std::format("[consumer {}] message:{}\n", id, msg);
		std::this_thread::sleep_for(CONSUMER_SLEEP);
	}
	std::cout << std::format("[consumer {}] done!\n", id);
}

#define PRODUCER_SLEEP std::chrono::milliseconds(300)
#define PRODUCER_DURATION std::chrono::milliseconds(10000)
std::vector msgs{"hello!", "hi!", "good morning!", "good evning!"};
void message_producer(ID id) {
	auto start_time = NOW;
	while (NOW - start_time < PRODUCER_DURATION) {
		mtx.lock();
		std::string msg = msgs[std::rand()%msgs.size()];
		msg_queue.push(msg);
		mtx.unlock();
		std::cout << std::format("[producer {}] message:{}\n", id, msg);
		cv.notify_one();
		std::this_thread::sleep_for(CONSUMER_SLEEP);
	}
	std::cout << std::format("[producer {}] done!\n", id);
}


#define CONSUMER_COUNT 5
#define PRODUCER_COUNT 2
int main() {
	std::vector<std::thread> consumer_threads;
	for (int i=0; i<CONSUMER_COUNT; i++) {
		consumer_threads.emplace_back(message_printer, i);
	}

	std::vector<std::thread> producer_threads;
	for (int i=0; i<PRODUCER_COUNT; i++) {
		producer_threads.emplace_back(message_producer, i);
	}

	for (std::thread &t : consumer_threads) {
		t.join();
	}

	for (std::thread &t : producer_threads) {
		t.join();
	}
}
