#pragma once

#include "d3d12_types.h"

namespace pxd::d3d12 {
class Nexus
{
public:
  void init();
  void destroy();

  inline ID3D12Debug*   get_debug() { return m_debug_controller.Get(); }
  inline IDXGIFactory4* get_factory() { return m_factory.Get(); }
  inline IDXGIAdapter1* get_physical_device()
  {
    return m_physical_device.Get();
  }
  inline ID3D12Device*       get_device() { return m_device.Get(); }
  inline D3D12MA::Allocator* get_allocator() { return m_allocator; }

private:
  void get_hardware_adapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter);
  void print_device_information(IDXGIAdapter1* adapter);

private:
  Microsoft::WRL::ComPtr<ID3D12Debug>   m_debug_controller;
  Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
  Microsoft::WRL::ComPtr<IDXGIAdapter1> m_physical_device;
  Microsoft::WRL::ComPtr<ID3D12Device>  m_device;
  D3D12MA::Allocator*                   m_allocator;
};
}