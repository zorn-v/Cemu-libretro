#pragma once

namespace Libretro::Cemu
{
	bool GameLoad(const fs::path launchPath);
	void LaunchTitle();
	void GameUnload();
} // namespace Libretro::Cemu
