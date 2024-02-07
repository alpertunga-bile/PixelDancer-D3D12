#include "d3d12_engine.h"

int APIENTRY
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR     lpCmdLine,
        int       nCmdShow)
{
  D3D12Engine d3d12_engine;

  d3d12_engine.init(hInstance, nCmdShow);
  d3d12_engine.run();
  d3d12_engine.cleanup();
}