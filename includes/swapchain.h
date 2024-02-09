#pragma once

#include "d3d12_types.h"

const unsigned int FRAME_COUNT = 2;

namespace pxd::d3d12 {

struct FrameData
{
  Microsoft::WRL::ComPtr<ID3D12Resource>         render_target;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator;
  unsigned int                                   fence_value;
};

class Swapchain
{
public:
  void init(IDXGIFactory4*      factory,
            ID3D12Device*       device,
            ID3D12CommandQueue* command_queue,
            HWND&               hwnd,
            unsigned int        width,
            unsigned int        height);
  void destroy();

  inline void update_frame_index()
  {
    m_frame_index = m_swapchain->GetCurrentBackBufferIndex();
  }

  inline unsigned int* get_current_fence_value()
  {
    return &m_frames[m_frame_index].fence_value;
  }

  inline ID3D12Resource* get_current_render_target()
  {
    return m_frames[m_frame_index].render_target.Get();
  }

  inline ID3D12CommandAllocator* get_current_command_allocator()
  {
    return m_frames[m_frame_index].command_allocator.Get();
  }

  inline FrameData& get_current_frame_data() { return m_frames[m_frame_index]; }

  FrameData& operator[](int index) { return m_frames[index]; }

private:
  Microsoft::WRL::ComPtr<IDXGISwapChain3>      m_swapchain;
  FrameData                                    m_frames[FRAME_COUNT];
  unsigned int                                 m_frame_index = 0;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
  unsigned int                                 m_rtv_descriptor_size;
};
}