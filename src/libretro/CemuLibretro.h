#pragma once

namespace Libretro
{
	namespace Cemu
	{
		bool GameLoad(const fs::path launchPath);
		void GameUnload();
	} // namespace Cemu
} // namespace Libretro
