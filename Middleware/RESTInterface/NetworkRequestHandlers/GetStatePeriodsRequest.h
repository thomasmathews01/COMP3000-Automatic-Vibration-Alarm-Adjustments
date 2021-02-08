#pragma once

#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include <IConfigurationAccess.h>
#include <IStateStorage.h>

class GetStatePeriodsRequest {
public:
	static crow::response get_state_periods(const crow::request& request, const std::shared_ptr<IConfigurationAccess>& config_storage, const std::shared_ptr<IStateStorage>& state_storage);
};


