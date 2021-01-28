#pragma once

#include "IClock.h"

class RealClock : public IClock {
public:
private:
	time_point_t get_current_time() const noexcept override;
};


