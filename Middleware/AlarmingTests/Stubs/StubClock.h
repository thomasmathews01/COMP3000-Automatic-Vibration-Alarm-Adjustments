#include "IClock.h"

class StubClock : public IClock {
public:
	virtual std::chrono::system_clock::time_point get_current_time() const noexcept override {
		return point;
	}

	std::chrono::system_clock::time_point point;
};