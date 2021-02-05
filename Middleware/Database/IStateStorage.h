#pragma once
#include <stateTypes.h>

class IStateStorage {
public:
	virtual void add_new_state_period(int machine_id, state_period_t state_period) = 0;
	virtual void add_state(int machine_id, const state_t& state) = 0;
	virtual void remove_state(int machine_id, int state_id) = 0;
	virtual std::vector<state_t> get_states_for_machine(int machine_id) = 0;
	virtual std::vector<state_change_t> get_state_changes_for_machine(int machine_id) = 0;
};