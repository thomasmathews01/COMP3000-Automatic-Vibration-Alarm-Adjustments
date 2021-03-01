// Generators.hpp

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Generators.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <nlohmann/json.hpp>
#include "helper.h"

#include "MatrixResponse.h"
#include "MatrixInputs.h"

namespace nlohmann
{
	void from_json(const json& j, quicktype::MatrixResponse& x);
	void to_json(json& j, const quicktype::MatrixResponse& x);

	inline void from_json(const json& j, quicktype::MatrixResponse& x) {
		x.set_answer(j.at("answer").get<std::vector<float>>());
	}

	inline json to_json(const quicktype::MatrixResponse& x) {
		auto j = json::object();
		j["answer"] = x.get_answer();
		return j;
	}

	void from_json(const json& j, quicktype::MatrixInput& x);
	void to_json(json& j, const quicktype::MatrixInput& x);

	inline void from_json(const json& j, quicktype::MatrixInput& x) {
		x.set_first(j.at("first").get<std::vector<float>>());
		x.set_second(j.at("second").get<std::vector<float>>());
	}

	inline json to_json(const quicktype::MatrixInput& x) {
		auto j = json::object();
		j["first"] = x.get_first();
		j["second"] = x.get_second();
		return j;
	}
}
