#ifndef DOWNLOAD_MSVS_H_
#define DOWNLOAD_MSVS_H_

#pragma once

#include <filesystem>
#include <iostream>
#include <vector>

#include "globals.h"

int bad_handle_int;

bool custom_url = false;
bool nuke_files = false;
bool get_2015 = false;
bool get_2017 = false;
bool get_2019 = false;
bool get_2022 = false;

void clearScreen();

const std::string getVersionNumber();
const std::wstring getVersionNumberW();

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);

std::wstring stringToWString(std::string in_str);

std::filesystem::path getPathFromName(const char* output_name);

bool fileExists(const std::filesystem::path filepath);

LPCWSTR charToLPCWSTR(const char* input_char);

bool deleteFile(const char* nuke_name);

bool nukeFiles();

void downloadFiles(CURL* curl_handle, char* download_url, const char* output_name);

void showHelp(char* exe);

bool isDebug();

void setIsDebug(bool is_debug_mode);

int endCode(int status_code);

int main(int argc, char* argv[], char *envp[]);

#endif // DOWNLOAD_MSVS_H_
