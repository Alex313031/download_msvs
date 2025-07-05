/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
/*
 Description:
 Downloads a given URL into a local file.
*/

#include <codecvt>
#include <stdio.h>
#include <stdlib.h>

#include "download_msvs.h"

void clearScreen() {
  COORD topLeft  = { 0, 0 };
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO screen;
  DWORD written;

  GetConsoleScreenBufferInfo(console, &screen);
  FillConsoleOutputCharacterA(
      console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
  );
  FillConsoleOutputAttribute(
    console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
    screen.dwSize.X * screen.dwSize.Y, topLeft, &written
  );
  SetConsoleCursorPosition(console, topLeft);
  std::cout << std::endl;
}

const std::string getVersionNumber() {
  static std::string kDecimalPoint = ".";
  std::string kVersionNumber = std::to_string(kMajorVersion) + kDecimalPoint
      + std::to_string(kMinorVersion) + kDecimalPoint + std::to_string(kPatchVersion);
  return kVersionNumber;
}

const std::wstring getVersionNumberW() {
  const std::string version_str = getVersionNumber();
  const std::wstring version_wstr =
      stringToWString(version_str);
  return version_wstr;
}

std::wstring stringToWString(std::string in_str) {
  if (in_str.empty()) {
    return std::wstring();
  }

  size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0, 
      in_str.data(), (int)in_str.size(), NULL, 0);
  if (charsNeeded == 0) {
    std::cerr << "Failed converting UTF-8 string to UTF-16" << std::endl;
  }

  std::vector<wchar_t> buffer(charsNeeded);
  int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0, 
      in_str.data(), (int)in_str.size(), &buffer[0], buffer.size());
  if (charsConverted == 0) {
    std::cerr << "Failed converting UTF-8 string to UTF-16" << std::endl;
  }

  const std::wstring return_wstring =
      std::wstring(&buffer[0], charsConverted);

  return return_wstring;
}

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
  return written;
}

std::filesystem::path getPathFromName(const char* output_name) {
  std::string filename(output_name);
  const std::filesystem::path current_dir = std::filesystem::current_path();
  const std::filesystem::path file_path = current_dir / filename;
  return file_path;
}

bool fileExists(const std::filesystem::path filepath) {
  const std::string file_path_str = filepath.string();
  bool file_already_present = std::filesystem::exists(filepath);

  if (isDebug()) {
    if (file_already_present) {
      std::cerr << __func__ << ": "
                << file_path_str << " exists." << std::endl;
    } else {
      std::cerr << __func__ << ": "
                << file_path_str << " doesn't exist." << std::endl;
    }
  }

  return file_already_present;
}

LPCWSTR charToLPCWSTR(const char* input_char) {
  if (!input_char) {
    return nullptr;
  }

  // Calculate the required buffer size for the wide string
  long kBufferSize = MultiByteToWideChar(CP_ACP, 0, input_char, -1, nullptr, 0);
  if (kBufferSize == 0) {
    return nullptr; // Error in conversion
  }

  // Allocate memory for the wide string
  wchar_t* wide_char_buffer = new wchar_t[kBufferSize];

  // Perform the conversion
  MultiByteToWideChar(CP_ACP, 0, input_char, -1, wide_char_buffer, kBufferSize);

  // Note: The caller is responsible for deleting wideStringBuffer
  return wide_char_buffer;
}

bool deleteFile(const char* nuke_name) {
  const std::filesystem::path current_dir = std::filesystem::current_path();
  const std::filesystem::path file_path = getPathFromName(nuke_name);
  const std::string file_path_str = file_path.string();
  bool success = false;

  if (!fileExists(file_path)) {
    std::cerr << __func__ << ": "
              << file_path_str << " doesn't exist!" << std::endl;
    success = false;
  } else {
    std::cout << "NOTE: Deleting " << file_path_str << std::endl;
  }

  const char* file_path_char = file_path_str.c_str();
  LPCWSTR filename = charToLPCWSTR(file_path_char);
  bool deletion_success = DeleteFile(filename);

  if (deletion_success) {
    std::cout << "Deleted " << file_path_str << std::endl;
  } else {
    std::cerr << "Failed to delete " << file_path_str << std::endl;
  }
  success = deletion_success;

  return success;
}

bool nukeFiles() {
  const bool d1 = deleteFile(k2015_name);
  const bool d2 = deleteFile(k2015_chman_name);
  const bool d3 = deleteFile(k2017_name);
  const bool d4 = deleteFile(k2017_chman_name);
  const bool d5 = deleteFile(k2019_name);
  const bool d6 = deleteFile(k2019_chman_name);
  const bool d7 = deleteFile(k2022_name);
  const bool d8 = deleteFile(k2022_chman_name);
  const bool nuked_everything = d1 && d2 && d3 && d4 && d5 && d6 && d7 && d8;
  if (nuked_everything) {
    return true;
  }
  return false;
}

void downloadFiles(CURL* curl_handle, char* download_url, const char* output_name) {
  FILE* output_file;
  std::string filename(output_name);

  // Find current dir to place all downloaded files side by side with the executable
  const std::filesystem::path file_path = getPathFromName(output_name);
  const std::string file_path_str = file_path.string();

  if (fileExists(file_path)) {
    std::cout << "WARNING: Not downloading: "
              << filename << " exists." << std::endl;
    return;
  }

  /* Set URL to fetch here */
  curl_easy_setopt(curl_handle, CURLOPT_URL, download_url);

  /* Open the file */
  errno_t file_handle;
  file_handle = fopen_s(&output_file, output_name, "wbx");

  if (file_handle == 0) {
    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, output_file);

    if (isDebug()) {
      std::cout << "Downloading " << download_url
                 << " to " << file_path_str << std::endl;
    }

    /* Download it! */
    curl_easy_perform(curl_handle);

    errno_t file_close;
    /* Close the file */
    file_close = fclose(output_file);
    if (file_close != 0) {
      std::cerr << "ERROR: Failed to close "
                << output_name << " handle!" << std::endl;
    }

    bool download_success = fileExists(file_path);
    if (download_success) {
      std::cout << "Successfully downloaded " << filename << std::endl;
    }

    if (isDebug()) {
      std::cout << "Closed file handle for " << file_path_str << std::endl;
    }
  } else {
    std::cerr << "ERROR: Failed to open "
              << output_name << " handle!" << std::endl;
  }
}

void showHelp(char* exe) {
  if (isDebug()) {
    std::cout << exe << "\n" << std::endl;
  }
  std::wcout << " MSVS Downloader v" << getVersionNumberW() << std::endl;
  std::wcout << " Copyright (c) 2025 Alex313031" << std::endl;
  std::wcout << " Usage: \n"
             << "   [ -h, --help ]    - Show this help. \n"
             << "   [ -v, --version ] - Show version number and exit. \n"
             << "   [ -d, --debug ]   - Enable debug/verbose mode. \n"
             << "   [ -n, --nuke ]    - Delete any previously downloaded files. \n"
             << "   [ -14, --2015 ]   - Download MSVS 2015 .iso \n"
             << "   [ -15, --2017 ]   - Download MSVS 2017 .exe \n"
             << "   [ -16, --2019 ]   - Download MSVS 2019 .exe \n"
             << "   [ -17, --2022 ]   - Download MSVS 2022 .exe \n" << "\n"
             << "   [ -c, --custom <URL> <file> ] - Download from custom URL to custom filename. "
             << std::endl;
}

bool isDebug() {
  return is_debug;
}

void setIsDebug(bool is_debug_mode) {
  is_debug = is_debug_mode;
}

int endCode(int status_code) {
  if (isDebug()) {
    std::cout << "status " << status_code <<std::endl;
  }

  /* Dirty way to hold console open, without function or cmd.exe /k */
  system("pause");

  if (status_code == STATUS_CODE::OKAY ||
      status_code == STATUS_CODE::WARN ||
      status_code == STATUS_CODE::FAIL) {
    return status_code;
  } else {
    std::cerr << "ERROR: Invalid STATUS_CODE: " << status_code << std::endl;
  }

  return STATUS_CODE::FAIL;
}

int main(int argc, char* argv[], char *envp[]) {
  clearScreen();
  if (argc < 2) {
    showHelp(argv[0]);
    return endCode(STATUS_CODE::WARN);
  }

  std::vector<std::string> args(argv + 1, argv + argc);  // From first to last argument in the argv array

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--debug") == 0) {
      setIsDebug(true);
    }
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      showHelp(argv[0]);
      return endCode(STATUS_CODE::OKAY);
    } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
      std::wcout << "MSVS Downloader v" << getVersionNumberW() << "\n" << std::endl;
      return endCode(STATUS_CODE::OKAY);
    }
    if (strcmp(argv[i], "--custom") == 0) {
      i++;
      custom_url = true;
    } else if (strcmp(argv[i], "--nuke") == 0) {
      nuke_files = true;
    } else {
      if (strcmp(argv[i], "-14") == 0 || strcmp(argv[i], "--2015") == 0) {
        get_2015 = true;
      }
      if (strcmp(argv[i], "-15") == 0 || strcmp(argv[i], "--2017") == 0) {
        get_2017 = true;
      }
      if (strcmp(argv[i], "-16") == 0 || strcmp(argv[i], "--2019") == 0) {
        get_2019 = true;
      }
      if (strcmp(argv[i], "-17") == 0 || strcmp(argv[i], "--2022") == 0) {
        get_2022 = true;
      }
    }
  }

  CURL* curl_handle;

  /* Initialize globals */
  if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK) {
    std::wstring kVersionString = getVersionNumberW();
    if (isDebug()) {
      std::wcout << "Welcome to MSVS Downloader v" << kVersionString << "! [DEBUG]" << std::endl;
    } else {
      std::wcout << "Welcome to MSVS Downloader v" << kVersionString << "!" << std::endl;
    }
    std::cout << std::endl;
  } else {
    std::cerr << "Error initializing cURL!" << std::endl;
    return STATUS_CODE::FAIL;
  }

  if (nuke_files) {
    std::wcout << "Deleting any previously downloaded files because --nuke was passed." << std::endl;
    if (nukeFiles()) {
      return endCode(STATUS_CODE::OKAY);
    } else {
      return endCode(STATUS_CODE::WARN);
    }
  }

  /* Init the cURL session */
  curl_handle = curl_easy_init();

  /* Switch on full protocol/debug output while testing */
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, OFF);

  /* Handle TLS/SSL */
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, ON);

  /* Follow redirects */
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, ON);

  /* Enable progress meter, set to 1L to enable it */
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, ON);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

  if (custom_url) {
    char* custom_url = argv[2];
    char* custom_name = argv[3];
    downloadFiles(curl_handle, custom_url, custom_name);
  }

  if (get_2015) {
    std::wcout << "Downloading Visual Studio 2015..." << std::endl;
    //downloadFiles(curl_handle, const_cast<char*>(k2015_url), k2015_name);
    //downloadFiles(curl_handle, const_cast<char*>(k2015_chman_url), k2015_chman_name);
    std::wcout << k2015RelNotes << std::endl;
  }

  if (get_2017) {
    std::wcout << "Downloading Visual Studio 2017..." << std::endl;
    downloadFiles(curl_handle, const_cast<char*>(k2017_url), k2017_name);
    downloadFiles(curl_handle, const_cast<char*>(k2017_chman_url), k2017_chman_name);
  }

  if (get_2019) {
    std::wcout << "Downloading Visual Studio 2019..." << std::endl;
    downloadFiles(curl_handle, const_cast<char*>(k2019_url), k2019_name);
    downloadFiles(curl_handle, const_cast<char*>(k2019_chman_url), k2019_chman_name);
  }

  if (get_2022) {
    std::wcout << "Downloading Visual Studio 2022..." << std::endl;
    downloadFiles(curl_handle, const_cast<char*>(k2022_url), k2022_name);
    downloadFiles(curl_handle, const_cast<char*>(k2022_chman_url), k2022_chman_name);
  }

  int num_files_closed = _fcloseall();

  if (num_files_closed > 0) {
    std::cerr << "ERROR: Num unclosed file handles: "
              << num_files_closed << std::endl;
  }

  /* Cleanup cURL stuff */
  curl_easy_cleanup(curl_handle);

  /* Force cleanup globals */
  curl_global_cleanup();

  return STATUS_CODE::OKAY;
}
