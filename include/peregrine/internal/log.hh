#pragma once

#if !defined(SPDLOG_ACTIVE_LEVEL) && (!defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG))
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif // !defined(SPDLOG_ACTIVE_LEVEL) && (!defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG))

#include <spdlog/spdlog.h>

#define PEREGRINE_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define PEREGRINE_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define PEREGRINE_LOG_INFO(...)  SPDLOG_INFO(__VA_ARGS__)
#define PEREGRINE_LOG_WARN(...)  SPDLOG_WARN(__VA_ARGS__)
#define PEREGRINE_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define PEREGRINE_LOG_FAULT(...) SPDLOG_CRITICAL(__VA_ARGS__)
