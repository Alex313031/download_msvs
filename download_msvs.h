#ifndef DOWNLOAD_MSVS_H_
#define DOWNLOAD_MSVS_H_

#pragma once

#include <filesystem>
#include <iostream>
#include <string>

#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#define PATCH_VERSION 3

constexpr int kMajorVersion = MAJOR_VERSION;
constexpr int kMinorVersion = MINOR_VERSION;
constexpr int kPatchVersion = PATCH_VERSION;

void clearScreen();

const std::string getVersionNumber();

// Names of the files to output
static const char* msvs_2017_name = "msvc_2017.exe";
static const char* msvs_2017_chman_name = "msvc_2017_release.chman";
static const char* msvs_2017_url = "aka.ms/vs/15/release/vs_community.exe";
static const char* msvs_2017_chman_url = "https://aka.ms/vs/15/release/channel";
static const char* msvs_2019_name = "msvc_2019.exe";
static const char* msvs_2019_chman_name = "msvc_2017_release.chman";
static const char* msvs_2019_url = "aka.ms/vs/16/release/vs_community.exe";
static const char* msvs_2019_chman_url = "https://aka.ms/vs/16/release/channel";
static const char* msvs_2022_name = "msvc_2022.exe";
static const char* msvs_2022_chman_name = "msvc_2017_release.chman";
static const char* msvs_2022_url = "aka.ms/vs/17/release/vs_community.exe";
static const char* msvs_2022_chman_url = "https://aka.ms/vs/16/release/channel";

constexpr long ON = 1L;
constexpr long OFF = 0L;

#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif // CURL_STATICLIB

#include "libcurl/curl.h"
#include "libcurl/easy.h"

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);

bool fileExists(const std::filesystem::path filepath);

void downloadFiles(CURL* curl_handle, char* download_url, const char* output_name);

int main(int argc, char* argv[], char *envp[]);

#endif DOWNLOAD_MSVS_H_
