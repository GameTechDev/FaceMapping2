//--------------------------------------------------------------------------------------
// Copyright 2013 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

#include "CPUTBufferDX11.h"

void CPUTBufferDX11::SetData(UINT offset, UINT sizeBytes, void* pData)
{
    ID3D11DeviceContext* pContext = CPUT_DX11::GetContext();
    D3D11_MAPPED_SUBRESOURCE mapData;
    HRESULT hr = pContext->Map(mpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapData);
    if (SUCCEEDED(hr))
    {
        if (mapData.RowPitch >= offset + sizeBytes)
        {
            memcpy((BYTE*)mapData.pData + offset, (BYTE*)pData, sizeBytes);
        }
        pContext->Unmap(mpBuffer, 0);
    }
    return;
}

CPUTBuffer* CPUTBufferDX11::Create(std::string &name, CPUTBufferDesc *pData)
{
    return new CPUTBufferDX11(name, pData);
}

CPUTBuffer* CPUTBuffer::Create(std::string &name, CPUTBufferDesc *pData)
{
    return CPUTBufferDX11::Create(name, pData);
}

CPUTBufferDX11::CPUTBufferDX11(std::string &name, CPUTBufferDesc *pDesc)
    : CPUTBuffer(name, pDesc)
{
    mpShaderResourceView = NULL;
    mpUnorderedAccessView = NULL;
    mpBuffer = NULL;

    if (pDesc != NULL)
    {
        D3D11_BUFFER_DESC bd = { 0 };
        bd.ByteWidth = pDesc->sizeBytes;
        bd.BindFlags = pDesc->target == BUFFER_VERTEX ? D3D11_BIND_VERTEX_BUFFER
            : pDesc->target == BUFFER_INDEX ? D3D11_BIND_INDEX_BUFFER
            : pDesc->target == BUFFER_UNIFORM ? D3D11_BIND_CONSTANT_BUFFER
            : NULL;
        bd.Usage = pDesc->memory == BUFFER_IMMUTABLE ? D3D11_USAGE_IMMUTABLE
            : pDesc->memory == BUFFER_DYNAMIC || pDesc->memory == BUFFER_STREAMING ? D3D11_USAGE_DYNAMIC
            : pDesc->memory == BUFFER_STAGING ? D3D11_USAGE_STAGING
            : D3D11_USAGE_DEFAULT;
        if ((pDesc->cpuAccess & BUFFER_CPU_READ) != 0)
            bd.CPUAccessFlags += D3D11_CPU_ACCESS_READ;
        if ((pDesc->cpuAccess & BUFFER_CPU_WRITE) != 0)
            bd.CPUAccessFlags += D3D11_CPU_ACCESS_WRITE;

        ID3D11Device* pDevice = CPUT_DX11::GetDevice();
        HRESULT hr = S_OK;
        hr = pDevice->CreateBuffer(&bd, NULL, &mpBuffer);
        ASSERT(!FAILED(hr), "Error creating constant buffer.");
        CPUTSetDebugName(mpBuffer, mName);
    }
}

void CPUTBufferDX11::GetData(void* pData)
{
    ID3D11DeviceContext* pContext = CPUT_DX11::GetContext();
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    UINT flags = 0;
    pContext->Map(mpBuffer, 0, D3D11_MAP_READ, flags, &mappedResource);
    memcpy(pData, mappedResource.pData, mappedResource.RowPitch);
    pContext->Unmap(mpBuffer, 0);
}