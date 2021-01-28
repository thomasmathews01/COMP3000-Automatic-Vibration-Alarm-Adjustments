#include "RealClock.h"

time_point_t RealClock::get_current_time() const noexcept {
	return std::chrono::system_clock::now();
}
