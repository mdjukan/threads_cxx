#include <thread>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <queue>
#include <format>

template<typename T>
class Channel {
	private:
		std::queue<T> m_messages;
		std::mutex m_mtx;
		std::condition_variable m_cv;
	public:
		Channel() {}
		void operator<<(T &message) {
			std::unique_lock ul(m_mtx);
			m_messages.push(message);
			ul.unlock();
			m_cv.notify_one();
		}

		void operator>>(T &message) {
			std::unique_lock ul(m_mtx);
			while (m_messages.empty()) {
				m_cv.wait(ul);
			}

			message = m_messages.front();
			m_messages.pop();
			ul.unlock();
		}
};

int main() {
	Channel<std::string> channel;
	std::thread t1([&channel](){
			std::string msg;
			channel >> msg;
			std::cout << std::format("t1: {}\n", msg);
			});

	std::thread t2([&channel](){
			std::string msg;
			channel >> msg;
			std::cout << std::format("t2: {}\n", msg);
			});

	std::thread t3([&channel](){
			std::string msg("hello from thread3");
			channel << msg;
			channel << msg;
			});

	t1.join();
	t2.join();
	t3.join();
}
