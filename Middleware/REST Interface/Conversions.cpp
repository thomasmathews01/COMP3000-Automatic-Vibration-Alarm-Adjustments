#include "Conversions.h"
#include <doctest/doctest.h>
#include <algorithm>
#include <iterator>

std::vector<state_period_t> Conversions::convert_state_changes_to_state_periods(std::vector<state_change_t>&& state_changes) {
	std::sort(state_changes.begin(), state_changes.end());
	state_changes.emplace_back(0, time_point_t::max()); // We assume we continue in the final state until the end of time, so add a change at the end of time and prevent branching.

	std::vector<state_period_t> state_periods;
	std::transform(state_changes.cbegin() + 1, state_changes.cend(), state_changes.begin(), std::back_inserter(state_periods), [](const auto& next_state, const auto& current_state)
	{
		return state_period_t(current_state.new_state_id, current_state.change_time, next_state.change_time);
	});

	return state_periods;
}

std::vector<state_period_t> Conversions::convert_state_changes_to_state_periods(const std::vector<state_change_t>& state_changes) {
	auto cpy = state_changes;

	return Conversions::convert_state_changes_to_state_periods(std::move(cpy));
}

TEST_CASE ("Simple single continuous state") {
	std::vector<state_change_t> state_changes;
	state_changes.emplace_back(1, time_point_t(150s));
	const auto state_periods = Conversions::convert_state_changes_to_state_periods(std::move(state_changes));

		CHECK_EQ(1, state_periods.size());
		CHECK_EQ(time_point_t(150s), state_periods.at(0).start);
		CHECK_EQ(time_point_t::max(), state_periods.at(0).end);
}

TEST_CASE ("Handles multiple states") {
	std::vector<state_change_t> state_changes;
	state_changes.emplace_back(1, time_point_t(150s));
	state_changes.emplace_back(2, time_point_t(300s));
	const auto state_periods = Conversions::convert_state_changes_to_state_periods(std::move(state_changes));

		CHECK_EQ(2, state_periods.size());
		CHECK_EQ(time_point_t(150s), state_periods.at(0).start);
		CHECK_EQ(time_point_t(300s), state_periods.at(0).end);
		CHECK_EQ(time_point_t(300s), state_periods.at(1).start);
		CHECK_EQ(time_point_t::max(), state_periods.at(1).end);
}