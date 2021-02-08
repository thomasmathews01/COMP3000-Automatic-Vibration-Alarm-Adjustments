#include "SOMStateDetector.h"
#include "SOMLearningFunctions.h"

void SOMStateDetector::update() noexcept {
	if (requires_update())
		retrain();

	remap_states();

	const int current_state = get_current_state();
	publish(current_state, clock->get_current_time());
}

bool SOMStateDetector::requires_update() const noexcept {
	const auto total_number_of_points = 100000;
	const auto number_of_points_not_within_model = 100; // TODO: Data storage needs an extra method to expose the total number of collected points up to a given time.
	const auto proportion_of_points_outside_model = number_of_points_not_within_model / total_number_of_points;
	const auto too_many_points_missing = proportion_of_points_outside_model > minimum_model_representation;

	const auto time_passed_since_last_excluded_point_was_taken = clock->get_current_time() - time_of_last_point_within_model;
	const auto too_much_time_has_passed = time_passed_since_last_excluded_point_was_taken > minimum_time_before_waiting_to_retrain;

	return too_many_points_missing || too_much_time_has_passed;
}

void SOMStateDetector::retrain() noexcept {
	const auto current_time = clock->get_current_time();
	std::vector<std::pair<time_point_t, map_type_t::som_point_t>> data_points;
	auto idx = 0;
	auto first = true;
	for (const auto& channel : configuration_storage->get_channel_information_for_machine(machine(machine_id))) {
		for (const auto&[type_id, name] : data_storage->get_data_types_available_for_channel(channel.id)) {
			const auto data = data_storage->get_data(channel.id, type_id, time_point_t(0s), current_time);
			if (first) {
				first = false;
				data_points |= actions::push_back(data | views::transform([](const auto& pair)
																		  {
																			  map_type_t::som_point_t data_point{};
																			  data_point.at(0) = pair.second;
																			  return std::make_pair(pair.first, data_point);
																		  }));

				data_points |= actions::sort([](const auto& first, const auto& second) { return first.first < second.first; });
			}
			else {
				for (const auto& pair : data) {
					const auto&[time, value] = pair;
					auto lower_bound = std::lower_bound(data_points.begin(), data_points.end(), time, [](const std::pair<time_point_t, map_type_t::som_point_t> pair, const time_point_t& time)
					{
						return pair.first < time;
					});

					lower_bound->second.at(idx) = value;
				}
			}
			++idx;
		}

	}

	generate_new_model_and_update_values(data_points);
}

void SOMStateDetector::generate_new_model_and_update_values(const std::vector<std::pair<time_point_t, map_type_t::som_point_t>>& data_points) {
	underlying_map.initialise();
	underlying_map.train(data_points | views::values, LearningFunctions::get_inverse_time_learning_function(map_type_t::iterations), NeighbourhoodFunctions::get_inverse_time_neighbourhood_function(map_type_t::iterations, map_type_t::map_size));
	points_contained_within_model = data_points.size();
	cache_the_things(data_points);
}

void SOMStateDetector::remap_states() noexcept {
	/* TODO: If performance proves a problem, abseil have a flat map which might do the trick */
	const auto states = state_storage->get_state_changes_for_machine(machine_id);
	const auto state_changes = state_storage->get_state_changes_for_machine(machine_id);
	state_interpretation = {};

	auto final_time = time_point_t::max();
	for (const auto& change : state_changes | views::reverse) {
		const auto time_comp = [](const auto& pair, const auto time) { return pair.first < time; };
		const auto first = std::lower_bound(time_mapping.cbegin(), time_mapping.cend(), change.change_time, time_comp);
		const auto last = std::lower_bound(time_mapping.cbegin(), time_mapping.cend(), final_time, time_comp);
		// If one was being clever, the first value is always the last value for the next iteration and it could be kept, for the sake of < 30 comparisons.

		for (const auto& [time, node_index] : ranges::subrange(first, last)) {
			auto& map = state_interpretation.at(node_index);
			if (map.count(change.new_state_id))
				++map.at(change.new_state_id);
			else map.insert({change.new_state_id, 1});
		}

		final_time = change.change_time;
	}
}

int SOMStateDetector::get_current_state() const noexcept {
	const auto current_time = clock->get_current_time();
	map_type_t::som_point_t current_points;

	auto idx = 0;
	for (const auto& channel : configuration_storage->get_channel_information_for_machine(machine(machine_id))) {
		const auto data_types_available_for_channel = data_storage->get_data_types_available_for_channel(channel.id);
		for (const auto&[type_id, name] : data_types_available_for_channel) {
			const auto [_, value] = data_storage->get_last_data_point_before(channel.id, type_id, current_time);
			current_points.at(idx++) = value;
		}
	}

	const auto bmu = underlying_map.find_bmu_for(current_points);
	const auto state_map = state_interpretation.at(bmu);
	const auto state_with_most_attached_points = std::max_element(state_map.cbegin(), state_map.cend(), [](const std::pair<int, int>& x, const std::pair<int, int>& y) { return x.second < y.second; });

	return state_with_most_attached_points->first;
	/*
	 * The problem with this implementation is that is has absolutely no stability. Really we want to sample the last n points, and determine whether we are sure what state they belong to
	 * and if that doesn't give us a clear option we then want to report the special state of unknown.
	 * TODO: Improve, and consider things like time decay
	 */
}

void SOMStateDetector::publish(const int state, const time_point_t time) const noexcept {
	const auto current_states_in_database = state_storage->get_state_changes_for_machine(machine_id);
	const auto current_state = current_states_in_database.back().new_state_id;

	if (state != current_state)
		state_storage->add_new_state_changes_for_machine({state_change_t(state, time)}, machine_id);
}

SOMStateDetector::SOMStateDetector(int machine_number, std::shared_ptr<IDataAccess> data_storage, std::shared_ptr<IClock> clock, std::shared_ptr<IStateStorage> state_storage)
	: machine_id(machine_number), data_storage(std::move(data_storage)), clock(std::move(clock)), state_storage(std::move(state_storage)) {
}

void SOMStateDetector::cache_the_things(const std::vector<std::pair<time_point_t, map_type_t::som_point_t>>& points) noexcept {
	time_mapping.clear();
	auto& map = underlying_map;
	time_mapping |= actions::push_back(points | views::transform([&](const auto& pair) { return std::pair<time_point_t, int>{pair.first, map.find_bmu_for(pair.second)}; }));
}
