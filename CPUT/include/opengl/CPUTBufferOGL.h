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
#ifndef _CPUTBUFFEROGL_H
#define _CPUTBUFFEROGL_H

#include "CPUT_OGL.h"
#include "CPUTBuffer.h"

//--------------------------------------------------------------------------------------
// TODO: Move to dedicated file
class CPUTBufferOGL : public CPUTBuffer
{
private:
    GLuint mBufferID;
    GLenum mGLTarget;
    GLenum mGLUsage;

    CPUTBufferOGL();
    CPUTBufferOGL(std::string &name, CPUTBufferDesc *pDesc);
    // Destructor is not public.  Must release instead of delete.
    ~CPUTBufferOGL();

public:
    static CPUTBuffer* Create(std::string &name, CPUTBufferDesc *pDesc);
    GLuint GetBufferID() { 
        return mBufferID; 
    }
    void SetData(UINT offset, UINT size, void* pData);
    void GetData(void* pData);
};
#endif //_CPUTBUFFEROGL_H

