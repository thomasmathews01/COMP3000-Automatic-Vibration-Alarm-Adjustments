#include "StateInterpretter.h"
#include <range/v3/all.hpp>

std::vector<state_period_t> convert_state_changes_to_state_periods(std::vector<state_change_t> state_changes) {
	std::sort(state_changes.begin(), state_changes.end());
	state_changes.emplace_back(0, time_point_t::max()); // We assume we continue in the final state until the end of time, so add a change at the end of time and prevent branching.

	std::vector<state_period_t> state_periods;
	std::transform(state_changes.cbegin() + 1, state_changes.cend(), state_changes.begin(), std::back_inserter(state_periods), [](const auto& next_state, const auto& current_state)
	{
		return state_period_t(current_state.new_state_id, current_state.change_time, next_state.change_time);
	});

	return state_periods;
}

void insert_or_increment_state_id(std::map<int, int>& map, int state_id) {
	if (map.count(state_id))
		++map.at(state_id);
	else map.insert({state_id, 1});
}

auto nodes_in_time_range(time_point_t start, time_point_t end, const std::vector<std::pair<time_point_t, int>>& time_node_map) {
	const auto time_comp = [](const std::pair<time_point_t, int>& pair, const time_point_t time) { return pair.first < time; };
	const auto first = std::lower_bound(time_node_map.cbegin(), time_node_map.cend(), start, time_comp);
	const auto last = std::lower_bound(time_node_map.cbegin(), time_node_map.cend(), end, time_comp);

	return ranges::subrange(first, last);
}

std::vector<std::map<int, int>> StateInterpretter::get_state_mappings(const std::vector<state_change_t>& changes, const std::vector<std::pair<time_point_t, int>>& time_node_map) noexcept {
	if (time_node_map.empty()) return {};
	const auto node_count = ranges::max(time_node_map | ranges::views::values);
	std::vector<std::map<int, int>> state_interpretation(node_count + 1);

	for (const auto& state_period : convert_state_changes_to_state_periods(changes))
		for (const auto node_index : nodes_in_time_range(state_period.start, state_period.end, time_node_map) | ranges::views::values)
			insert_or_increment_state_id(state_interpretation.at(node_index), state_period.state_id);

	return state_interpretation;
}
