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

