/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

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