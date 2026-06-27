#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <SKSE/SKSE.h>

namespace logger = SKSE::log;

// NOTE: This is only the startup default used before the ini is parsed.
// IniParser::Load() (utility.cpp) reads the user's saved "logginglevel" and
// calls spdlog::set_level/flush_on directly, overriding whatever is passed here.
inline void setupLog(const spdlog::level::level_enum& user_level = spdlog::level::info) {

    auto logsFolder = SKSE::log::log_directory();
    
    if (!logsFolder) {
        
        SKSE::stl::report_and_fail("SKSE log directory not provided, logs disabled.");

    }

    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(user_level);
    spdlog::flush_on(user_level);

}