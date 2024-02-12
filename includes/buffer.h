#pragma once

#include "d3d12_types.h"

namespace pxd::d3d12 {
class Buffer
{
public:
  void init(D3D12MA::Allocator* allocator, unsigned int buffer_size);
  void destroy();

  void update(unsigned int size, void* buffer_data);

  inline ID3D12Resource* get_buffer() { return m_buffer.Get(); }

private:
  Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
  D3D12MA::Allocation*                   m_allocation;
};
}