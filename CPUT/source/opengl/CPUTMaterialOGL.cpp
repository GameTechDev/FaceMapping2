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
#include "CPUT.h"
#if (defined(CPUT_FOR_OGL) || defined(CPUT_FOR_OGLES))
#include "CPUTMaterialOGL.h"
#include "CPUT_OGL.h"
#include "CPUTRenderStateBlockOGL.h"
#include "CPUTTextureOGL.h"
#include "CPUTBufferOGL.h"
#include "CPUTShaderOGL.h"
#include "CPUTAssetLibraryOGL.h"
#define OUTPUT_BINDING_DEBUG_INFO(x)

void ReadMacrosFromConfigBlock(
    CPUTConfigBlock   *pMacrosBlock,
    CPUT_SHADER_MACRO  *pShaderMacros,
    CPUT_SHADER_MACRO **pUserSpecifiedMacros,
    int               *pNumUserSpecifiedMacros,
    CPUT_SHADER_MACRO **pFinalShaderMacros
    );
int UniformTypeSize(unsigned int glType);
//-----------------------------------------------------------------------------
CPUTShaderParameters::~CPUTShaderParameters()
{
    for(int ii=0; ii<CPUT_MATERIAL_MAX_TEXTURE_SLOTS; ii++)
    {
        SAFE_RELEASE(mpTexture[ii]);
    }
    for(int ii=0; ii<CPUT_MATERIAL_MAX_UAV_SLOTS; ii++)
    {
        SAFE_RELEASE(mpUAV[ii]);
    }
    for(int ii=0; ii<CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS; ii++)
    {
        SAFE_RELEASE(mpConstantBuffer[ii]);
    }
}

CPUTMaterialOGL::CPUTMaterialOGL() :
    mpVertexShader(NULL),
    mpFragmentShader(NULL),
    mpGeometryShader(NULL),
    mpEvaluationShader(NULL),
    mpControlShader(NULL),
    mShaderProgram(0xFFFFFFFF)
{
    memset(mSamplerIDs, 0, sizeof(mSamplerIDs));
}

// Destructor
//-----------------------------------------------------------------------------
CPUTMaterialOGL::~CPUTMaterialOGL()
{
    // release any shaders
    SAFE_RELEASE(mpFragmentShader);
    SAFE_RELEASE(mpVertexShader);
    SAFE_RELEASE(mpGeometryShader);
    SAFE_RELEASE(mpEvaluationShader);
    SAFE_RELEASE(mpControlShader);
    SAFE_RELEASE(mpRenderStateBlock);
}

//-----------------------------------------------------------------------------
void CPUTMaterialOGL::SetRenderStates()
{
    if(mConstantBuffer.pBuffer)
        mConstantBuffer.pBuffer->SetData(0, mConstantBuffer.size, mConstantBuffer.pData);

    GL_CHECK(glUseProgram(mShaderProgram));

    if( mVertexShaderParameters.mTextureCount ) {
        for( unsigned int ii=0; ii < mVertexShaderParameters.mTextureCount; ii++ )
        {
            GLint bindPoint = mVertexShaderParameters.mpTextureBindPoint[ii];
            GLint location = mVertexShaderParameters.mpTextureLocation[ii];
            CPUTTextureOGL *pTex = (CPUTTextureOGL *)mVertexShaderParameters.mpTexture[ii];
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + bindPoint));
            GL_CHECK(glBindTexture(pTex->mTextureType, pTex->mTextureID));
            glUniform1i(location, bindPoint); // set the image unit to the bindpoint I think those are unique
            
            //
            // Samplers are given in the renderstate but we need to always specify one as they can be different for each texture.
            // The renderstate won't necessarily change since it is the same material.
            // 
            GL_CHECK(glBindSampler(bindPoint, mSamplerIDs[ii])); 
            // why is the texture unit here not GL_TEXTURE0 + bindpoint?
            // because uint v. enum in function signatures. more discussion in OpenGL red book Chapter 6 in Sampler Objects
            // glBindSampler(GLuint unit, GLuint sampler)
            // glBindTexture(GLActiveTexture(GLenum texture)
        }
    }
    
        for (unsigned int ii=0; ii < mVertexShaderParameters.mConstantBufferCount; ii++ )
        {
            GLint bindPoint = mVertexShaderParameters.mConstantBufferBindPoint[ii];
	        GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, ((CPUTBufferOGL*)mVertexShaderParameters.mpConstantBuffer[ii])->GetBufferID()));
        }
#ifdef CPUT_SUPPORT_IMAGE_STORE
    if( mVertexShaderParameters.mUAVCount)
        for(unsigned int ii=0; ii < mVertexShaderParameters.mUAVCount; ii++)
        {
            CPUTTextureOGL *pTex = (CPUTTextureOGL *)mVertexShaderParameters.mpUAV[ii];
            GLint mode = mVertexShaderParameters.mpUAVMode[ii];
            GL_CHECK(glBindImageTexture(0, pTex->GetTexture(), 0, GL_FALSE, 0, mode, GL_RGBA8));
        }
#endif

        if( mpRenderStateBlock )
        {
            // We know we have a OGL class.  Does this correctly bypass the virtual?
            // Should we move it to the OGL class.
            ((CPUTRenderStateBlockOGL*)mpRenderStateBlock)->SetRenderStates();
        }
        else
        {
            CPUTRenderStateBlock::GetDefaultRenderStateBlock()->SetRenderStates();
        }
    }


void SetMaterialStates(CPUTMaterial* pNewBase, CPUTMaterial* pCurrentBase)
{
    if (pNewBase == pCurrentBase)
        return;

    CPUTMaterialOGL* pNew = (CPUTMaterialOGL*)pNewBase;
    CPUTMaterialOGL* pCurrent = (CPUTMaterialOGL*)pCurrentBase;

    int newProgID = pNew->GetShaderProgramID();
    if (pCurrent == NULL || pCurrent->GetShaderProgramID() != newProgID)
        GL_CHECK(glUseProgram(newProgID));
        
    if (pNew->mVertexShaderParameters.mTextureCount) {
        for (unsigned int ii = 0; ii < pNew->mVertexShaderParameters.mTextureCount; ii++)
        {
            GLint bindPoint = pNew->mVertexShaderParameters.mpTextureBindPoint[ii];
            GLint location = pNew->mVertexShaderParameters.mpTextureLocation[ii];
            CPUTTextureOGL *pTex = (CPUTTextureOGL *)pNew->mVertexShaderParameters.mpTexture[ii];
            if (pTex != NULL)
            {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + bindPoint));
                GL_CHECK(glBindTexture(pTex->mTextureType, pTex->mTextureID));
                glUniform1i(location, bindPoint); // set the image unit to the bindpoint I think those are unique

                //
                // Samplers are given in the renderstate but we need to always specify one as they can be different for each texture.
                // The renderstate won't necessarily change since it is the same material.
                // 
                GL_CHECK(glBindSampler(bindPoint, pNew->mSamplerIDs[ii]));
                // why is the texture unit here not GL_TEXTURE0 + bindpoint?
                // because uint v. enum in function signatures. more discussion in OpenGL red book Chapter 6 in Sampler Objects
                // glBindSampler(GLuint unit, GLuint sampler)
                // glBindTexture(GLActiveTexture(GLenum texture)
            }
        }
}

    for (unsigned int ii = 0; ii < pNew->mVertexShaderParameters.mConstantBufferCount; ii++)
    {
        GLint bindPoint = pNew->mVertexShaderParameters.mConstantBufferBindPoint[ii];
        GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, ((CPUTBufferOGL*)pNew->mVertexShaderParameters.mpConstantBuffer[ii])->GetBufferID()));
    }
#ifdef CPUT_SUPPORT_IMAGE_STORE
    if (mVertexShaderParameters.mUAVCount)
    for (unsigned int ii = 0; ii < mVertexShaderParameters.mUAVCount; ii++)
    {
        CPUTTextureOGL *pTex = (CPUTTextureOGL *)mVertexShaderParameters.mpUAV[ii];
        GLint mode = mVertexShaderParameters.mpUAVMode[ii];
        GL_CHECK(glBindImageTexture(0, pTex->GetTexture(), 0, GL_FALSE, 0, mode, GL_RGBA8));
    }
#endif    
}


CPUTResult CPUTMaterialOGL::LoadMaterial(
    const std::string   &fileName,
    CPUT_SHADER_MACRO* pShaderMacros
){
    CPUTResult result = CPUT_SUCCESS;
	CPUTConfigEntry *pValue;

    mMaterialName = fileName;

    // Open/parse the file
    CPUTConfigFile file;
    result = file.LoadFile(fileName);
    if(CPUTFAILED(result))
    {
        return result;
    }

    // Make a local copy of all the parameters
    CPUTConfigBlock *pBlock = file.GetBlock(0);
    ASSERT( pBlock, "Error getting parameter block" );
    if( !pBlock )
    {
        return CPUT_ERROR_PARAMETER_BLOCK_NOT_FOUND;
    }
    mpConfigBlock = pBlock;
    
    CPUT_SHADER_MACRO *pFinalShaderMacros = (CPUT_SHADER_MACRO*)pShaderMacros;
    CPUT_SHADER_MACRO *pUserSpecifiedMacros = NULL;
    // Read additional macros from .mtl file
    std::string macroBlockName = "defines" + cput_to_string(0);
    CPUTConfigBlock *pMacrosBlock = file.GetBlockByName(macroBlockName);
    int numUserSpecifiedMacros = 0;
    if (pMacrosBlock)
    {
        ReadMacrosFromConfigBlock(pMacrosBlock, pShaderMacros, &pUserSpecifiedMacros, &numUserSpecifiedMacros, &pFinalShaderMacros);
    }

    CPUTAssetLibraryOGL *pAssetLibrary = (CPUTAssetLibraryOGL*)CPUTAssetLibrary::GetAssetLibrary();
   {
        CPUTConfigEntry *pEntryPointName, *pProfileName;
        int numFiles = 0;
        std::string pBase = "VertexShaderFileOGL_";
        std::string pName = pBase + cput_to_string(numFiles+1);
        std::vector<std::string> filenames;
        while (mpConfigBlock->GetValueByName(pName)->IsValid())
        {
            filenames.push_back(mpConfigBlock->GetValueByName(pName)->ValueAsString());
            numFiles++;
            pName = pBase + cput_to_string(numFiles+1);
        }
        if(numFiles > 0)
        {
            pEntryPointName = mpConfigBlock->GetValueByName("VertexShaderMain");
            pProfileName = mpConfigBlock->GetValueByName("VertexShaderProfile");
            pAssetLibrary->GetVertexShader(
                filenames,
                "VertexShaderMain",
                pProfileName->ValueAsString(),
                &mpVertexShader,
                false,
                (CPUT_SHADER_MACRO*)pFinalShaderMacros
            );
        }
    }

#ifdef CPUT_SUPPORT_TESSELLATION
    {
        int numFiles = 0;
        std::string pBase = "ControlShaderFileOGL_";
        std::string pName = pBase + cput_to_string(numFiles+1);
        std::vector<std::string> filenames;
        while (mpConfigBlock->GetValueByName(pName)->IsValid())
        {
            filenames.push_back(mpConfigBlock->GetValueByName(pName)->ValueAsString());
            numFiles++;
            pName = pBase + cput_to_string(numFiles+1);
        }
        if(numFiles > 0) 
        {
			pAssetLibrary->GetHullShader(
                filenames,
                /* pD3dDevice,*/
                "ControlShaderMain", 
                "",
                &mpControlShader,
                false,
                (CPUT_SHADER_MACRO*)pShaderMacros
            );
        }
    }
    {
        int numFiles = 0;
        std::string pBase = "EvaluationShaderFileOGL_";
        std::string pName = pBase + cput_to_string(numFiles+1);
        std::vector<std::string> filenames;
        while (mpConfigBlock->GetValueByName(pName)->IsValid())
        {
            filenames.push_back(mpConfigBlock->GetValueByName(pName)->ValueAsString());
            numFiles++;
            pName = pBase + cput_to_string(numFiles+1);
        }
        if(numFiles > 0) 
        {
			pAssetLibrary->GetDomainShader(
                filenames,
                /* pD3dDevice,*/
                "EvaluationShaderMain", 
                "",
                &mpEvaluationShader,
                false,
                (CPUT_SHADER_MACRO*)pShaderMacros
            );
        }
    }
#endif
    {
        int numFiles = 0;
        std::string pBase = "GeometryShaderFileOGL_";
        std::string pName = pBase + cput_to_string(numFiles+1);
        std::vector<std::string> filenames;
        while (mpConfigBlock->GetValueByName(pName)->IsValid())
        {
            filenames.push_back(mpConfigBlock->GetValueByName(pName)->ValueAsString());
            numFiles++;
            pName = pBase + cput_to_string(numFiles+1);
        }
        if(numFiles > 0) 
        {
			pAssetLibrary->GetGeometryShader(
                filenames,
                /* pD3dDevice,*/
                "GeometryShaderMain", 
                "",
                &mpGeometryShader,
                false,
                (CPUT_SHADER_MACRO*)pFinalShaderMacros
            );
        }
    }

    // load and store the pixel shader if it was specified
    {
        int numFiles = 0;
        std::string pBase = "FragmentShaderFileOGL_";
        std::string pName = pBase + cput_to_string(numFiles+1);
        std::vector<std::string> filenames;
        while (mpConfigBlock->GetValueByName(pName)->IsValid())
        {
            filenames.push_back(mpConfigBlock->GetValueByName(pName)->ValueAsString());
            numFiles++;
            pName = pBase + cput_to_string(numFiles+1);
        }
        if(numFiles > 0) 
        {
            pAssetLibrary->GetPixelShader(
                filenames,
                "FragmentShaderMain", 
                mpConfigBlock->GetValueByName("FragmentShaderProfile")->ValueAsString(),
                &mpFragmentShader,
                false,
                (CPUT_SHADER_MACRO*)pFinalShaderMacros
            );
        }
    }

    // load and store the render state file if it was specified
    pValue = mpConfigBlock->GetValueByName("RenderStateFile");
    if( pValue->IsValid() )
    {
        mpRenderStateBlock = pAssetLibrary->GetRenderStateBlock(pValue->ValueAsString());
    }


    int IsLinked;
    char *shaderProgramInfoLog;
    int maxLength;
    
     mShaderProgram = glCreateProgram();
 
    if (mpVertexShader) {
        GL_CHECK(glAttachShader(mShaderProgram, mpVertexShader->GetShaderID()));
    }
    if (mpFragmentShader) {
        GL_CHECK(glAttachShader(mShaderProgram, mpFragmentShader->GetShaderID()));
    }
    if (mpControlShader) {
        GL_CHECK(glAttachShader(mShaderProgram, mpControlShader->GetShaderID()));
    }
    if (mpEvaluationShader) {
        GL_CHECK(glAttachShader(mShaderProgram, mpEvaluationShader->GetShaderID()));
    }
	if (mpGeometryShader) {
		GL_CHECK(glAttachShader(mShaderProgram, mpGeometryShader->GetShaderID()));
	}
    // Link our program
    // At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader.
    // The binary code is uploaded to the GPU, if there is no error.
    GL_CHECK(glLinkProgram(mShaderProgram));
 
    // Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex
    // and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or
    // too many texel fetch instructions or too many interpolators or dynamic loops.
 
    GL_CHECK(glGetProgramiv(mShaderProgram, GL_LINK_STATUS, (int *)&IsLinked));
    if(IsLinked == false)
    {
        // Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv.
        glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
 
        // The maxLength includes the NULL character
        shaderProgramInfoLog = (char *)malloc(maxLength);
 
        // Notice that glGetProgramInfoLog, not glGetShaderInfoLog.
        glGetProgramInfoLog(mShaderProgram, maxLength, &maxLength, shaderProgramInfoLog);
        DEBUG_PRINT("Failed to link shader program:\n%s\n", shaderProgramInfoLog);
        ASSERT(false, "glLinkProgram failed");

        // Handle the error in an appropriate way such as displaying a message or writing to a log file.
        // In this simple program, we'll just leave
        free(shaderProgramInfoLog);
    }
    
    // Shader must be successfully linked before we can query uniform locations
    ReadShaderSamplersAndTextures( mShaderProgram, &mVertexShaderParameters );
    glUseProgram(0);
    {
        BindTextures(        mVertexShaderParameters);
        BindUAVs(            mVertexShaderParameters);
        BindConstantBuffers( mVertexShaderParameters);
    }
	CPUTConfigBlock* pExternalBlock = file.GetBlockByName(EXTERNALS_BLOCK_NAME);
	if (pExternalBlock != NULL && mConstantBuffer.numUniforms > 0)
	{
		for (int i = 0; i < mConstantBuffer.numUniforms; i++)
		{
			float f[16];
			pValue = pExternalBlock->GetValueByName(mConstantBuffer.pUniformNames[i]);
			if (pValue->IsValid())
			{
				pValue->ValueAsFloatArray(f, 16);
				memcpy(mConstantBuffer.pData + mConstantBuffer.pUniformOffsets[i], f, mConstantBuffer.pUniformSizes[i]);
			}
		}
		mConstantBuffer.pBuffer->SetData(0, mConstantBuffer.size, mConstantBuffer.pData);
	}
    for (int i = 0; i < numUserSpecifiedMacros; i++)
    {
        free(pFinalShaderMacros[i].Name);
        free(pFinalShaderMacros[i].Definition);
    }
    SAFE_DELETE_ARRAY(pFinalShaderMacros);
    SAFE_DELETE_ARRAY(pUserSpecifiedMacros);
    return result;
}

// https://sites.google.com/site/opengltutorialsbyaks/introduction-to-opengl-4-1---tutorial-05
void DebugOutputToFile2(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message)
{
#ifndef CPUT_FOR_OGLES
       {
             char debSource[16], debType[20], debSev[16];
             
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

void CheckDebugLog2()
{
#ifndef CPUT_FOR_OGLES
       unsigned int count = 100; // max. num. of messages that will be read from the log
       int bufsize = 2048;

       unsigned int* sources      = new unsigned int[count];
       unsigned int* types        = new unsigned int[count];
       unsigned int* ids   = new unsigned int[count];
       unsigned int* severities = new unsigned int[count];
       int* lengths = new int[count];
       char* messageLog = new char[bufsize];

       unsigned int retVal = glGetDebugMessageLogARB(count, bufsize, sources, types, ids, severities, lengths, messageLog);
       if(retVal > 0)
       {
             unsigned int pos = 0;
             for(unsigned int i=0; i<retVal; i++)
             {
                    DebugOutputToFile2(sources[i], types[i], ids[i], severities[i], &messageLog[pos]);
                    pos += lengths[i];
              }
       }
       delete [] sources;
       delete [] types;
       delete [] ids;
       delete [] severities;
       delete [] lengths;
       delete [] messageLog;
#endif
}

//-----------------------------------------------------------------------------
void CPUTMaterialOGL::ReadShaderSamplersAndTextures( GLuint shaderProgram, CPUTShaderParameters *pShaderParameter )
{
    GLint numActiveUniforms;
    GLint activeUniformMaxLength;
    GL_CHECK(glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numActiveUniforms));
    GL_CHECK(glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformMaxLength));
    
    GLint numActiveUniformBlocks;
    GLint activeUniformBlockMaxLength = 50;
    GL_CHECK(glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_BLOCKS, &numActiveUniformBlocks));
	//
	// #### This parameter is currently unsupported by Intel OGL drivers.
	//
    
    GLchar* uniformVariableName = new GLchar[activeUniformMaxLength];
    GLenum  dataType;
    GLint   size;
    for (int i = 0; i < numActiveUniforms; i++) {
        GL_CHECK(glGetActiveUniform(shaderProgram, i, activeUniformMaxLength, NULL, &size, &dataType, uniformVariableName));
        switch(dataType) {
#ifndef CPUT_FOR_OGLES
            case GL_SAMPLER_1D:
#endif
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
		case GL_SAMPLER_2D_SHADOW:
        case GL_SAMPLER_CUBE:
		        pShaderParameter->mpTextureName.push_back(uniformVariableName);
                pShaderParameter->mpTextureLocation.push_back(glGetUniformLocation(shaderProgram, uniformVariableName));
                pShaderParameter->mTextureCount++;
                break;
#ifdef CPUT_SUPPORT_IMAGE_STORE
        case GL_IMAGE_2D:
            pShaderParameter->mpUAVParameterNames.push_back(uniformVariableName);
            pShaderParameter->mpUAVParameterLocations.push_back(glGetUniformLocation(shaderProgram, uniformVariableName));
            pShaderParameter->mUAVParameterCount++;
            break;
#endif
            default:
                // unsupported uniform type
            break;
        }
    }
	delete[] uniformVariableName;

    GLchar* uniformBlockName = new GLchar[activeUniformBlockMaxLength];
    for (int i = 0; i < numActiveUniformBlocks; i++) 
    {
        GL_CHECK(glGetActiveUniformBlockName(shaderProgram, i, activeUniformBlockMaxLength, NULL, uniformBlockName));
        pShaderParameter->mConstantBufferName.push_back(uniformBlockName);
		GL_CHECK(pShaderParameter->mConstantBufferBindPoint.push_back(glGetUniformBlockIndex(shaderProgram, uniformBlockName)));
        pShaderParameter->mConstantBufferCount++;
		GLsizei numUniforms;
		GLint blockSize;
		if (!strcmp(uniformBlockName, EXTERNALS_SHADER_NAME.c_str()))
		{

			GL_CHECK(glGetActiveUniformBlockiv(shaderProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize));
			mConstantBuffer.pData = new char[blockSize];
			mConstantBuffer.size = blockSize;
            CPUTBufferDesc desc;
            desc.cpuAccess = BUFFER_CPU_WRITE;
            desc.memory = BUFFER_DYNAMIC;
            desc.target = BUFFER_UNIFORM;
            desc.pData = mConstantBuffer.pData;
            desc.sizeBytes = blockSize;
            std::string buffername = "Material Externals";

			//mConstantBuffer.pBuffer = CPUTBufferOGL::Create("Material Externals", GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, blockSize, mConstantBuffer.pData);
            mConstantBuffer.pBuffer = CPUTBuffer::Create(buffername, &desc);

			pShaderParameter->mpConstantBuffer[i] = mConstantBuffer.pBuffer;
            mConstantBuffer.pBuffer->AddRef();
			GL_CHECK(glGetActiveUniformBlockiv(shaderProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numUniforms));
			mConstantBuffer.numUniforms = numUniforms;
            GLint* uniformArrayLengths = new GLint[numUniforms];
			GLuint* uniformIndices = new GLuint[numUniforms];
			GLint* uniformTypes = new GLint[numUniforms];
			GLint* uniformOffsets = new GLint[numUniforms];
			GLint* uniformSizes = new int[numUniforms];

            mConstantBuffer.pUniformArrayLengths = (int*)uniformArrayLengths;
            mConstantBuffer.pUniformIndices = (int*)uniformIndices;
			mConstantBuffer.pUniformNames = new std::string[numUniforms];
            mConstantBuffer.pUniformOffsets = (int*)uniformOffsets;
			mConstantBuffer.pUniformSizes = (int*)uniformSizes;
            mConstantBuffer.pUniformTypes = (int*)uniformTypes;

			GL_CHECK(glGetActiveUniformBlockiv(shaderProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)uniformIndices));
			glGetActiveUniformsiv(shaderProgram, numUniforms, uniformIndices, GL_UNIFORM_TYPE, uniformTypes);
			glGetActiveUniformsiv(shaderProgram, numUniforms, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
            glGetActiveUniformsiv(shaderProgram, numUniforms, uniformIndices, GL_UNIFORM_SIZE, uniformArrayLengths);


			GLchar* name = new GLchar[activeUniformMaxLength];
			for (int uniform = 0; uniform < numUniforms; uniform++)
			{
				GLint size;
				GLenum type;
				GL_CHECK(glGetActiveUniform(shaderProgram,
					uniformIndices[uniform],
					activeUniformMaxLength,
					NULL,
					&size,
					&type,
					name));
				mConstantBuffer.pUniformNames[uniform] = std::string(name);
                mConstantBuffer.pUniformSizes[uniform] = UniformTypeSize(mConstantBuffer.pUniformTypes[uniform]);
            }
            delete[] name;
		}       
	}
    delete[] uniformBlockName;
}

void CPUTMaterialOGL::BindTextures( CPUTShaderParameters &params)
{
    CPUTAssetLibraryOGL *pAssetLibrary = (CPUTAssetLibraryOGL*)CPUTAssetLibrary::GetAssetLibrary();

    for (unsigned int texture = 0; texture < params.mTextureCount; texture++)
    {
        std::string textureName;

        std::string tagName = params.mpTextureName[texture];

        CPUTConfigEntry *pValue = mpConfigBlock->GetValueByName(tagName);
        if (!pValue->IsValid())
        {
            std::string output = "CPUT WARNING: " + tagName + " not found in material " + mMaterialName + "\n";
            DEBUG_PRINT("%s\n",output.c_str());
        }
        textureName = pValue->ValueAsString();
        // If the texture name not specified.  Load default.dds instead
        if( 0 == textureName.length() ) 
        { 
            textureName = "default.dds"; 
        }

        UINT bindPoint = texture;
        params.mpTextureBindPoint.push_back(bindPoint);
        ASSERT( bindPoint < CPUT_MATERIAL_MAX_TEXTURE_SLOTS, "Texture bind point out of range." );

        params.mBindTextureMin = (std::min)( params.mBindTextureMin, bindPoint );
        params.mBindTextureMax = (std::max)( params.mBindTextureMax, bindPoint );

        std::string SRGBName = tagName + "sRGB";

        CPUTConfigEntry *pSRGBValue = mpConfigBlock->GetValueByName(SRGBName);
        bool loadAsSRGB = pSRGBValue->IsValid() ?  loadAsSRGB = pSRGBValue->ValueAsBool() : false;

        if (!params.mpTexture[texture])
        {
            params.mpTexture[texture] = pAssetLibrary->GetTexture(textureName, false, loadAsSRGB);
//            ASSERT(params.mpTexture[texture], "Failed getting texture " + textureName);
        }

        // The shader file (e.g. .fx) can specify the texture bind point (e.g., t0).  Those specifications 
        // might not be contiguous, and there might be gaps (bind points without assigned textures)
        // TODO: Warn about missing bind points?
        //params.mppBindViews[bindPoint] = ((CPUTTextureOGL*)mpTexture[textureCount])->GetShaderResourceView();
        //params.mppBindViews[bindPoint]->AddRef();

        //
        // Match up textures with samplers in order. If there are not enough samplers then just use the default sampler
        //
        CPUTRenderStateOGL *pRenderState = ((CPUTRenderStateBlockOGL*)mpRenderStateBlock)->GetState();
        UINT numSamplers = ((CPUTRenderStateBlockOGL*)mpRenderStateBlock)->GetNumSamplers();

        if (texture < numSamplers) {
            mSamplerIDs[texture] = pRenderState->SamplerIDs[texture];
        }
        else {
            mSamplerIDs[texture] = pRenderState->DefaultSamplerID;
        }
    }
}

void CPUTMaterialOGL::BindUAVs( CPUTShaderParameters &params)
{
#ifdef CPUT_SUPPORT_IMAGE_STORE

    CPUTAssetLibraryOGL *pAssetLibrary = (CPUTAssetLibraryOGL*)CPUTAssetLibrary::GetAssetLibrary();

    for(params.mUAVCount=0; params.mUAVCount < params.mUAVParameterCount; params.mUAVCount++)
    {
        std::string UAVName;
        unsigned int UAVCount = params.mUAVCount;

// Dirty fix
#ifndef CPUT_OS_WINDOWS
        std::string tagName = params.mpUAVParameterNames[UAVCount];
#else
        std::string tagName = s2ws(params.mpUAVParameterNames[UAVCount].c_str());
#endif

        CPUTConfigEntry *pValue = mpConfigBlock->GetValueByName(tagName);
        ASSERT( pValue->IsValid(), L"Can't find UAV '" + tagName + L"'." ); //  TODO: fix message
        UAVName = pValue->ValueAsString();
        // If the UAV name not specified.  Load default.dds instead
        if( 0 == UAVName.length() ) 
        { 
            UAVName = "default.dds"; 
        }

        UINT bindPoint = params.mUAVCount;
        params.mpUAVParameterBindPoint.push_back(bindPoint);
        ASSERT( bindPoint < CPUT_MATERIAL_MAX_UAV_SLOTS, "UAV bind point out of range." );

        params.mBindViewMin = std::min( params.mBindViewMin, bindPoint );
        params.mBindViewMax = std::max( params.mBindViewMax, bindPoint );

        // Get the sRGB flag (default to true)
        std::string SRGBName = tagName + "sRGB";
        CPUTConfigEntry *pSRGBValue = mpConfigBlock->GetValueByName(SRGBName);
        bool loadAsSRGB = pSRGBValue->IsValid() ?  loadAsSRGB = pSRGBValue->ValueAsBool() : false;

        if( !params.mpUAV[UAVCount] )
        {
            params.mpUAV[UAVCount] = pAssetLibrary->GetTexture( UAVName, false, loadAsSRGB );
            ASSERT( params.mpUAV[UAVCount], "Failed getting UAV " + UAVName);
        }

        std::string ReadName = tagName + "READ";
        CPUTConfigEntry *pRead = mpConfigBlock->GetValueByName(ReadName);
        bool read = pRead->IsValid() ? pRead->ValueAsBool() : true;
        std::string WriteName = tagName + "WRITE";
        CPUTConfigEntry *pWrite = mpConfigBlock->GetValueByName(WriteName);
        bool write = pWrite->IsValid() ? pWrite  ->ValueAsBool() : true;
        if(write && read)
            params.mpUAVMode[UAVCount] = GL_READ_WRITE;
        else if(read)
            params.mpUAVMode[UAVCount] = GL_READ_ONLY;
        else
            params.mpUAVMode[UAVCount] = GL_WRITE_ONLY;

        // The shader file (e.g. .fx) can specify the UAV bind point (e.g., t0).  Those specifications 
        // might not be contiguous, and there might be gaps (bind points without assigned UAVs)
        // TODO: Warn about missing bind points?
//        params.mppBindViews[bindPoint] = ((CPUTTextureOGL*)mpTexture[textureCount])->GetShaderResourceView();
//        params.mppBindViews[bindPoint]->AddRef();

        //
        // Match up the UAV name in any UAV samplers given in the renderstate file. If there wasn't
        // one specified for a particular UAV then it just uses the default sampler.
        //
        CPUTRenderStateOGL *pRenderState;
        pRenderState = ((CPUTRenderStateBlockOGL*)mpRenderStateBlock)->GetState();
        
        mSamplerIDs[UAVCount] = pRenderState->DefaultSamplerID;
        for (int renderStateIDX = 0; renderStateIDX < NUM_SAMPLERS_PER_RENDERSTATE; renderStateIDX++) {
			if(renderStateIDX<((CPUTRenderStateBlockOGL*)mpRenderStateBlock)->GetNumSamplers())
            {
                mSamplerIDs[UAVCount] = pRenderState->SamplerIDs[renderStateIDX];
            }
        }
    }
#endif

}

//-----------------------------------------------------------------------------
void CPUTMaterialOGL::BindConstantBuffers( CPUTShaderParameters &params)
{
    CPUTAssetLibraryOGL *pAssetLibrary = (CPUTAssetLibraryOGL*)CPUTAssetLibrary::GetAssetLibrary();
    
    for (int buffer = 0; buffer < params.mConstantBufferCount; buffer++)
    {
        std::string constantBufferName;
        //UINT constantBufferCount = params.mConstantBufferCount;

        std::string tagName = params.mConstantBufferName[buffer];

		if (!tagName.compare(EXTERNALS_SHADER_NAME))
		{
			constantBufferName = tagName;
            params.mpConstantBuffer[buffer] = mConstantBuffer.pBuffer;
		}
		else
		{
            CPUTConfigEntry *pValue = mpConfigBlock->GetValueByName(tagName);
		ASSERT( pValue->IsValid(), "Can't find constant buffer '" + tagName + "'." ); //  TODO: fix message
        
        constantBufferName = pValue->ValueAsString();
        }
        UINT bindPoint = params.mConstantBufferBindPoint[buffer];
        ASSERT( bindPoint < CPUT_MATERIAL_MAX_CONSTANT_BUFFER_SLOTS, "Constant buffer bind point out of range." );

        params.mBindConstantBufferMin = std::min( params.mBindConstantBufferMin, bindPoint );
        params.mBindConstantBufferMax = std::max( params.mBindConstantBufferMax, bindPoint );

        if (!params.mpConstantBuffer[buffer])
        {
            params.mpConstantBuffer[buffer] = pAssetLibrary->GetConstantBuffer(constantBufferName);
            ASSERT(params.mpConstantBuffer[buffer], "Failed getting constant buffer " + constantBufferName);
        }

        glUniformBlockBinding(mShaderProgram, bindPoint, bindPoint); // just use the index as the binding point
    }
}

int UniformTypeSize(unsigned int glType)
{
    int size = 0;
    switch (glType)
    {
    case GL_FLOAT:
        size = sizeof(float);
        break;
    case GL_FLOAT_VEC2:
        size = sizeof(float) * 2;
        break;
    case GL_FLOAT_VEC3:
        size = sizeof(float) * 3;
        break;
    case GL_FLOAT_VEC4:
        size = sizeof(float) * 4;
        break;
    case GL_FLOAT_MAT2:
        size = sizeof(float) * 2 * 2;
        break;
    case GL_FLOAT_MAT3:
        size = sizeof(float) * 3 * 3;
        break;
    case GL_FLOAT_MAT4:
        size = sizeof(float) * 4 * 4;
        break;
    case GL_FLOAT_MAT2x3:
        size = sizeof(float) * 2 * 3;
        break;
    case GL_FLOAT_MAT2x4:
        size = sizeof(float) * 2 * 4;
        break;
    case GL_FLOAT_MAT3x2:
        size = sizeof(float) * 3 * 2;
        break;
    case GL_FLOAT_MAT3x4:
        size = sizeof(float) * 3 * 4;
        break;
    case GL_FLOAT_MAT4x2:
        size = sizeof(float) * 4 * 2;
        break;
    case GL_FLOAT_MAT4x3:
        size = sizeof(float) * 4 * 3;
        break;
    case GL_BOOL:
        size = sizeof(float);
        break;
    default:
        DEBUG_PRINT("CPUT - ERROR: shader refection unexpected type\n");
        break;
    }
    return size;
}

//-----------------------------------------------------------------------------
void ReadMacrosFromConfigBlock(
    CPUTConfigBlock   *pMacrosBlock,
    CPUT_SHADER_MACRO  *pShaderMacros,
    CPUT_SHADER_MACRO **pUserSpecifiedMacros,
    int               *pNumUserSpecifiedMacros,
    CPUT_SHADER_MACRO **pFinalShaderMacros
    ){
    *pNumUserSpecifiedMacros = pMacrosBlock->ValueCount();

    // Count the number of macros passed in
    CPUT_SHADER_MACRO *pMacro = (CPUT_SHADER_MACRO*)pShaderMacros;
    int numPassedInMacros = 0;
    if (pMacro)
    {
        while (pMacro->Name)
        {
            ++numPassedInMacros;
            ++pMacro;
        }
    }

    // Allocate an array of macro pointer large enough to contain the passed-in macros plus those specified in the .mtl file.
    *pFinalShaderMacros = new CPUT_SHADER_MACRO[*pNumUserSpecifiedMacros + numPassedInMacros + 1];

    // Copy the passed-in macro pointers to the final array
    int jj;
    for (jj = 0; jj<numPassedInMacros; jj++)
    {
        (*pFinalShaderMacros)[jj] = *(CPUT_SHADER_MACRO*)&pShaderMacros[jj];
    }

    // Create a CPUT_SHADER_MACRO for each of the macros specified in the .mtl file.
    // And, add their pointers to the final array
    *pUserSpecifiedMacros = new CPUT_SHADER_MACRO[*pNumUserSpecifiedMacros];
    for (int kk = 0; kk<*pNumUserSpecifiedMacros; kk++, jj++)
    {
        CPUTConfigEntry *pValue = pMacrosBlock->GetValue(kk);
        (*pUserSpecifiedMacros)[kk].Name = (char *)malloc(pValue->NameAsString().size() + 1); // + 1 for NULL terminator
        (*pUserSpecifiedMacros)[kk].Definition = (char *)malloc(pValue->ValueAsString().size() + 1);

        strcpy((*pUserSpecifiedMacros)[kk].Name, pValue->NameAsString().c_str());
        strcpy((*pUserSpecifiedMacros)[kk].Definition, pValue->ValueAsString().c_str());
        (*pFinalShaderMacros)[jj] = (*pUserSpecifiedMacros)[kk];
    }
    (*pFinalShaderMacros)[jj].Name = NULL;
    (*pFinalShaderMacros)[jj].Definition = NULL;
}


#endif

