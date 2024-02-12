#pragma once

#include "buffer.h"
#include "command_list.h"
#include "deletion_queue.h"
#include "fence.h"
#include "nexus.h"
#include "pipeline.h"

class D3D12Engine
{
public:
  static D3D12Engine&     Get();
  static LRESULT CALLBACK WindowProc(HWND   hwnd,
                                     UINT   message,
                                     WPARAM wParam,
                                     LPARAM lParam);

  void init(HINSTANCE hInstance, int nCmdShow);
  void cleanup();
  void draw();
  void run();

private:
  void init_base();
  void init_pipeline();
  void init_data();
  void init_sync();

  void populate_commands();

private:
  unsigned int m_width  = 1700;
  unsigned int m_height = 900;
  int          m_frame_number{ 0 };
  RECT         m_window_rect = { 0,
                                 0,
                                 static_cast<LONG>(m_width),
                                 static_cast<LONG>(m_height) };

  HWND          m_hwnd;
  bool          m_is_initialized{ false };
  DeletionQueue m_deletion_queue;

  //////////////////////////////////////////////////////////////////
  // D3D12 Variables
  pxd::d3d12::Nexus               m_nexus;
  pxd::d3d12::CommandQueue        m_command_queue;
  pxd::d3d12::GraphicsCommandList m_command_list;
  pxd::d3d12::Swapchain           m_swapchain;
  pxd::d3d12::Fence               m_fence;
  pxd::d3d12::Pipeline            m_pipeline;
  pxd::d3d12::Shader              m_vertex_shader;
  pxd::d3d12::Shader              m_pixel_shader;
  pxd::d3d12::Buffer              m_vertex_buffer;
  D3D12_VERTEX_BUFFER_VIEW        m_vertex_buffer_view;

  CD3DX12_VIEWPORT m_viewport{ 0.f,
                               0.f,
                               static_cast<float>(m_width),
                               static_cast<float>(m_height) };
  CD3DX12_RECT     m_scissor_rect{ 0,
                               0,
                               static_cast<LONG>(m_width),
                               static_cast<LONG>(m_height) };
};