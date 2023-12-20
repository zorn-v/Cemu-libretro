#include <cstdarg>
#include <cstdio>
#include "Environment.h"

namespace Libretro
{
	namespace
	{
		static retro_environment_t environ_cb;
		static retro_log_printf_t log_cb = nullptr;
	} // namespace

	bool EnvCb(unsigned cmd, void* data)
	{
		return environ_cb(cmd, data);
	}

	void LogStart()
	{
		static struct retro_log_callback logging;
		if (EnvCb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
		{
			log_cb = logging.log;
		}
	}

	bool LogEnabled()
	{
		return log_cb != nullptr;
	}

	void LogStop()
	{
		log_cb = nullptr;
	}

	void Log(const char* fmt, ...)
	{
		if (!LogEnabled())
		{
			return;
		}
		va_list va;
		va_start(va, fmt);
		log_cb(RETRO_LOG_INFO, fmt, va);
		va_end(va);
	}

	std::string GetEnvStr(unsigned cmd)
	{
		char* var = nullptr;
		if (!environ_cb(cmd, &var) || var == nullptr)
		{
			// Fetching variable failed.
			Log("Fetching %u libretro env var failed\n", cmd);
			return std::string();
		}
		return std::string(var);
	}

	std::string GetSaveDir()
	{
		return GetEnvStr(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY);
	}

	std::string GetSystemDir()
	{
		return GetEnvStr(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY);
	}

	bool DisplayMessage(const char* sg)
	{
		retro_message msg;
		msg.msg = sg;
		msg.frames = 60 * 10;
		return environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);
	}

} // namespace Libretro

void retro_set_environment(retro_environment_t cb)
{
	Libretro::environ_cb = cb;

	static const struct retro_controller_description controllers[] = {
		{"Wii U GamePad", RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)},
	};

	static const struct retro_controller_info ports[] = {
		{controllers, 1},
		{NULL, 0},
	};

	cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}
