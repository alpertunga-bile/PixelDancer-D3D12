#include "d3d12_engine.h"

#include <cassert>

D3D12Engine* loaded_engine = nullptr;

D3D12Engine&
D3D12Engine::Get()
{
  return *loaded_engine;
}

LRESULT
D3D12Engine::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
    case WM_CREATE:
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

void
D3D12Engine::init(HINSTANCE hInstance, int nCmdShow)
{
  assert(loaded_engine == nullptr);
  loaded_engine = this;

  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // creating console for log outputs
  if (AllocConsole()) {
    FILE* p_cout;
    freopen_s(&p_cout, "CONOUT$", "w", stdout);
    SetConsoleTitle("Debug Console");
  }

  WNDCLASSEX window_class    = {};
  window_class.cbSize        = sizeof(WNDCLASSEX);
  window_class.style         = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc   = WindowProc;
  window_class.hInstance     = hInstance;
  window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
  window_class.lpszClassName = "D3D12Engine";
  RegisterClassEx(&window_class);

  AdjustWindowRect(&m_window_rect, WS_OVERLAPPEDWINDOW, FALSE);

  m_hwnd = CreateWindowW(L"D3D12Engine",
                         L"D3D12Engine",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         m_window_rect.right - m_window_rect.left,
                         m_window_rect.bottom - m_window_rect.top,
                         nullptr,
                         nullptr,
                         hInstance,
                         nullptr);

  ShowWindow(m_hwnd, nCmdShow);

  LOG_INFO("Window is created");

  m_is_initialized = true;
}

void
D3D12Engine::cleanup()
{
}

void
D3D12Engine::draw()
{
}

void
D3D12Engine::run()
{
  MSG msg = {};

  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}
