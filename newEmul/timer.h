#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>
#include <functional>
#include "machine.h"

class Timer {
public:
	Timer(uint32_t clock);
	~Timer();
	void start(std::function<void()> callback, bool async = true);
	void start(Machine *timer, bool async = true);
	void stop();
private:
	uint32_t tickCountInClock;
	bool enabled;
	long long counter;
	long long oldCounter;
	uint32_t ticks;
	float eps;
	void wait();
	Machine* machene;
};


#endif // !__TIMER_H__
