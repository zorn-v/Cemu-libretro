#pragma once

#include "libretro.h"

namespace Libretro {

bool EnvCb(unsigned cmd, void *data);

void LogStart();
bool LogEnabled();
void LogStop();
void Log(const char *fmt, ...);

} // namespace Libretro
