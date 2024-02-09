#include "buffer.h"

namespace pxd::d3d12 {
void
Buffer::init(D3D12MA::Allocator* allocator, unsigned int buffer_size)
{
  D3D12_RESOURCE_DESC resrc_desc = {};
  resrc_desc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
  resrc_desc.Alignment           = 0;
  resrc_desc.Width               = buffer_size;
  resrc_desc.Height              = 1;
  resrc_desc.DepthOrArraySize    = 1;
  resrc_desc.MipLevels           = 1;
  resrc_desc.Format              = DXGI_FORMAT_UNKNOWN;
  resrc_desc.SampleDesc.Count    = 1;
  resrc_desc.SampleDesc.Quality  = 0;
  resrc_desc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  resrc_desc.Flags               = D3D12_RESOURCE_FLAG_NONE;

  D3D12MA::ALLOCATION_DESC alloc_desc = {};
  alloc_desc.HeapType                 = D3D12_HEAP_TYPE_UPLOAD;

  D3D12_CHECK(allocator->CreateResource(&alloc_desc,
                                        &resrc_desc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ,
                                        NULL,
                                        &m_allocation,
                                        IID_PPV_ARGS(&m_buffer)));
}

void
Buffer::destroy()
{
  SAFE_RELEASE(m_buffer);
  SAFE_RELEASE(m_allocation);
}
}