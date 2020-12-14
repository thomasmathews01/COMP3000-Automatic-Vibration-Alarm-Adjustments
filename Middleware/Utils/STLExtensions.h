#pragma once
#include <optional>
#include <string>



namespace STLExtensions {
	std::optional<int> opt_stoi(const std::string& string);

	template <class T>
	T value_or_default(const std::optional<T>& an_optional, const T default_value) {
		return an_optional ? *an_optional : default_value;
	}
};


