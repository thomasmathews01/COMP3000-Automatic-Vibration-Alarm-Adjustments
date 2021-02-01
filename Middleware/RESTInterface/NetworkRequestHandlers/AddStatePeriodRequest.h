#pragma once

#include <memory>
#include <string>
#include "../Utils/include_crow.h"
#include "../../Database/IDatabase.h"

class AddStatePeriodRequest {
public:
	static std::string add_state_period(const crow::request& request, std::shared_ptr<IConfigurationAccess>& config_storage, std::shared_ptr<IStateStorage>& state_storage);
};

//TODO: This is never called???? Why?? Something has gone wrong here and needs fixing as part of the integration
