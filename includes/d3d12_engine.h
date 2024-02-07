#pragma once

#include "d3d12_types.h"

class D3D12Engine
{
public:
  int m_frame_number{ 0 };

  RECT m_window_rect = { 0,
                         0,
                         static_cast<LONG>(1700),
                         static_cast<LONG>(900) };

  HWND m_hwnd;

  static D3D12Engine&     Get();
  static LRESULT CALLBACK WindowProc(HWND   hwnd,
                                     UINT   message,
                                     WPARAM wParam,
                                     LPARAM lParam);

  bool m_is_initialized{ false };

  void init(HINSTANCE hInstance, int nCmdShow);

  void cleanup();

  void draw();

  void run();
};