#pragma once
#include <gmock/gmock.h>

class MockClock : public IClock {
public:
	MOCK_METHOD(time_point_t, get_current_time, (), (const, noexcept));
};