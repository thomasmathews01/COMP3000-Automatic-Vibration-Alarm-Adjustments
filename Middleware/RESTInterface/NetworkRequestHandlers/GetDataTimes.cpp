#include "GetDataTimes.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include "../Utils/CrowUtils.h"
#include <chrono>

using namespace rapidjson;

crow::response get_time_JSON_response(const time_point_t& time) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("secondsSinceEpoch");
	writer.Int(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count());
	writer.EndObject();

	return CrowUtils::add_cors_headers(crow::response(buff.GetString()));
}

crow::response GetDataTimes::get_earliest_point(const crow::request& request, const std::shared_ptr<IDataAccess>& database) {
	const auto machine_id = std::stoi(request.url_params.get("machine"));
	const auto earliest_point = database->get_earliest_data_point_for_machine(machine_id);
	return get_time_JSON_response(earliest_point);
}

crow::response GetDataTimes::get_latest_point(const crow::request& request, const std::shared_ptr<IDataAccess>& database) {
	const auto machine_id = std::stoi(request.url_params.get("machine"));
	const auto latest_point = database->get_latest_data_point_for_machine(machine_id);
	return get_time_JSON_response(latest_point);
}
