#include "GetStatePeriodsRequest.h"

std::string GetStatePeriodsRequest::get_state_periods(const crow::request& request, const std::shared_ptr<IDatabase>& database) {

	// We allow requests by channel, because that i tlikely what is being displayed, but in reality they are specified at machine level, so we translate.
	const auto channel_id = std::stoi(request.url_params.get("channel"));
	const auto machine_id = database->get_machine_id_from_channel_id(channel_id);

}