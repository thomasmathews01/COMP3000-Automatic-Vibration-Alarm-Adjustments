#pragma once

#include "./types.h"

struct state_t {
	int state_id;
	std::string state_name;
};

struct state_change_t {
	int new_state_id;
	time_point_t change_time;

	state_change_t(int new_state_id, const time_point_t& change_time) : new_state_id(new_state_id), change_time(change_time) {}

	bool operator<(const state_change_t& rhs) const {
		return change_time < rhs.change_time;
	}

	bool operator>(const state_change_t& rhs) const {
		return rhs < *this;
	}

	bool operator<=(const state_change_t& rhs) const {
		return !(rhs < *this);
	}

	bool operator>=(const state_change_t& rhs) const {
		return !(*this < rhs);
	}
};

struct state_period_t {
	int state_id;
	time_point_t start;
	time_point_t end;

	state_period_t(int state_id, const time_point_t& start, const time_point_t& an_end) : state_id(state_id), start(start), end(an_end) {}

	bool operator<(const state_period_t& rhs) const {
		return start < rhs.start;
	}

	bool operator>(const state_period_t& rhs) const {
		return rhs < *this;
	}

	bool operator<=(const state_period_t& rhs) const {
		return !(rhs < *this);
	}

	bool operator>=(const state_period_t& rhs) const {
		return !(*this < rhs);
	}
};