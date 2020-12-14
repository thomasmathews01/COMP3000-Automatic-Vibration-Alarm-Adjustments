#pragma once
#include <optional>
#include <string>

#include <crow/crow_all.h>

class CrowExtractionHelpers {
public:
	static std::optional<int> extract_int_from_url_params(const crow::request& request, const std::string& item_name);
	static std::optional<std::string> extract_string_from_url_params(const crow::request& request, const std::string& item_name);
};


