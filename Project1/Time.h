#pragma once
#include <chrono>

class Timer
{
public:
	Timer() {

		start_time = std::chrono::steady_clock::now();
	};
	void Reset(){
		 start_time =  std::chrono::steady_clock::now();
	}
	const long long TimePassedMilli(){	
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
	}
	const long long TimePassedNano() {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start_time).count();
	}
	const long long TimePassedSec() {
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time).count();
	}
	const long long TimePassedMicro() {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_time).count();
	}
private:	
	std::chrono::steady_clock::time_point start_time;
};