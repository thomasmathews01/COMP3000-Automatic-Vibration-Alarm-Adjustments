#pragma once
#include "IConfigurationAccess.h"

class StubConfigurationAccess : public IConfigurationAccess {
public:
    StubConfigurationAccess() = default;
    ~StubConfigurationAccess() = default;

    std::vector<site> get_site_data() override {
        return sites;
    }

    std::vector<machine> get_machines_for_site(const site& site) override {
        return machines;
    }

    std::vector<channel> get_channel_information_for_machine(const machine& machine) override {
        return channels;
    }

    std::vector<channel> get_all_channels() override {
        return channels;
    }

    int get_machine_id_from_channel_id(int channel_id) override {
        return 0;
    }

    std::vector<site> sites;
    std::vector<machine> machines;
    std::vector<channel> channels;
};