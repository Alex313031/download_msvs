#ifndef GLOBALS_H_
#define GLOBALS_H_

#pragma once

#include <string>

#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define PATCH_VERSION 5

#if defined DEBUG || defined _DEBUG && !defined(NDEBUG)
 bool is_debug = true;
#else
 bool is_debug = false;
#endif // defined DEBUG || defined DEBUG

constexpr int kMajorVersion = MAJOR_VERSION;
constexpr int kMinorVersion = MINOR_VERSION;
constexpr int kPatchVersion = PATCH_VERSION;

// MSVC 14
static const char* k2015_name = "msvc_2015.3.iso";
static const char* k2015_url = "https://archive.org/download/vs2015.3.com_enu_202102/vs2015.3.com_enu.iso";
static const char* k2015_chman_name = "msvc_2015_release.chman";
static const char* k2015_chman_url = "https://learn.microsoft.com/en-us/visualstudio/releasenotes/vs2015-update3-vs";
static const std::wstring k2015RelNotes =
    L"See MSVS 2015 release notes at https://learn.microsoft.com/en-us/visualstudio/releasenotes/vs2015-update3-vs";

// MSVC 15
static const char* k2017_name = "msvc_2017.exe";
static const char* k2017_url = "https://aka.ms/vs/15/release/vs_community.exe";
static const char* k2017_chman_name = "msvc_2017_release.chman";
static const char* k2017_chman_url = "https://aka.ms/vs/15/release/channel";

// MSVC 16
static const char* k2019_name = "msvc_2019.exe";
static const char* k2019_url = "https://aka.ms/vs/16/release/vs_community.exe";
static const char* k2019_chman_name = "msvc_2019_release.chman";
static const char* k2019_chman_url = "https://aka.ms/vs/16/release/channel";

// MSVC 17
static const char* k2022_name = "msvc_2022.exe";
static const char* k2022_url = "https://aka.ms/vs/17/release/vs_community.exe";
static const char* k2022_chman_name = "msvc_2022_release.chman";
static const char* k2022_chman_url = "https://aka.ms/vs/16/release/channel";

inline constexpr long ON = 1L;
inline constexpr long OFF = 0L;

constexpr enum RETURN_CODE {
  OKAY = 0,
  WARN = 1,
  ERROR = 2
};

constexpr int STATUS_OKAY = RETURN_CODE::OKAY;
constexpr int STATUS_WARN = RETURN_CODE::WARN;
constexpr int STATUS_ERROR = RETURN_CODE::ERROR;

namespace STATUS_CODE {
  constexpr errno_t OKAY = STATUS_OKAY;
  constexpr errno_t WARN = STATUS_WARN;
  constexpr errno_t FAIL = STATUS_ERROR;
}

#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif // CURL_STATICLIB

#include "libcurl/curl.h"
#include "libcurl/easy.h"

#endif // GLOBALS_H_
