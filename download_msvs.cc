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

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
  size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
  return written;
}

bool fileExists(const std::filesystem::path filepath) {
  std::cout << "Checking if " << filepath << " exists." << std::endl;
  return std::filesystem::exists(filepath);
}

void downloadFiles(CURL* curl_handle, char* download_url, const char* output_name) {
  FILE* output_file;

  std::string filename(output_name);
  const std::filesystem::path current_dir = std::filesystem::current_path();
  const std::filesystem::path file_path = current_dir / filename;
  bool file_already_present = fileExists(file_path);
  if (file_already_present) {
    std::cerr << "Warning: " << filename
              << " already exists on the filesystem!" << std::endl;
  }

  /* Set URL to get here */
  curl_easy_setopt(curl_handle, CURLOPT_URL, download_url);

  /* open the file */
  fopen_s(&output_file, output_name, "wbx");

  if (output_file) {
    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, output_file);

    /* get it! */
    curl_easy_perform(curl_handle);

    /* close the header file */
    fclose(output_file);
    std::cout << "Closed file handle for " << filename << std::endl;
  }
}

int main(int argc, char* argv[], char *envp[]) {
  CURL* curl_handle;

  clearScreen();

  if (argc < 1) {
    printf("Usage: %s <URL>\n", argv[0]);
    return 1;
  }

  /* Initialize globals */
  if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK) {
    std::string kVersionString = getVersionNumber();
    std::cout << "Welcome to MSVS Downloader v." << kVersionString << std::endl;
  } else {
    std::cerr << "Error initializing Curl!" << std::endl;
  }

  /* Init the Curl session */
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

  // const chat* custom_name = argv[2];
  //downloadFiles(curl_handle, argv[1], custom_name);
  downloadFiles(curl_handle, const_cast<char*>(msvs_2017_url), msvs_2017_name);
  downloadFiles(curl_handle, const_cast<char*>(msvs_2019_url), msvs_2019_name);
  downloadFiles(curl_handle, const_cast<char*>(msvs_2022_url), msvs_2022_name);

  int num_files_closed = _fcloseall();
#if defined(_DEBUG) && !defined(NDEBUG)
  constexpr int bad_handle_int = 0;
    std::string unclosed_handles_message =
        "_fcloseall() result: ";
#else
  constexpr int bad_handle_int = 1;
  std::string unclosed_handles_message =
      "ERROR: Num unclosed file handles: ";
#endif
  if (num_files_closed >= bad_handle_int) {
    std::cerr << unclosed_handles_message
              << num_files_closed << std::endl;
  }

  /* Cleanup Curl stuff */
  curl_easy_cleanup(curl_handle);

  /* Force cleanup globals */
  curl_global_cleanup();

  /* Dirty way to hold console open, without function or cmd.exe /k */
  system("pause");

  return 0;
}
