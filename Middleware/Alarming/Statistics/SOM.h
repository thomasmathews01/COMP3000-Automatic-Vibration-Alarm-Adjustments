#pragma once

#include <array>
#include <memory>
#include <random>
#include <execution>
#include <range/v3/all.hpp>
#include <STLExtensions.h>

using namespace ranges;

template<int NodeDims, int FeatureDims, int IterationCount>
class SOM {
public:
	using som_point_t = std::array<float, FeatureDims>;
	using net = std::array<som_point_t, NodeDims * NodeDims>;
	constexpr static auto iterations = IterationCount;
	constexpr static auto map_size = NodeDims;
	constexpr static auto featureDimensions = FeatureDims;
	std::unique_ptr<net> nodes;

	void initialise() noexcept {
		nodes = std::make_unique<net>(); // Has to be a pointer, because it is unlikely to fit on the stack.

		for (auto& node : *nodes)
			ranges::generate(node, get_random_number_generator()); // Place node in random location.
	}

	struct point_t {
		int x, y;

		constexpr point_t(int x, int y) : x(x), y(y) {}
		constexpr point_t(const std::tuple<int, int>& pair) : x(std::get<0>(pair)), y(std::get<1>(pair)) {}
		constexpr explicit point_t(int idx) : x(idx % NodeDims), y(idx / NodeDims) {}

		[[nodiscard]] constexpr float semi_distance_from(const point_t& other) const noexcept {
			return STLExtensions::semi_euclidean_distance<int, 2>({x, y}, {other.x, other.y});
		}

		constexpr bool operator==(const point_t& rhs) const noexcept {
			return x == rhs.x &&
				   y == rhs.y;
		}

		constexpr bool operator!=(const point_t& rhs) const noexcept {
			return !(rhs == *this);
		}

		constexpr operator int() const noexcept {
			return x + (y * NodeDims);
		}
	};

	struct bounding_box {
		point_t lower, upper;

		constexpr explicit bounding_box(const std::pair<point_t, point_t>& bounds) : lower(bounds.first), upper(bounds.second) {}

		constexpr bool contains(const point_t& p) const noexcept {
			return p.x >= lower.x && p.x <= upper.x &&
				   p.y >= lower.y && p.y <= upper.y;
		}

		constexpr auto all_contained_points() const noexcept {
			return views::cartesian_product(views::iota(lower.x, upper.x + 1), views::iota(lower.y, upper.y + 1))
				   | views::transform([](const auto& pair) { return point_t(pair); });
		}
	};

	constexpr point_t find_bmu_for(const som_point_t& point) const noexcept {
		const auto distance_from_point = [&point](const som_point_t& other) { return STLExtensions::semi_euclidean_distance<float, FeatureDims>(point, other); };
		const auto closer_to_point = [&point, distance_from_point](const auto& first, const auto& second) { return distance_from_point(first) < distance_from_point(second); };

		const auto closest_element = std::min_element(std::execution::par_unseq, nodes->cbegin(), nodes->cend(), closer_to_point); // CPU parallel and vectorised search for closest node.

		return point_t(std::distance(nodes->cbegin(), closest_element));
	}

	constexpr bounding_box get_inner_bounding_box(const point_t& element, const float distance) const noexcept {
		const auto inner_boundary_distance = static_cast<int>(std::floor(distance / STLExtensions::sqrt(2.0)));

		return bounding_box({{std::max(0, element.x - inner_boundary_distance), std::max(0, element.y - inner_boundary_distance)},
							 {std::min(NodeDims, element.x + inner_boundary_distance), std::min(NodeDims, element.y + inner_boundary_distance)}});
	}

	constexpr bounding_box get_outer_bounding_box(const point_t& element, const float distance) const noexcept {
		return bounding_box({{std::max(0, static_cast<int>(std::floor(element.x - distance))),        std::max(0, static_cast<int>(std::floor(element.y - distance)))},
							 {std::min(NodeDims, static_cast<int>(std::floor(element.x + distance))), std::min(NodeDims, static_cast<int>(std::floor(element.y + distance)))}});
	}

	constexpr auto points_within_distance(const point_t& element, const float distance) const noexcept {
		const auto inner_box = get_inner_bounding_box(element, distance); // Points that have to be within the distance
		const auto outer_box = get_outer_bounding_box(element, distance); // Points that could be within the distance, and need testing with euclidean distance.

		const auto point_within_distance = [&element, distance, &inner_box](const auto& point) { return inner_box.contains(point) || element.semi_distance_from(point) <= (distance * distance); }; // We take squared distance to avoid expensive sqrts.

		const auto inner_points = outer_box.all_contained_points() | views::filter([&element, distance, &inner_box](const auto& point) { return inner_box.contains(point); });
		return outer_box.all_contained_points() | views::filter(point_within_distance);
	}

	constexpr auto pull_together(const float learning_weight) const noexcept {
		return [learning_weight](const auto first, const auto second) { return first + (STLExtensions::difference(first, second) * learning_weight); };
	}

	constexpr void update_net_based_on_bmu(const point_t& bmu, const som_point_t& training_point, const float learning_weight, const int iteration_number, const float neighbourhood_size) const noexcept {
		auto points = points_within_distance(bmu, neighbourhood_size);

		std::for_each(std::execution::par_unseq, points.begin(), points.end(), [&](const int unit) {
			std::transform(nodes->at(unit).cbegin(), nodes->at(unit).cend(), training_point.cbegin(), nodes->at(unit).begin(), pull_together(learning_weight));
		});
	}

	template<class learningFunction, class neighbourhoodFunction, class Rng>
	constexpr void train(const Rng& training_data, learningFunction learning_function, neighbourhoodFunction neighbourhood_function) {
		for (auto iteration_number = 1; iteration_number <= IterationCount; ++iteration_number) {
			const auto learning_weight = learning_function(iteration_number);
			const auto neighbourhood_size = neighbourhood_function(iteration_number);

			for (const auto& training_point : training_data) {
				const auto bmu = find_bmu_for(training_point);
				update_net_based_on_bmu(bmu, training_point, learning_weight, iteration_number, neighbourhood_size);
			}
		}
	}

	auto get_random_number_generator() {
		std::random_device dev;
		std::mt19937 rng(dev());
		std::normal_distribution<float> dist(-1.f * std::numeric_limits<float>::max(),
											 std::numeric_limits<float>::max());
		return [rng, dist]() mutable {

			return dist(rng);
		};
	}
};

