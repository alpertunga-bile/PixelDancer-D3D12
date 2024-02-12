#include "command_list.h"

namespace pxd::d3d12 {
void
GraphicsCommandList::init(ID3D12Device*           device,
                          ID3D12PipelineState*    pipeline_state,
                          ID3D12CommandAllocator* command_allocator)
{
  D3D12_CHECK(device->CreateCommandList(0,
                                        D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        command_allocator,
                                        pipeline_state,
                                        IID_PPV_ARGS(&m_command_list)));

  D3D12_CHECK(m_command_list->Close());
}

void
GraphicsCommandList::destroy()
{
  SAFE_RELEASE(m_command_list);
}
}