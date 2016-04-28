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
#include "CPUTModel.h"
#include "CPUTMaterial.h"
#include "CPUTMesh.h"
#include "CPUTAssetLibrary.h"
#include "CPUTSkeleton.h"
#include "CPUTBuffer.h"
#include "CPUTCamera.h"
#include "CPUTInputLayoutCache.h"

DrawModelCallBackFunc CPUTModel::mDrawModelCallBackFunc = CPUTModel::DrawModelCallBack;

CPUTModel* CPUTModel::Create()
{
    return new CPUTModel();
}

//-----------------------------------------------------------------------------
CPUTModel::~CPUTModel()
{
    for( int ii=0; ii<mMeshCount; ii++ )
    {
        for( int jj=0; jj<mpMaterialCount[ii]; jj++ )
        {
            SAFE_RELEASE(mpMaterial[ii][jj]);
        }
        SAFE_DELETE( mpMaterial[ii] );

        HEAPCHECK;
        if( mpMesh[ii] )
        {
            mpMesh[ii]->DecrementInstanceCount();
        }
        SAFE_RELEASE(mpMesh[ii]);
        HEAPCHECK;
    }

    SAFE_DELETE_ARRAY(mpMaterialCount);
    SAFE_DELETE_ARRAY(mpMaterial);

    SAFE_DELETE_ARRAY(mpMesh);

    if(mSkeleton)
    {
        delete mSkeleton;
    }
}

//-----------------------------------------------------------------------------
void CPUTModel::GetBoundsObjectSpace(float3 *pCenter, float3 *pHalf)
{
    *pCenter = mBoundingBoxCenterObjectSpace;
    *pHalf   = mBoundingBoxHalfObjectSpace;
}

//-----------------------------------------------------------------------------
void CPUTModel::GetBoundsWorldSpace(float3 *pCenter, float3 *pHalf)
{
    *pCenter = mBoundingBoxCenterWorldSpace;
    *pHalf   = mBoundingBoxHalfWorldSpace;
}

//-----------------------------------------------------------------------------
void CPUTModel::UpdateBoundsWorldSpace()
{
    // If an object is rigid, then it's object-space bounding box doesn't change.
    // However, if it moves, then it's world-space bounding box does change.
    // Call this function when the model moves

    float4x4 *pWorld =  GetWorldMatrix();
    float4 center    =  float4(mBoundingBoxCenterObjectSpace, 1.0f); // W = 1 because we want the xlation (i.e., center is a position)
    float4 half      =  float4(mBoundingBoxHalfObjectSpace,   0.0f); // W = 0 because we don't want xlation (i.e., half is a direction)

    // TODO: optimize this
    float4 positions[8] = {
        center + float4( 1.0f, 1.0f, 1.0f, 0.0f ) * half,
        center + float4( 1.0f, 1.0f,-1.0f, 0.0f ) * half,
        center + float4( 1.0f,-1.0f, 1.0f, 0.0f ) * half,
        center + float4( 1.0f,-1.0f,-1.0f, 0.0f ) * half,
        center + float4(-1.0f, 1.0f, 1.0f, 0.0f ) * half,
        center + float4(-1.0f, 1.0f,-1.0f, 0.0f ) * half,
        center + float4(-1.0f,-1.0f, 1.0f, 0.0f ) * half,
        center + float4(-1.0f,-1.0f,-1.0f, 0.0f ) * half
    };

    float4 minPosition( FLT_MAX,  FLT_MAX,  FLT_MAX, 1.0f );
    float4 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1.0f );
    for( UINT ii=0; ii<8; ii++ )
    {
        float4 position = positions[ii] * *pWorld;
        minPosition = Min( minPosition, position );
        maxPosition = Max( maxPosition, position );
    }
    mBoundingBoxCenterWorldSpace = (maxPosition + minPosition) * 0.5f;
    mBoundingBoxHalfWorldSpace   = (maxPosition - minPosition) * 0.5f;
}

CPUTResult CPUTModel::LoadModel(CPUTConfigBlock *pBlock, int *pParentID, CPUTModel *pMasterModel, int numSystemMaterials, std::string *pSystemMaterialNames)
{
    CPUTResult result = CPUT_SUCCESS;
    CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();

    std::string modelSuffix = ptoc(this);

    // set the model's name
    mName = pBlock->GetValueByName("name")->ValueAsString();
    mName = mName + ".mdl";

    // resolve the full path name
    std::string modelLocation;
    std::string resolvedPathAndFile;
    modelLocation = CPUTAssetLibrary::GetAssetLibrary()->GetModelDirectoryName();
    modelLocation = modelLocation + mName;
    CPUTFileSystem::ResolveAbsolutePathAndFilename(modelLocation, &resolvedPathAndFile);

    // Get the parent ID.  Note: the caller will use this to set the parent.
    *pParentID = pBlock->GetValueByName("parent")->ValueAsInt();

    LoadParentMatrixFromParameterBlock(pBlock);

    // Get the bounding box information
    float3 center(0.0f), half(0.0f);
    pBlock->GetValueByName("BoundingBoxCenter")->ValueAsFloatArray(center.f, 3);
    pBlock->GetValueByName("BoundingBoxHalf")->ValueAsFloatArray(half.f, 3);
    mBoundingBoxCenterObjectSpace = center;
    mBoundingBoxHalfObjectSpace = half;

    // Get Skin Info
    CPUTConfigEntry *pSkinValue = pBlock->GetValueByName("skeleton");
    if (pSkinValue != &CPUTConfigEntry::sNullConfigValue)
    {
        std::string name = CPUTAssetLibrary::GetAssetLibrary()->GetAnimationSetDirectoryName() + pSkinValue->ValueAsString() + ".skl";
        std::string path;
        CPUTFileSystem::ResolveAbsolutePathAndFilename(name, &path);
        mSkeleton = CPUTSkeleton::Create();
        mSkeleton->LoadSkeleton(path);

    }

    mMeshCount = pBlock->GetValueByName("meshcount")->ValueAsInt();

    if (pMasterModel)
    {
        for (int ii = 0; ii < mMeshCount; ii++)
        {
            mpMesh = new CPUTMesh*[mMeshCount];
            // Reference the master model's mesh.  Don't create a new one.
            mpMesh[ii] = pMasterModel->mpMesh[ii];
            mpMesh[ii]->AddRef();
        }
    }
    else
    {
        // Not a clone/instance.  So, load the model's binary payload (i.e., vertex and index buffers)
        // TODO: Change to use GetModel()
        result = LoadModelPayload(resolvedPathAndFile);
        ASSERT(CPUTSUCCESS(result), "Failed loading model");
    }

    mpMaterialCount = new int[mMeshCount];
    mpMaterial = new CPUTMaterial**[mMeshCount];
    memset(mpMaterial, 0, mMeshCount * sizeof(CPUTMaterial*));

    std::string materialList;

    std::string materialValueName;
    for (int ii = 0; ii<mMeshCount; ii++)
    {
        // get the right material number ('material0', 'material1', 'material2', etc)
        materialValueName = "material";
        materialValueName.append(cput_to_string(ii));
        materialList = pBlock->GetValueByName(materialValueName)->ValueAsString();
        unsigned int first = 0; unsigned int last = 0;
        std::vector<std::string>materialNames;
		unsigned int l = (unsigned int)materialList.length();
        while (first < l)
        {
			last = (unsigned int)materialList.find(" ", first);
            materialNames.push_back(materialList.substr(first, last-first));
            first = std::min(l, last);
            first++;
        }

        int numEffects = (int)materialNames.size();
        CPUTMaterial** pEffects = (CPUTMaterial**)malloc(sizeof(CPUTMaterial*)*numEffects);
        for (int effect = 0; effect < numEffects; effect++)
        {
            if (materialNames[effect] == "NULL")
            {
                pEffects[effect] = NULL;
            }
            else
            {
                pEffects[effect] = pAssetLibrary->GetMaterial(materialNames[effect], false);
                if (pEffects[effect] == NULL)
                {
                    DEBUGMESSAGEBOX("File Load Error", "Unable to load " + materialNames[effect]);
                    ASSERT(pEffects[effect], "Couldn't find material.");
                }
            }
        }
        mpMaterialCount[ii] = 0;
        HEAPCHECK;
        SetMaterial(ii, pEffects, numEffects);
        HEAPCHECK;

        // Release the extra refcount we're holding from the GetMaterial operation earlier
        // now the asset library, and this model have the only refcounts on that material

        mpMaterialCount[ii] = numEffects;

        for (int effect = 0; effect < numEffects; effect++)
        {
            SAFE_RELEASE(pEffects[effect]);
        }
        if (pEffects) free(pEffects);
    }

    return result;
}

//-----------------------------------------------------------------------------
CPUTResult CPUTModel::LoadModelPayload(const std::string &FileName)
{
    CPUTResult result = CPUT_SUCCESS;

    CPUTFileSystem::CPUTOSifstream file(FileName, std::ios::in | std::ios::binary);

    ASSERT( !file.fail(), "CPUTModel::LoadModelPayload() - Could not find binary model file: " + FileName );

    // set up for mesh creation loop
    int meshIndex = 0;
    std::vector<CPUTMesh*> pMeshVector;
    while(file.good() && !file.eof())
    {
        // TODO: rearrange while() to avoid if(eof).  Should perform only one branch per loop iteration, not two
        CPUTRawMeshData vertexFormatDesc;
        vertexFormatDesc.Read(file);
        if(file.eof())
        {
            // TODO: We check eof at the top of loop.  If it isn't eof there, why is it eof here?
            break;
        }
        if (!(meshIndex < mMeshCount))
        {
            DEBUG_PRINT("possibly unexpected number of meshes for model");
        }
        //ASSERT(meshIndex < mMeshCount, "Actual mesh count doesn't match stated mesh count");

        // create the mesh.
        CPUTMesh *pMesh = CPUTMesh::Create();
        pMeshVector.push_back(pMesh);
        //= mpMesh[meshIndex];

        // always a triangle list (at this point)
        pMesh->SetMeshTopology(CPUT_TOPOLOGY_INDEXED_TRIANGLE_LIST);

        // get number of data blocks in the vertex element (pos,norm,uv,etc)
        CPUTBufferElementInfo *pVertexElementInfo = new CPUTBufferElementInfo[vertexFormatDesc.mFormatDescriptorCount];

        // running count of each type of  element
        int positionStreamCount=0;
        int normalStreamCount=0;
        int texCoordStreamCount=0;
        int tangentStreamCount=0;
        int binormalStreamCount=0;
        int colorStreamCount=0;
		int blendWeightStreamCount = 0;
		int blendIndexStreamCount = 0;

        int runningOffset = 0;
        for(UINT ii=0; ii<vertexFormatDesc.mFormatDescriptorCount; ii++)
        {
            // lookup the CPUT data type equivalent
            pVertexElementInfo[ii].mElementType = CPUT_FILE_ELEMENT_TYPE_TO_CPUT_TYPE_CONVERT(vertexFormatDesc.mpElements[ii].mVertexElementType);
            ASSERT((pVertexElementInfo[ii].mElementType !=CPUT_UNKNOWN ) , ".MDL file load error.  This model file has an unknown data type in it's model data.");
            // calculate the number of elements in this stream block (i.e. F32F32F32 = 3xF32)
            pVertexElementInfo[ii].mElementComponentCount = vertexFormatDesc.mpElements[ii].mElementSizeInBytes/CPUT_DATA_FORMAT_SIZE[pVertexElementInfo[ii].mElementType];
            // store the size of each element type in bytes (i.e. 3xF32, each element = F32 = 4 bytes)
            pVertexElementInfo[ii].mElementSizeInBytes = vertexFormatDesc.mpElements[ii].mElementSizeInBytes;
            // store the number of elements (i.e. 3xF32, 3 elements)
            // calculate the offset from the first element of the stream - assumes all blocks appear in the vertex stream as the order that appears here
            pVertexElementInfo[ii].mOffset = runningOffset;
            runningOffset = runningOffset + pVertexElementInfo[ii].mElementSizeInBytes;

            // extract the name of stream
            pVertexElementInfo[ii].mpSemanticName = CPUT_VERTEX_ELEMENT_SEMANTIC_AS_STRING[ii];

            //TODO: Calculate Opengl semantic index elsewhere
            switch(vertexFormatDesc.mpElements[ii].mVertexElementSemantic)
            {
			//FIXME - this isn't right, and needs to change for DX and OpenGL
			//Probably just need to move semantic bind point into OpenGL, or something.
            //Currently, TEXCOORD is the only semantic with multiples in common use. Adding
            //semantic index works provided addtional attributes (e.g. vertex color) are not
            //present
            case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_POSITON:
                pVertexElementInfo[ii].mpSemanticName = "POSITION";
                pVertexElementInfo[ii].mSemanticIndex = positionStreamCount++;
				pVertexElementInfo[ii].mBindPoint = CPUTSemanticBindPoint::POSITION;
                break;
            case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_NORMAL:
                pVertexElementInfo[ii].mpSemanticName = "NORMAL";
                pVertexElementInfo[ii].mSemanticIndex = normalStreamCount++;
				pVertexElementInfo[ii].mBindPoint = CPUTSemanticBindPoint::NORMAL;
                break;
            case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_TEXTURECOORD:
                pVertexElementInfo[ii].mpSemanticName = "TEXCOORD";
				pVertexElementInfo[ii].mSemanticIndex = texCoordStreamCount++;
                pVertexElementInfo[ii].mBindPoint = CPUTSemanticBindPoint::TEXCOORD + pVertexElementInfo[ii].mSemanticIndex;
                break;
            case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_TANGENT:
                pVertexElementInfo[ii].mpSemanticName = "TANGENT";
                pVertexElementInfo[ii].mSemanticIndex = tangentStreamCount++;
				pVertexElementInfo[ii].mBindPoint = CPUTSemanticBindPoint::TANGENT;
                break;
            case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_BINORMAL:
                pVertexElementInfo[ii].mpSemanticName = "BINORMAL";
                pVertexElementInfo[ii].mSemanticIndex = binormalStreamCount++;
				pVertexElementInfo[ii].mBindPoint = CPUTSemanticBindPoint::BINORMAL;
                break;
            case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_VERTEXCOLOR:
                pVertexElementInfo[ii].mpSemanticName = "COLOR";
                pVertexElementInfo[ii].mSemanticIndex = colorStreamCount++;
				pVertexElementInfo[ii].mBindPoint = CPUTSemanticBindPoint::COLOR;
                break;
			case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_BLEND_WEIGHT:
				pVertexElementInfo[ii].mpSemanticName = "BLEND_WEIGHT";
				pVertexElementInfo[ii].mSemanticIndex = blendWeightStreamCount++;
				break;
			case eCPUT_VERTEX_ELEMENT_SEMANTIC::CPUT_VERTEX_ELEMENT_BLEND_INDEX:
				pVertexElementInfo[ii].mpSemanticName = "BLEND_INDEX";
				pVertexElementInfo[ii].mSemanticIndex = blendIndexStreamCount++;
				break;
            default:
                std::string errorString = "Invalid vertex semantic in: '"+FileName+"'\n";
                TRACE(errorString.c_str());
                ASSERT(0, errorString);
            }
        }

        // Index buffer
        CPUTBufferElementInfo indexDataInfo;
        indexDataInfo.mElementType           = (vertexFormatDesc.mIndexType == eCPUT_VERTEX_ELEMENT_TYPE::tUINT32) ? CPUT_U32 : CPUT_U16;
        indexDataInfo.mElementComponentCount = 1;
        indexDataInfo.mElementSizeInBytes    = (vertexFormatDesc.mIndexType == eCPUT_VERTEX_ELEMENT_TYPE::tUINT32) ? sizeof(uint32_t) : sizeof(uint16_t);
        indexDataInfo.mOffset                = 0;
        indexDataInfo.mSemanticIndex         = 0;
        indexDataInfo.mpSemanticName         = NULL;

        if( vertexFormatDesc.mVertexCount && vertexFormatDesc.mIndexCount )
        {
            result = pMesh->CreateNativeResources(
                this,
                meshIndex,
                vertexFormatDesc.mFormatDescriptorCount,
                pVertexElementInfo,
                vertexFormatDesc.mVertexCount,
                (void*)vertexFormatDesc.mpVertices,
                &indexDataInfo,
                vertexFormatDesc.mIndexCount,
                vertexFormatDesc.mpIndices
            );
            if(CPUTFAILED(result))
            {
                return result;
            }
        }
        delete [] pVertexElementInfo;
        pVertexElementInfo = NULL;
        ++meshIndex;
    }
    ASSERT( file.eof(), "" );

    if (mpMesh == NULL)
    {        
        mMeshCount = (int)pMeshVector.size();
        mpMesh = new CPUTMesh*[mMeshCount];

    }
    for (int i = 0; i < mMeshCount; i++)
    {
        mpMesh[i] = pMeshVector[i];
    }
    // close file
    file.close();
    
    return result;
}

// Set the material associated with this mesh and create/re-use a
void CPUTModel::SetMaterial(UINT ii, CPUTMaterial **pMaterial, int numMaterials)
{
    // release previous submaterials
    for (int effect = 0; effect < mpMaterialCount[ii]; effect++)
    {
            SAFE_RELEASE( mpMaterial[ii][effect] );
    }
    SAFE_DELETE_ARRAY(mpMaterial[ii]);

    mpMaterialCount[ii] = numMaterials;
    mpMaterial[ii] = new CPUTMaterial*[numMaterials + 1];
    for (int jj = 0; jj<numMaterials; jj++)
    {
        if (pMaterial[jj])
        {
            mpMaterial[ii][jj] = pMaterial[jj];
            pMaterial[jj]->AddRef();
        }
        else
        {
            mpMaterial[ii][jj] = NULL;
        }
	}
    mpMaterial[ii][numMaterials] = NULL;
    mpMaterialCount[ii] = numMaterials;
} 

CPUTMaterial* CPUTModel::GetMaterial(int meshIndex, int materialIndex) {
    if (meshIndex < mMeshCount && materialIndex < mpMaterialCount[meshIndex] && mpMaterial[meshIndex][materialIndex] != NULL)
    {
        mpMaterial[meshIndex][materialIndex]->AddRef();
        return mpMaterial[meshIndex][materialIndex];
    }
    else
        return NULL;
}
//-----------------------------------------------------------------------------
void CPUTModel::UpdateRecursive( float deltaSeconds )
{
    if(mSkeleton && mpCurrentAnimation)
    {
        std::vector<CPUTNodeAnimation * > *jointAnimation = mpCurrentAnimation->FindJointNodeAnimation(mSkeleton->mJointsList[0].mName);
        for(UINT i = 0; i < mSkeleton->mNumberOfJoints && jointAnimation != NULL; ++i)
        {
            float4x4 worldXform = (*jointAnimation)[i]->Interpolate(mAnimationTime,mSkeleton->mJointsList[i],mIsLoop);
            UINT parentId = (UINT)mSkeleton->mJointsList[i].mParentIndex;
            
            if( parentId < 255)
            {
                mSkeleton->mJointsList[i].mRTMatrix =  worldXform * mSkeleton->mJointsList[parentId].mRTMatrix;
            }
            else
            {
                mSkeleton->mJointsList[i].mRTMatrix = worldXform;
            }
        }
        mAnimationTime += deltaSeconds * mPlaybackSpeed;
    }
    else if(mpCurrentNodeAnimation != NULL && mpCurrentNodeAnimation->IsValidAnimation())
    {
        SetParentMatrix(mpCurrentNodeAnimation->Interpolate(mAnimationTime,mIsLoop));
        mAnimationTime += deltaSeconds * mPlaybackSpeed;
    }

    Update(deltaSeconds);

    if(mpSibling)
    {
        mpSibling->UpdateRecursive(deltaSeconds);
    }
    if(mpChild)
    {
        mpChild->UpdateRecursive(deltaSeconds);
    }
}

void CPUTModel::UpdateShaderConstants(CPUTRenderParameters &renderParams)
{
    float4x4     world(*GetWorldMatrix());
    float4x4     NormalMatrix(*GetWorldMatrix());

    //Local transform if node baked into animation
    if (mSkeleton && mpCurrentAnimation)
    {
        world = GetParentsWorldMatrix();
        NormalMatrix = GetParentsWorldMatrix();
    }

    if (renderParams.mpPerModelConstants)
    {
        CPUTBuffer *pBuffer = (CPUTBuffer*)(renderParams.mpPerModelConstants);
        CPUTModelConstantBuffer cb;
        cb.World = world;
        cb.InverseWorld = cb.World;
        cb.InverseWorld.invert();

        CPUTCamera *pCamera = renderParams.mpCamera;

        if (pCamera)
        {
            cb.WorldViewProjection = cb.World * *pCamera->GetViewMatrix() * *pCamera->GetProjectionMatrix();
        }

        float4x4    shadowView, shadowProjection;
        CPUTCamera *pShadowCamera = renderParams.mpShadowCamera;
        if (pShadowCamera)
        {
            shadowView = *pShadowCamera->GetViewMatrix();
            shadowProjection = *pShadowCamera->GetProjectionMatrix();
            cb.LightWorldViewProjection = cb.World * shadowView * shadowProjection;
        }
		else
		{
			cb.LightWorldViewProjection = renderParams.mLightMatrixOverride;
		}

        cb.BoundingBoxCenterWorldSpace = float4(mBoundingBoxCenterWorldSpace, 0);
        cb.BoundingBoxHalfWorldSpace = float4(mBoundingBoxHalfWorldSpace, 0);
        cb.BoundingBoxCenterObjectSpace = float4(mBoundingBoxCenterObjectSpace, 0);
        cb.BoundingBoxHalfObjectSpace = float4(mBoundingBoxHalfObjectSpace, 0);

		cb.UserData1 = mUserData1;
		cb.UserData2 = mUserData2;
		cb.UserData3 = mUserData3;
        pBuffer->SetData(0, sizeof(CPUTModelConstantBuffer), &cb);
    }
    //TODO: Should this process if object not visible?
    //Only do this if Model has a skin and is animated
    if (mSkeleton && mpCurrentAnimation && renderParams.mpSkinningData)
    {
        CPUTBuffer* pBuffer = renderParams.mpSkinningData;
        CPUTAnimationConstantBuffer acb;// = (CPUTAnimationConstantBuffer *)mapInfo.pData;

        ASSERT(mSkeleton->mNumberOfJoints < 255, "Skin Exceeds maximum number of allowable joints: 255");
        for (UINT i = 0; i < mSkeleton->mNumberOfJoints; ++i)
        {
            CPUTJoint *pJoint = &mSkeleton->mJointsList[i];
            acb.SkinMatrix[i] = pJoint->mInverseBindPoseMatrix * pJoint->mScaleMatrix * pJoint->mRTMatrix;
            float4x4 skinNormalMatrix = acb.SkinMatrix[i];
            skinNormalMatrix.invert(); skinNormalMatrix.transpose();
            acb.SkinNormalMatrix[i] = skinNormalMatrix;
        }
        pBuffer->SetData(0, sizeof(CPUTAnimationConstantBuffer), &acb);
    }
}

void CPUTModel::Render(CPUTRenderParameters &renderParams, int materialIndex)
{
    CPUTCamera* pCamera = renderParams.mpCamera;
    UpdateBoundsWorldSpace();
    if (renderParams.mDrawModels && !renderParams.mRenderOnlyVisibleModels || !pCamera || pCamera->mFrustum.IsVisible(mBoundingBoxCenterWorldSpace, mBoundingBoxHalfWorldSpace))
    {
        UpdateShaderConstants(renderParams);
        // loop over all meshes in this model and draw them
        for (int ii = 0; ii<mMeshCount; ii++)
        {
            if (materialIndex < mpMaterialCount[ii])
            {
                CPUTMaterial* pMaterial = mpMaterial[ii][materialIndex];
                mDrawModelCallBackFunc(this, renderParams, mpMesh[ii], pMaterial, NULL, NULL);
            }
        }
    }
}

void CPUTModel::RenderMaterialOverride(CPUTRenderParameters &renderParams, CPUTMaterial *material, int meshIndex, bool culling)
{
	CPUTCamera* pCamera = renderParams.mpCamera;
	UpdateBoundsWorldSpace();
	if (culling || (renderParams.mDrawModels && !renderParams.mRenderOnlyVisibleModels || !pCamera || pCamera->mFrustum.IsVisible(mBoundingBoxCenterWorldSpace, mBoundingBoxHalfWorldSpace)))
	{
		UpdateShaderConstants(renderParams);
		// loop over all meshes in this model and draw them
		for (int i = 0; i<mMeshCount; i++)
		{
			if (meshIndex == -1 || meshIndex == i )
				mDrawModelCallBackFunc(this, renderParams, mpMesh[i], material, NULL, NULL);
		}
	}
}

void CPUTModel::RenderMeshOverride(CPUTRenderParameters &renderParams, CPUTMesh *mesh, int meshIndex, int matIndex)
{
	UpdateShaderConstants(renderParams);
	// loop over all meshes in this model and draw them
	if (matIndex < mpMaterialCount[meshIndex])
	{
		CPUTMaterial* pMaterial = mpMaterial[meshIndex][matIndex];
		mDrawModelCallBackFunc(this, renderParams, mesh, pMaterial, NULL, NULL);
	}
}

void CPUTModel::RenderMeshCustom(CPUTRenderParameters &renderParams, CPUTMesh *mesh, CPUTMaterial *material)
{
	UpdateShaderConstants(renderParams);
	mDrawModelCallBackFunc(this, renderParams, mesh, material, NULL, NULL);
}

void CPUTModel::SetMeshes(CPUTMesh **meshes, int meshCount)
{
	SAFE_DELETE_ARRAY(mpMesh);
	mpMesh = new CPUTMesh *[meshCount];
	for (int i = 0; i < meshCount; i++)
	{
		mpMesh[i] = meshes[i];
	}
	mMeshCount = meshCount;
}

bool CPUTModel::DrawModelCallBack(CPUTModel*, CPUTRenderParameters &renderParams, CPUTMesh* pMesh, CPUTMaterial* pMaterial, CPUTMaterial* pOriginalMaterial, void* pInputLayout)
{
    pMaterial->SetRenderStates();
    CPUTInputLayoutCache::GetInputLayoutCache()->Apply(pMesh, pMaterial);
    pMesh->Draw();

    return true;
}
