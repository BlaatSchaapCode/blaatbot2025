/*
 * timer.hpp
 *
 *  Created on: 18 apr. 2025
 *      Author: andre
 */

#ifndef UTILS_TIMER_HPP_
#define UTILS_TIMER_HPP_

#include <chrono>
#include <thread>
#include <condition_variable>
#include <functional>

class Timer{
public:
	using callBack = std::function<void()>;
	void afterSeconds(callBack cb, std::chrono::seconds timeout);
	void abortTimer(void);
private:
	callBack cb = nullptr;
	std::thread thread;
	std::condition_variable cv;
	std::chrono::seconds timeout;
	void theadCode(void);



};



#endif /* UTILS_TIMER_HPP_ */
