#pragma once

#include <array>
#include <memory>
#include <random>
#include <range/v3/all.hpp>
#include <boost/hof/compose.hpp>
#include <STLExtensions.h>

using namespace ranges;

struct point_t {
    int x, y;

    point_t(int x, int y) : x(x), y(y) {}

    [[nodiscard]] std::array<int, 2> as_array() const { return {x, y}; };

    bool operator==(const point_t& rhs) const {
        return x == rhs.x &&
               y == rhs.y;
    }

    bool operator!=(const point_t& rhs) const {
        return !(rhs == *this);
    }
};

// TODO: Move learning functions out into either function objects or lambdas contained in a separate header


// Learning Functions supported, most common according to : http://www.ijmo.org/vol6/504-M08.pdf
// Linear, inverse time, power series

// Supported neighbourhood functions:
// Bubble - just a fixed neighbourhood to be a certain number of node edge traversals for any given node, this could easily be compile time embedded.
// gaussian - continuous, although presumably you then have to go through and further tune the mean/stdd

// TODO: Look into either templating the function call or using boost function references to lower the call cost and maybe get some compile time optimisations in there.

auto get_inverse_time_learning_function(int total_iteration_number) {
    return [total_iteration_number](int iteration_number) {
        const auto initial_learning_rate = 1.0;
        const auto phase = iteration_number / total_iteration_number;

        return initial_learning_rate - (1 - phase);
    };
}

constexpr auto get_max_distance_for_current_iteration = [](int iteration_number, int total_iterations) {
    const auto initial_distance = 50.0; // TODO: Inject - Should be about half the distance across the map.
    const auto phase = iteration_number / total_iterations;

    return initial_distance * phase;
};

template<int NodeDims, int FeatureDims, int IterationCount>
class SOM {
    using som_point_t = std::array<float, FeatureDims>;
    using net = std::array<som_point_t, FeatureDims * FeatureDims>;

public:
    void initialise() {
        nodes = std::make_unique<net>();

        for (auto& arr : *nodes)
            ranges::generate(arr, STLExtensions::get_random_number_generator());
    }

    std::vector<point_t> get_indexes_within_distance_of_element(const point_t& element, const float distance) {
        // We can presumably draw 2 limits. Some indexes are just unusable, anything that is further than d in any dimension, and som have to be, if they are within the distance that is given by the hypotenuse
        const auto dimensions = FeatureDims;

        const auto lower_outer_limit_x = std::max(0, static_cast<int>(std::floor(element.x - distance)));
        const auto upper_outer_limit_x = std::min(dimensions, static_cast<int>(std::floor(element.x + distance)));

        const auto lower_outer_limit_y = std::max(0, static_cast<int>(std::floor(element.y - distance)));
        const auto upper_outer_limit_y = std::min(dimensions, static_cast<int>(std::floor(element.y + distance)));

        const auto inner_boundary_distance = static_cast<int>(std::floor(distance / 1.41)); // TODO: Use a more specific constant

        const auto lower_inner_limit_x = std::max(0, element.x - inner_boundary_distance);
        const auto upper_inner_limit_x = std::min(0, element.x - inner_boundary_distance);
        const auto lower_inner_limit_y = std::max(0, element.y - inner_boundary_distance);
        const auto upper_inner_limit_y = std::min(0, element.y - inner_boundary_distance);

        std::vector<point_t> definite_values;

        const auto is_in_inner_box = [&](const point_t& p) {
            return p.x >= lower_inner_limit_x && p.x <= upper_inner_limit_x &&
                   p.x >= lower_inner_limit_y && p.x <= upper_inner_limit_y;
        };

        for (auto x = lower_outer_limit_x; x <= upper_outer_limit_x; ++x) {
            for (auto y = lower_outer_limit_y; y <= upper_outer_limit_y; ++y) {
                // For all points that exist inside the possible range
                point_t p(x, y);

                if (is_in_inner_box(p))
                    definite_values.emplace_back(p);
                else {
                    const auto real_distance = STLExtensions::euclidean_distance<int, 2>(element.as_array(), p.as_array());
                    if (real_distance <= distance)
                        definite_values.emplace_back(p);
                }
            }
        }

        return definite_values;
    }

    auto lowest_distance_from(const som_point_t& point) {
        return [&point, this](const auto& first, const auto& second) {
            return STLExtensions::euclidean_distance<float, FeatureDims>(first, point) <
                   STLExtensions::euclidean_distance<float, FeatureDims>(second, point);
        };
    }

    typename net::iterator find_bmu(const som_point_t& point) {
        return min_element(*nodes, lowest_distance_from(point));
    }

    constexpr std::array<int, 2> get_idx_as_coords(int idx) {
        return {idx % NodeDims, idx / NodeDims};
    }

    void train(const std::vector<som_point_t>& points, const std::function<float(float, int)>& neighbourhood_function, const std::function<float(int)>& learning_function) {
        for (auto iteration_number = 1; iteration_number <= IterationCount; ++iteration_number) {
            const auto learning_weight = learning_function(iteration_number);

            for (const auto& point : points) {
                const auto bmu = find_bmu(point);
                update_net_based_on_bmu(bmu, point, neighbourhood_function, learning_weight, iteration_number);
            }
        }
    }

    void update_net_based_on_bmu(typename net::iterator bmu, const som_point_t& point,
                                 const std::function<float(float, int)>& neighbourhood_function, float learning_weight,
                                 int iteration_number) {
        const auto idx = std::distance(nodes->begin(), bmu);

        const auto coords = get_idx_as_coords(idx);
        const point_t bmu_point = point_t(coords.at(0), coords.at(1));

        const auto neighbourhood_distance_for_this_iteration = get_max_distance_for_current_iteration(iteration_number, IterationCount); //TODO: Inject the iteration number
        const auto units_that_need_updating = get_indexes_within_distance_of_element(bmu_point, neighbourhood_distance_for_this_iteration);

        for (const auto& unit : units_that_need_updating) {
            const auto unit_idx = unit.x * FeatureDims + FeatureDims;
            std::transform(nodes->at(unit_idx).begin(), nodes->at(unit_idx).end(), point.cbegin(), nodes->at(unit_idx).begin(),
                           [&](const auto first, const auto second) {
                               return first + (STLExtensions::difference(first, second) * learning_weight);
                           });
        }
    }

private:
    std::unique_ptr<net> nodes;
};

