#pragma once

#include <array>
#include <deque>
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "directx/d3dx12.h"

#include <d3d12.h>
#include <dxgi1_4.h>

#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include "D3D12MemAlloc.h"

///////////////////////////////////////////////////////////////////////////////////////////
// Debug / Logging

inline void
log_info(const char* msg)
{
  printf("[    INFO] /_\\ %s\n", msg);
}

inline void
log_success(const char* msg)
{
  printf(std::format("\033[0;32m[ SUCCESS] /_\\ {} \033[0m\n", msg).c_str());
}

inline std::string
get_output_string(const char* filename, int line)
{
  return std::format("{} | {}", filename, line);
}

inline void
log_warning(const char* msg, const char* file, int line)
{
  printf(std::format("\033[0;33m[ WARNING] /_\\ {} /_\\ {} \033[0m\n",
                     msg,
                     get_output_string(file, line).c_str())
           .c_str());
}

inline void
log_error(const char* msg, const char* file, int line)
{
  printf(std::format("\033[0;31m[  FAILED] /_\\ {} /_\\ {} \033[0m\n",
                     msg,
                     get_output_string(file, line).c_str())
           .c_str());
  exit(EXIT_FAILURE);
}

#if defined(_DEBUG)
#define LOG_INFO(msg)    log_info(msg)
#define LOG_SUCCESS(msg) log_success(msg);
#define LOG_WARNING(msg) log_warning(msg, __FILE__, __LINE__);
#define LOG_ERROR(msg)   log_error(msg, __FILE__, __LINE__);
#else
#define LOG_INFO(msg)
#define LOG_SUCCESS(msg)
#define LOG_WARNING(msg)
#define LOG_ERROR(msg)
#endif

inline std::string
HrToString(HRESULT hr)
{
  char s_str[64] = {};
  sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
  return std::string(s_str);
}

inline void
d3d12_check(HRESULT result, const char* file, int line)
{
  if (SUCCEEDED(result)) {
#if defined(_DEBUG)
    // LOG_SUCCESS( get_output_string(file, line).c_str() );
#endif
    return;
  }

  std::string result_str = HrToString(result);
  std::string msg = result_str + " " + get_output_string(file, line).c_str();

  LOG_ERROR(msg.c_str());
}

inline bool
d3d12_ret(HRESULT result, const char* file, int line)
{
  if (SUCCEEDED(result)) {
#if defined(_DEBUG)
    // LOG_SUCCESS( get_output_string(file, line).c_str() );
#endif
    return true;
  }

  std::string result_str = HrToString(result);
  std::string msg = result_str + " " + get_output_string(file, line).c_str();

  LOG_WARNING(msg.c_str());

  return false;
}

#define D3D12_CHECK(res) d3d12_check(res, __FILE__, __LINE__)
#define D3D12_RET(res)   d3d12_ret(res, __FILE__, __LINE__)
