#include "Common/precompiled.h"

#include "CemuLibretro.h"
#include "Environment.h"
#include "Cafe/CafeSystem.h"
#include "Cafe/TitleList/TitleList.h"
// #include "Cafe/HW/Latte/Core/LatteOverlay.h"
#include "Cafe/HW/Latte/Renderer/OpenGL/OpenGLRenderer.h"
#include "util/crypto/aes128.h"

void LoadOpenGLImports()
{
	dbgLog("glDrawBuffer = %p", Libretro::hw_render.get_proc_address("glDrawBuffer"));
	PFNGLGETSTRINGPROC l_glGetString = (PFNGLGETSTRINGPROC)Libretro::hw_render.get_proc_address("glGetString");
	dbgLog("glGetString(GL_VENDOR) = %s", l_glGetString(GL_VENDOR));
	glGetString = (PFNGLGETSTRINGPROC)Libretro::hw_render.get_proc_address("glGetString"); // <= segfault
	dbgLog("NO SEGFAULT happens !!!");
#define GLFUNC(__type, __name)	__name = (__type)Libretro::hw_render.get_proc_address(#__name);
#define EGLFUNC(__type, __name) __name = (__type)Libretro::hw_render.get_proc_address(#__name);
#include "Common/GLInclude/glFunctions.h"
#undef GLFUNC
#undef EGLFUNC
	dbgLog("LoadOpenGLImports END");
}

namespace Libretro::Cemu
{
	class OGLRenderer : public OpenGLRenderer {};

	// check MainWindow::FileLoad
	bool GameLoad(const fs::path launchPath)
	{
		// crypto init
		AES128_init();
		// init PPC timer
		// call this as early as possible because it measures frequency of RDTSC using an asynchronous thread over 3 seconds
		PPCTimer_init();

		// init Cafe system
		CafeSystem::Initialize();

		TitleInfo launchTitle{launchPath};
		if (launchTitle.IsValid())
		{
			// the title might not be in the TitleList, so we add it as a temporary entry
			CafeTitleList::AddTitleFromPath(launchPath);
			// title is valid, launch from TitleId
			TitleId baseTitleId;
			if (!CafeTitleList::FindBaseTitleId(launchTitle.GetAppTitleId(), baseTitleId))
			{
				Libretro::DisplayMessage("Unable to launch game because the base files were not found.");
				return false;
			}
			CafeSystem::STATUS_CODE r = CafeSystem::PrepareForegroundTitle(baseTitleId);
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
		else // if (launchTitle.GetFormat() == TitleInfo::TitleDataFormat::INVALID_STRUCTURE )
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
				if (launchTitle.GetInvalidReason() == TitleInfo::InvalidReason::NO_DISC_KEY)
				{
					Libretro::DisplayMessage("Could not decrypt title. Make sure that keys.txt contains the correct disc key for this title.");
				}
				if (launchTitle.GetInvalidReason() == TitleInfo::InvalidReason::NO_TITLE_TIK)
				{
					Libretro::DisplayMessage("Could not decrypt title because title.tik is missing.");
				}
				return false;
			}
		}

		return true;
	}

	void LaunchTitle()
	{
		//dbgLog("glDrawBuffer = %p", &glDrawBuffer);
		/*FILE *fp = fopen("/proc/self/maps", "r");
		int BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];
		if (fp != NULL)
		{
			while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
				fprintf(stderr, "%s", buffer);
			pclose(fp);
		}*/
		//glDrawBuffer = nullptr; // <= segfault
		//dbgLog("glDrawBuffer = %p", glDrawBuffer); 
		// LatteOverlay_init();
		g_renderer = std::make_unique<OGLRenderer>();
		CafeSystem::LaunchForegroundTitle();
	}

	void GameUnload()
	{
		CafeSystem::Shutdown();
	}
} // namespace Libretro::Cemu
