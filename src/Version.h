﻿#pragma once
#ifndef VERSION_H
#define VERSION_H

#define WSTR(x) WSTR_(x)
#define WSTR_(x) L ## #x
#define STR(x) STR_(x)
#define STR_(x) #x

#pragma region Release build version numbering

// Indicates project maturity and completeness
#define VERSION_MAJOR 0

// Indicates major changes and significant additions, like new logics
#define VERSION_MINOR 1

// Indicates minor changes, like vanilla bugfixes, unhardcodings or hacks
#define VERSION_REVISION 0

// Indicates Kratos-related bugfixes only
#define VERSION_PATCH 0

#pragma endregion

// Build number. Incremented on each released build.
#define BUILD_NUMBER 1

// version infomation
#define PRODUCT_NAME "Kratos PP"
#define COMPANY_NAME "ChrisLv_CN"
#define LEGAL_COPYRIGHT "© The ChrisLv_CN 🐼 Contributors 2023"
#define FILE_DESCRIPTION "Kratos, Ares-like YR engine extension"
#define FILE_VERSION_STR STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_REVISION) "." STR(VERSION_PATCH)
#define FILE_VERSION VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_PATCH
#ifdef IS_RELEASE_VER // Release build metadata
	#define SAVEGAME_ID ((VERSION_MAJOR << 24) | (VERSION_MINOR << 16) | (VERSION_REVISION << 8) | VERSION_PATCH)
	#define PRODUCT_VERSION "Release Build " STR(BUILD_NUMBER)
#else // Regular debug metadata
	#define SAVEGAME_ID ((BUILD_NUMBER << 24) | (BUILD_NUMBER << 12) | (BUILD_NUMBER))
	#define PRODUCT_VERSION "Debug Build " STR(BUILD_NUMBER)
#endif
#define INTERNAL_NAME "KratosPP.dll"
#define ORIGINAL_FILENAME "KratosPP.dll"

#define VERSION_SHORT_STR STR(VERSION_MAJOR) "." STR(VERSION_MINOR)
#define VERSION_SHORT_WSTR WSTR(VERSION_MAJOR) L"." WSTR(VERSION_MINOR)

#endif // VERSION_H
