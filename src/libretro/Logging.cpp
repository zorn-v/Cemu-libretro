#include "Common/precompiled.h"
#include "Environment.h"

uint64 s_loggingFlagMask = cemuLog_getFlag(LogType::Force);

void cemuLog_writeLineToLog(std::string_view text, bool date, bool new_line)
{
    std::string result;
	if (date)
	{
		const auto now = std::chrono::system_clock::now();
		const auto temp_time = std::chrono::system_clock::to_time_t(now);
		const auto& time = *std::localtime(&temp_time);

		auto time_str = fmt::format("[{:02d}:{:02d}:{:02d}.{:03d}] ", time.tm_hour, time.tm_min, time.tm_sec,
			std::chrono::duration_cast<std::chrono::milliseconds>(now - std::chrono::time_point_cast<std::chrono::seconds>(now)).count());

		result = std::move(time_str);
	}

	result += text;

	if (new_line)
		result += "\n";

    Libretro::Log(result.c_str());
}

bool cemuLog_log(LogType type, std::string_view text)
{
	if (!cemuLog_isLoggingEnabled(type))
		return false;

    cemuLog_writeLineToLog(text);

    return true;
}

void cemuLog_setActiveLoggingFlags(uint64 flagMask)
{
	s_loggingFlagMask = flagMask | cemuLog_getFlag(LogType::Force);
}

bool cemuLog_advancedPPCLoggingEnabled()
{
	return false;
}

void cemuLog_createLogFile(bool triggeredByCrash)
{
}

void cemuLog_waitForFlush()
{
}

fs::path cemuLog_GetLogFilePath()
{
    return "log.txt";
}

std::recursive_mutex log_mutex;

// used to atomically write multiple lines to the log
std::unique_lock<decltype(log_mutex)> cemuLog_acquire()
{
	return std::unique_lock(log_mutex);
}
