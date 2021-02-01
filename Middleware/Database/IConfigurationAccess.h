#pragma once
#include "types.h"

class IConfigurationAccess {
public:
	// Basic configuration access
	virtual std::vector<site> get_site_data() = 0;
	virtual std::vector<machine> get_machines_for_site(const site& site) = 0;
	virtual std::vector<channel> get_channel_information_for_machine(const machine& machine) = 0;
	virtual std::vector<channel> get_all_channels() = 0;
	virtual int get_machine_id_from_channel_id(int channel_id) = 0;
};