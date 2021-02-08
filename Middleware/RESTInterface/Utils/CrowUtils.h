#pragma once
#include "include_crow.h"

class CrowUtils {
public:
	static crow::response add_cors_headers(crow::response&& response);
};


