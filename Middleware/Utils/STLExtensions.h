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
		return std::transform_reduce(first.cbegin(), first.cend(), second.cbegin(), 0, [](const auto first, const auto second) {
			return (first - second) * (first - second); }, std::plus<T>{});
    }

	template<class T, int N>
	constexpr float euclidean_distance(const std::array<T, N>& first, const std::array<T, N>& second) {
		return std::sqrt(semi_euclidean_distance(first, second));
	}
};


