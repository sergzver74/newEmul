#include "timer.h"
#include <thread>
#include <Windows.h>

Timer::Timer(uint32_t clock) {
	uint32_t a, b, c, d;
	long long x, y, z;
	long long m[10];
	uint8_t i;
	for (i = 0; i < 10; i++)
	{
		__asm
		{
			rdtsc;
			mov a, eax;
			mov b, edx;
		}
		Sleep(100);
		__asm
		{
			rdtsc;
			mov c, eax;
			mov d, edx;
		}
		x = b;
		x = (x << 32) | a;
		y = d;
		y = (y << 32) | c;
		m[i] = y - x;
	}
	z = 0;
	for (i = 0; i < 10; i++) z = z + m[i];
	z = z / 10;
	z = z * 10;
	z = z / clock;
	//tickCountInClock = (uint32_t)((float)z / (float)1.62);
	tickCountInClock = z;
	enabled = false;
	counter = 0;
	oldCounter = 0;
	ticks = clock;
	eps = 1;
}

Timer::~Timer() {
	enabled = false;
	tickCountInClock = 0;
	ticks = 0;
	counter = 0;
	oldCounter = 0;
	eps = 0;
}

void Timer::wait() {
	uint32_t a, b, c, d;
	long long x, y, z;
	__asm
	{
		rdtsc;
		mov a, eax;
		mov b, edx;
	}
	x = b;
	x = (x << 32) | a;
	do {
		__asm
		{
			rdtsc;
			mov c, eax;
			mov d, edx;
		}
		y = d;
		y = (y << 32) | c;
	} while (y - x <= tickCountInClock);

}

void Timer::start(std::function<void()> callback, bool async) {
	enabled = true;
	
	std::thread([=] {
		while (1)
		{
			if (!enabled) break;
			Sleep(1000);
			long long dCounter = counter - oldCounter;
			oldCounter = counter;
			eps = (float)dCounter / (float)ticks;
			tickCountInClock = (uint32_t)((float)tickCountInClock * (float)eps);
			//printf("counter=%lld ticks=%d eps=%f:2:4 tickCountInClock=%d\n", dCounter, ticks, eps, tickCountInClock);
		}
		}).detach();

	if (async) {
		std::thread([=] {
			while (1)
			{
				if (!enabled) break;
				wait();
				counter++;
				callback();
			}
			}).detach();
	}
	else {
		wait();
		counter++;
		callback();
	}
}

void Timer::start(Machine *timer, bool async) {
	enabled = true;

	std::thread([=] {
		while (1)
		{
			Sleep(1000);
			long long dCounter = counter - oldCounter;
			oldCounter = counter;
			eps = (float)dCounter / (float)ticks;
			tickCountInClock = (uint32_t)((float)tickCountInClock * (float)eps);
			//printf("counter=%lld ticks=%d eps=%f:2:4 tickCountInClock=%d\n", dCounter, ticks, eps, tickCountInClock);
			if (!enabled) break;
		}
		}).detach();

		if (async) {
			std::thread([=] {
				while (1)
				{
					wait();
					counter++;
					timer->timer();
					if (!enabled) break;
				}
				}).detach();
		}
		else {
			wait();
			counter++;
			timer->timer();
		}
}

void Timer::stop() {
	enabled = false;
}