#pragma once

#include <string>
#include "libretro.h"

namespace Libretro
{
	bool EnvCb(unsigned cmd, void* data);

	void LogStart();
	bool LogEnabled();
	void LogStop();
	void Log(const char* fmt, ...);

	std::string GetSaveDir();
	std::string GetSystemDir();

	bool DisplayMessage(const char* sg);
} // namespace Libretro
