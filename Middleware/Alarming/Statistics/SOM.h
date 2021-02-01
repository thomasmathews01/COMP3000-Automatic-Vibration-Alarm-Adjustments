#pragma once

#include <array>
#include <memory>
#include <random>
#include <range/v3/all.hpp>
#include <boost/hof/compose.hpp>

using namespace ranges;

constexpr auto difference = [](const auto first, const auto second) { return first - second; };
constexpr auto square = [](const auto first) { return first * first; }; // TODO: Move to STL Extensions

template<class T, int N>
float euclidean_distance_between(const std::array<T, N>& first, const std::array<T, N>& second) {
    return std::sqrt(inner_product(first, second, 0.f, {}, boost::hof::compose(square, difference)));
}

template<int NodeDims, int FeatureDims>
class SOM {
    using som_point_t = std::array<float, FeatureDims>;
    using net = std::array<som_point_t, FeatureDims * FeatureDims>;

public:
    void initialise() {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::normal_distribution<float> dist(-1.f * std::numeric_limits<float>::max(),
                                             std::numeric_limits<float>::max());

        nodes = std::make_unique<net>();

        for (auto& arr : *nodes)
            std::generate(arr.begin(), arr.end(), [&dist, &rng]() mutable { return dist(rng); });
    }

    auto lowest_distance_from(const som_point_t& point) {
        return [&point, this](const auto& first, const auto& second) {
            return euclidean_distance_between<float, FeatureDims>(first, point) <
                    euclidean_distance_between<float, FeatureDims>(second, point);
        };
    }

    typename net::iterator find_bmu(const som_point_t& point) {
        return min_element(*nodes, lowest_distance_from(point));
    }

    constexpr std::array<int, 2> get_idx_as_coords(int idx) {
        return {idx % NodeDims, idx / NodeDims};
    }

    void
    train(const std::vector<som_point_t>& points, const std::function<float(float, int)>& neighbourhood_function,
          const std::function<float(int)>& learning_function) {
        for (auto iteration_number = 1; iteration_number < 1000; ++iteration_number) {
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

        for (int i = 0; i < nodes->size(); ++i) {
            const auto distance_from_bmu = euclidean_distance_between<int, 2>(get_idx_as_coords(idx), get_idx_as_coords(i));
            const auto neighbourhood_weight = neighbourhood_function(distance_from_bmu, iteration_number);

            std::transform(nodes->at(i).begin(), nodes->at(i).end(), point.cbegin(), nodes->at(i).begin(),
                           [&](const auto first, const auto second) {
                               return first + (difference(first, second) * neighbourhood_weight * learning_weight);
                           });
        }
    }

private:
    std::unique_ptr<net> nodes;
};

