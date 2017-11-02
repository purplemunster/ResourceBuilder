//=====================================================================================================================
/* Copyright (c) 2017 Rohan Mehalwal

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
//=====================================================================================================================

#pragma once
#include <d3d12.h>
#include <cstdint>
#include <algorithm>

#undef min
#undef max

namespace AR {

//=====================================================================================================================
// Returns whether the DXGI_FORMAT input is a typeless format
constexpr bool IsTypeless(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC7_TYPELESS:
        return true;
        break;
    default:
        return false;
    }
}

//=====================================================================================================================
// D3D12 resource builder helper
struct ResourceBuilder : D3D12_RESOURCE_DESC
{
    /// Initialise builder from an existing resource
    ///
    /// @param pResource    [in] Existing D3D12 resource pointer
    ///
    D3D12_RESOURCE_DESC& FromExistingResource(ID3D12Resource* pResource);

    /// Initialise builder as a buffer
    ///
    /// @param byteWidth    [in] Buffer size in bytes
    ///
    D3D12_RESOURCE_DESC& Buffer(uint64_t byteWidth);

    /// Initialise builder as a one-dimensional texture from input
    ///
    /// @param width     [in] Texture width in pixels
    /// @param format    [in] Texture format @see DXGI_FORMAT
    /// @param arraySize [in] Array slice count
    /// @param mipLevels [in] Mip level count
    ///
    D3D12_RESOURCE_DESC& Texture1D(
        uint64_t width, DXGI_FORMAT format, uint16_t arraySize = 1, uint16_t mipLevels = 1);

    /// Initialise builder as a two-dimensional texture from input
    ///
    /// @param width     [in] Texture width in pixels
    /// @param height    [in] Texture height in pixels
    /// @param format    [in] Texture format @see DXGI_FORMAT
    /// @param arraySize [in] Array slice count
    /// @param mipLevels [in] Mip level count
    ///
    D3D12_RESOURCE_DESC& Texture2D(
        uint64_t width, uint32_t height, DXGI_FORMAT format, uint16_t arraySize = 1, uint16_t mipLevels = 1);

    /// Initialise builder as a three-dimensional texture from input
    ///
    /// @param width     [in] Texture width in pixels
    /// @param height    [in] Texture height in pixels
    /// @param depth     [in] Texture depth in pixels
    /// @param format    [in] Texture format @see DXGI_FORMAT
    /// @param mipLevels [in] Mip level count
    ///
    D3D12_RESOURCE_DESC& Texture3D(
        uint64_t width, uint32_t height, uint16_t depth, DXGI_FORMAT format, uint16_t mipLevels = 1);

    /// Set builder resource format
    ///
    /// @param format [in] Resource format
    ///
    D3D12_RESOURCE_DESC& SetFormat(DXGI_FORMAT format);

    /// Returns builder resource description as a color target
    ///
    /// @param allowUAV [optional] Allow unordered access
    ///
    D3D12_RESOURCE_DESC AsColorTarget(bool allowUAV = false) const;

    /// Returns builder resource description as a depth target
    ///
    /// @param allowSRV [optional] Allow shader read access
    ///
    D3D12_RESOURCE_DESC AsDepthTarget(bool allowSRV = true) const;

    /// Returns color target view
    ///
    /// @param viewFormat [optional] Color target view format
    /// @param baseMip    [optional] Base mip level
    ///
    D3D12_RENDER_TARGET_VIEW_DESC AsColorTargetView(
        DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN, uint32_t baseMip = 0) const;

    /// Returns color target view for an array
    ///
    /// @param viewFormat [optional] Color target view format
    /// @param baseMip    [optional] Base mip level
    /// @param baseArray  [optional] Base array slice
    /// @param arraySize  [optional] Array count
    ///
    D3D12_RENDER_TARGET_VIEW_DESC AsColorTargetViewArray(
        DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN,
        uint16_t    baseMip    = 0,
        uint16_t    baseArray  = 0,
        uint16_t    arraySize  = D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) const;

    /// Returns depth stencil view
    ///
    /// @param viewFormat [optional] Depth-stencil view format
    /// @param baseMip    [optional] Base mip level
    ///
    D3D12_DEPTH_STENCIL_VIEW_DESC AsDepthStencilView(
        DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN, uint16_t baseMip = 0) const;

    /// Returns depth-stencil view for an array
    ///
    /// @param viewFormat [optional] Depth-stencil target view format
    /// @param baseMip    [optional] Base mip level
    /// @param baseArray  [optional] Base array slice
    /// @param arraySize  [optional] Array count
    ///
    D3D12_DEPTH_STENCIL_VIEW_DESC AsDepthStencilViewArray(
        DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN,
        uint16_t    baseMip    = 0,
        uint16_t    baseArray  = 0,
        uint16_t    arraySize  = D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) const;

     // Returns shader resource view for a buffer
     ///
     /// @param firstElement [optional] First buffer element index
     /// @param numElements  [optional] Element count
     /// @param byteStride   [optional] Structured buffer byte stride
     /// @param viewFormat   [optional] Override view format (typed buffers only)
     ///
    D3D12_SHADER_RESOURCE_VIEW_DESC AsBufferResourceView(
        uint32_t    firstElement = 0,
        uint32_t    numElements  = 0xffffffff,
        uint32_t    byteStride   = 0,
        DXGI_FORMAT viewFormat   = DXGI_FORMAT_UNKNOWN) const;

    /// Returns shader resource view
    ///
    /// @param viewFormat   [optional] Override view format
    /// @param baseMip      [optional] Base mip level
    /// @param mipLevels    [optional] Mip count
    /// @param minLod       [optional] Minimum level of detail
    ///
    D3D12_SHADER_RESOURCE_VIEW_DESC AsShaderResourceView(
        DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN,
        uint16_t    baseMip    = 0,
        uint16_t    mipLevels  = D3D12_REQ_MIP_LEVELS,
        float       minLod     = 0.0f) const;

    /// Set heap type property
    ///
    /// @param type [in] Heap type
    ///
    ResourceBuilder& SetHeapType(D3D12_HEAP_TYPE type);

    // Returns shader resource view for an array
    D3D12_SHADER_RESOURCE_VIEW_DESC AsShaderResourceViewArray(
        DXGI_FORMAT viewFormat = DXGI_FORMAT_UNKNOWN,
        uint16_t    baseMip    = 0,
        uint16_t    baseArray  = 0,
        uint16_t    mipLevels  = D3D12_REQ_MIP_LEVELS,
        uint16_t    arraySize  = D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION,
        float       minLod     = 0.0f) const;

    D3D12_RTV_DIMENSION   RTVDimension;      ///< RTV dimension
    D3D12_RTV_DIMENSION   RTVDimensionArray; ///< RTV dimension (valid only for array texture)
    D3D12_DSV_DIMENSION   DSVDimension;      ///< DSV dimension
    D3D12_DSV_DIMENSION   DSVDimensionArray; ///< DSV dimension (valid only for array texture)
    D3D12_SRV_DIMENSION   SRVDimension;      ///< SRV dimension
    D3D12_SRV_DIMENSION   SRVDimensionArray; ///< SRV dimension (valid only for array texture)
    D3D12_HEAP_PROPERTIES HeapProperties;    ///< Heap properties
    D3D12_HEAP_FLAGS      HeapFlags;         ///< Heap flags

private:

    /// @internal Internal helper functions for building view dimension structures
    void BuildViewDimensions();
};
} // AR