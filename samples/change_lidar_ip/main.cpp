//
// The MIT License (MIT)
//
// Copyright (c) 2022 Livox. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "livox_lidar_def.h"
#include "livox_lidar_api.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

static std::string g_old_ip;
static std::string g_new_ip;

static void SetIpInfoCallback(livox_status status, uint32_t handle,
                              LivoxLidarAsyncControlResponse *response,
                              void *client_data) {
  if (response == nullptr) {
    return;
  }
  printf("SetIpInfoCallback, status:%u, handle:%u, ret_code:%u, error_key:%u\n",
         status, handle, response->ret_code, response->error_key);
  if (status == kLivoxLidarStatusSuccess && response->ret_code == 0 &&
      response->error_key == 0) {
    LivoxLidarRequestReboot(handle, nullptr, nullptr);
  }
}

static void LidarInfoChangeCallback(const uint32_t handle,
                                    const LivoxLidarInfo *info,
                                    void *client_data) {
  if (info == nullptr) {
    printf("lidar info change callback failed, the info is nullptr.\n");
    return;
  }
  printf("Lidar handle: %u ip: %s SN: %s\n", handle, info->lidar_ip, info->sn);

  if (g_old_ip == info->lidar_ip) {
    LivoxLidarIpInfo ip_info;
    memset(&ip_info, 0, sizeof(ip_info));
    strncpy(ip_info.ip_addr, g_new_ip.c_str(), sizeof(ip_info.ip_addr) - 1);
    strncpy(ip_info.net_mask, "255.255.255.0", sizeof(ip_info.net_mask) - 1);
    std::string gw = g_new_ip.substr(0, g_new_ip.rfind('.') + 1) + "1";
    strncpy(ip_info.gw_addr, gw.c_str(), sizeof(ip_info.gw_addr) - 1);
    SetLivoxLidarIp(handle, &ip_info, SetIpInfoCallback, nullptr);
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s <old_ip> <new_ip>\n", argv[0]);
    return -1;
  }

  g_old_ip = argv[1];
  g_new_ip = argv[2];

  if (!LivoxLidarSdkInit("config.json")) {
    printf("Livox Init Failed\n");
    LivoxLidarSdkUninit();
    return -1;
  }

  SetLivoxLidarInfoChangeCallback(LidarInfoChangeCallback, nullptr);

#ifdef WIN32
  Sleep(3000);
#else
  sleep(3);
#endif

  LivoxLidarSdkUninit();
  printf("Change lidar ip demo end!\n");
  return 0;
}

