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
#ifndef __CPUTMESHDX11_H__
#define __CPUTMESHDX11_H__

#pragma once

#include "CPUTMesh.h"
#include "CPUTInputLayoutCacheDX11.h"
#include "CPUT.h"
#include "CPUTOSServices.h"

class CPUTMaterial;
class CPUTMaterialDX11;
class CPUTComputeShaderDX11;
class CPUTModelDX11;

//-----------------------------------------------------------------------------
class CPUTMeshDX11 : public CPUTMesh
{
protected:
    CPUTMeshDX11();

    D3D_PRIMITIVE_TOPOLOGY    mD3DMeshTopology;
    D3D11_INPUT_ELEMENT_DESC *mpLayoutDescription;
    int                       mNumberOfInputLayoutElements;

    UINT                      mVertexStride;

    D3D11_BUFFER_DESC         mVertexBufferDesc;
    UINT                      mVertexBufferOffset;
    UINT                      mVertexCount;
    ID3D11Buffer             *mpVertexBuffer;
    ID3D11Buffer             *mpStagingVertexBuffer;
    eCPUTMapType              mVertexBufferMappedType;
    ID3D11Buffer             *mpVertexBufferForSRVDX; // Need SRV, but _real_ DX won't allow for _real_ VB
    ID3D11ShaderResourceView *mpVertexView;

    UINT                      mIndexCount;
    DXGI_FORMAT               mIndexBufferFormat;
    ID3D11Buffer             *mpIndexBuffer;
    D3D11_BUFFER_DESC         mIndexBufferDesc;
    ID3D11Buffer             *mpStagingIndexBuffer;
    eCPUTMapType              mIndexBufferMappedType;

public:
	static CPUTMeshDX11* Create();
    virtual ~CPUTMeshDX11();
	
    D3D11_INPUT_ELEMENT_DESC *GetLayoutDescription() { return mpLayoutDescription; }
    ID3D11Buffer             *GetIndexBuffer()  { return mpIndexBuffer; }
    ID3D11Buffer             *GetVertexBuffer() { return mpVertexBuffer; }
	DXGI_FORMAT				  GetIndexFormat() { return mIndexBufferFormat; }
    void                      SetMeshTopology(const eCPUT_MESH_TOPOLOGY eDrawTopology);
    CPUTResult                CreateNativeResources(
                                  CPUTModel             *pModel,
                                  UINT                   meshIdx,
                                  int                    vertexElementCount,
                                  CPUTBufferElementInfo *pVertexInfo,
                                  UINT                   vertexCount,
                                  void                  *pVertexData,
                                  CPUTBufferElementInfo *pIndexInfo,
                                  UINT                   indexCount,
                                  void                  *pIndex
                              );
    void                      Draw();

    D3D11_MAPPED_SUBRESOURCE  MapVertices(   CPUTRenderParameters &params, eCPUTMapType type, bool wait=true );
    D3D11_MAPPED_SUBRESOURCE  MapIndices(    CPUTRenderParameters &params, eCPUTMapType type, bool wait=true );
	virtual void FreeStagingIndexBuffer();
	virtual void FreeStagingVertexBuffer();
    void                      UnmapVertices( CPUTRenderParameters &params );
    void                      UnmapIndices(  CPUTRenderParameters &params );
    UINT                      GetTriangleCount() { return mIndexCount/3; }
    UINT                      GetVertexCount() { return mVertexCount; }
    UINT                      GetIndexCount()  { return mIndexCount; }
    void                      SetNumVertices(int numVertices) { this->mVertexCount = numVertices; }
	void                      UpdateVerts(void *vertData, int size);
    UINT                      GetVertexStride() { return mVertexStride; }

	void                      UpdateVertCountAndFormat(int count, D3D11_INPUT_ELEMENT_DESC *format, int stride);

	void CopyIndexBufferFrom(CPUTMeshDX11 *srcMesh);
	// Make a deep copy of srcMesh
	void CopyFrom(CPUTMeshDX11 *srcMesh);
	//void UpdateVertexDataAndFormat(D3D11_INPUT_ELEMENT_DESC *format, void *vertData, int vertCount);

	bool GetVertexData(D3D11_INPUT_ELEMENT_DESC *format, int formatCount, void *destVertices, int destStride, int srcVertexOffset, int vertexCount);
	bool UpdateVertexData(D3D11_INPUT_ELEMENT_DESC *format, const void *srcVertices, int srcStride, int vertexOffset, int vertexCount);

	void SetIndexBufferData(uint32 *buffer, int indexCount);
	
		
protected:

	void CreateIndexBuffer(DXGI_FORMAT format, void *initialData, int count, bool force);

    // Mapping vertex and index buffers is very similar.  This internal function does both
    D3D11_MAPPED_SUBRESOURCE Map(
        UINT                   count,
        ID3D11Buffer          *pBuffer,
        D3D11_BUFFER_DESC     &bufferDesc,
        ID3D11Buffer         **pStagingBuffer,
        eCPUTMapType          *pMappedType,
        CPUTRenderParameters  &params,
        eCPUTMapType           type,
        bool                   wait = true
    );
    void  Unmap(
        ID3D11Buffer         *pBuffer,
        ID3D11Buffer         *pStagingBuffer,
        eCPUTMapType         *pMappedType,
        CPUTRenderParameters &params
    );
    void ClearAllObjects(); // delete all allocations held by this object
    DXGI_FORMAT ConvertToDirectXFormat(CPUT_DATA_FORMAT_TYPE DataFormatElementType, int NumberDataFormatElements);

	
};

#endif // __CPUTMESHDX11_H__
