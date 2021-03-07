#pragma once
#include "IDatabaseFactory.h"
#include "DatabaseInitialiser.h"

class DatabaseFactory : public IDatabaseFactory {
public:
	explicit DatabaseFactory(std::shared_ptr<tao::pq::connection>&& default_database) : database(DatabaseInitialiser::intialise_database(std::move(default_database))) {}
	DatabaseFactory() = default;
	std::shared_ptr<tao::pq::connection> get_database(std::string_view location) final;

private:
	std::shared_ptr<tao::pq::connection> database;
};


