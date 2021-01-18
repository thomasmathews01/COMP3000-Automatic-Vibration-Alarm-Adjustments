#pragma once

class IServer {
public:
	virtual void startServer() = 0;

	virtual ~IServer() = default;
};

