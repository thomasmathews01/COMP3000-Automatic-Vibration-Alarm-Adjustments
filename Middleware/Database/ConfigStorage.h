#pragma once
#include "IConfigurationAccess.h"
#include "IDatabaseFactory.h"
#include "Helpers/sqlite3pp.h"

class ConfigStorage : public IConfigurationAccess {
public:
    explicit ConfigStorage(const std::shared_ptr<IDatabaseFactory>& factory) : database(factory->get_database()) {}

    std::vector<site> get_site_data() final;
    std::vector<machine> get_machines_for_site(const site& site) final;
    std::vector<channel> get_channel_information_for_machine(const machine& machine) final;
    std::vector<channel> get_all_channels() final;
    int get_machine_id_from_channel_id(int channel_id) final;

private:
    std::shared_ptr<sqlite3pp::database> database;
};