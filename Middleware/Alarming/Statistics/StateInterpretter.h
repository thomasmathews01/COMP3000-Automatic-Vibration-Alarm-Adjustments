#pragma once
#include <vector>
#include <map>
#include <types.h>
#include <stateTypes.h>

class StateInterpretter {
public:
	static std::vector<std::map<int , int>> get_state_mappings(const std::vector<state_change_t>& changes, const std::vector<std::pair<time_point_t, int>>& time_node_map) noexcept;
};


