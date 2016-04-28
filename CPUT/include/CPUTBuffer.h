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
#ifndef _CPUTBUFFER_H
#define _CPUTBUFFER_H

#include "CPUT.h"
#include "CPUTRefCount.h"

enum BufferTarget
{
    BUFFER_VERTEX  = 0x1,
    BUFFER_INDEX   = 0x2,
    BUFFER_UNIFORM = 0x4,
};

enum BufferMemory
{
    BUFFER_IMMUTABLE,
    BUFFER_DYNAMIC,
    BUFFER_STREAMING,
    BUFFER_STAGING,
};

enum BufferCPUAccess
{
    BUFFER_CPU_NO_ACCESS = 0x0,
    BUFFER_CPU_READ      = 0x1,
    BUFFER_CPU_WRITE     = 0x2,
};

struct CPUTBufferDesc
{
    UINT sizeBytes;
    void* pData;
    enum BufferTarget target;
    enum BufferMemory memory;
    enum BufferCPUAccess cpuAccess;
};

class CPUTBuffer : public CPUTRefCount
{
public:
    const std::string &GetName() { return mName; }
    virtual void SetData(UINT offset, UINT sizeBytes, void* pData) = 0;
    virtual void GetData(void* pData)=0;
    static CPUTBuffer* Create(std::string &name, CPUTBufferDesc *pDesc);

    UINT GetSize();

protected:
    std::string mName;
    BufferTarget mTarget;
    BufferMemory mMemory;
    BufferCPUAccess mCPUAccess;
    UINT mSizeBytes;

    CPUTBuffer(std::string &name, CPUTBufferDesc* pDesc) 
        :mName(name), mSizeBytes(0)
    {
        if (pDesc)
        {
            mTarget = pDesc->target;
            mMemory = pDesc->memory;
            mCPUAccess = pDesc->cpuAccess;
            mSizeBytes = pDesc->sizeBytes;
        }
    };
    ~CPUTBuffer(){}; // Destructor is not public.  Must release instead of delete.
};

#endif //_CPUTBUFFER_H
