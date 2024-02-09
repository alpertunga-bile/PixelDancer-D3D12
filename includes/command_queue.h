#pragma once

#include "d3d12_types.h"

namespace pxd::d3d12 {
class CommandQueue
{
public:
  void init(ID3D12Device* device);
  void destroy();

  void execute(unsigned int        command_list_count,
               ID3D12CommandList** command_lists);

  void signal(ID3D12Fence* fence, unsigned int fence_value);

  inline ID3D12CommandQueue* get_command_queue()
  {
    return m_command_queue.Get();
  }

private:
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_command_queue;
};
}