#include "pipeline.h"

namespace pxd::d3d12 {
void
Pipeline::init(ID3D12Device*             device,
               D3D12_INPUT_ELEMENT_DESC* input_element_descs,
               unsigned int              input_element_count,
               Shader&                   vs,
               Shader&                   ps)
{
  CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc;
  root_signature_desc.Init(
    0,
    nullptr,
    0,
    nullptr,
    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  Microsoft::WRL::ComPtr<ID3DBlob> signature;
  Microsoft::WRL::ComPtr<ID3DBlob> error;

  D3D12_CHECK(D3D12SerializeRootSignature(
    &root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));

  D3D12_CHECK(device->CreateRootSignature(0,
                                          signature->GetBufferPointer(),
                                          signature->GetBufferSize(),
                                          IID_PPV_ARGS(&m_root_signature)));

  D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
  pso_desc.InputLayout     = { input_element_descs, input_element_count };
  pso_desc.pRootSignature  = m_root_signature.Get();
  pso_desc.VS              = vs.get_bytecode();
  pso_desc.PS              = ps.get_bytecode();
  pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  pso_desc.BlendState      = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  pso_desc.DepthStencilState.DepthEnable   = FALSE;
  pso_desc.DepthStencilState.StencilEnable = FALSE;
  pso_desc.SampleMask                      = UINT_MAX;
  pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  pso_desc.NumRenderTargets      = 1;
  pso_desc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM;
  pso_desc.SampleDesc.Count      = 1;

  D3D12_CHECK(device->CreateGraphicsPipelineState(
    &pso_desc, IID_PPV_ARGS(&m_pipeline_state)));
}

void
Pipeline::destroy()
{
  SAFE_RELEASE(m_pipeline_state);
  SAFE_RELEASE(m_root_signature);
}
}