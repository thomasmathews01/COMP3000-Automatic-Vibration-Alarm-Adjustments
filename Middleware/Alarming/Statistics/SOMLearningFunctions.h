
namespace LearningFunctions
{
	constexpr auto get_inverse_time_learning_function(int total_iterations) noexcept {
		return [total_iterations](int iteration_number) constexpr noexcept
		{
			const auto initial_learning_rate = 1.0;
			const auto phase = iteration_number / total_iterations;

			return initial_learning_rate - (1 - phase);
		};
	}
}

namespace NeighbourhoodFunctions {
	constexpr auto get_inverse_time_neighbourhood_function(const int total_iterations, const int map_size) noexcept {
		return [total_iterations, map_size] (const int iteration_number) constexpr noexcept
		{
			const auto initial_distance = map_size / 2; // Initial size encompasses half the map
			const auto phase = iteration_number / total_iterations; // Linear reduction in neighbourhood size.

			return initial_distance * phase; // Final size should reach 0, ie only the unit itself being updated.
		};
	}
}