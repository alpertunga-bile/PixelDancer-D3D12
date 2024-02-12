#pragma once

#include "d3d12_types.h"

namespace pxd::d3d12 {
class GraphicsCommandList
{
public:
  void init(ID3D12Device*           device,
            ID3D12PipelineState*    pipeline_state,
            ID3D12CommandAllocator* command_allocator);
  void destroy();

  inline void reset(ID3D12CommandAllocator* cmd_alloc,
                    ID3D12PipelineState*    pipe_state)
  {
    m_command_list->Reset(cmd_alloc, pipe_state);
  }

  inline void set_root_signature(ID3D12RootSignature* root_sign)
  {
    m_command_list->SetGraphicsRootSignature(root_sign);
  }

  inline void end() { D3D12_CHECK(m_command_list->Close()); }

  inline ID3D12GraphicsCommandList* get_cmd_list()
  {
    return m_command_list.Get();
  }

private:
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;
};
}