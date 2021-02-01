#pragma once
#include <string>
#include <vector>
#include "../Types/stateTypes.h"
#include "../Types/alarmTypes.h"

#include "IAlarmStorage.h"
#include "IConfigurationAccess.h"
#include "IStateStorage.h"
#include "IStatistics.h"
#include "IDataAccess.h"

class IDatabase : public IAlarmStorage, public IStateStorage, public IStatistics , public IDataAccess {
public:
	virtual ~IDatabase() = default;
};



