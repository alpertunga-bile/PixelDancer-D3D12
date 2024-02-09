#pragma once

#include "command_queue.h"
#include "swapchain.h"

namespace pxd::d3d12 {
class Fence
{
public:
  void init(ID3D12Device* device,
            unsigned int& fence_value,
            CommandQueue& command_queue);
  void destroy();

  void wait_for_gpu(CommandQueue& command_queue, unsigned int& fence_value);

  void move_next_frame(Swapchain& swapchain, CommandQueue& command_queue);

private:
  HANDLE                              m_fence_event;
  Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
};
}