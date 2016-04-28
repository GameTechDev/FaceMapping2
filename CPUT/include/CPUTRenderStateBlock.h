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
#ifndef _CPUTRENDERSTATEBLOCK_H
#define _CPUTRENDERSTATEBLOCK_H

#include "CPUT.h"
#include "CPUTRefCount.h"

class CPUTRenderParameters;

class CPUTRenderStateBlock : public CPUTRefCount
{
protected:
    static CPUTRenderStateBlock *mpDefaultRenderStateBlock;

    std::string mMaterialName;
    
    CPUTRenderStateBlock(){}
    ~CPUTRenderStateBlock(){} // Destructor is not public.  Must release instead of delete.

public:
    static CPUTRenderStateBlock *Create( const std::string &name, const std::string &absolutePathAndFilename );
    static CPUTRenderStateBlock *GetDefaultRenderStateBlock() { return mpDefaultRenderStateBlock; }
    static void SetDefaultRenderStateBlock( CPUTRenderStateBlock *pBlock ) { SAFE_RELEASE( mpDefaultRenderStateBlock ); mpDefaultRenderStateBlock = pBlock; }

    virtual CPUTResult LoadRenderStateBlock(const std::string &fileName) = 0;
    virtual void SetRenderStates() = 0;
    virtual void CreateNativeResources() = 0;
};

//-----------------------------------------------------------------------------
class StringToIntMapEntry
{
public:
    std::string mName;
    int     mValue;

	// Why not std::map ????

    bool FindMapEntryByName( int *pValue, const std::string name ) const{
        StringToIntMapEntry const *pstart = this;
        for( StringToIntMapEntry const *pEntry = pstart; pEntry->mName.length() > 0; pEntry++ ){
#ifndef CPUT_OS_WINDOWS
            if( 0 == strcasecmp( pEntry->mName.data(), name.data() ) )
#else
            if( 0 == _stricmp( pEntry->mName.data(), name.data() ) )
#endif
			{
                *pValue = pEntry->mValue;
                return true;
            }
        }
        return false;
    }
};
void SetRenderStateBlock(CPUTRenderStateBlock* pNew, CPUTRenderStateBlock* pOld);

#endif // _CPUTRENDERSTATEBLOCK_H
