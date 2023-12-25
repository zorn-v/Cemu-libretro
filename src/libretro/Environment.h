#pragma once

#include <string>
#include "libretro.h"

namespace Libretro
{
	extern struct retro_hw_render_callback hw_render;

	bool EnvCb(unsigned cmd, void* data);

	void LogStart();
	bool LogEnabled();
	void LogStop();
	void Log(const char* fmt, ...);

	std::string GetSaveDir();
	std::string GetSystemDir();

	bool DisplayMessage(const char* sg);
} // namespace Libretro

#define dbgLog(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
