#pragma once

#include <vector>
#include "Server.h"

class Conversions {
public:
	static std::vector<state_period_t> convert_state_changes_to_state_periods(std::vector<state_change_t>&& state_changes);
};


