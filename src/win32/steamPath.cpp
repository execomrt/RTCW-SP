
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <shlwapi.h>
#include "steamPath.h"
#pragma comment(lib, "Shlwapi.lib")

static std::string ReadRegistrySteamPath() {
    HKEY hKey;
    char path[MAX_PATH];
    DWORD pathLen = sizeof(path);
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExA(hKey, "SteamPath", NULL, NULL, (LPBYTE)path, &pathLen) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return std::string(path);
        }
        RegCloseKey(hKey);
    }
    return "";
}

static std::vector<std::string> GetSteamLibraryFolders(const std::string& steamPath) {
    std::vector<std::string> libraries;
    std::string libraryFile = steamPath + "\\steamapps\\libraryfolders.vdf";

    std::ifstream file(libraryFile);
    if (!file.is_open()) {
        return libraries;
    }

    libraries.push_back(steamPath); // Always include default path

    std::string line;
    while (std::getline(file, line)) {
        size_t pathPos = line.find("\"path\"");
        if (pathPos != std::string::npos) {
            size_t quote1 = line.find("\"", pathPos + 6);
            size_t quote2 = line.find("\"", quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos) {
                std::string path = line.substr(quote1 + 1, quote2 - quote1 - 1);
                std::replace(path.begin(), path.end(), '/', '\\');
                libraries.push_back(path);
            }
        }
    }

    return libraries;
}

static std::string ParseInstallDirFromACF(const std::string& acfPath) {
    std::ifstream file(acfPath);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("\"installdir\"") != std::string::npos) {
            size_t firstQuote = line.find("\"", line.find("\"installdir\"") + 12);
            size_t secondQuote = line.find("\"", firstQuote + 1);
            if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
                return line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
            }
        }
    }
    return "";
}

int SteamDir_GetInstallPath(int appId, char* outPath, int maxLen) {
    std::string steamPath = ReadRegistrySteamPath();
    if (steamPath.empty())
        return false;

    std::vector<std::string> libraryPaths = GetSteamLibraryFolders(steamPath);

    for (const std::string& libPath : libraryPaths) {
        std::string steamAppsPath = libPath + "\\steamapps";

        char appManifest[MAX_PATH];
        snprintf(appManifest, MAX_PATH, "%s\\appmanifest_%d.acf", steamAppsPath.c_str(), appId);

        if (!PathFileExistsA(appManifest))
            continue;

        std::string installDir = ParseInstallDirFromACF(appManifest);
        if (installDir.empty())
            continue;

        std::string fullPath = steamAppsPath + "\\common\\" + installDir;

        if (fullPath.length() >= (size_t)maxLen)
            return false;

        strcpy_s(outPath, maxLen, fullPath.c_str());
        return true;
    }

    return false; // appmanifest not found in any library
}
