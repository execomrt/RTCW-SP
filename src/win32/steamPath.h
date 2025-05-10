#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	/// Returns true on success, false on failure.
	/// The output buffer should be at least MAX_PATH characters.
	int SteamDir_GetInstallPath(int appId, char* outPath, int maxLen);

#ifdef __cplusplus
}
#endif
