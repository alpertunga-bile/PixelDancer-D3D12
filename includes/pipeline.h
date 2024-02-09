#pragma once

#include "shader.h"

namespace pxd::d3d12 {
class Pipeline
{
public:
  void init(ID3D12Device*             device,
            D3D12_INPUT_ELEMENT_DESC* input_element_descs,
            unsigned int              input_element_count,
            Shader&                   vs,
            Shader&                   ps);
  void destroy();

  inline ID3D12PipelineState* get_pipeline_state()
  {
    return m_pipeline_state.Get();
  }

  inline ID3D12RootSignature* get_root_signature()
  {
    return m_root_signature.Get();
  }

private:
  Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature;
  Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_state;
};
}