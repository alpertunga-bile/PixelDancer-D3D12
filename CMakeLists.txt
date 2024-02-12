cmake_minimum_required(VERSION 3.5.0)
project(d3d12_sandbox VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

################################################################################
## GLOBAL DEFINITIONS

set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakes")

find_package(D3D12 REQUIRED)

#[[
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
	add_compile_options(-mavx2)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
	add_compile_options(/QxAVX2)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/arch:AVX2)
endif()
]]

file(GLOB D3DX12_HEADER_FILES ${CMAKE_SOURCE_DIR}/includes/directx/*.h)

set(HEADER_FILES
    ${D3DX12_HEADER_FILES}
    ${D3D12_INCLUDE_DIR}/d3d12.h
    ${D3D12_INCLUDE_DIR}/d3dcompiler.h
    ${D3D12_INCLUDE_DIR}/DirectXMath.h
    ${DXGI_INCLUDE_DIR}/dxgi1_4.h
    ${DXGI_INCLUDE_DIR}/dxgi1_6.h
    includes/d3d12_engine.h
    includes/nexus.h
    includes/swapchain.h
    includes/deletion_queue.h
    includes/command_queue.h
    includes/pipeline.h
    includes/fence.h
    includes/shader.h
    includes/buffer.h
    includes/command_list.h
    includes/d3d12_types.h
    includes/D3D12MemAlloc.h
)
    
set(SOURCE_FILES
    main.cpp
    sources/d3d12_engine.cpp
    sources/swapchain.cpp
    sources/command_queue.cpp
    sources/fence.cpp
    sources/pipeline.cpp
    sources/shader.cpp
    sources/nexus.cpp
    sources/command_list.cpp    
    sources/buffer.cpp
    sources/D3D12MemAlloc.cpp
    ${HEADER_FILES}
)

include_directories(
    includes
    ${D3D12_INCLUDE_DIRS}
)


add_executable(${PROJECT_NAME} WIN32 main.cpp ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${D3D12_LIBRARIES} )

target_precompile_headers(
    ${PROJECT_NAME} PRIVATE
    ${HEADER_FILES}
    <array>
    <deque>
    <format>
    <functional>
    <memory>
    <optional>
    <span>
    <string>
    <vector>
    <Windows.h>
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
