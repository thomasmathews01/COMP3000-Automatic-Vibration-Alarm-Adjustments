#pragma once
#include "types.h"

class IConfigurationAccess {
public:
	// Basic configuration access
	[[nodiscard]] virtual std::vector<site> get_site_data() const noexcept = 0;
	[[nodiscard]] virtual std::vector<machine> get_machines_for_site(const site& site) const noexcept = 0;
	[[nodiscard]] virtual std::vector<channel> get_channel_information_for_machine(const machine& machine) const noexcept = 0;
	[[nodiscard]] virtual std::vector<channel> get_all_channels() const noexcept = 0;
	[[nodiscard]] virtual std::vector<data_type> get_all_types() const noexcept = 0;
	[[nodiscard]] virtual int get_machine_id_from_channel_id(int channel_id) const noexcept = 0;
};