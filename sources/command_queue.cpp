#include "command_queue.h"

namespace pxd::d3d12 {
void
CommandQueue::init(ID3D12Device* device)
{
  D3D12_COMMAND_QUEUE_DESC queue_desc = {};
  queue_desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queue_desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;

  D3D12_CHECK(
    device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_command_queue)));
}

void
CommandQueue::destroy()
{
  SAFE_RELEASE(m_command_queue);
}

void
CommandQueue::execute(unsigned int        command_list_count,
                      ID3D12CommandList** command_lists)
{
  m_command_queue->ExecuteCommandLists(command_list_count, command_lists);
}

void
CommandQueue::signal(ID3D12Fence* fence, unsigned int fence_value)
{
  D3D12_CHECK(m_command_queue->Signal(fence, fence_value));
}
}