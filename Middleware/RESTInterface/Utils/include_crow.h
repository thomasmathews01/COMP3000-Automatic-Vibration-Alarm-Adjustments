#pragma once

#ifdef WIN32
#pragma warning(push)

#pragma warning(disable:4820)
#pragma warning(disable:4619)
#pragma warning(disable:4668)
#pragma warning(disable:4625)
#pragma warning(disable:4267)
#pragma warning(disable:4626)
#pragma warning(disable:4571)
#pragma warning(disable:4347)
#pragma warning(disable:4640)
#pragma warning(disable:4365)
#pragma warning(disable:4710)
#pragma warning(disable:4820)
#pragma warning(disable:4350)
#pragma warning(disable:4686)
#pragma warning(disable:4711)
#pragma warning(disable:4548)

#include "crow.h"

#pragma warning(pop)
#else

#include "crow.h"

#endif
