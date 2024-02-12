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
                         m_width,
                         m_height,
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
  populate_commands();

  ID3D12CommandList* command_lists[] = { m_command_list.get_cmd_list() };
  m_command_queue.execute(_countof(command_lists), command_lists);

  m_swapchain.present();

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

  m_command_list.init(m_nexus.get_device(),
                      m_pipeline.get_pipeline_state(),
                      m_swapchain.get_current_command_allocator());

  m_deletion_queue.push_function([&]() {
    m_pipeline.destroy();
    m_command_list.destroy();
  });
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

  m_vertex_buffer.update(vertex_buffer_size, triangle_vertices);

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

void
D3D12Engine::populate_commands()
{
  m_swapchain.reset_current_command_allocator();

  m_command_list.reset(m_swapchain.get_current_command_allocator(),
                       m_pipeline.get_pipeline_state());

  ID3D12GraphicsCommandList* cmd_list = m_command_list.get_cmd_list();

  cmd_list->SetGraphicsRootSignature(m_pipeline.get_root_signature());
  cmd_list->RSSetViewports(1, &m_viewport);
  cmd_list->RSSetScissorRects(1, &m_scissor_rect);

  m_swapchain.transition(cmd_list, D3D12_RESOURCE_STATE_RENDER_TARGET);

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle = m_swapchain.get_rtv_handle();
  cmd_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

  const float clear_color[] = { 0.f, 0.5f, 0.34f, 1.f };
  cmd_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
  cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  cmd_list->IASetVertexBuffers(0, 1, &m_vertex_buffer_view);
  cmd_list->DrawInstanced(3, 1, 0, 0);

  m_swapchain.transition(cmd_list, D3D12_RESOURCE_STATE_PRESENT);

  m_command_list.end();
}
