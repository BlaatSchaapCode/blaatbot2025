/*
 * timer.cpp
 *
 *  Created on: 18 apr. 2025
 *      Author: andre
 */

#include "timer.hpp"

void Timer::theadCode(void) {
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    if (cv.wait_for(lck, timeout) == std::cv_status::timeout) {
        if (cb)
            cb();
    }
}

void Timer::afterSeconds(callBack cb, std::chrono::seconds timeout) {
    abortTimer();
    this->cb = cb;
    this->timeout = timeout;
    thread = std::thread([this]() { theadCode(); });
}
void Timer::abortTimer(void) {
    cv.notify_all();
    if (thread.joinable())
        thread.join();
}
