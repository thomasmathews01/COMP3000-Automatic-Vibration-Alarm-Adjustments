#pragma once
#include "IConfigurationAccess.h"
#include "IDatabaseFactory.h"
#include "Helpers/sqlite3pp.h"

class ConfigStorage : public IConfigurationAccess {
public:
    explicit ConfigStorage(const std::shared_ptr<IDatabaseFactory>& factory) : database(factory->get_database()) {}

    [[nodiscard]] std::vector<site> get_site_data() const noexcept final;
    [[nodiscard]] std::vector<machine> get_machines_for_site(const site& site) const noexcept final;
    [[nodiscard]] std::vector<channel> get_channel_information_for_machine(const machine& machine) const noexcept final;
    [[nodiscard]] std::vector<channel> get_all_channels() const noexcept final;
    [[nodiscard]] int get_machine_id_from_channel_id(int channel_id) const noexcept final;

private:
    const std::shared_ptr<sqlite3pp::database> database;
};