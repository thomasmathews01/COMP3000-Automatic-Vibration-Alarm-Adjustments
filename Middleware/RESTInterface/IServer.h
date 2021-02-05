#pragma once

class IServer {
public:
	virtual void startServer() noexcept = 0;

	virtual ~IServer() = default;
};

