#include <iostream>
#include "Server.h"

int main() {
	Server server;
	server.start();

	char x;
	std::cin >> x;

	server.stop();
}