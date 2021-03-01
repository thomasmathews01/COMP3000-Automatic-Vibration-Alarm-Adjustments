// MatrixResponse.hpp

//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     MatrixResponse.hpp data = nlohmann::json::parse(jsonString);

#pragma once

#include <nlohmann/json.hpp>
#include "helper.h"

namespace quicktype {
	using nlohmann::json;

	class MatrixResponse {
	public:
		MatrixResponse() = default;
		virtual ~MatrixResponse() = default;

	private:
		std::vector<float> answer;

	public:
		const std::vector<float> & get_answer() const { return answer; }
		std::vector<float> & get_mutable_answer() { return answer; }
		void set_answer(const std::vector<float> & value) { this->answer = value; }
	};
}