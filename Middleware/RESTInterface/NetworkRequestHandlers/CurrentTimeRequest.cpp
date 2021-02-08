#include <IClock.h>
#include <memory>
#include "CurrentTimeRequest.h"
#include "../Utils/CrowUtils.h"

crow::response CurrentTimeRequest::get_current_time_response(const std::shared_ptr<IClock>& clock) {
	const auto current_time = clock->get_current_time();
	const auto current_time_string = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(current_time.time_since_epoch()).count());

	return CrowUtils::add_cors_headers(crow::response(current_time_string));
}
