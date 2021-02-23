#pragma once
#include "IConfigurationAccess.h"

class StubConfigurationAccess : public IConfigurationAccess {
public:
    StubConfigurationAccess() = default;
    ~StubConfigurationAccess() = default;

    std::vector<site> get_site_data() const noexcept final {
        return sites;
    }

    std::vector<machine> get_machines_for_site(const site& site) const noexcept final {
        return machines;
    }

    std::vector<channel> get_channel_information_for_machine(const machine& machine) const noexcept final {
        return channels;
    }

    std::vector<channel> get_all_channels() const noexcept final {
        return channels;
    }

    std::vector<data_type> get_all_types() const noexcept final {
        return data_types;
    }

    int get_machine_id_from_channel_id(int channel_id) const noexcept final {
        return 0;
    }

    std::vector<site> sites;
    std::vector<machine> machines;
    std::vector<channel> channels;
    std::vector<data_type> data_types;
};