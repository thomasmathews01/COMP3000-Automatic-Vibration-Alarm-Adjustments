#pragma once
#include <optional>
#include <string>
#include <range/v3/all.hpp>
#include <boost/hof.hpp>

namespace STLExtensions {
	std::optional<int> opt_stoi(const std::string& string);

	template <class T>
	constexpr T value_or_default(const std::optional<T>& an_optional, const T default_value) {
		return an_optional ? *an_optional : default_value;
	}

    constexpr auto sqr = [](const auto x) { return x * x; };
    constexpr auto difference = [](const auto x, const auto y) { return x - y; };

    template<class T>
    T constexpr sqrtNewtonRaphson(T x, T curr, T prev) {
        return curr == prev
               ? curr
               : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
    }

    constexpr auto sqrt = [](double x) {
        return x >= 0 && x < std::numeric_limits<double>::infinity()
        ? sqrtNewtonRaphson<double>(x, x, 0)
        : std::numeric_limits<double>::quiet_NaN();
    };

    template<class T, int N>
    constexpr float semi_euclidean_distance(const std::array<T, N>& first, const std::array<T, N>& second) {
        return ranges::inner_product(first, second, 0.f, {}, boost::hof::compose(sqr, difference));
    }

	template<class T, int N>
	constexpr float euclidean_distance(const std::array<T, N>& first, const std::array<T, N>& second) {
		return std::sqrt(semi_euclidean_distance(first, second));
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


