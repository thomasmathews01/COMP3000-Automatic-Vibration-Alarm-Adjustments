#include "Database.h"
#include "DatabaseInitialiser.h"
#include "database_statements.h"
#include <range/v3/all.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace ranges;


Database::Database(std::shared_ptr<IDatabaseFactory> db_factory) : factory(std::move(db_factory)) {
    database = DatabaseInitialiser::intialise_database(factory->get_database());
}