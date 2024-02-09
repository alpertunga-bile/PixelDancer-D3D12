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

#if defined(_DEBUG)
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // creating console for log outputs
  if (AllocConsole()) {
    FILE* p_cout;
    freopen_s(&p_cout, "CONOUT$", "w", stdout);
    SetConsoleTitleW(L"Debug Console");
  }
#endif

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

  init_base();
  init_pipeline();
  init_data();
  init_sync();

  m_is_initialized = true;
}

void
D3D12Engine::cleanup()
{
  if (m_is_initialized) {
    m_fence.wait_for_gpu(m_command_queue,
                         *m_swapchain.get_current_fence_value());

    m_deletion_queue.flush();
  }

  loaded_engine = nullptr;
}

void
D3D12Engine::draw()
{
  m_fence.move_next_frame(m_swapchain, m_command_queue);
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

    draw();
  }
}

void
D3D12Engine::init_base()
{
  m_nexus.init();
  m_command_queue.init(m_nexus.get_device());
  m_swapchain.init(m_nexus.get_factory(),
                   m_nexus.get_device(),
                   m_command_queue.get_command_queue(),
                   m_hwnd,
                   m_width,
                   m_height);

  m_deletion_queue.push_function([&]() {
    m_swapchain.destroy();
    m_command_queue.destroy();
    m_nexus.destroy();
  });
}

void
D3D12Engine::init_pipeline()
{
  m_vertex_shader.init(L"shaders\\triangle.hlsl", "VSMain", "vs_5_0");
  m_pixel_shader.init(L"shaders\\triangle.hlsl", "PSMain", "ps_5_0");

  D3D12_INPUT_ELEMENT_DESC input_element_descs[] = {
    {"POSITION",
     0,    DXGI_FORMAT_R32G32B32_FLOAT,
     0,  0,
     D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {   "COLOR",
     0, DXGI_FORMAT_R32G32B32A32_FLOAT,
     0, 12,
     D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
  };

  m_pipeline.init(m_nexus.get_device(),
                  input_element_descs,
                  _countof(input_element_descs),
                  m_vertex_shader,
                  m_pixel_shader);

  m_vertex_shader.destroy();
  m_pixel_shader.destroy();

  m_deletion_queue.push_function([&]() { m_pipeline.destroy(); });
}

void
D3D12Engine::init_data()
{
  float aspect_ratio =
    static_cast<float>(m_width) / static_cast<float>(m_height);

  Vertex triangle_vertices[] = {
    {   { 0.0f, 0.25f * aspect_ratio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
    { { 0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
    {{ -0.25f, -0.25f * aspect_ratio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }}
  };

  unsigned int vertex_buffer_size = sizeof(triangle_vertices);

  m_vertex_buffer.init(m_nexus.get_allocator(), vertex_buffer_size);

  void* data = nullptr;
  D3D12_CHECK(m_vertex_buffer.get_buffer()->Map(0, NULL, &data));
  memcpy(data, triangle_vertices, vertex_buffer_size);
  m_vertex_buffer.get_buffer()->Unmap(0, NULL);

  m_vertex_buffer_view.BufferLocation =
    m_vertex_buffer.get_buffer()->GetGPUVirtualAddress();
  m_vertex_buffer_view.StrideInBytes = sizeof(Vertex);
  m_vertex_buffer_view.SizeInBytes   = vertex_buffer_size;

  m_deletion_queue.push_function([&]() { m_vertex_buffer.destroy(); });
}

void
D3D12Engine::init_sync()
{
  m_fence.init(m_nexus.get_device(),
               *m_swapchain.get_current_fence_value(),
               m_command_queue);

  m_fence.wait_for_gpu(m_command_queue, *m_swapchain.get_current_fence_value());

  m_deletion_queue.push_function([&]() { m_fence.destroy(); });
}
