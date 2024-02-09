#pragma once

#include "d3d12_types.h"
#include "d3dcompiler.h"

namespace pxd::d3d12 {
class Shader
{
public:
  void init(LPCWSTR shader_filename, LPCSTR entry_point, LPCSTR shader_version);

  void destroy();

  inline D3D12_SHADER_BYTECODE get_bytecode()
  {
    return CD3DX12_SHADER_BYTECODE(m_shader.Get());
  }

private:
  Microsoft::WRL::ComPtr<ID3DBlob> m_shader;
};
}