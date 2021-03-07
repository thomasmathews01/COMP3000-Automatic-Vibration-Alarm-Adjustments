#pragma once
#include <mutex>
#include <map>
#include "IDatabase.h"
#include "Helpers/sqlite3pp.h"
#include "IDatabaseFactory.h"
#include "IStatistics.h"

class Database : public IDatabase {
public:
	explicit Database(std::shared_ptr<IDatabaseFactory> factory);
	~Database() override = default;

private:
	std::shared_ptr<IDatabaseFactory> factory;
	std::mutex database_access_mutex;
	/*
	 * Documentation seems to suggest that mutex locking this is not needed, and it has some in built locks anyway
	 * So possibly this could be removed, it is definitely the case that we can have concurrent reads which makes much
	 * more sense, the only time consuming actions are getting data out, not adding data, which is always added slowly
	 * and in relatively tiny chunks. Look into this when a performance bottleneck becomes problematic.
	*/

	std::shared_ptr<tao::pq::connection> database; // This is a shared pointer for testing purposes, but should never be accessed by anyone other than this class, with the mutex, unless they take ownership whilst they use it, and we can't access it in that period.
};


