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
#include "CPUT_OGL.h"
#include "CPUTMeshOGL.h"
#include "CPUTMaterial.h"

GLenum ConvertToOpenGLFormat(CPUT_DATA_FORMAT_TYPE dataFormatType) {

    // TODO:
    // Please consider removing this in favor for lookup static array of GL types 
    // indexed by CPUT_DATA_FORMAT_TYPE. Before lookup we do range check in assert
    // of course.
    switch( dataFormatType ) {
#ifndef CPUT_FOR_OGLES
         case CPUT_DOUBLE:
            return GL_DOUBLE;
            break;
#endif
        case CPUT_F32:
            return GL_FLOAT;
            break;
        case CPUT_U32:
            return GL_UNSIGNED_INT;
            break;
        case CPUT_I32:
            return GL_INT;
            break;
        case CPUT_U16:
            return GL_UNSIGNED_SHORT;
            break;
        case CPUT_I16:
            return GL_SHORT;
            break;
        case CPUT_U8:
            return GL_UNSIGNED_BYTE;
            break;
        case CPUT_I8:
        case CPUT_CHAR:
            return GL_BYTE;
            break;
        case CPUT_BOOL:
            return GL_BOOL;
            break;
        default:
            assert(0);
            break;
    }
assert(0);
return 0;
}

//
// This class mimics the behaviour of GL vertex arrays which were introduced in GLES3.0
//
class CPUTVertexArrayOGL
{
private:

    CPUTVertexArrayOGL(GLint vertexElementCount) :
    mIndexBufferID(0), mVertexBufferID(0), mVertexElementCount(vertexElementCount)
    {
        GL_CHECK(glGenVertexArrays(1, &mVertexArray)); 
        GL_CHECK(glBindVertexArray(mVertexArray));
    }
	
public:
    GLint mIndexBufferID;
    GLint mVertexBufferID;
    GLint mVertexElementCount;
    GLuint mVertexArray;
        
public:
    
    void AddIBO(CPUTBufferOGL *pBuffer) { 
        if(pBuffer) 
            mIndexBufferID = pBuffer->GetBufferID(); } 
    void AddVBO(CPUTBufferOGL *pBuffer) { if(pBuffer) mVertexBufferID = pBuffer->GetBufferID(); } 
    
    virtual ~CPUTVertexArrayOGL()
    {
        if (mVertexArray != 0)
        {
            GL_CHECK(glDeleteVertexArrays(1, &mVertexArray));
        }
    }
    
    void AddVertexPointer(GLint index, GLint count, GLenum type, GLboolean norm, GLint stride, void * offset)
    {
		GL_CHECK(glBindBuffer( GL_ARRAY_BUFFER, mVertexBufferID));
		GL_CHECK(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBufferID));
        GL_CHECK(glEnableVertexAttribArray(index));
        GL_CHECK(glVertexAttribPointer(index, count, type, norm, stride, offset));
    }
    
#ifndef CPUT_FOR_OGLES
    // Not supported in ES
    void AddVertexLPointer(GLint index, GLint count, GLenum type, GLint stride, void * offset)
    {
        GL_CHECK(glEnableVertexAttribArray(index));
        GL_CHECK(glVertexAttribLPointer(index, count, type, stride, offset));
    }
#endif
    // Not supported in ES
    void AddVertexIPointer(GLint index, GLint count, GLenum type, GLint stride, void * offset)
    {
        GL_CHECK(glEnableVertexAttribArray(index));
        GL_CHECK(glVertexAttribIPointer(index, count, type, stride, offset));
    }
    
    void Enable()
    {
        GL_CHECK(glBindVertexArray(mVertexArray));
    }
    
    void Disable()
    {
        GL_CHECK(glBindVertexArray(0));
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
	void AddVertexPointers(int vertexElementCount, CPUTBufferElementInfo *pVertexDataInfo)
	{
		int vertexStride = pVertexDataInfo[vertexElementCount-1].mOffset + pVertexDataInfo[vertexElementCount-1].mElementSizeInBytes; // size in bytes of a single vertex block

		//Enable();
        for (int i = 0; i < vertexElementCount; i++) 
		{
			switch (pVertexDataInfo[i].mElementType) 
			{
			case CPUT_F32:
				AddVertexPointer(pVertexDataInfo[i].mBindPoint, pVertexDataInfo[i].mElementComponentCount, ConvertToOpenGLFormat(pVertexDataInfo[i].mElementType), GL_FALSE, vertexStride, (void *)(pVertexDataInfo[i].mOffset));
				break;

#ifndef CPUT_FOR_OGLES
			case CPUT_DOUBLE:
				AddVertexLPointer(pVertexDataInfo[i].mBindPoint, pVertexDataInfo[i].mElementComponentCount, ConvertToOpenGLFormat(pVertexDataInfo[i].mElementType), vertexStride, (void *)(pVertexDataInfo[i].mOffset));
				break;
#endif
			case CPUT_U32:
			case CPUT_I32:
			case CPUT_U16:
			case CPUT_I16:
			case CPUT_U8:
			case CPUT_I8:
				AddVertexIPointer(pVertexDataInfo[i].mBindPoint, pVertexDataInfo[i].mElementComponentCount, ConvertToOpenGLFormat(pVertexDataInfo[i].mElementType), vertexStride, (void *)(pVertexDataInfo[i].mOffset));
				break;
	        default:
            // unrecognized type
//            DEBUG_PRINT("Unrecognised type for vertex data");
            break;
			}
		}
        Disable();
	};

	static CPUTVertexArrayOGL* Create(int vertexElementCount)
	{
		return new CPUTVertexArrayOGL(vertexElementCount);
	}
};

//-----------------------------------------------------------------------------
CPUTMeshOGL::CPUTMeshOGL() :
//    mD3DMeshTopology(0),
    mpVertexBuffer(NULL),
    mpIndexBuffer(NULL),
    mIndexCount(0),
    mVertexCount(0),
    mVertexStride(0),
    mpVertexArray(NULL)
{
}

//-----------------------------------------------------------------------------
CPUTMeshOGL::~CPUTMeshOGL()
{
    SAFE_RELEASE(mpVertexBuffer);
    SAFE_RELEASE(mpIndexBuffer);
    SAFE_DELETE(mpVertexArray);
}

//-----------------------------------------------------------------------------
CPUTResult CPUTMeshOGL::CreateNativeResources(
    CPUTModel              *pModel0,
    UINT                    meshIdx0,
    int                     vertexElementCount,
    CPUTBufferElementInfo  *pVertexDataInfo,
    UINT                    vertexCount,
    void                   *pVertexData,
    CPUTBufferElementInfo  *pIndexDataInfo,
    UINT                    indexCount,
    void                   *pIndexData
)
{
    mpVertexArray = CPUTVertexArrayOGL::Create(vertexElementCount);
	mIndexCount = indexCount;
	mVertexCount = vertexCount;
	SAFE_RELEASE(mpIndexBuffer);
	if(indexCount > 0)
	{
        CPUTBufferDesc desc;
        desc.memory = BUFFER_IMMUTABLE;
        desc.target = BUFFER_INDEX;
        desc.cpuAccess = BUFFER_CPU_NO_ACCESS;
        desc.sizeBytes = mIndexCount * pIndexDataInfo->mElementSizeInBytes;
        desc.pData = pIndexData;
        std::string name = "index buffer";
        mpIndexBuffer = CPUTBuffer::Create(name, &desc);
	}
    
	
	SAFE_RELEASE(mpVertexBuffer);
	if(vertexCount > 0)
	{
		mVertexCount  = vertexCount;
		mVertexStride = pVertexDataInfo[vertexElementCount-1].mOffset + pVertexDataInfo[vertexElementCount-1].mElementSizeInBytes; // size in bytes of a single vertex block
        CPUTBufferDesc desc;
        desc.memory = BUFFER_IMMUTABLE;
        desc.target = BUFFER_VERTEX;
        desc.cpuAccess = BUFFER_CPU_NO_ACCESS;
        desc.sizeBytes = mVertexCount * mVertexStride;
        desc.pData = pVertexData;
        std::string name = "vertex buffer";

        mpVertexBuffer = CPUTBuffer::Create(name, &desc);
	}
    
    mpVertexArray->AddIBO((CPUTBufferOGL*)mpIndexBuffer);
    mpVertexArray->AddVBO((CPUTBufferOGL*)mpVertexBuffer);
	mpVertexArray->AddVertexPointers(vertexElementCount, pVertexDataInfo);
    
    return CPUT_SUCCESS;
}

// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
void DebugOutputToFile(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message)
{
#ifndef CPUT_FOR_OGLES
    {
             char debSource[16], debType[20], debSev[5];
             
             if(source == GL_DEBUG_SOURCE_API_ARB)
                    strcpy(debSource, "OpenGL");
             else if(source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
                    strcpy(debSource, "Windows");
             else if(source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
                    strcpy(debSource, "Shader Compiler");
             else if(source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
                    strcpy(debSource, "Third Party");
             else if(source == GL_DEBUG_SOURCE_APPLICATION_ARB)
                    strcpy(debSource, "Application");
             else if(source == GL_DEBUG_SOURCE_OTHER_ARB)
                    strcpy(debSource, "Other");
             if(type == GL_DEBUG_TYPE_ERROR_ARB)
                    strcpy(debType, "Error");
             else if(type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
                    strcpy(debType, "Deprecated behavior");
             else if(type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
                    strcpy(debType, "Undefined behavior");
             else if(type == GL_DEBUG_TYPE_PORTABILITY_ARB)
                    strcpy(debType, "Portability");
             else if(type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
                    strcpy(debType, "Performance");
             else if(type == GL_DEBUG_TYPE_OTHER_ARB)
                    strcpy(debType, "Other");
             if(severity == GL_DEBUG_SEVERITY_HIGH_ARB)
                    strcpy(debSev, "High");
             else if(severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
                    strcpy(debSev, "Medium");
             else if(severity == GL_DEBUG_SEVERITY_LOW_ARB)
                    strcpy(debSev, "Low");

             printf("Source:%s\tType:%s\tID:%d\tSeverity:%s\tMessage:%s\n", debSource, debType, id, debSev, message);
    }
#endif
}

void CPUTMeshOGL::Draw()
{
    if(mVertexCount == 0 && mIndexCount == 0)
        return;
    mpVertexArray->Enable();
    if(mpIndexBuffer != NULL)
	{
		GL_CHECK(glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, NULL));
	}
	else
	{
		GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, mVertexCount));
	}
    mpVertexArray->Disable();
    
    GL_CHECK(glUseProgram(0));
}

void CPUTMeshOGL::DrawPatches()
{
    //todo - need to enable patch options - default is 3 control points.
    if(mVertexCount == 0 && mIndexCount == 0)
        return;
    mpVertexArray->Enable();
    if(mpIndexBuffer != NULL)
	{
		GL_CHECK(glDrawElements(GL_PATCHES, mIndexCount, GL_UNSIGNED_INT, NULL));
	}
	else
	{
		GL_CHECK(glDrawArrays(GL_PATCHES, 0, mVertexCount));
	}
    mpVertexArray->Disable();
    
    GL_CHECK(glUseProgram(0));
}

void CPUTMeshOGL::SetIndexSubData( UINT offset, UINT size, void* pData)
{
    mpIndexBuffer->SetData(offset, size, pData);
}
void CPUTMeshOGL::SetVertexSubData( UINT offset, UINT size, void* pData)
{
    mpVertexBuffer->SetData(offset, size, pData);
}

CPUTMeshOGL* CPUTMeshOGL::Create()
{
	return new CPUTMeshOGL();
}

