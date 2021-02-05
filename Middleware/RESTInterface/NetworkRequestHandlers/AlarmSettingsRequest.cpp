#include "AlarmSettingsRequest.h"
#include "../Utils/CrowExtractionHelpers.h"
#include "../../Utils/STLExtensions.h"
#include <rapidjson/document.h>

#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

using rapidjson::StringBuffer;
using rapidjson::PrettyWriter;
using namespace rapidjson;


std::string serialise_alarm_settings_as_json(const std::vector<alarm_settings_t>& settings) {
	StringBuffer buff;
	PrettyWriter<StringBuffer> writer(buff);
	writer.StartObject();

	writer.Key("alarm_settings");
	writer.StartArray();

	for (const auto& setting : settings) {
		writer.StartObject();
		writer.Key("channel_id");
		writer.Int(setting.channel_id);
		writer.Key("type_id");
		writer.Int(setting.type_id);
		writer.Key("severity");
		writer.String(toString(setting.severity).c_str());
		writer.Key("threshold_type");
		writer.String(toString(setting.threshold).c_str());
		writer.Key("custom_level");
		writer.Double(STLExtensions::value_or_default(setting.customLevel, -1.f * std::numeric_limits<float>::max()));

		writer.EndObject();
	}

	writer.EndArray();
	writer.EndObject();

	return buff.GetString();}

std::string get_alarm_settings(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IAlarmStorage>& database) {
	const auto machine_id = CrowExtractionHelpers::extract_int_from_url_params(request, "machine_id");
	if (!machine_id)
		return "Error";

	const auto all_settings_for_machine = database->get_alarm_settings_for_machine(*machine_id);

	return serialise_alarm_settings_as_json(all_settings_for_machine);
}

std::string set_alarm_settings(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IAlarmStorage>& database) {
	Document document;
	document.Parse(request.body.c_str());

	if (document.HasMember("alarm_setting") && document["alarm_setting"].IsObject()) {
		const auto& new_setting = document["alarm_setting"].GetObject();
		const auto channel_id = new_setting["channel_id"].GetInt();
		const auto type_id = new_setting["type_id"].GetInt();
		const auto threshold_type = new_setting["threshold_type"].GetString();
		const auto severity_type = new_setting["severity"].GetString();

		if (parse_threshold(threshold_type) == alarmThreshold::Custom) {
			const auto custom_level = new_setting["custom_level"].GetFloat(); // TODO: Error case.
			database->update_alarm_setting(alarm_settings_t(channel_id, type_id, parse_severity(severity_type), parse_threshold(threshold_type), custom_level));
		}
		else {
			database->update_alarm_setting(alarm_settings_t(channel_id, type_id, parse_severity(severity_type), parse_threshold(threshold_type)));
		}

		return "success"; // TODO: Convert this to a return code when we get the better error messages starting to come through anyway.
	}

	return "Error"; // TODO: Better.
}

std::string AlarmSettingsRequest::alarm_settings(const crow::request& request, alarmSeverity severity, const std::shared_ptr<IAlarmStorage>& database) {
	switch (request.method) {
		case crow::HTTPMethod::Get: return get_alarm_settings(request, severity, database);
		case crow::HTTPMethod::Post: return set_alarm_settings(request, severity, database);
		default: return "Error"; // TODO: Better.
	}
}
