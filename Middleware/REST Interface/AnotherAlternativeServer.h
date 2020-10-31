#pragma once

#include "crow.h"

constexpr auto default_port = 1234;
constexpr auto default_host = "http://127.0.0.1";

class AnotherAlternativeServer {
public:
	void runServer(const std::string& address);
};


