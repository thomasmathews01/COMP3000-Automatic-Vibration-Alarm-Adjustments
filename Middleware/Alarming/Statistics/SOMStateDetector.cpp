#include "SOMStateDetector.h"

void SOMStateDetector::update() noexcept {
	if (requires_update())
		retrain();

	remap_states();

	const int current_state = get_current_state();
	publish(current_state);
}

bool SOMStateDetector::requires_update() const noexcept {
	const auto total_number_of_points = 100000;
	const auto number_of_points_not_within_model = 100;
	const auto proportion_of_points_outside_model = number_of_points_not_within_model / total_number_of_points;
	const auto too_many_points_missing = proportion_of_points_outside_model > minimum_model_representation;

	const auto time_passed_since_last_excluded_point_was_taken = clock->get_current_time() - time_of_last_point_within_model;
	const auto too_much_time_has_passed = time_passed_since_last_excluded_point_was_taken > minimum_time_before_waiting_to_retrain;

	return too_many_points_missing || too_much_time_has_passed;
}

void SOMStateDetector::retrain() noexcept {
	// const auto data = data_storage->get_points_only_between(start, end);
	// Merge the data points into fixed size array of the correct length, with
	// some sort of deterministic way of determining the order of the features
	// within those arrays, so that they can be realistically found later.
	// som.train(data, LearningFunctions::inverse_time, NeighbourhoodFunctions::inverse_time;
}

void SOMStateDetector::remap_states() noexcept {
	// This could end up being expensive as fuck, so we need to have some sort of switch that can tell us when the state periods have updated.
	// Thankfully having split out the management of the state storage into its own container this now shouldn't be that difficult, we should
	// be able to hold an in-memory cache, which we can then update along with a timestamp indicating the last update time, without that ever
	// having to enter the database, because that isn't information we ever want to have persistently stored anyway.

	// Data structure:
	// The format for the storage of the states should probably be a map of maps, each point in the SOM needs to be mapped to a map detailing
	// the number of points assigned to a given state that are closest to that map point
	// We need a cleverer caching system than just re-accessing all the points all the time, which is far to expensive for the disk.
	// Which I think we could do just with timestamps. As long as the underlying SOM doesn't change, which would invalidate the cache, we could
	// just cache time points against the individual nodes in the SOM, then to calculate the state of a particular point all we really need
	// to do is map the times to states, which should be easier. Then mapping the state to the statistics would be easier as well.
}

int SOMStateDetector::get_current_state() const noexcept {
	// We want to get the latest points for the latest timestamp from the data storage
	// Map these to a BMU, then take the map associated with the BMU and select the state with the
	// highest number of matches logged against that unit.
	return 0;
}

void SOMStateDetector::publish(const int state) const noexcept {
	// Publish the current state of the machine to the state cache. If this differs wih the last user accepted state that isn't our problem, that is for an external system ( which can arrive later ) to handle.
}

SOMStateDetector::SOMStateDetector(int machine_number, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IClock> clock, std::shared_ptr<IStateStorage> state_storage)
	: machine(machine_number), data_storage(std::move(data_storage)), clock(std::move(clock)), state_storage(std::move(state_storage)) {
	underlying_map.initialise();
}
