#pragma once
#include <chrono>
using namespace std::chrono;

class IClock
{
public:
	[[nodiscard]] virtual system_clock::time_point get_current_time() const noexcept = 0;
};