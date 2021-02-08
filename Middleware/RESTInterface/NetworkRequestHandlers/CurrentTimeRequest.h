#pragma once
#include <string>
#include "../Utils/include_crow.h"
#include <IClock.h>

class CurrentTimeRequest {
public:
	static crow::response get_current_time_response(const std::shared_ptr<IClock>& clock);
};


