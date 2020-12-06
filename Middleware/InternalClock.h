#pragma once

#include <chrono>

class InternalClock {
public:
	static InternalClock& get();
	std::chrono::time_point<std::chrono::high_resolution_clock> get_current_time() const;
private:
	int adjustment_milliseconds = 0;
};


