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
#include "CPUTInputLayoutCacheOGL.h"

CPUTInputLayoutCacheOGL* CPUTInputLayoutCacheOGL::mpInputLayoutCache = NULL;

CPUTInputLayoutCache* CPUTInputLayoutCache::GetInputLayoutCache()
{
    return CPUTInputLayoutCacheOGL::GetInputLayoutCache();
}
void CPUTInputLayoutCache::DeleteInputLayoutCache()
{
    CPUTInputLayoutCacheOGL::DeleteInputLayoutCache();
}

//-----------------------------------------------------------------------------
void CPUTInputLayoutCacheOGL::ClearLayoutCache()
{
}

CPUTInputLayoutCacheOGL* CPUTInputLayoutCacheOGL::GetInputLayoutCache()
{
    if (NULL == mpInputLayoutCache)
    {
        mpInputLayoutCache = new CPUTInputLayoutCacheOGL();
    }
    return mpInputLayoutCache;
}

// singleton destroy routine
//-----------------------------------------------------------------------------
CPUTResult CPUTInputLayoutCacheOGL::DeleteInputLayoutCache()
{
    SAFE_DELETE(mpInputLayoutCache);
    return CPUT_SUCCESS;
}

void GetInputLayoutCache()
{

}
void CPUTInputLayoutCacheOGL::Apply(CPUTMesh* pMesh, CPUTMaterial* pMaterial)
{
 /*   D3D11_INPUT_ELEMENT_DESC *pDXLayout = ((CPUTMeshDX11*)pMesh)->GetLayoutDescription();
    CPUTVertexShaderDX11 *pVertexShader = ((CPUTMaterialDX11*)pMaterial)->GetVertexShader();
    ID3D11InputLayout *pInputLayout;

    if (CPUT_SUCCESS == GetLayout(pDevice, pDXLayout, pVertexShader, &pInputLayout))
    {
        CPUT_DX11::GetContext()->IASetInputLayout(pInputLayout);
    }*/
}
//-----------------------------------------------------------------------------
