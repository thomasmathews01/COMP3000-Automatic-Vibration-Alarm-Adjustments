#include "InternalClock.h"

std::chrono::time_point<std::chrono::high_resolution_clock> InternalClock::get_current_time() const {
	return std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(adjustment_milliseconds);
}

InternalClock& InternalClock::get() {
	static InternalClock clock;

	return clock;
}


