#include "swapchain.h"

namespace pxd::d3d12 {
void
Swapchain::init(IDXGIFactory4*      factory,
                ID3D12Device*       device,
                ID3D12CommandQueue* command_queue,
                HWND&               hwnd,
                unsigned int        width,
                unsigned int        height)
{
  DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
  swapchain_desc.BufferCount           = FRAME_COUNT;
  swapchain_desc.Width                 = width;
  swapchain_desc.Height                = height;
  swapchain_desc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapchain_desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapchain_desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapchain_desc.SampleDesc.Count      = 1;

  Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
  D3D12_CHECK(factory->CreateSwapChainForHwnd(
    command_queue, hwnd, &swapchain_desc, nullptr, nullptr, &swapchain));

  D3D12_CHECK(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

  D3D12_CHECK(swapchain.As(&m_swapchain));
  m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

  D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
  rtv_heap_desc.NumDescriptors             = FRAME_COUNT;
  rtv_heap_desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtv_heap_desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

  D3D12_CHECK(
    device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&m_rtv_heap)));
  set_name(m_rtv_heap.Get(), L"render target view descriptor heap");

  m_rtv_descriptor_size =
    device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(
    m_rtv_heap->GetCPUDescriptorHandleForHeapStart());

  for (unsigned int i = 0; i < FRAME_COUNT; i++) {
    D3D12_CHECK(
      m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_frames[i].render_target)));

    device->CreateRenderTargetView(
      m_frames[i].render_target.Get(), nullptr, rtv_handle);

    rtv_handle.Offset(1, m_rtv_descriptor_size);
    set_name(m_frames[i].render_target.Get(), L"per frame render target");

    D3D12_CHECK(device->CreateCommandAllocator(
      D3D12_COMMAND_LIST_TYPE_DIRECT,
      IID_PPV_ARGS(&m_frames[i].command_allocator)));
    set_name(m_frames[i].command_allocator.Get(),
             L"per frame command allocator");
  }
}

void
Swapchain::destroy()
{
  for (unsigned int i = 0; i < FRAME_COUNT; i++) {
    SAFE_RELEASE(m_frames[i].render_target);
    SAFE_RELEASE(m_frames[i].command_allocator);
  }

  SAFE_RELEASE(m_rtv_heap);
  SAFE_RELEASE(m_swapchain);
}
}