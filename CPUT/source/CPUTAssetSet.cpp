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
#include "CPUTAssetSet.h"

#include "CPUTModel.h"
#include "CPUTMesh.h"
#include "CPUTAssetLibrary.h"
#include "CPUTCamera.h"
#include "CPUTLight.h"
#include "CPUTMaterial.h"
#include "CPUTRenderStateBlock.h"
#include "CPUTInputLayoutCache.h"
//-----------------------------------------------------------------------------
CPUTAssetSet::CPUTAssetSet() :
    mppAssetList(NULL),
    mAssetCount(0),
    mpRootNode(NULL),
    mpFirstCamera(NULL),
    mCameraCount(0)
{
}

//-----------------------------------------------------------------------------
CPUTAssetSet::~CPUTAssetSet()
{
    SAFE_RELEASE(mpFirstCamera);

    // Deleteing the asset set implies recursively releasing all the assets in the hierarchy
    if(mpRootNode && !mpRootNode->ReleaseRecursive() )
    {
        mpRootNode = NULL;
    }
    // Release all the elements in the asset list.  Note that we don't
    // recursively delete the hierarchy here.
    // We release the entries here because this class is where we add them.
    // TODO: Howevere, all derivations will have this, so perhaps it should go in the base.
    for( UINT ii=0; ii<mAssetCount; ii++ )
    {
        SAFE_RELEASE( mppAssetList[ii] );
    }
    SAFE_DELETE_ARRAY(mppAssetList);
}

//-----------------------------------------------------------------------------
void CPUTAssetSet::RenderRecursive(CPUTRenderParameters &renderParams, int materialIndex)
{
	if (!IsEnabled)
		return;
    CPUTMaterial* pCurrentMaterial = NULL;
    CPUTRenderStateBlock* pCurrentRenderState = NULL;
    CPUTRenderNode* pCurrent = mpRootNode;
    CPUTInputLayoutCache* pInputLayoutCache = CPUTInputLayoutCache::GetInputLayoutCache();
    while (pCurrent)
    {
        if (pCurrent->GetNodeType() == CPUTRenderNode::CPUT_NODE_MODEL)
        {
            CPUTModel* pModel = (CPUTModel*)pCurrent;
            pModel->UpdateShaderConstants(renderParams);
            int meshCount = pModel->GetMeshCount();
            for (int mesh = 0; mesh < meshCount; mesh++)
            {
                CPUTMaterial* pMaterial = pModel->GetMaterial(mesh, materialIndex);
                if (pMaterial != NULL)
                {
                    CPUTRenderStateBlock* pRenderStateBlock = pMaterial->GetRenderStateBlock();
                    CPUTMesh* pMesh = pModel->GetMesh(mesh);
                    SetMaterialStates(pMaterial, pCurrentMaterial);
                    SetRenderStateBlock(pRenderStateBlock, pCurrentRenderState);
                    pInputLayoutCache->Apply(pMesh, pMaterial);
                    pMesh->Draw();
                    SAFE_RELEASE(pCurrentMaterial);
                    pCurrentMaterial = pMaterial;
                    SAFE_RELEASE(pCurrentRenderState)
                    pCurrentRenderState = pRenderStateBlock;
                }
            }
        }
        CPUTRenderNode* pNext = pCurrent->GetChild();
        if (pNext == NULL)
        {
            pNext = pCurrent->GetSibling();

            if (pNext == NULL)
            {
                pNext = pCurrent->GetParent();
                if (pNext != NULL)
                {
                    pNext = pNext->GetSibling();
                }
            }
        }
        pCurrent = pNext;
    }
    SAFE_RELEASE(pCurrentMaterial);
    SAFE_RELEASE(pCurrentRenderState);
}

//-----------------------------------------------------------------------------
void CPUTAssetSet::UpdateRecursive( float deltaSeconds )
{
	if(mpRootNode)
	{
		mpRootNode->UpdateRecursive(deltaSeconds);
	}
}

//-----------------------------------------------------------------------------
void CPUTAssetSet::GetBoundingBox(float3 *pCenter, float3 *pHalf)
{
    *pCenter = *pHalf = float3(0.0f);
    if(mpRootNode)
    {
        mpRootNode->GetBoundingBoxRecursive(pCenter, pHalf);
    }
}

//-----------------------------------------------------------------------------
CPUTResult CPUTAssetSet::GetAssetByIndex(const UINT index, CPUTRenderNode **ppRenderNode)
{
    ASSERT( NULL != ppRenderNode, "Invalid NULL parameter" );
    *ppRenderNode = mppAssetList[index];
    mppAssetList[index]->AddRef();
    return CPUT_SUCCESS;
}


//-----------------------------------------------------------------------------
CPUTResult CPUTAssetSet::LoadAssetSet(std::string name, int numSystemMaterials, std::string *pSystemMaterialNames)
{
    CPUTResult result = CPUT_SUCCESS;

    // if not found, load the set file
    CPUTConfigFile ConfigFile;
    result = ConfigFile.LoadFile(name);
    if( !CPUTSUCCESS(result) )
    {
        return result;
    }
    // ASSERT( CPUTSUCCESS(result), "Failed loading set file '" + name + "'." );

    mAssetCount = ConfigFile.BlockCount() + 1; // Add one for the implied root node
    mppAssetList = new CPUTRenderNode*[mAssetCount];
    mppAssetList[0] = mpRootNode;
    mpRootNode->AddRef();

    CPUTAssetLibrary *pAssetLibrary = (CPUTAssetLibrary*)CPUTAssetLibrary::GetAssetLibrary();

	CPUTAnimation *pDefaultAnimation = NULL;
    for(UINT ii=0; ii<mAssetCount-1; ii++) // Note: -1 because we added one for the root node (we don't load it)
    {
        CPUTConfigBlock *pBlock = ConfigFile.GetBlock(ii);
		ASSERT(pBlock != NULL, "Cannot find block");

        if( !pBlock )
        {
            return CPUT_ERROR_PARAMETER_BLOCK_NOT_FOUND;
        }

        std::string nodeType = pBlock->GetValueByName("type")->ValueAsString();
        CPUTRenderNode *pParentNode = NULL;

        // TODO: use Get*() instead of Load*() ?

        std::string name = pBlock->GetValueByName("name")->ValueAsString();

        int parentIndex;
        CPUTRenderNode *pNode = NULL;
        if(0==nodeType.compare("null"))
        {
            pNode  = pNode = CPUTNullNode::Create();
            result = ((CPUTNullNode*)pNode)->LoadNullNode(pBlock, &parentIndex);
            pParentNode = mppAssetList[parentIndex+1];
            std::string &parentPrefix = pParentNode->GetPrefix();
            std::string prefix = parentPrefix;
            prefix.append(".");
            prefix.append(name);
            pNode->SetPrefix(prefix);
      //      pNode->SetPrefix( parentPrefix + "." + name );
            pAssetLibrary->AddNullNode("", parentPrefix + name, "", (CPUTNullNode*)pNode);
            // Add this null's name to our prefix
            // Append this null's name to our parent's prefix
            pNode->SetParent( pParentNode );
            pParentNode->AddChild( pNode );
        }
        else if(0==nodeType.compare("model"))
        {
            CPUTConfigEntry *pValue = pBlock->GetValueByName( "instance" );
            CPUTModel *pModel = CPUTModel::Create();
            if( pValue == &CPUTConfigEntry::sNullConfigValue )
            {
                // Not found.  So, not an instance.
                pModel->LoadModel(pBlock, &parentIndex, NULL, numSystemMaterials, pSystemMaterialNames);
            }
            else
            {
                int instance = pValue->ValueAsInt();

                // If the current node is an instance of itself then it has no "master" model
                if(ii == instance)
                {
                    // TODO: create instance model object
                    pModel->LoadModel(pBlock, &parentIndex, NULL, numSystemMaterials, pSystemMaterialNames);
                }
                else
                {
                    pModel->LoadModel(pBlock, &parentIndex, (CPUTModel*)mppAssetList[instance+1], numSystemMaterials, pSystemMaterialNames);
                }
            }
            pParentNode = mppAssetList[parentIndex+1];
            pModel->SetParent( pParentNode );
            pParentNode->AddChild( pModel );
            std::string &parentPrefix = pParentNode->GetPrefix();
            pModel->SetPrefix( parentPrefix );
            pAssetLibrary->AddModel(name, parentPrefix, "", pModel);

            pModel->UpdateBoundsWorldSpace();

#ifdef  SUPPORT_DRAWING_BOUNDING_BOXES
            // Create a mesh for rendering the bounding box
            // TODO: There is definitely a better way to do this.  But, want to see the bounding boxes!
            pModel->CreateBoundingBoxMesh();
#endif
            pNode = pModel;
        }
        else if(0==nodeType.compare("light"))
        {
            pNode = CPUTLight::Create();
            ((CPUTLight*)pNode)->LoadLight(pBlock, &parentIndex);
            pParentNode = mppAssetList[parentIndex+1]; // +1 because we added a root node to the start
            pNode->SetParent( pParentNode );
            pParentNode->AddChild( pNode );
            std::string &parentPrefix = pParentNode->GetPrefix();
            pNode->SetPrefix( parentPrefix );
            pAssetLibrary->AddLight(name, parentPrefix, "", (CPUTLight*)pNode);
        }
        else if(0==nodeType.compare("camera"))
        {
            pNode = CPUTCamera::Create(CPUT_PERSPECTIVE);
            ((CPUTCamera*)pNode)->LoadCamera(pBlock, &parentIndex);
            pParentNode = mppAssetList[parentIndex+1]; // +1 because we added a root node to the start
            pNode->SetParent( pParentNode );
            pParentNode->AddChild( pNode );
            std::string &parentPrefix = pParentNode->GetPrefix();
            pNode->SetPrefix( parentPrefix );
            pAssetLibrary->AddCamera(name, parentPrefix, "", (CPUTCamera*)pNode);
            if( !mpFirstCamera ) { mpFirstCamera = (CPUTCamera*)pNode; mpFirstCamera->AddRef();}
            ++mCameraCount;
        }
        else
        {
            ASSERT(0,"Unsupported node type '" + nodeType + "'.");
        }

        CPUTConfigEntry *pAnimValue = pBlock->GetValueByName( "Animation0" );
		//int autoPlayAnimationIndex = pBlock->GetValueByName( "DefaultAnimation" );
        for(int i = 1; pAnimValue != &CPUTConfigEntry::sNullConfigValue; ++i )
        {
			CPUTAnimation *pCurrentAnimation = pAssetLibrary->GetAnimation(pAnimValue->ValueAsString());
			
            //First Animation will be default for now
			if(pCurrentAnimation != NULL && i == 1 /*autoPlayAnimationIndex*/)
			{
				pDefaultAnimation = pCurrentAnimation ;
                pCurrentAnimation->AddRef();
			}

            std::stringstream animName;
            animName << "Animation" << i;
            pAnimValue = pBlock->GetValueByName(animName.str());
            pNode->mAnimationList.push_back(pCurrentAnimation);
        }

        // Add the node to our asset list (i.e., the linear list, not the hierarchical)
        mppAssetList[ii+1] = pNode;
        // Don't AddRef.Creating it set the refcount to 1.  We add it to the list, and then we're done with it.
        // Net effect is 0 (+1 to add to list, and -1 because we're done with it)
        // pNode->AddRef();
    }

    //Set the default animation to the current Asset Set
	if(pDefaultAnimation != NULL && mAssetCount > 1)
	{
        //Apply Animation to root of the setfile
		mppAssetList[1]->SetAnimation(pDefaultAnimation);
	}
	SAFE_RELEASE(pDefaultAnimation);
    return result;
}

//-----------------------------------------------------------------------------
CPUTAssetSet *CPUTAssetSet::Create( const std::string &name, const std::string &absolutePathAndFilename, int numSystemMaterials, std::string *pSystemMaterialNames )
{
    CPUTAssetLibrary *pAssetLibrary = CPUTAssetLibrary::GetAssetLibrary();

    // Create the root node.
    CPUTNullNode *pRootNode = CPUTNullNode::Create();
    pRootNode->SetName("_CPUTAssetSetRootNode_");

    // Create the asset set, set its root, and load it
    CPUTAssetSet   *pNewAssetSet = new CPUTAssetSet();
	pNewAssetSet->IsEnabled = true;
    pNewAssetSet->SetRoot( pRootNode );
    pAssetLibrary->AddNullNode( name, "", "_Root", pRootNode );

    CPUTResult result = pNewAssetSet->LoadAssetSet(absolutePathAndFilename, numSystemMaterials, pSystemMaterialNames);
    if( CPUTSUCCESS(result) )
    {
        pAssetLibrary->AddAssetSet(name, "", "", pNewAssetSet);
        return pNewAssetSet;
    }
    ASSERT( CPUTSUCCESS(result), "Error loading AssetSet\n'"+absolutePathAndFilename+"'");
    pNewAssetSet->Release();
    return NULL;
}

//-----------------------------------------------------------------------------
CPUTResult CPUTAssetSet::GetAssetByName(const std::string &name, CPUTRenderNode **ppRenderNode)
{
	for (UINT ii = 0; ii < mAssetCount; ii++)
	{
		std::string &assetName = mppAssetList[ii]->GetName();
		if (_stricmp( assetName.c_str(), name.c_str()) == 0)
		{
			*ppRenderNode = mppAssetList[ii];
			mppAssetList[ii]->AddRef();
			return CPUT_SUCCESS;
		}
	}
	return CPUT_ERROR_NOT_FOUND;
}