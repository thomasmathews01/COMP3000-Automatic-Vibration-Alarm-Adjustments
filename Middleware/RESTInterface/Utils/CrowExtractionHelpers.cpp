#include "CrowExtractionHelpers.h"
#include "../../Utils/STLExtensions.h"

std::optional<int> CrowExtractionHelpers::extract_int_from_url_params(const crow::request& request, const std::string& item_name) {
	if (const auto result = request.url_params.get(item_name); result != nullptr)
		return STLExtensions::opt_stoi(result);

	return std::nullopt;
}

std::optional<std::string> CrowExtractionHelpers::extract_string_from_url_params(const crow::request& request, const std::string& item_name) {
	const auto result = request.url_params.get(item_name);

	return (result != nullptr) ? std::make_optional(std::string(result)) : std::nullopt;
}
