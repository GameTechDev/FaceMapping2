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
#ifndef _CPUTTEXTUREOGL_H
#define _CPUTTEXTUREOGL_H

#include "CPUTTexture.h"
#include "CPUT_OGL.h"

class CPUTTextureOGL : public CPUTTexture
{
private:
    // Destructor is not public.  Must release instead of delete.
    ~CPUTTextureOGL() {
        glDeleteTextures(1, &mTextureID);
    }

    void SetTextureInformation();
    
public:
    GLuint mTextureID;
    GLint mWidth, mHeight;
    GLint mTextureType;
    static CPUTTexture   *Create( const std::string &name );
    static CPUTTexture   *Create( const std::string &name, const std::string &absolutePathAndFilename, bool loadAsSRGB );
    static CPUTResult     CreateNativeTexture(
                              const std::string &fileName,
                              GLint *textureID,
                              GLint *textureType,
                              bool forceLoadAsSRGB
                          );
    static CPUTTexture *Create(const std::string &name, GLenum internalFormat, int width, int height, 
                   GLenum format, GLenum type, void* pData);


    CPUTTextureOGL() :
        mTextureID(0),
        mWidth(0),
        mHeight(0),
        mTextureType(GL_TEXTURE_2D)
    {};

    void ReleaseTexture()
    {
    }

    void SetTexture(GLuint texture) { mTextureID = texture;};
    GLuint GetTexture() { return mTextureID; };
    void SetTextureAndShaderResourceView(GLint id/*, ID3D11ShaderResourceView *pShaderResourceView*/)
    {
        ASSERT(0, "Set Texture and Shader Resource View -- don't do it like this");
    }

    void GetWidthAndHeight(int *width, int *height) { *width = (int)mWidth; *height = (int)mHeight; }
    virtual void *MapTexture(CPUTRenderParameters &params, eCPUTMapType type, bool wait = true );
    void                      UnmapTexture( CPUTRenderParameters &params );
};

#endif //_CPUTTEXTUREOGL_H

