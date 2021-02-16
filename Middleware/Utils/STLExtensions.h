#pragma once

#include <optional>
#include <string>
#include <numeric>
#include <algorithm>
#include <algorithm>
#include <range/v3/all.hpp>
#include <boost/hof.hpp>

namespace STLExtensions
{
	std::optional<int> opt_stoi(const std::string& string);

	template<class T>
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

	constexpr auto sqrt = [](double x)
	{
		return x >= 0 && x < std::numeric_limits<double>::infinity()
			   ? sqrtNewtonRaphson<double>(x, x, 0)
			   : std::numeric_limits<double>::quiet_NaN();
	};

	template<class T, int N>
	constexpr float semi_euclidean_distance(const std::array<T, N>& first, const std::array<T, N>& second) {
		return std::transform_reduce(first.cbegin(), first.cend(), second.cbegin(), 0, std::plus<T>{}, [](const auto first, const auto second)
		{
			return (first - second) * (first - second);
		});
	}

	template<class T, int N>
	constexpr float euclidean_distance(const std::array<T, N>& first, const std::array<T, N>& second) {
		return std::sqrt(semi_euclidean_distance(first, second));
	}

	template<typename It, typename It2>
	int count_occurances_of_subrange(It first, It second, It2 substring_first, It2 substring_second) {
		int occurances = -1;
		while (first != second) {
			++occurances;
			first = std::search(first, second, substring_first, substring_second);
			if (first != second)
				++first; // Prevent infinite loop
		}

		return occurances;
	}
};


