//--------------------------------------------------------------------------------------
// Copyright 2011 Intel Corporation
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
#ifndef __CPUTMESHOGL_H__
#define __CPUTMESHOGL_H__

#pragma once

#include "CPUTMesh.h"
#include "CPUT.h"
#include "CPUTOSServices.h"

class CPUTVertexArrayOGL;
class CPUTBuffer;

//-----------------------------------------------------------------------------
class CPUTMeshOGL : public CPUTMesh
{
protected:

    CPUTMeshOGL();

    CPUTVertexArrayOGL * mpVertexArray;
    
    CPUTBuffer    *mpVertexBuffer;
    UINT           mVertexCount;
    UINT           mVertexStride;

    CPUTBuffer    *mpIndexBuffer;
    UINT           mIndexCount;
    
    // This is a handle to the shader program
    GLuint shaderprogram;

    GLuint vao,vbo[2]; /* Create handles for our Vertex Array Object and two Vertex Buffer Objects */
    eCPUTMapType              mIndexBufferMappedType;

public:
	static CPUTMeshOGL* Create();
    virtual ~CPUTMeshOGL();
    
    CPUTResult CreateNativeResources(
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
    void Draw();
    void DrawPatches();

    void SetNumVertices(int numVertices) {this->mVertexCount = numVertices;};
    void SetNumIndices(int numIndices) {this->mIndexCount = numIndices;};

	// FIXME force get of buffer and map there
    void SetIndexSubData( UINT offset, UINT size, void* pData);
    void SetVertexSubData( UINT offset, UINT size, void* pData);
	void* MapVertices(   CPUTRenderParameters &params, eCPUTMapType type, bool wait=true ) {ASSERT(0, "unsupported"); return NULL;};
    void* MapIndices(    CPUTRenderParameters &params, eCPUTMapType type, bool wait=true ){ASSERT(0, "unsupported");return NULL;};
    void  UnmapVertices( CPUTRenderParameters &params ){ASSERT(0, "unsupported");};
    void  UnmapIndices(  CPUTRenderParameters &params ){ASSERT(0, "unsupported");};
};

#endif // __CPUTMESHOGL_H__
