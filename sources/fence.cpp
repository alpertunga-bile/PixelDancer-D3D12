#include "fence.h"

namespace pxd::d3d12 {
void
Fence::init(ID3D12Device* device,
            unsigned int& fence_value,
            CommandQueue& command_queue)
{
  D3D12_CHECK(device->CreateFence(
    fence_value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

  fence_value++;

  m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
  if (m_fence_event == nullptr) {
    LOG_ERROR("Fence event creation is failed");
  }

  wait_for_gpu(command_queue, fence_value);
}

void
Fence::destroy()
{
  if (m_fence_event != nullptr) {
    CloseHandle(m_fence_event);
  }

  SAFE_RELEASE(m_fence);
}

void
Fence::wait_for_gpu(CommandQueue& command_queue, unsigned int& fence_value)
{
  command_queue.signal(m_fence.Get(), fence_value);

  D3D12_CHECK(m_fence->SetEventOnCompletion(fence_value, m_fence_event));
  WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);

  fence_value++;
}

void
Fence::move_next_frame(Swapchain& swapchain, CommandQueue& command_queue)
{
  const UINT64 current_fence_value = *swapchain.get_current_fence_value();
  command_queue.signal(m_fence.Get(), current_fence_value);

  swapchain.update_frame_index();

  if (m_fence->GetCompletedValue() < *swapchain.get_current_fence_value()) {
    D3D12_CHECK(m_fence->SetEventOnCompletion(
      *swapchain.get_current_fence_value(), m_fence_event));

    WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);
  }

  *swapchain.get_current_fence_value() = current_fence_value + 1;
}
}