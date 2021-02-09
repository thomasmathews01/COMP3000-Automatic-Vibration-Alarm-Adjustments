#include <gtest/gtest.h>
#include <Statistics/StateInterpretter.h>
#include <random>
#include <range/v3/all.hpp>

namespace
{
	auto get_random_number_generator(const int min, const int max) {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<> dist(min, max);
		return [rng, dist]() mutable -> int {

			return dist(rng);
		};
	}
}
TEST (StateInterpretterTests, canCallMethod) {
	ASSERT_NO_THROW(StateInterpretter::get_state_mappings({}, {}));
}

TEST (StateInterpretterTests, canhandleSingleState) {
	state_change_t change(2, time_point_t(100s));
	const auto mappings = StateInterpretter::get_state_mappings({change}, {{time_point_t(105s), 1}, {time_point_t(106s), 2}, {time_point_t(107s), 1}});
	// We fed in a state of 2 for 2 points on node 1 and 1 point on node 2
	EXPECT_EQ(mappings.at(1).at(2), 2);
	EXPECT_EQ(mappings.at(2).at(2), 1);
}

TEST (StateInterpretterTests, CanHandleManyStates) {
	std::vector<state_change_t> changes = {
		{1, time_point_t(100s)},
		{2, time_point_t(200s)}
	};
	std::vector<std::pair<time_point_t, int>> node_map = {
		{time_point_t(105s), 1},
		{time_point_t(106s), 2},
		{time_point_t(107s), 2},
		{time_point_t(201s), 1},
		{time_point_t(202s), 3},
		{time_point_t(203s), 3},
	};

	const auto mappings = StateInterpretter::get_state_mappings(changes, node_map);
	EXPECT_EQ(1, mappings.at(1).at(1));
	EXPECT_EQ(1, mappings.at(1).at(2));
	EXPECT_EQ(2, mappings.at(2).at(1));
	EXPECT_EQ(2, mappings.at(3).at(2));
}

TEST (StateInterpretterTests, CanPerformMappingInReasonableTime) {
	constexpr auto data_point_count = 200000;
	std::vector<state_change_t> changes = {
		{1, time_point_t(100s)},
		{2, time_point_t(200s)},
		{3, time_point_t(500s)},
		{4, time_point_t(6000s)},
		{5, time_point_t(12000s)},
		{6, time_point_t(12345s)},
		{7, time_point_t(50000s)},
		{8, time_point_t(90000s)},
		{9, time_point_t(90001s)},
	};

	auto generator = get_random_number_generator(0,10000);

	const auto node_map = ranges::views::iota(0, data_point_count)
					| ranges::views::transform([&generator](const auto i){return std::make_pair(time_point_t(std::chrono::seconds(100 + i)), generator());})
					| ranges::to<std::vector>();

	const auto start = std::chrono::high_resolution_clock::now();
	const auto mappings = StateInterpretter::get_state_mappings(changes, node_map);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Remapping the states for " << data_point_count << " points took: " << duration.count() << "ms. " << std::endl;

	EXPECT_LE(duration.count(), 500);
}