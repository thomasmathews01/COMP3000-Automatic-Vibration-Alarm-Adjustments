#pragma once

#include <chrono>
#include "types.h"

using namespace std::chrono;

class IClock {
public:
    virtual ~IClock() = default;
	[[nodiscard]] virtual time_point_t get_current_time() const noexcept = 0;
};