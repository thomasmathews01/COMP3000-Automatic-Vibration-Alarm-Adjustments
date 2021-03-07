#pragma once
#include <stateTypes.h>
#include <optional>
#include "IStateStorage.h"
#include "IDatabaseFactory.h"
#include "Helpers/sqlite3pp.h"

class StateStorage : public IStateStorage {
public:
	explicit StateStorage(const std::shared_ptr<IDatabaseFactory>& factory) : database(factory->get_database()) {}

	void add_new_state_period(int machine_id, state_period_t state_period) final;
	void add_state(int machine_id, const state_t& state) final;
    void remove_state(int machine_id, int state_id) final;
    std::vector<state_t> get_states_for_machine(int machine_id) final;
    std::vector<state_change_t> get_state_changes_for_machine(int machine_id) final;
	void add_new_state_changes_for_machine(const std::vector<state_change_t>& changes, int machine_id) final;

private:
    std::optional<int> get_state_at_time(const time_point_t& time, int machine_id);
    std::optional<time_point_t> get_time_of_last_state_change_before(const time_point_t& time, int machine_id);

    const std::shared_ptr<tao::pq::connection> database; // This is a shared pointer for testing purposes, but should never be accessed by anyone other than this class, with the mutex, unless they take ownership whilst they use it, and we can't access it in that period.
};
