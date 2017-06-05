/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or imlied.
// See the License for the specific language governing permissions and
// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CPUTBUFFERDX11_H
#define _CPUTBUFFERDX11_H

#include "CPUTBuffer.h"
#include "CPUT_DX11.h"

//--------------------------------------------------------------------------------------
class CPUTBufferDX11 : public CPUTBuffer
{
private:
    // resource view pointer
    ID3D11ShaderResourceView  *mpShaderResourceView;
    ID3D11UnorderedAccessView *mpUnorderedAccessView;
    ID3D11Buffer              *mpBuffer;

    // Destructor is not public.  Must release instead of delete.
    ~CPUTBufferDX11() {
        SAFE_RELEASE(mpShaderResourceView);
        SAFE_RELEASE(mpUnorderedAccessView);
        SAFE_RELEASE(mpBuffer);
    }

public:
	void ReleaseShaderResourceView()
	{
		SAFE_RELEASE(mpShaderResourceView);
	}
	void ReleaseUnorderedAccessView()
	{
		SAFE_RELEASE(mpUnorderedAccessView);
	}
	void ReleaseNativeBuffer()
	{
		SAFE_RELEASE(mpBuffer);
	}
    ID3D11ShaderResourceView *GetShaderResourceView()
    {
        return mpShaderResourceView;
    }

    ID3D11UnorderedAccessView *GetUnorderedAccessView()
    {
        return mpUnorderedAccessView;
    }

    void SetShaderResourceView(ID3D11ShaderResourceView *pShaderResourceView)
    {
        SAFE_RELEASE(mpShaderResourceView);
        mpShaderResourceView = pShaderResourceView;
        mpShaderResourceView->AddRef();
    }
    void SetUnorderedAccessView(ID3D11UnorderedAccessView *pUnorderedAccessView)
    {
        SAFE_RELEASE(mpUnorderedAccessView);
        mpUnorderedAccessView = pUnorderedAccessView;
		if ( mpUnorderedAccessView != NULL )
			mpUnorderedAccessView->AddRef();
    }
    void SetBuffer(ID3D11Buffer *pBuffer)
    {
        SAFE_RELEASE(mpBuffer);
        mpBuffer = pBuffer;
        if (mpBuffer) 
            mpBuffer->AddRef();
    }

    ID3D11Buffer *GetNativeBuffer() { return mpBuffer; };

    void SetData(UINT offset, UINT sizeBytes, void* pData);
    void GetData(void* pData);

    static CPUTBuffer* Create(std::string &name, CPUTBufferDesc* pDesc);

private:
    CPUTBufferDX11(std::string &name, CPUTBufferDesc* pDesc);
};
#endif //_CPUTBUFFERDX11_H

