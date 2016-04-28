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

#include "CPUTBufferOGL.h"

CPUTBuffer* CPUTBuffer::Create(std::string &name, CPUTBufferDesc* pDesc)
{
    return CPUTBufferOGL::Create(name, pDesc);
}

CPUTBufferOGL::~CPUTBufferOGL()
{
    glDeleteBuffers(1, &mBufferID);
}
void CPUTBufferOGL::SetData(UINT offset, UINT size, void* pData)
{
    if (offset + size <= mSizeBytes)
    {
        GL_CHECK(glBindBuffer(mGLTarget, mBufferID));
        GL_CHECK(glBufferSubData(mGLTarget, offset, size, pData));
        GL_CHECK(glBindBuffer(mGLTarget, 0));
    }
    else
    {
        DEBUG_PRINT("Error attempting to write past end of buffer: %s", mName.c_str());
    }
}

void CPUTBufferOGL::GetData(void* pData)
{
    GL_CHECK(glBindBuffer(mGLTarget, mBufferID));
    void* pMappedData = glMapBufferRange(mTarget, 0, mSizeBytes, GL_MAP_READ_BIT);
    memcpy(pData, pMappedData, mSizeBytes);
    glUnmapBuffer(mGLTarget);
    GL_CHECK(glBindBuffer(mGLTarget, 0));
}

CPUTBuffer* CPUTBufferOGL::Create(std::string &name, CPUTBufferDesc *pDesc)
{
    return new CPUTBufferOGL(name, pDesc);
}

CPUTBufferOGL::CPUTBufferOGL(std::string &name, CPUTBufferDesc *pDesc)
    : CPUTBuffer(name, pDesc)
{
    glGenBuffers(1, &mBufferID);
    mGLTarget = pDesc->target == BUFFER_VERTEX ? GL_ARRAY_BUFFER
            : pDesc->target == BUFFER_INDEX ? GL_ELEMENT_ARRAY_BUFFER
            : pDesc->target == BUFFER_UNIFORM ? GL_UNIFORM_BUFFER
            : NULL;
    if (pDesc != NULL)
    {
        switch (pDesc->memory)
        {
        case BUFFER_IMMUTABLE:
            mGLUsage = GL_STATIC_DRAW;
            break;
        case BUFFER_DYNAMIC:
            mGLUsage = GL_DYNAMIC_DRAW;
            break;
        case BUFFER_STREAMING:
            mGLUsage = GL_STREAM_DRAW;
            break;
        default:
            GL_STATIC_DRAW;
        }
        if (pDesc->memory == BUFFER_STAGING || pDesc->cpuAccess == BUFFER_CPU_WRITE)
            mGLUsage += 2; //copy
        else if (pDesc->cpuAccess = BUFFER_CPU_READ)
            mGLUsage += 1;//read

        GL_CHECK(glBindBuffer(mGLTarget, mBufferID));
        GL_CHECK(glBufferData(mGLTarget, mSizeBytes, pDesc->pData, mGLUsage));
        GL_CHECK(glBindBuffer(mGLTarget, 0));

    }
}