#pragma once

#include <string>

class CurrentTimeRequest {
public:
	static std::string get_current_time_response(const std::shared_ptr<IClock>& clock);
};


