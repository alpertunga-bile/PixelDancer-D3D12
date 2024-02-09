#include "shader.h"

namespace pxd::d3d12 {
void
Shader::init(LPCWSTR shader_filename, LPCSTR entry_point, LPCSTR shader_version)
{
#if defined(_DEBUG)
  UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compile_flags = 0;
#endif

  D3D12_CHECK(D3DCompileFromFile(shader_filename,
                                 nullptr,
                                 nullptr,
                                 entry_point,
                                 shader_version,
                                 compile_flags,
                                 0,
                                 &m_shader,
                                 nullptr));
}

void
Shader::destroy()
{
  SAFE_RELEASE(m_shader);
}
}