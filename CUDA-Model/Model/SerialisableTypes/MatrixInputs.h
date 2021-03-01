// MatrixInput.hpp

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     MatrixInput.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <nlohmann/json.hpp>
#include "helper.h"

namespace quicktype {
	using nlohmann::json;

	class MatrixInput {
	public:
		MatrixInput() = default;
		virtual ~MatrixInput() = default;

	private:
		std::vector<float> first;
		std::vector<float> second;

	public:
		const std::vector<float> & get_first() const { return first; }
		std::vector<float> & get_mutable_first() { return first; }
		void set_first(const std::vector<float> & value) { this->first = value; }

		const std::vector<float> & get_second() const { return second; }
		std::vector<float> & get_mutable_second() { return second; }
		void set_second(const std::vector<float> & value) { this->second = value; }
	};
}