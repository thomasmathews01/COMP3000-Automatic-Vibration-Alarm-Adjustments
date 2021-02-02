#pragma once
#include <string>
#include <vector>
#include <alarmTypes.h>
#include <stateTypes.h>

#include "IAlarmStorage.h"
#include "IConfigurationAccess.h"
#include "IStateStorage.h"
#include "IStatistics.h"
#include "IDataAccess.h"

class IDatabase {
public:
	virtual ~IDatabase() = default;
};



