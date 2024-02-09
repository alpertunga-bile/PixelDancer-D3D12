#include "nexus.h"

#include "dxgi1_6.h"

namespace pxd::d3d12 {
void
Nexus::init()
{
  unsigned int dxgi_factory_flags = 0;

#if defined(_DEBUG)
  if (D3D12_RET(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug_controller)))) {
    m_debug_controller->EnableDebugLayer();

    dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
  }
#endif

  D3D12_CHECK(CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&m_factory)));

  get_hardware_adapter(m_factory.Get(), &m_physical_device);

  D3D12_CHECK(D3D12CreateDevice(
    m_physical_device.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
  set_name(m_device.Get(), L"noxus device");

  D3D12MA::ALLOCATOR_DESC alloc_desc = {};
  alloc_desc.pDevice                 = m_device.Get();
  alloc_desc.pAdapter                = m_physical_device.Get();

  D3D12_CHECK(D3D12MA::CreateAllocator(&alloc_desc, &m_allocator));
}

void
Nexus::destroy()
{
  SAFE_RELEASE(m_device);
  SAFE_RELEASE(m_physical_device);
  SAFE_RELEASE(m_allocator);
  SAFE_RELEASE(m_factory);
  SAFE_RELEASE(m_debug_controller);
}

void
Nexus::get_hardware_adapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter)
{
  *adapter = nullptr;

  Microsoft::WRL::ComPtr<IDXGIAdapter1> temp_adapter;

  Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;

  if (D3D12_RET(factory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
    for (unsigned int adapter_index = 0;
         D3D12_RET(factory6->EnumAdapterByGpuPreference(
           adapter_index,
           DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
           IID_PPV_ARGS(&temp_adapter)));
         adapter_index++) {
      DXGI_ADAPTER_DESC1 desc;
      temp_adapter->GetDesc1(&desc);

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        continue;
      }

      if (D3D12_RET(D3D12CreateDevice(temp_adapter.Get(),
                                      D3D_FEATURE_LEVEL_11_0,
                                      _uuidof(ID3D12Device),
                                      nullptr))) {
        break;
      }
    }
  }

  if (temp_adapter.Get() == nullptr) {
    for (unsigned int adapter_index = 0;
         D3D12_RET(factory->EnumAdapters1(adapter_index, &temp_adapter));
         adapter_index++) {
      DXGI_ADAPTER_DESC1 desc;
      temp_adapter->GetDesc1(&desc);

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        continue;
      }

      if (D3D12_RET(D3D12CreateDevice(temp_adapter.Get(),
                                      D3D_FEATURE_LEVEL_11_0,
                                      _uuidof(ID3D12Device),
                                      nullptr))) {
        break;
      }
    }
  }

  *adapter = temp_adapter.Detach();

  if (*adapter == nullptr) {
    return;
  }

  print_device_information(*adapter);
}

void
Nexus::print_device_information(IDXGIAdapter1* adapter)
{
#if defined(_DEBUG)
  DXGI_ADAPTER_DESC1 desc;
  adapter->GetDesc1(&desc);

  std::wstring device_info = desc.Description;
  std::string  name_string(device_info.begin(), device_info.end());

  // byte to gigabyte (B -> GB)
  float denominator = 1024.f * 1024.f * 1024.f;

  float dedicated_memory_gb = (float)desc.DedicatedSystemMemory / denominator;
  float dedicated_video_memory_gb =
    (float)desc.DedicatedVideoMemory / denominator;
  float shared_system_memory_gb = (float)desc.SharedSystemMemory / denominator;

  printf(
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  printf("Selected Device Information  :\n");
  printf("Name                         : %s\n", name_string.c_str());
  printf("Dedicated Video Memory  (GB) : %3.2f\n", dedicated_video_memory_gb);
  printf("Dedicated System Memory (GB) : %3.2f\n", dedicated_memory_gb);
  printf("Shared System Memory    (GB) : %3.2f\n", shared_system_memory_gb);
  printf(
    "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
#endif
}
}