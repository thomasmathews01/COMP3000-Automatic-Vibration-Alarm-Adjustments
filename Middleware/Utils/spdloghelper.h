#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace spdlog_helpers {
	auto get_logger(const std::string& name) {
		return spdlog::get(name) == nullptr ? spdlog::stdout_color_mt(name) : spdlog::get(name);
	}	
}
