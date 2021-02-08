#include "CrowUtils.h"

crow::response CrowUtils::add_cors_headers(crow::response&& response) {
	response.add_header("Access-Control-Allow-Origin", "*");
	response.add_header("Access-Control-Allow-Headers", "*");
	response.add_header("Access-Control-Allow-Methods", "*");
	response.add_header("Access-Control-Allow-Credentials", "true");

	return std::move(response);
}
