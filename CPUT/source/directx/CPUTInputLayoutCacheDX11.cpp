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
#include "CPUTInputLayoutCacheDX11.h"
#include "CPUTVertexShaderDX11.h"
#include "CPUT_DX11.h"
#include "CPUTMaterialDX11.h"
#include "CPUTMeshDX11.h"
#include <D3D11.h> // D3D11_INPUT_ELEMENT_DESC
extern const std::string *gpDXGIFormatNames;

CPUTInputLayoutCacheDX11* CPUTInputLayoutCacheDX11::mpInputLayoutCache = NULL;
//-----------------------------------------------------------------------------
CPUTInputLayoutCache* CPUTInputLayoutCache::GetInputLayoutCache()
{
    return CPUTInputLayoutCacheDX11::GetInputLayoutCache();
}
//-----------------------------------------------------------------------------
void CPUTInputLayoutCache::DeleteInputLayoutCache()
{
    CPUTInputLayoutCacheDX11::DeleteInputLayoutCache();
}

//-----------------------------------------------------------------------------
void CPUTInputLayoutCacheDX11::ClearLayoutCache()
{
    // iterate over the entire map - and release each layout object
    std::map<LayoutKey, ID3D11InputLayout*>::iterator mapIterator;

    for(mapIterator = mLayoutList.begin(); mapIterator != mLayoutList.end(); mapIterator++)
    {
        mapIterator->second->Release();  
    }
    mLayoutList.clear();
}

// singleton retriever

CPUTInputLayoutCacheDX11* CPUTInputLayoutCacheDX11::GetInputLayoutCache()
{
    if (NULL == mpInputLayoutCache)
    {
        mpInputLayoutCache = new CPUTInputLayoutCacheDX11();
    }
    return mpInputLayoutCache;
}

// singleton destroy routine
//-----------------------------------------------------------------------------
CPUTResult CPUTInputLayoutCacheDX11::DeleteInputLayoutCache()
{
    if(mpInputLayoutCache)
    {
        delete mpInputLayoutCache;
        mpInputLayoutCache = NULL;
    }
    return CPUT_SUCCESS;
}

void CPUTInputLayoutCacheDX11::Apply(CPUTMesh* pMesh, CPUTMaterial* pMaterial)
{
    ID3D11Device* pDevice = CPUT_DX11::GetDevice();
    D3D11_INPUT_ELEMENT_DESC *pDXLayout = ((CPUTMeshDX11*)pMesh)->GetLayoutDescription();
    CPUTVertexShaderDX11 *pVertexShader = ((CPUTMaterialDX11*)pMaterial)->GetVertexShader();
    ID3D11InputLayout *pInputLayout;
    
    if (CPUT_SUCCESS == GetLayout(pDevice, pDXLayout, pVertexShader, &pInputLayout))
    {
        CPUT_DX11::GetContext()->IASetInputLayout(pInputLayout);
        pInputLayout->Release();
    }
}
// find existing, or create new, ID3D11InputLayout layout
//-----------------------------------------------------------------------------
CPUTResult CPUTInputLayoutCacheDX11::GetLayout(
    ID3D11Device *pDevice,
    D3D11_INPUT_ELEMENT_DESC *pDXLayout,
    CPUTVertexShaderDX11 *pVertexShader,
    ID3D11InputLayout **ppInputLayout
){
    // Generate the vertex layout key
    LayoutKey layoutKey(pDXLayout, pVertexShader, true);

    // Do we already have one like this?
    if( mLayoutList[layoutKey] )
    {
        *ppInputLayout = mLayoutList[layoutKey];
        (*ppInputLayout)->AddRef();
        return CPUT_SUCCESS;
    }
    // Not found, create a new ID3D11InputLayout object

    // How many elements are in the input layout?
    int numInputLayoutElements=0;
    while(NULL != pDXLayout[numInputLayoutElements].SemanticName)
    {
        numInputLayoutElements++;
    }
    // Create the input layout
    HRESULT hr;
    ID3DBlob *pBlob = pVertexShader->GetBlob();
    hr = pDevice->CreateInputLayout( pDXLayout, numInputLayoutElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), ppInputLayout );
    if (!SUCCEEDED(hr))
    {
        ID3D11ShaderReflection *pReflector = NULL;

        D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
        // Walk through the shader input bind descriptors.
        // If any of them begin with '@', then we need a unique material per model (i.e., we need to clone the material).
        int ii = 0;
        D3D11_SIGNATURE_PARAMETER_DESC desc;
        int i = 0;
        DEBUG_PRINT("Shader Inputs\n");
        while (SUCCEEDED(pReflector->GetInputParameterDesc(i, &desc)))
        {
            DEBUG_PRINT("\t%s\n", desc.SemanticName);
            i++;
        }
        DEBUG_PRINT("Mesh Inputs\n");
        for (int i = 0; i < numInputLayoutElements; i++)
        {
            DEBUG_PRINT("\t%s\n", pDXLayout[i].SemanticName);
        }
        DEBUG_PRINT("\n");
    }
    ASSERT( SUCCEEDED(hr), "Error creating input layout." );
    CPUTSetDebugName( *ppInputLayout, "CPUTInputLayoutCacheDX11::GetLayout()" );

    // Store this layout object in our map
    mLayoutList[layoutKey] = *ppInputLayout;

    // Addref for storing it in our map as well as returning it (count should be = 2 at this point)
    (*ppInputLayout)->AddRef();

    return CPUT_SUCCESS;
}

//-----------------------------------------------------------------------------
CPUTInputLayoutCacheDX11::LayoutKey::LayoutKey()
    : layout(NULL), vs(NULL), nElems(0), layout_owned(false)
{
}

CPUTInputLayoutCacheDX11::LayoutKey::LayoutKey(const D3D11_INPUT_ELEMENT_DESC *pDXLayout, void *vs, bool just_ref)
{
    nElems = 0;
    while (pDXLayout[nElems].SemanticName)
    {
        ++nElems;
    }

    if (just_ref)
    {
        layout = pDXLayout;
        this->vs = vs;
        layout_owned = false;
    }
    else
    {
        D3D11_INPUT_ELEMENT_DESC *copy = new D3D11_INPUT_ELEMENT_DESC[nElems];
        memcpy(copy, pDXLayout, nElems * sizeof(*copy));
        layout = copy;
        this->vs = vs;
        layout_owned = true;
    }
}

CPUTInputLayoutCacheDX11::LayoutKey::LayoutKey(const LayoutKey &x)
{
    D3D11_INPUT_ELEMENT_DESC *copy = new D3D11_INPUT_ELEMENT_DESC[x.nElems];
    memcpy(copy, x.layout, x.nElems * sizeof(*copy));
    layout = copy;
    vs = x.vs;
    nElems = x.nElems;
    layout_owned = true;
}

CPUTInputLayoutCacheDX11::LayoutKey::~LayoutKey()
{
    if (layout_owned)
    {
        SAFE_DELETE_ARRAY(layout);
    }
}

CPUTInputLayoutCacheDX11::LayoutKey &CPUTInputLayoutCacheDX11::LayoutKey::operator =(const LayoutKey &x)
{
    LayoutKey copy(x);
    std::swap(layout, copy.layout);
    std::swap(vs, copy.vs);
    std::swap(nElems, copy.nElems);
    std::swap(layout_owned, copy.layout_owned);
    return *this;
}
