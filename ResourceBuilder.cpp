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
#include "ResourceBuilder.h"

#undef min
#undef max

namespace AR {

//=====================================================================================================================
// Initialise builder from existing resource
D3D12_RESOURCE_DESC& ResourceBuilder::FromExistingResource(
    ID3D12Resource* pResource)
{
    if (pResource != nullptr)
    {
        *(D3D12_RESOURCE_DESC*)this = pResource->GetDesc();
        HRESULT hr = pResource->GetHeapProperties(&this->HeapProperties, &this->HeapFlags);

        if (hr == S_OK)
        {
            BuildViewDimensions();
        }
    }

    return *this;
}

//=====================================================================================================================
// Initialise builder as a buffer
D3D12_RESOURCE_DESC& ResourceBuilder::Buffer(
    uint64_t byteWidth)
{
    ZeroMemory(this, sizeof(this));
    
    this->Width     = byteWidth;
    this->Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

    BuildViewDimensions();

    return *this;
}

//=====================================================================================================================
// Initialise builder as texture1D
D3D12_RESOURCE_DESC& ResourceBuilder::Texture1D(
    uint64_t width, DXGI_FORMAT format, uint16_t arraySize, uint16_t mipLevels)
{
    ZeroMemory(this, sizeof(this));

    this->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);

    this->Width            = width;
    this->Height           = 1;
    this->DepthOrArraySize = arraySize;
    this->MipLevels        = mipLevels;
    this->Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    this->Alignment        = 0;
    this->Format           = format;
    this->SampleDesc       = { 1, 0 };

    BuildViewDimensions();

    return *this;
}

 //=====================================================================================================================
 // Initialise builder as texture2D
D3D12_RESOURCE_DESC& ResourceBuilder::Texture2D(
    uint64_t width, uint32_t height, DXGI_FORMAT format, uint16_t arraySize, uint16_t mipLevels)
{
    ZeroMemory(this, sizeof(this));

    this->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);

    this->Width            = width;
    this->Height           = height;
    this->DepthOrArraySize = arraySize;
    this->MipLevels        = mipLevels;
    this->Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    this->Alignment        = 0;
    this->Format           = format;
    this->SampleDesc       = { 1, 0 };

    BuildViewDimensions();

    return *this;
}

//=====================================================================================================================
// Initialise builder as texture3D
D3D12_RESOURCE_DESC& ResourceBuilder::Texture3D(
    uint64_t width, uint32_t height, uint16_t depth, DXGI_FORMAT format, uint16_t mipLevels)
{
    ZeroMemory(this, sizeof(this));

    this->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);

    this->Width            = width;
    this->Height           = height;
    this->DepthOrArraySize = depth;
    this->MipLevels        = mipLevels;
    this->Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    this->Alignment        = 0;
    this->Format           = format;
    this->SampleDesc       = { 1, 0 };

    BuildViewDimensions();

    return *this;
}

//=====================================================================================================================
// Set builder resource format
D3D12_RESOURCE_DESC& ResourceBuilder::SetFormat(
    DXGI_FORMAT format)
{
    this->Format = format;
    return *this;
}

//=====================================================================================================================
// Returns builder resource description as a color target
D3D12_RESOURCE_DESC ResourceBuilder::AsColorTarget(
    bool allowUAV
    ) const
{
    D3D12_RESOURCE_DESC Value = *this;
    Value.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    if (allowUAV)
    {
        Value.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    return Value;
}

//=====================================================================================================================
// Returns builder resource description as a depth target
D3D12_RESOURCE_DESC ResourceBuilder::AsDepthTarget(
    bool allowSRV
    ) const
{
    D3D12_RESOURCE_DESC Value = *this;
    Value.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    if (allowSRV == false)
    {
        Value.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    }

    return Value;
}

//=====================================================================================================================
// Returns color target view
D3D12_RENDER_TARGET_VIEW_DESC ResourceBuilder::AsColorTargetView(
    DXGI_FORMAT viewFormat, uint32_t baseMip
    ) const
{
    D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
    ViewDesc.Format             = (IsTypeless(Format) ? viewFormat : Format);
    ViewDesc.ViewDimension      = RTVDimension;
    ViewDesc.Texture2D.MipSlice = baseMip;

    return ViewDesc;
}

//=====================================================================================================================
// Returns color target view for an array
D3D12_RENDER_TARGET_VIEW_DESC ResourceBuilder::AsColorTargetViewArray(
    DXGI_FORMAT viewFormat,
    uint16_t    baseMip,
    uint16_t    baseArray,
    uint16_t    arraySize
    ) const
{
    D3D12_RENDER_TARGET_VIEW_DESC ViewDesc = {};
    ViewDesc.Format                         = (IsTypeless(Format) ? viewFormat : Format);
    ViewDesc.ViewDimension                  = RTVDimensionArray;
    ViewDesc.Texture2DArray.MipSlice        = baseMip;
    ViewDesc.Texture2DArray.FirstArraySlice = baseArray;
    ViewDesc.Texture2DArray.ArraySize       = std::min(DepthOrArraySize, arraySize) - baseArray;

    return ViewDesc;
}

//=====================================================================================================================
// Returns depth stencil view
D3D12_DEPTH_STENCIL_VIEW_DESC ResourceBuilder::AsDepthStencilView(
    DXGI_FORMAT viewFormat, uint16_t baseMip
    ) const
{
    D3D12_DEPTH_STENCIL_VIEW_DESC ViewDesc = {};
    ViewDesc.Format             = (IsTypeless(Format) ? viewFormat : Format);
    ViewDesc.ViewDimension      = DSVDimension;
    ViewDesc.Texture2D.MipSlice = baseMip;

    return ViewDesc;
}

//=====================================================================================================================
// Returns depth stencil view for an array
D3D12_DEPTH_STENCIL_VIEW_DESC ResourceBuilder::AsDepthStencilViewArray(
    DXGI_FORMAT viewFormat,
    uint16_t    baseMip,
    uint16_t    baseArray,
    uint16_t    arraySize
    ) const
{
    D3D12_DEPTH_STENCIL_VIEW_DESC ViewDesc = {};
    ViewDesc.Format                         = (IsTypeless(Format) ? viewFormat : Format);
    ViewDesc.ViewDimension                  = DSVDimensionArray;
    ViewDesc.Texture2DArray.MipSlice        = baseMip;
    ViewDesc.Texture2DArray.FirstArraySlice = baseArray;
    ViewDesc.Texture2DArray.ArraySize       = std::min(DepthOrArraySize, arraySize) - baseArray;

    return ViewDesc;
}

//=====================================================================================================================
// Returns shader resource view
D3D12_SHADER_RESOURCE_VIEW_DESC ResourceBuilder::AsShaderResourceView(
    DXGI_FORMAT viewFormat,
    uint16_t    baseMip,
    uint16_t    mipLevels,
    float       minLod
    ) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc = {};
    ViewDesc.Format                        = (IsTypeless(Format) ? viewFormat : Format);
    ViewDesc.ViewDimension                 = SRVDimension;
    ViewDesc.Texture2D.MostDetailedMip     = baseMip;
    ViewDesc.Texture2D.MipLevels           = std::min(mipLevels, MipLevels) - baseMip;
    ViewDesc.Texture2D.ResourceMinLODClamp = minLod;
    ViewDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    return ViewDesc;
}

//=====================================================================================================================
// Returns shader resource view
D3D12_SHADER_RESOURCE_VIEW_DESC ResourceBuilder::AsBufferResourceView(
    uint32_t    firstElement,
    uint32_t    numElements,
    uint32_t    byteStride,
    DXGI_FORMAT viewFormat
    ) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc = {};
    ViewDesc.Format                        = viewFormat;
    ViewDesc.ViewDimension                 = D3D12_SRV_DIMENSION_BUFFER;
    ViewDesc.Buffer.FirstElement           = firstElement;
    ViewDesc.Buffer.NumElements            = numElements;
    ViewDesc.Buffer.StructureByteStride    = byteStride;

    if ((byteStride == 0) && (viewFormat == DXGI_FORMAT_UNKNOWN))
    {
        ViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
    }

    ViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    ///TODO: Clamp NumElements to resource Width / byteStride (assume 4-byte for raw buffers)
    return ViewDesc;
}

//=====================================================================================================================
ResourceBuilder& ResourceBuilder::SetHeapType(D3D12_HEAP_TYPE type)
{
    HeapProperties.Type = type;
    return *this;
}

//=====================================================================================================================
// Returns shader resource view for an array
D3D12_SHADER_RESOURCE_VIEW_DESC ResourceBuilder::AsShaderResourceViewArray(
    DXGI_FORMAT viewFormat,
    uint16_t    baseMip,
    uint16_t    baseArray,
    uint16_t    mipLevels,
    uint16_t    arraySize,
    float       minLod
    ) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc = {};
    ViewDesc.Format                             = (IsTypeless(Format) ? viewFormat : Format);
    ViewDesc.ViewDimension                      = SRVDimensionArray;
    ViewDesc.Texture2DArray.MostDetailedMip     = baseMip;
    ViewDesc.Texture2DArray.MipLevels           = std::min(mipLevels, MipLevels) - baseMip;
    ViewDesc.Texture2DArray.FirstArraySlice     = baseArray;
    ViewDesc.Texture2DArray.ArraySize           = std::min(DepthOrArraySize, arraySize) - baseArray;
    ViewDesc.Texture2DArray.ResourceMinLODClamp = minLod;
    ViewDesc.Shader4ComponentMapping            = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    return ViewDesc;
}

//=====================================================================================================================
// Internal helper function
void ResourceBuilder::BuildViewDimensions()
{
    if (Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    {
        RTVDimension = D3D12_RTV_DIMENSION_BUFFER;
        DSVDimension = D3D12_DSV_DIMENSION_UNKNOWN; // Not supported
        SRVDimension = D3D12_SRV_DIMENSION_BUFFER;
    }
    else if (Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D)
    {
        RTVDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
        DSVDimension = D3D12_DSV_DIMENSION_UNKNOWN; // Not supported
        SRVDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
    }
    else if (Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
    {
        RTVDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
        DSVDimension = D3D12_DSV_DIMENSION_UNKNOWN; // Not supported
        SRVDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
    }
    else // D3D12_RESOURCE_DIMENSION_TEXTURE2D
    {
        if (SampleDesc.Count > 1)
        {
            RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            DSVDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
            SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;

            RTVDimensionArray =
                (DepthOrArraySize == 1) ? D3D12_RTV_DIMENSION_TEXTURE2DMS : D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
            DSVDimensionArray =
                (DepthOrArraySize == 1) ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
            SRVDimensionArray =
                (DepthOrArraySize == 1) ? D3D12_SRV_DIMENSION_TEXTURE2DMS : D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
        }
        else
        {
            RTVDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            DSVDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            SRVDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

            RTVDimensionArray =
                (DepthOrArraySize == 1) ? D3D12_RTV_DIMENSION_TEXTURE2D : D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            DSVDimensionArray =
                (DepthOrArraySize == 1) ? D3D12_DSV_DIMENSION_TEXTURE2D : D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            SRVDimensionArray =
                (DepthOrArraySize == 1) ? D3D12_SRV_DIMENSION_TEXTURE2D : D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        }
    }
}
} // AR