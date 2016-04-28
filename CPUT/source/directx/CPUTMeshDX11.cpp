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
#include "CPUT_DX11.h"
#include "CPUTMeshDX11.h"
#include "CPUTMaterialDX11.h"
#include "CPUTBufferDX11.h"
#include "CPUTRenderParams.h"

//-----------------------------------------------------------------------------
CPUTMeshDX11::CPUTMeshDX11():
    mVertexStride(0),
    mVertexBufferOffset(0),
    mVertexCount(0),
    mpVertexBuffer(NULL),
    mpVertexBufferForSRVDX(NULL),
    mpVertexView(NULL),
    mpStagingVertexBuffer(NULL),
    mVertexBufferMappedType(CPUT_MAP_UNDEFINED),
    mIndexBufferMappedType(CPUT_MAP_UNDEFINED),
    mIndexCount(0),
    mIndexBufferFormat(DXGI_FORMAT_UNKNOWN),
    mpIndexBuffer(NULL),
    mpStagingIndexBuffer(NULL),
    mNumberOfInputLayoutElements(0),
    mpLayoutDescription(NULL)
{
}

//-----------------------------------------------------------------------------
CPUTMeshDX11::~CPUTMeshDX11()
{
    ClearAllObjects();
}

//-----------------------------------------------------------------------------
void CPUTMeshDX11::ClearAllObjects()
{
    SAFE_RELEASE(mpStagingIndexBuffer);
    SAFE_RELEASE(mpIndexBuffer);
    SAFE_RELEASE(mpStagingVertexBuffer);
    SAFE_RELEASE(mpVertexBuffer);
    SAFE_RELEASE(mpVertexBufferForSRVDX);
    SAFE_RELEASE(mpVertexView);
    SAFE_DELETE_ARRAY(mpLayoutDescription);
}

// Create the DX vertex/index buffers and D3D11_INPUT_ELEMENT_DESC
//-----------------------------------------------------------------------------
CPUTResult CPUTMeshDX11::CreateNativeResources(
    CPUTModel              *pModel,
    UINT                    meshIdx,
    int                     vertexElementCount,
    CPUTBufferElementInfo  *pVertexDataInfo,
    UINT                    vertexCount,
    void                   *pVertexData,
    CPUTBufferElementInfo  *pIndexDataInfo,
    UINT                    indexCount,
    void                   *pIndexData
){

    CPUTResult result = CPUT_SUCCESS;
    HRESULT hr;

    ID3D11Device *pD3dDevice = CPUT_DX11::GetDevice();

    // Release the layout, offset, stride, and vertex buffer structure objects
    ClearAllObjects();

    // allocate the layout, offset, stride, and vertex buffer structure objects
    mpLayoutDescription = new D3D11_INPUT_ELEMENT_DESC[vertexElementCount+1];

    // Create the index buffer
    D3D11_SUBRESOURCE_DATA resourceData;
    if(NULL!=pIndexData)
    {
        mIndexCount = indexCount;

        // set the data format info
        ZeroMemory( &mIndexBufferDesc, sizeof(mIndexBufferDesc) );
        mIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        mIndexBufferDesc.ByteWidth = mIndexCount * pIndexDataInfo->mElementSizeInBytes;
        mIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        mIndexBufferDesc.CPUAccessFlags = 0;  // default to no cpu access for speed

        // create the buffer
        ZeroMemory( &resourceData, sizeof(resourceData) );
        resourceData.pSysMem = pIndexData;
        hr = pD3dDevice->CreateBuffer( &mIndexBufferDesc, &resourceData, &mpIndexBuffer );
        ASSERT(!FAILED(hr), "Failed creating index buffer" );
        CPUTSetDebugName( mpIndexBuffer, "Index buffer" );

        // set the DX index buffer format
        mIndexBufferFormat = ConvertToDirectXFormat(pIndexDataInfo->mElementType, pIndexDataInfo->mElementComponentCount);
    }

    // Create the vertex buffer
    mVertexCount = vertexCount;
    ZeroMemory( &mVertexBufferDesc, sizeof(mVertexBufferDesc) );
    mVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    mVertexStride = pVertexDataInfo[vertexElementCount-1].mOffset + pVertexDataInfo[vertexElementCount-1].mElementSizeInBytes; // size in bytes of a single vertex block
    mVertexBufferDesc.ByteWidth = mVertexCount * mVertexStride; // size in bytes of entire buffer
    mVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    mVertexBufferDesc.CPUAccessFlags = 0;  // default to no cpu access for speed

    ZeroMemory( &resourceData, sizeof(resourceData) );
    resourceData.pSysMem = pVertexData;
    if (pVertexData) {
        hr = pD3dDevice->CreateBuffer( &mVertexBufferDesc, &resourceData, &mpVertexBuffer );
    } else {
        hr = pD3dDevice->CreateBuffer( &mVertexBufferDesc, NULL, &mpVertexBuffer );
    }
    ASSERT( !FAILED(hr), "Failed creating vertex buffer" );
    CPUTSetDebugName( mpVertexBuffer, "Vertex buffer" );

    // create the buffer for the shader resource view
    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth           = mVertexCount * mVertexStride; // size in bytes of entire buffer
    desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags      = 0;
    desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = mVertexStride;

    if (pVertexData) {
        hr = pD3dDevice->CreateBuffer( &desc, &resourceData, &mpVertexBufferForSRVDX );
    } else {
        hr = pD3dDevice->CreateBuffer( &desc, NULL, &mpVertexBufferForSRVDX );
    }
    ASSERT( !FAILED(hr), "Failed creating vertex buffer for SRV" );
    CPUTSetDebugName( mpVertexBuffer, "Vertex buffer for SRV" );

    // Create the shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;  
    ZeroMemory( &srvDesc, sizeof(srvDesc) );  
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;  
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;  
    srvDesc.Buffer.ElementOffset = 0;  
    srvDesc.Buffer.NumElements = mVertexCount;

    hr = pD3dDevice->CreateShaderResourceView( mpVertexBufferForSRVDX, &srvDesc, &mpVertexView );
    ASSERT( !FAILED(hr), "Failed creating vertex buffer SRV" );

    // build the layout object
    int currentByteOffset=0;
    mNumberOfInputLayoutElements = vertexElementCount;
    for(int ii=0; ii<vertexElementCount; ii++)
    {
        mpLayoutDescription[ii].SemanticName  = pVertexDataInfo[ii].mpSemanticName; // string name that matches
        mpLayoutDescription[ii].SemanticIndex = pVertexDataInfo[ii].mSemanticIndex; // if we have more than one
        mpLayoutDescription[ii].Format = ConvertToDirectXFormat(pVertexDataInfo[ii].mElementType, pVertexDataInfo[ii].mElementComponentCount);
        mpLayoutDescription[ii].InputSlot = 0; // TODO: We support only a single stream now.  Support multiple streams.
        mpLayoutDescription[ii].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        mpLayoutDescription[ii].InstanceDataStepRate = 0;
        mpLayoutDescription[ii].AlignedByteOffset = currentByteOffset;
        currentByteOffset += pVertexDataInfo[ii].mElementSizeInBytes;
    }
    // set the last 'dummy' element to null.  Not sure if this is required, as we also pass in count when using this list.
    memset( &mpLayoutDescription[vertexElementCount], 0, sizeof(D3D11_INPUT_ELEMENT_DESC) );

    return result;
}

//-----------------------------------------------------------------------------
void CPUTMeshDX11::Draw()
{
    // Skip empty meshes.
    if( mIndexCount == 0 && mVertexCount == 0)
     return; 


    ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();

    pContext->IASetPrimitiveTopology( mD3DMeshTopology );
    pContext->IASetVertexBuffers(0, 1, &mpVertexBuffer, &mVertexStride, &mVertexBufferOffset);

    if (mIndexCount) 
    {
        pContext->IASetIndexBuffer(mpIndexBuffer, mIndexBufferFormat, 0);
        pContext->DrawIndexed( mIndexCount, 0, 0 );
    }
    else 
    {
        pContext->Draw( mVertexCount, 0 );
    }
}

// Sets the mesh topology, and converts it to it's DX format
//-----------------------------------------------------------------------------
void CPUTMeshDX11::SetMeshTopology(const eCPUT_MESH_TOPOLOGY meshTopology)
{
    //ASSERT( meshTopology > 0 && meshTopology <= 5, "" );
    CPUTMesh::SetMeshTopology(meshTopology);
    // The CPUT enum has the same values as the D3D enum.  Will likely need an xlation on OpenGL.
    mD3DMeshTopology = (D3D_PRIMITIVE_TOPOLOGY)meshTopology;
}

// Translate an internal CPUT data type into it's equivalent DirectX type
//-----------------------------------------------------------------------------
DXGI_FORMAT CPUTMeshDX11::ConvertToDirectXFormat(CPUT_DATA_FORMAT_TYPE dataFormatType, int componentCount)
{
    ASSERT( componentCount>0 && componentCount<=4, "Invalid vertex element count." );
    switch( dataFormatType )
    {
    case CPUT_F32:
    {
        const DXGI_FORMAT componentCountToFormat[4] = {
            DXGI_FORMAT_R32_FLOAT,
            DXGI_FORMAT_R32G32_FLOAT,
            DXGI_FORMAT_R32G32B32_FLOAT,
            DXGI_FORMAT_R32G32B32A32_FLOAT
        };
        return componentCountToFormat[componentCount-1];
    }
    case CPUT_U32:
    {
        const DXGI_FORMAT componentCountToFormat[4] = {
            DXGI_FORMAT_R32_UINT,
            DXGI_FORMAT_R32G32_UINT,
            DXGI_FORMAT_R32G32B32_UINT,
            DXGI_FORMAT_R32G32B32A32_UINT
        };
        return componentCountToFormat[componentCount-1];
        break;
    }
    case CPUT_U16:
    {
        ASSERT( 3 != componentCount, "Invalid vertex element count." );
        const DXGI_FORMAT componentCountToFormat[4] = {
            DXGI_FORMAT_R16_UINT,
            DXGI_FORMAT_R16G16_UINT,
            DXGI_FORMAT_UNKNOWN, // Count of 3 is invalid for 16-bit type
            DXGI_FORMAT_R16G16B16A16_UINT
        };
        return componentCountToFormat[componentCount-1];
    }
	case CPUT_U8:
		{
			ASSERT( 3 != componentCount, "Invalid vertex element count." );
			const DXGI_FORMAT componentCountToFormat[4] = {
				DXGI_FORMAT_R8_UINT,
				DXGI_FORMAT_R8G8_UINT,
				DXGI_FORMAT_UNKNOWN, // Count of 3 is invalid for 8-bit type
				DXGI_FORMAT_R8G8B8A8_UINT
			};
			return componentCountToFormat[componentCount-1];
		}
    default:
    {
        // todo: add all the other data types you want to support
        ASSERT(0,"Unsupported vertex element type");
    }
    return DXGI_FORMAT_UNKNOWN;
    }
} // CPUTMeshDX11::ConvertToDirectXFormat()

//-----------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE CPUTMeshDX11::Map(
    UINT                   count,
    ID3D11Buffer          *pBuffer,
    D3D11_BUFFER_DESC     &bufferDesc,
    ID3D11Buffer         **pStagingBuffer,
    eCPUTMapType          *pMappedType,
    CPUTRenderParameters  &params,
    eCPUTMapType           type,
    bool                   wait
)
{
    // Mapping for DISCARD requires dynamic buffer.  Create dynamic copy?
    // Could easily provide input flag.  But, where would we specify? Don't like specifying in the .set file
    // Because mapping is something the application wants to do - it isn't inherent in the data.
    // Could do Clone() and pass dynamic flag to that.
    // But, then we have two.  Could always delete the other.
    // Could support programatic flag - apply to all loaded models in the .set
    // Could support programatic flag on model.  Load model first, then load set.
    // For now, simply support CopyResource mechanism.
    HRESULT hr;
    ID3D11Device *pD3dDevice = CPUT_DX11::GetDevice();
    ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();

	if (*pStagingBuffer && *pMappedType != type)
	{
		SAFE_RELEASE(*pStagingBuffer);
	}

    if( !*pStagingBuffer )
    {
        D3D11_BUFFER_DESC desc = bufferDesc;
        // First time.  Create the staging resource
        desc.Usage = D3D11_USAGE_STAGING;
        switch( type )
        {
        case CPUT_MAP_READ:
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.BindFlags = 0;
            break;
        case CPUT_MAP_READ_WRITE:
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            desc.BindFlags = 0;
            break;
        case CPUT_MAP_WRITE:
        case CPUT_MAP_WRITE_DISCARD:
        case CPUT_MAP_NO_OVERWRITE:
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.BindFlags = 0;
            break;
        };
        hr = pD3dDevice->CreateBuffer( &desc, NULL, pStagingBuffer );
        ASSERT( SUCCEEDED(hr), "Failed to create staging buffer" );
        CPUTSetDebugName( *pStagingBuffer, "Mesh Staging buffer" );
    }
    else
    {
        ASSERT( *pMappedType == type, "Mapping with a different CPU access than creation parameter." );
    }

    D3D11_MAPPED_SUBRESOURCE info;
    switch( type )
    {
    case CPUT_MAP_READ:
    case CPUT_MAP_READ_WRITE:
        // TODO: Copying and immediately mapping probably introduces a stall.
        // Expose the copy externally?
        // TODO: copy only if vb has changed?
        // Copy only first time?
        // Copy the GPU version before we read from it.
        pContext->CopyResource( *pStagingBuffer, pBuffer );
        break;
    };
    hr = pContext->Map( *pStagingBuffer, wait ? 0 : D3D11_MAP_FLAG_DO_NOT_WAIT, (D3D11_MAP)type, 0, &info );
    *pMappedType = type;
    return info;
} // CPUTMeshDX11::Map()

//-----------------------------------------------------------------------------
void  CPUTMeshDX11::Unmap(
    ID3D11Buffer         *pBuffer,
    ID3D11Buffer         *pStagingBuffer,
    eCPUTMapType         *pMappedType,
    CPUTRenderParameters &params
)
{
    ASSERT( *pMappedType != CPUT_MAP_UNDEFINED, "Can't unmap a buffer that isn't mapped." );

    ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();

    pContext->Unmap( pStagingBuffer, 0 );

    // If we were mapped for write, then copy staging buffer to GPU
    switch( *pMappedType )
    {
    case CPUT_MAP_READ:
        break;
    case CPUT_MAP_READ_WRITE:
    case CPUT_MAP_WRITE:
    case CPUT_MAP_WRITE_DISCARD:
    case CPUT_MAP_NO_OVERWRITE:
        pContext->CopyResource( pBuffer, pStagingBuffer );
        break;
    };

} // CPUTMeshDX11::Unmap()


//-----------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE CPUTMeshDX11::MapVertices( CPUTRenderParameters &params, eCPUTMapType type, bool wait )
{
    return Map(
        mVertexCount,
        mpVertexBuffer,
        mVertexBufferDesc,
       &mpStagingVertexBuffer,
       &mVertexBufferMappedType,
        params,
        type,
        wait
    );
}

//-----------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE CPUTMeshDX11::MapIndices( CPUTRenderParameters &params, eCPUTMapType type, bool wait )
{
    return Map(
        mIndexCount,
        mpIndexBuffer,
        mIndexBufferDesc,
       &mpStagingIndexBuffer,
       &mIndexBufferMappedType,
        params,
        type,
        wait
    );
}

void CPUTMeshDX11::FreeStagingIndexBuffer()
{
	SAFE_RELEASE(mpStagingIndexBuffer);
}
void CPUTMeshDX11::FreeStagingVertexBuffer()
{
	SAFE_RELEASE(mpStagingVertexBuffer);
}

//-----------------------------------------------------------------------------
void CPUTMeshDX11::UnmapVertices( CPUTRenderParameters &params )
{
    Unmap( mpVertexBuffer, mpStagingVertexBuffer, &mVertexBufferMappedType, params );
}

//-----------------------------------------------------------------------------
void CPUTMeshDX11::UnmapIndices( CPUTRenderParameters &params )
{
    Unmap( mpIndexBuffer, mpStagingIndexBuffer, &mIndexBufferMappedType, params );
}

void CPUTMeshDX11::UpdateVerts(void *vertData, int size)
{
	ID3D11DeviceContext *pContext = CPUT_DX11::GetContext();
	pContext->UpdateSubresource(mpVertexBuffer, 0, NULL, vertData, size, 0);
}

CPUTMeshDX11* CPUTMeshDX11::Create()
{
	return new CPUTMeshDX11();
}

static bool AreLayoutsEqual(D3D11_INPUT_ELEMENT_DESC *format1, D3D11_INPUT_ELEMENT_DESC *format2)
{
	if (format1 == NULL || format2 == NULL)
	{
		return format1 == format2;
	}

	while (true)
	{
		if (format1->SemanticName == NULL && format2->SemanticName == NULL)
			return true;

		if (
			format1->SemanticName == NULL || format2->SemanticName == NULL ||
			format1->AlignedByteOffset != format2->AlignedByteOffset ||
			format1->Format != format2->Format ||
			format1->SemanticIndex != format2->SemanticIndex ||
			_stricmp(format1->SemanticName, format2->SemanticName)
			)
			return false;
		
		format1++;
		format2++;
	}
	return false;
}
static int GetLayoutElementCount(D3D11_INPUT_ELEMENT_DESC *format)
{
	int count = 0;
	while (format->SemanticName != NULL)
	{
		count++;
		format++;
	}
	return count;
}

void CPUTMeshDX11::UpdateVertCountAndFormat(int count, D3D11_INPUT_ELEMENT_DESC *format, int stride)
{
	ID3D11DeviceContext *context = CPUT_DX11::GetContext();
	ID3D11Device *device = CPUT_DX11::GetDevice();
	bool layoutsEqual = AreLayoutsEqual(format, mpLayoutDescription);
	if (!layoutsEqual)
	{
		SAFE_DELETE_ARRAY(mpLayoutDescription);
		mNumberOfInputLayoutElements = GetLayoutElementCount(format);
		mpLayoutDescription = new D3D11_INPUT_ELEMENT_DESC[mNumberOfInputLayoutElements + 1];
		memcpy(mpLayoutDescription, format, sizeof(D3D11_INPUT_ELEMENT_DESC) * (mNumberOfInputLayoutElements + 1));
	}
	if (count != mVertexCount || !layoutsEqual)
	{
		mVertexStride = stride;
		
		mVertexCount = count;
		mVertexBufferMappedType;

		ZeroMemory(&mVertexBufferDesc, sizeof(mVertexBufferDesc));
		mVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		mVertexStride = stride;
		mVertexBufferDesc.ByteWidth = stride * count;
		mVertexBufferDesc.StructureByteStride = stride;
		mVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		mVertexBufferDesc.CPUAccessFlags = 0;  // default to no cpu access for speed

		SAFE_RELEASE(mpVertexBuffer);
		SAFE_RELEASE(mpStagingVertexBuffer);
		SAFE_RELEASE(mpVertexBufferForSRVDX);
		SAFE_RELEASE(mpVertexView);

		device->CreateBuffer(&mVertexBufferDesc, NULL, &mpVertexBuffer);
		
	}
	
}
void CPUTMeshDX11::CopyIndexBufferFrom(CPUTMeshDX11 *srcMesh)
{
	ID3D11DeviceContext *context = CPUT_DX11::GetContext();
	ID3D11Device *device = CPUT_DX11::GetDevice();
	if (memcmp(&mIndexBufferDesc, &srcMesh->mIndexBufferDesc, sizeof(D3D11_BUFFER_DESC)) != 0)
	{
		SAFE_RELEASE(mpIndexBuffer);
		SAFE_RELEASE(mpStagingIndexBuffer);

		mIndexCount = srcMesh->mIndexCount;
		mIndexBufferFormat = srcMesh->mIndexBufferFormat;
		mIndexBufferDesc = srcMesh->mIndexBufferDesc;
		mIndexBufferMappedType = srcMesh->mIndexBufferMappedType;

		device->CreateBuffer(&mIndexBufferDesc, NULL, &mpIndexBuffer);
	}
	context->CopyResource(mpIndexBuffer, srcMesh->mpIndexBuffer);
}

// Make a deep copy of srcMesh
void CPUTMeshDX11::CopyFrom(CPUTMeshDX11 *srcMesh) 
{ 
	mD3DMeshTopology = srcMesh->mD3DMeshTopology;
	
	SAFE_DELETE_ARRAY(mpLayoutDescription);
	mNumberOfInputLayoutElements = srcMesh->mNumberOfInputLayoutElements;
	mpLayoutDescription = new D3D11_INPUT_ELEMENT_DESC[srcMesh->mNumberOfInputLayoutElements + 1];
	memcpy(mpLayoutDescription, srcMesh->mpLayoutDescription, sizeof(D3D11_INPUT_ELEMENT_DESC) * (srcMesh->mNumberOfInputLayoutElements + 1));
	
	mD3DMeshTopology = srcMesh->mD3DMeshTopology;
	ID3D11DeviceContext *context = CPUT_DX11::GetContext();
	ID3D11Device *device = CPUT_DX11::GetDevice();
	if (memcmp( &mVertexBufferDesc, &srcMesh->mVertexBufferDesc, sizeof(D3D11_BUFFER_DESC)) != 0)
	{
		mVertexStride = srcMesh->mVertexStride;
		mVertexBufferDesc = srcMesh->mVertexBufferDesc;
		mVertexBufferOffset = srcMesh->mVertexBufferOffset;
		mVertexCount = srcMesh->mVertexCount;
		mVertexBufferMappedType = srcMesh->mVertexBufferMappedType;
		
		SAFE_RELEASE(mpVertexBuffer);
		SAFE_RELEASE(mpStagingVertexBuffer);
		SAFE_RELEASE(mpVertexBufferForSRVDX);
		SAFE_RELEASE(mpVertexView);

		device->CreateBuffer(&mVertexBufferDesc, NULL, &mpVertexBuffer);
	}
	context->CopyResource(mpVertexBuffer, srcMesh->mpVertexBuffer);

	CopyIndexBufferFrom(srcMesh);
	
	
	mMeshTopology = srcMesh->mMeshTopology;
	mInstanceCount = srcMesh->mInstanceCount;

}

struct CopyStep
{
	int srcOffset;
	int dstOffset;
	int bytes;
};

static int GetBytesFromFormat(DXGI_FORMAT format)
{
	switch(format)
	{
	case DXGI_FORMAT_R32G32B32_FLOAT: 
		return 12;
	case DXGI_FORMAT_R32G32_FLOAT:
		return 8;
	}
	assert(false);
	return 0;
}

static int GenerateCopySteps(CopyStep *steps, int maxSteps, D3D11_INPUT_ELEMENT_DESC *srcFormat, D3D11_INPUT_ELEMENT_DESC *dstFormat)
{
	int stepCount = 0;
	while (dstFormat->SemanticName != NULL)
	{
		D3D11_INPUT_ELEMENT_DESC *srcFormatIter = srcFormat;
		while (srcFormatIter->SemanticName != NULL)
		{
			if (
				dstFormat->Format == srcFormatIter->Format &&
				dstFormat->SemanticIndex == srcFormatIter->SemanticIndex &&
				_stricmp(dstFormat->SemanticName, srcFormatIter->SemanticName) == 0
				)
			{
				assert(stepCount < maxSteps);
				steps[stepCount].srcOffset = srcFormatIter->AlignedByteOffset;
				steps[stepCount].dstOffset = dstFormat->AlignedByteOffset;
				steps[stepCount].bytes = GetBytesFromFormat(dstFormat->Format);
				stepCount++;
			}
			srcFormatIter++;
		}
		dstFormat++;
	}
	return stepCount;
}

// TODO: FACESCAN2
bool CPUTMeshDX11::GetVertexData(D3D11_INPUT_ELEMENT_DESC *format, int formatCount, void *destVertices, int destStride, int srcVertexOffset, int vertexCount)
{ 
	CPUTRenderParameters params = {};
	CopyStep steps[12];
	int stepCount = GenerateCopySteps(steps, ARRAYSIZE(steps), mpLayoutDescription, format);
	D3D11_MAPPED_SUBRESOURCE data = MapVertices(params, CPUT_MAP_READ, true);

	for (int i = 0; i < vertexCount; i++)
	{
		const byte *srcData = (const byte*)data.pData + mVertexStride * (i + srcVertexOffset);
		byte *dstData = (byte*)destVertices + destStride * i;
		for (int j = 0; j < stepCount; j++)
		{
			memcpy(&dstData[steps[j].dstOffset], &srcData[steps[j].srcOffset], steps[j].bytes);
		}

	}
	UnmapVertices(params);
	return true;
}

bool CPUTMeshDX11::UpdateVertexData(D3D11_INPUT_ELEMENT_DESC *format, const void *srcVertices, int srcStride, int vertexOffset, int vertexCount) 
{
	CPUTRenderParameters params = {};
	CopyStep steps[12];
	int stepCount = GenerateCopySteps(steps, ARRAYSIZE(steps), format, mpLayoutDescription);
	D3D11_MAPPED_SUBRESOURCE data = MapVertices(params, CPUT_MAP_READ_WRITE, true);

	assert((vertexCount + vertexOffset) <= (int)mVertexCount);
	for (int i = 0; i < vertexCount; i++)
	{
		byte *dstData = (byte*)data.pData + mVertexStride * (i + vertexOffset);
		const byte *srcData = (const byte*)srcVertices + srcStride * i;
		for (int j = 0; j < stepCount; j++)
		{
			memcpy(&dstData[steps[j].dstOffset], &srcData[steps[j].srcOffset], steps[j].bytes);
		}

	}
	UnmapVertices(params);
	return true;
}

void CPUTMeshDX11::SetIndexBufferData(uint32 *buffer, int indexCount)
{
	DXGI_FORMAT indexFormat = (indexCount < 65536) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	CreateIndexBuffer(indexFormat, NULL, indexCount, false);
	CPUTRenderParameters params;
	D3D11_MAPPED_SUBRESOURCE map = MapIndices(params, CPUT_MAP_WRITE);
	if (map.pData != NULL)
	{
		if (indexFormat == DXGI_FORMAT_R32_UINT)
		{
			memcpy(map.pData, buffer, indexCount * sizeof(uint32));
		}
		else if (indexFormat == DXGI_FORMAT_R16_UINT)
		{
			uint16 *dst = (uint16*)map.pData;
			for (int i = 0; i < indexCount; i++)
			{
				dst[i] = (uint16)buffer[i];
			}
		}
		else
		{
			assert(false);
		}
	}
	UnmapIndices(params);
}

void CPUTMeshDX11::CreateIndexBuffer(DXGI_FORMAT format, void *initialData, int indexCount, bool force)
{
	// Create the index buffer
	D3D11_SUBRESOURCE_DATA resourceData;
	if (mpIndexBuffer == NULL || indexCount != mIndexCount || format != mIndexBufferFormat || force)
	{
		SAFE_RELEASE(mpIndexBuffer);

		mIndexCount = indexCount;
		mIndexBufferFormat = format;

		assert(format == DXGI_FORMAT_R16_UINT || format == DXGI_FORMAT_R32_UINT);
		int bpi = format == DXGI_FORMAT_R16_UINT ? 2 : 4;

		// set the data format info
		ZeroMemory(&mIndexBufferDesc, sizeof(mIndexBufferDesc));
		mIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		mIndexBufferDesc.ByteWidth = mIndexCount * bpi;
		mIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		mIndexBufferDesc.CPUAccessFlags = 0;  // default to no cpu access for speed

		// create the buffer
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = initialData;
		HRESULT hr = CPUT_DX11::GetDevice()->CreateBuffer(&mIndexBufferDesc, (initialData != NULL) ? &resourceData : NULL, &mpIndexBuffer);
		ASSERT(!FAILED(hr), "Failed creating index buffer");
		CPUTSetDebugName(mpIndexBuffer, "Index buffer");

		FreeStagingIndexBuffer();
	}
}