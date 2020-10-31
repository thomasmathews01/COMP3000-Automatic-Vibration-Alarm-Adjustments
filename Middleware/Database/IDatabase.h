#pragma once


#include "../REST Interface/Model/types.h"

class IDatabase {
public:
	virtual std::vector<site> get_site_data() = 0;
	virtual void populate_sites_machine_information(site& site) = 0;
	virtual void populate_channel_information_for_a_machine(machine& machine) = 0;
};


