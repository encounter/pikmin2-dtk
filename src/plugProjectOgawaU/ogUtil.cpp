#include "Game/MoviePlayer.h"
#include "og/newScreen/ogUtil.h"
#include "nans.h"

namespace og {
namespace newScreen {

/**
 * @note Address: 0x80317F28
 * @note Size: 0x24
 */
bool checkMovieActive() { return ((Game::moviePlayer != nullptr) && (Game::moviePlayer->isFlag(Game::MVP_IsActive))); }

/**
 * @note Address: N/A
 * @note Size: 0x4
 */
void drawObjName(Graphics&, char*)
{
	// UNUSED FUNCTION
}

/**
 * TODO: This is currently slightly too small.
 *
 * @note Address: N/A
 * @note Size: 0x84
 * Tells the game which resource folder path to use, based on system language.
 */
void getLanguageDir(char*& path)
{
	// UNUSED FUNCTION
	if (LOCALIZED) {
		switch (sys->mRegion) {
		case System::LANG_English:
			path = "eng/";
			break;
		case System::LANG_French:
			path = "fra/";
			break;
		case System::LANG_German:
			path = "ger/";
			break;
		case System::LANG_Italian:
			path = "ita/";
			break;
		case System::LANG_Japanese:
			path = "jpn/";
			break;
		case System::LANG_Spanish:
			path = "spa/";
			break;
		case System::LANG_Unused:
		default:
			path = "";
			break;
		}
	} else {
		path = "";
	}
}

/**
 * @note Address: 0x80317F4C
 * @note Size: 0xDC
 */
void makeLanguageResName(char* languageResName, char const* path)
{
	char* langDir = nullptr;
	char langDirBuffer[16];

	if (*path == '/') {
		// TODO: Perhaps this is inlined from some path normalization func?
		// It's currently putting %s before the subdirs in rodata, when it
		// should be afterwards.

		// nah, it works now. -Epoch
		sprintf(languageResName, "%s", path);
		return;
	}

	getLanguageDir(langDir);
	sprintf(langDirBuffer, "%s", langDir);
	sprintf(languageResName, "/new_screen/%s%s", langDirBuffer, path);
}
} // namespace newScreen
} // namespace og
