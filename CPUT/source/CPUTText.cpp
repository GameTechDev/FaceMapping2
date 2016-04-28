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
#include "CPUTText.h"
#include "CPUTFont.h"
#include <string.h>
#include <algorithm>

//// texture atlas information
//float gTextAtlasWidth = 750.0f;
//float gTextAtlasHeight = 12.0f;

// Constructor
//------------------------------------------------------------------------------
CPUTText::CPUTText(CPUTFont *pFont):mVertexStride(0),
    mVertexOffset(0),
	mpFont(pFont),
	mZDepth(1.0f),
    mpMirrorBuffer(NULL),
    mNumCharsInString(0)
{
    // initialize the state variables
    InitialStateSet();

    mQuadSize.height=0; mQuadSize.width=0;
    mPosition.x=0; mPosition.y=0;
    mStaticText.clear();
}

// Constructor
//-----------------------------------------------------------------------------
CPUTText::CPUTText(std::string const& String, CPUTControlID id, CPUTFont *pFont):mVertexStride(0),
    mVertexOffset(0),
	mpFont(pFont),
	mZDepth(1.0f),
    mpMirrorBuffer(NULL),
    mNumCharsInString(0)
{
    // initialize the state variables
    InitialStateSet();

    // save the control ID for callbacks
    mControlID = id;

    // set as enabled
    CPUTControl::SetEnable(true);

    // reset position/sizes
    mQuadSize.height=0; mQuadSize.width=0;
    mPosition.x=0; mPosition.y=0;

    // set the text
    SetText(String);

    // store the control id
    mControlID = id;
}

// Initial state of the control's member variables
//-----------------------------------------------------------------------------
void CPUTText::InitialStateSet()
{
    mControlType = CPUT_TEXT;
    mStaticState = CPUT_CONTROL_ACTIVE;

    mPosition.x=0; mPosition.y=0;
    mQuadSize.width=0; mQuadSize.height=0;
    mDimensions.x=0; mDimensions.y=0; mDimensions.width=0; mDimensions.height=0;
}

// Destructor
//------------------------------------------------------------------------------
CPUTText::~CPUTText()
{
    ReleaseInstanceData();
}


// Return the dimensions of this static text object (in pixels)
//--------------------------------------------------------------------------------
void CPUTText::GetDimensions(int &width, int &height)
{
    width = mQuadSize.width;
    height = mQuadSize.height;
}

// Return the screen position of this static text object (in pixels)
//--------------------------------------------------------------------------------
void CPUTText::GetPosition(int &x, int &y)
{
    x = mPosition.x;
    y = mPosition.y;
}


// fills user defined buffer with static string
//--------------------------------------------------------------------------------
void CPUTText::GetString(std::string &ButtonText)
{

    // fill user defined buffer with the string
    ButtonText = mStaticText;
}


// Enable/disable the text
//--------------------------------------------------------------------------------
void CPUTText::SetEnable(bool in_bEnabled) 
{
    // set as enabled
    CPUTControl::SetEnable(in_bEnabled);

    // recalculate
    Recalculate();
}




// Release all instance data
//--------------------------------------------------------------------------------
void CPUTText::ReleaseInstanceData()
{
    SAFE_DELETE_ARRAY(mpMirrorBuffer);
}

//--------------------------------------------------------------------------------
CPUTResult CPUTText::RegisterInstanceData()
{
    CPUTResult result = CPUT_SUCCESS;

    return result;
}


// Register all static assets (used by all CPUTText objects)
//--------------------------------------------------------------------------------
CPUTResult CPUTText::RegisterStaticResources()
{
    return CPUT_SUCCESS;
}

//
//--------------------------------------------------------------------------------
CPUTResult CPUTText::UnRegisterStaticResources()
{
    return CPUT_SUCCESS;
}

//
//--------------------------------------------------------------------------------
void CPUTText::SetPosition(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;

    Recalculate();
}


//--------------------------------------------------------------------------------
void CPUTText::Draw(CPUTGUIVertex *pVertexBufferMirror, UINT *pInsertIndex, UINT pMaxBufferSize)
{
    if(!mControlVisible)
    {
        return;
    }

    if((NULL==pVertexBufferMirror) || (NULL==pInsertIndex))
    {
        return;
    }

    if(!mpMirrorBuffer)
    {
        return;
    }
#ifdef _DEBUG
    // Do we have enough room to put the text vertexes into the output buffer?
    int VertexCopyCount = GetOutputVertexCount();
    ASSERT( (pMaxBufferSize >= *pInsertIndex + VertexCopyCount), "Too many characters to fit in allocated GUI string vertex buffer.\n\nIncrease CPUT_GUI_BUFFER_STRING_SIZE size." );
#endif
    // copy the string quads into the target buffer
    memcpy(&pVertexBufferMirror[*pInsertIndex], mpMirrorBuffer, sizeof(CPUTGUIVertex)*6*mNumCharsInString);
    *pInsertIndex+=6*mNumCharsInString;
}


// Calculate the number of verticies will be needed to display this string
//--------------------------------------------------------------------------------
int CPUTText::GetOutputVertexCount()
{
    // A string is made of one quad per character (including spaces)
    //
    //   ---
    //  | 1 |
    //   ---

    //
    // calculation: (number of characters in string) * 3 verticies/triangle * 2 triangles/quad * 1 quad/character
    return mNumCharsInString * (2*3);
}

// using the supplied font, build up a quad for each character in the string
// and assemble the quads into a vertex buffer ready for drawing/memcpy
//--------------------------------------------------------------------------------
void CPUTText::Recalculate()
{
    SAFE_DELETE_ARRAY(mpMirrorBuffer);

	mNumCharsInString = (int) mStaticText.size();
    mpMirrorBuffer = new CPUTGUIVertex[(mNumCharsInString+1)*6];

	int width, height;
    mpFont->LayoutText(mpMirrorBuffer, &width, &height, mStaticText, mPosition.x, mPosition.y);
    for (int i = 0; i < mNumCharsInString * 6; i++)
    {
        mpMirrorBuffer[i].Color = mColor;
        //fixme hack for second texture
        mpMirrorBuffer[i].UV = mpMirrorBuffer[i].UV*-1.0f;
    }

    mQuadSize.height = height;
    mQuadSize.width = width;
}

// Register quad for drawing string on
//--------------------------------------------------------------------------------
CPUTResult CPUTText::SetText(std::string const& String, float depth)
{
    HEAPCHECK;

    mStaticText = String;
    mZDepth = depth;

    // call recalculate function to generate new quad
    // list to display this text
    Recalculate();

    HEAPCHECK;
    return CPUT_SUCCESS;
}

CPUTResult CPUTText::SetTextf(const char *fmt, ...)
{
	char txt[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(txt, fmt, args);
	va_end(args);
	return SetText(txt);

}

void CPUTText::SetColor(float r, float g, float b, float a)
{
	mColor.r = r;
	mColor.g = g;
	mColor.b = b;
	mColor.a = a;
}

CPUTText * CPUTText::Create(CPUTFont * mpFont)
{
	return new CPUTText(mpFont);
}
CPUTText* CPUTText::Create(std::string const& String, CPUTControlID id, CPUTFont *pFont) {
	return new CPUTText(String, id, pFont);
}
