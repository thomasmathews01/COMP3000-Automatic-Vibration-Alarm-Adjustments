#include "Database/Database.h"
#include "Server.h"

int main(int argc, char** argv) {
	std::shared_ptr<Database> db = std::make_shared<Database>();
	db->set_up_database_connection();

	Server server(db);
	server.startServer();

	return 0;
}