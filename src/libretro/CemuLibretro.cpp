#include "Common/precompiled.h"
#include "Environment.h"
#include "Cafe/CafeSystem.h"
#include "Cafe/TitleList/TitleList.h"


namespace Libretro {
namespace Cemu {

// check MainWindow::FileLoad
bool GameLoad(const fs::path launchPath)
{
    fprintf(stderr, "GameLoad START\n");
    TitleInfo launchTitle{ launchPath };
    fprintf(stderr, "launchTitle.IsValid ?\n");
    Libretro::Log("GameLoad -1\n");
	if (launchTitle.IsValid())
	{
        fprintf(stderr, "launchTitle.IsValid\n");
        Libretro::Log("GameLoad 0\n");
		// the title might not be in the TitleList, so we add it as a temporary entry
		CafeTitleList::AddTitleFromPath(launchPath);
		// title is valid, launch from TitleId
		TitleId baseTitleId;
        Libretro::Log("GameLoad 1\n");
		if (!CafeTitleList::FindBaseTitleId(launchTitle.GetAppTitleId(), baseTitleId))
		{
			Libretro::DisplayMessage("Unable to launch game because the base files were not found.");
			return false;
		}
        Libretro::Log("GameLoad 2\n");
		CafeSystem::STATUS_CODE r = CafeSystem::PrepareForegroundTitle(baseTitleId);
        Libretro::Log("GameLoad 3\n");
		if (r == CafeSystem::STATUS_CODE::INVALID_RPX)
		{
			cemu_assert_debug(false);
			return false;
		}
		else if (r == CafeSystem::STATUS_CODE::UNABLE_TO_MOUNT)
		{
			Libretro::DisplayMessage("Unable to mount title.\nMake sure the configured game paths are still valid and refresh the game list.\n\nFile which failed to load:\n");
			return false;
		}
		else if (r != CafeSystem::STATUS_CODE::SUCCESS)
		{
			Libretro::DisplayMessage("Failed to launch game.");
			return false;
		}
	}
	else //if (launchTitle.GetFormat() == TitleInfo::TitleDataFormat::INVALID_STRUCTURE )
	{
		// title is invalid, if it's an RPX/ELF we can launch it directly
		// otherwise it's an error
		CafeTitleFileType fileType = DetermineCafeSystemFileType(launchPath);
		if (fileType == CafeTitleFileType::RPX || fileType == CafeTitleFileType::ELF)
		{
			CafeSystem::STATUS_CODE r = CafeSystem::PrepareForegroundTitleFromStandaloneRPX(launchPath);
			if (r != CafeSystem::STATUS_CODE::SUCCESS)
			{
				cemu_assert_debug(false); // todo
				Libretro::DisplayMessage("Failed to launch executable.");
				return false;
			}
		}
		else
		{
			if(launchTitle.GetInvalidReason() == TitleInfo::InvalidReason::NO_DISC_KEY)
			{
				Libretro::DisplayMessage("Could not decrypt title. Make sure that keys.txt contains the correct disc key for this title.");
			}
			if(launchTitle.GetInvalidReason() == TitleInfo::InvalidReason::NO_TITLE_TIK)
			{
				Libretro::DisplayMessage("Could not decrypt title because title.tik is missing.");
			}
			return false;
		}
	}
    return true;
}

} // namespace Cemu
} // namespace Libretro
