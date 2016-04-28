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
#include "CPUTCheckbox.h"
#include "CPUTText.h"

CPUT_SIZE CPUTCheckbox::mpCheckboxTextureSizeList[CPUT_CHECKBOX_NUM_IMAGES_IN_CHECKBOX] = { {0,0},{0,0},{0,0} };

// texture atlas information
float gAtlasWidthCheckbox = 256.0f;
float gAtlasHeightCheckbox = 64.0f;

// Pixel coordinates of the active-idle button image within the texture atlas
int gUVLocationsCheckbox_active[] = { 
    109,3,  // tl
    124,3,  // tr
    109,17, // bl
    124,17  // br  
};

// Pixel coordinates of the pressed button image within the texture atlas
int gUVLocationsCheckbox_pressed[] = { 
    109,20, // tl
    124,20, // tr
    109,35, // bl
    124,35  // br
};

// Pixel coordinates of the disabled button image within the texture atlas
int gUVLocationsCheckbox_disabled[] = { 
    109,39, // tl
    124,39, // tr
    109,54, // bl
    124,54  // br
};

// Floating point 0.0f - 1.0f UV coordinates in the texture atlas for each corner of the image
float3 mpUVCoordsCheckbox_active[4];
float3 mpUVCoordsCheckbox_pressed[4];
float3 mpUVCoordsCheckbox_disabled[4];



// Constructor
//-----------------------------------------------------------------------------
CPUTCheckbox::CPUTCheckbox(const std::string ControlText, CPUTControlID id, CPUTFont *pFont, float scale) :
    mbMouseInside(false),
    mbStartedClickInside(false),
    mVertexStride(0),
    mVertexOffset(0),
    mpCheckboxText(NULL),
    mpFont(pFont),
    mScale(scale),
    mpMirrorBufferActive(NULL),
    mpMirrorBufferPressed(NULL),
    mpMirrorBufferDisabled(NULL)
{
    // initialize the state variables
    InitialStateSet();

    // save the control ID for callbacks
    mControlID = id;

    // store the font to be used by this control
    mpFont = pFont;

    // set as enabled
    CPUTControl::SetEnable(true);

    // register all the instance resources
    RegisterInstanceResources();

    // set the control's text string
    SetText(ControlText);
}

// Initial state of the control's member variables
//-----------------------------------------------------------------------------
void CPUTCheckbox::InitialStateSet()
{
    // state
    mControlType = CPUT_CHECKBOX;
    mControlState = CPUT_CONTROL_ACTIVE;
    mCheckboxState = CPUT_CHECKBOX_UNCHECKED;
    mCheckboxGuiState = CPUT_CHECKBOX_GUI_NEUTRAL;

    // control ID
    mControlID = 0;

    // size
    mControlDimensions.x=0;
    mControlDimensions.y=0;
    mControlDimensions.height=0;
    mControlDimensions.width=0;

    // default label
    SetText("checkbox");
}

// Destructor
//------------------------------------------------------------------------------
CPUTCheckbox::~CPUTCheckbox()
{
    UnRegisterInstanceResources();
}

// Get the x/y window position of this control
//-----------------------------------------------------------------------------
void CPUTCheckbox::GetPosition(int &x, int &y)
{
    x = mControlDimensions.x;
    y = mControlDimensions.y;
}


// Get checkbox selection state
//-----------------------------------------------------------------------------
CPUTCheckboxState CPUTCheckbox::GetCheckboxState()
{
    return mCheckboxState;
}

// Set checkbox selection state
//-----------------------------------------------------------------------------
void CPUTCheckbox::SetCheckboxState(CPUTCheckboxState State)
{
    // if state changed, save it, and recalculate the control
    if(State != mCheckboxState)
    {
        mCheckboxState = State;
        Recalculate();
    }
}

// Returns the number of quads needed to draw this control
//--------------------------------------------------------------------------------
unsigned int CPUTCheckbox::GetOutputVertexCount()
{
    // A checkbox is always made of 1 quad.
    //
    //   ---
    //  | 1 |            
    //   ---
    // Calculation: 3 quads/triangle * 2 triangles/quad * 1 quad

    return 3*2;
}







// Register assets
//-----------------------------------------------------------------------------
CPUTResult CPUTCheckbox::RegisterStaticResources()
{
    // calculate the floating point, 0.0f - 1.0f, UV coordinates of each of the 9 images that
    // make up a button.  Do this for the active, pressed, and disabled states.
    for(int ii=0; ii<4; ii++)
    {
        mpUVCoordsCheckbox_active[ii].x = gUVLocationsCheckbox_active[2*ii]/gAtlasWidthCheckbox;
        mpUVCoordsCheckbox_active[ii].y = gUVLocationsCheckbox_active[2*ii+1]/gAtlasHeightCheckbox;

        mpUVCoordsCheckbox_pressed[ii].x = gUVLocationsCheckbox_pressed[2*ii]/gAtlasWidthCheckbox;
        mpUVCoordsCheckbox_pressed[ii].y = gUVLocationsCheckbox_pressed[2*ii+1]/gAtlasHeightCheckbox;

        mpUVCoordsCheckbox_disabled[ii].x = gUVLocationsCheckbox_disabled[2*ii]/gAtlasWidthCheckbox;
        mpUVCoordsCheckbox_disabled[ii].y = gUVLocationsCheckbox_disabled[2*ii+1]/gAtlasHeightCheckbox;
    }



    // calculate the width/height in pixels of each of the image slices
    // that makes up the checkbox images
    mpCheckboxTextureSizeList[0].width = gUVLocationsCheckbox_active[2] - gUVLocationsCheckbox_active[0];
    mpCheckboxTextureSizeList[0].height = gUVLocationsCheckbox_active[5] - gUVLocationsCheckbox_active[1];

    mpCheckboxTextureSizeList[1].width = gUVLocationsCheckbox_pressed[2] - gUVLocationsCheckbox_pressed[0];
    mpCheckboxTextureSizeList[1].height = gUVLocationsCheckbox_pressed[5] - gUVLocationsCheckbox_pressed[1];

    mpCheckboxTextureSizeList[2].width = gUVLocationsCheckbox_disabled[2] - gUVLocationsCheckbox_disabled[0];
    mpCheckboxTextureSizeList[2].height = gUVLocationsCheckbox_disabled[5] - gUVLocationsCheckbox_disabled[1];

    return CPUT_SUCCESS;
}

// Release all static resources - only do this if NO more checkbox controls
// are used anywhere on the system
//-----------------------------------------------------------------------------
CPUTResult CPUTCheckbox::UnRegisterStaticResources()
{
    return CPUT_SUCCESS;
}

// Register any per-instance resources for this checkbox
//-----------------------------------------------------------------------------
CPUTResult CPUTCheckbox::RegisterInstanceResources()
{
    // clear any previously allocated buffers
    SAFE_DELETE_ARRAY(mpMirrorBufferActive);
    SAFE_DELETE_ARRAY(mpMirrorBufferPressed);
    SAFE_DELETE_ARRAY(mpMirrorBufferDisabled);

    // allocate the per-instance sizes (each button will have different dimensions)
    mpMirrorBufferActive = new CPUTGUIVertex[6];
    mpMirrorBufferPressed = new CPUTGUIVertex[6];
    mpMirrorBufferDisabled = new CPUTGUIVertex[6];
    return CPUT_SUCCESS;
}

// Unregister the checkbox's instance resources
//-----------------------------------------------------------------------------
CPUTResult CPUTCheckbox::UnRegisterInstanceResources()
{
    // delete the static text object
    SAFE_DELETE_ARRAY(mpMirrorBufferActive);
    SAFE_DELETE_ARRAY(mpMirrorBufferPressed);
    SAFE_DELETE_ARRAY(mpMirrorBufferDisabled);
    SAFE_DELETE(mpCheckboxText);

    return CPUT_SUCCESS;
}

//CPUTEventHandler
// Handle keyboard events - none for this control
//--------------------------------------------------------------------------------
CPUTEventHandledCode CPUTCheckbox::HandleKeyboardEvent(CPUTKey key, CPUTKeyState state)
{
    UNREFERENCED_PARAMETER(key);
    return CPUT_EVENT_UNHANDLED;
}

// Handle mouse events
//-----------------------------------------------------------------------------
CPUTEventHandledCode CPUTCheckbox::HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message)
{
    UNREFERENCED_PARAMETER(wheel);
    CPUTEventHandledCode handledCode = CPUT_EVENT_UNHANDLED;

    if((CPUT_CONTROL_INACTIVE == mControlState) || (false == mControlVisible) )
    {
        mbMouseInside = false;
        return handledCode;
    }

    // if we're continuing to be pressed, move around with the mouse movement

    if( (CPUT_CHECKBOX_GUI_PRESSED == mCheckboxGuiState) && (CPUT_MOUSE_LEFT_DOWN == state))
    {
        return CPUT_EVENT_HANDLED;
    }

    // handle events occuring in the control
    if( ContainsPoint(x,y) )
    {
        // did we start our click inside the button?
        if((state & CPUT_MOUSE_LEFT_DOWN) && (true == mbMouseInside))
        {
            mbStartedClickInside = true;
            mCheckboxGuiState = CPUT_CHECKBOX_GUI_PRESSED;
            handledCode = CPUT_EVENT_HANDLED;
            SetFocus(true);
        }

        // did they click inside the button?
        if(!(state & CPUT_MOUSE_LEFT_DOWN) && (true == mbStartedClickInside) && (CPUT_CHECKBOX_GUI_PRESSED == mCheckboxGuiState))
        {
            // set the GUI/mouse controller states
            handledCode = CPUT_EVENT_HANDLED;
            mCheckboxGuiState = CPUT_CHECKBOX_GUI_NEUTRAL;
            SetFocus(false);
            // toggle the checkbox state
            if(CPUT_CHECKBOX_UNCHECKED == mCheckboxState)
            {
                mCheckboxState = CPUT_CHECKBOX_CHECKED;
            }
            else if(CPUT_CHECKBOX_CHECKED == mCheckboxState)
            {
                mCheckboxState = CPUT_CHECKBOX_UNCHECKED;
            }

            // trigger the users callback
            mpCallbackHandler->HandleCallbackEvent(CPUT_EVENT_CLICK, mControlID, (CPUTControl*) this);
        }
        if(!(state & CPUT_MOUSE_LEFT_DOWN))
        {
            mbMouseInside = true;
        }
    }
    else
    {
        // we left the button
        mbMouseInside = false;
        mCheckboxGuiState = CPUT_CHECKBOX_GUI_NEUTRAL;
        mbStartedClickInside = false;
        SetFocus(false);
    }

    return handledCode;
}

//CPUTControl
// set the upper-left position of the checkbox control (screen space coords)
//-----------------------------------------------------------------------------
void CPUTCheckbox::SetPosition(int x, int y)
{
    // set the new position
    mControlDimensions.x = x;
    mControlDimensions.y = y;

    // recalculate the vertex buffer with new x/y coords
    Recalculate();

    // move the static text along with the bitmap graphic
    int textX, textY;
    GetTextPosition(textX, textY);
    mpCheckboxText->SetPosition(textX, textY);
}

//-----------------------------------------------------------------------------
void CPUTCheckbox::GetDimensions(int &width, int &height)
{
    CalculateBounds();
    width = mControlDimensions.width;
    height = mControlDimensions.height;
}

// Get the text label on this checkbox
//--------------------------------------------------------------------------------
void CPUTCheckbox::GetText(std::string &TextString)
{
    if(mpCheckboxText)
    {
        mpCheckboxText->GetString(TextString);
    }
}

// Sets the text label on this checkbox
//--------------------------------------------------------------------------------
void CPUTCheckbox::SetText(const std::string String)
{
    // create the static text object if it doesn't exist
    if(NULL == mpCheckboxText)
    {
        mpCheckboxText = CPUTText::Create(mpFont);
    }

    // set the static control's text
    mpCheckboxText->SetText(String);

    // move the text to the right spot
    int x,y;
    GetTextPosition(x,y);
    mpCheckboxText->SetPosition(x, y);
}

// Enable/disable the control
//--------------------------------------------------------------------------------
void CPUTCheckbox::SetEnable(bool in_bEnabled)
{
    // chain to CPUTControl 
    CPUTControl::SetEnable(in_bEnabled);

    // set the control's text to match
    mpCheckboxText->SetEnable(in_bEnabled);

    // recalculate control's quads 
    Recalculate();
}

// With the given window x/y coordinate, is that point in this control
//-----------------------------------------------------------------------------
bool CPUTCheckbox::ContainsPoint(int x, int y)
{   
    if( (x>mControlDimensions.x) && (y>mControlDimensions.y) &&
        (x< (mControlDimensions.x+mControlDimensions.width)) && (y< (mControlDimensions.y+mControlDimensions.height))
        )
    {
        return true;
    }

    return false;
}

// Calculate the bounding rectangle for the control
// For the checkbox it includes the checkbox image and the text
//-----------------------------------------------------------------------------
void CPUTCheckbox::CalculateBounds()
{
    int textX, textY;
    int textWidth, textHeight;

    // get the text
    GetTextPosition(textX, textY);
    mpCheckboxText->GetDimensions(textWidth, textHeight);

    mControlDimensions.width = (textX - mControlDimensions.x ) + textWidth;
    mControlDimensions.height = textHeight;

    if(mpCheckboxTextureSizeList[0].height * mScale > textHeight)
    {
        mControlDimensions.height = (int)(mpCheckboxTextureSizeList[0].height * mScale);
    }
}

// Calculate the correct location to place the text label - usually to the 
// right of the image allowing for spacing/image/etc
//--------------------------------------------------------------------------------
void CPUTCheckbox::GetTextPosition(int &x, int &y)
{
    // get the dimensions of the string in pixels
    CPUT_RECT TextRect;
    mpCheckboxText->GetDimensions(TextRect.width, TextRect.height);

    // calculate a good spot for the text to be in relation to the checkbox bitmap
    x = mControlDimensions.x + (int)(mpCheckboxTextureSizeList[0].width * mScale  + CPUT_CHECKBOX_PADDING); // move right far enough not to overlap the bitmap
    y = mControlDimensions.y;
}


// 'Draw' this control into the supplied vertex buffer object
//--------------------------------------------------------------------------------
void CPUTCheckbox::Draw(CPUTGUIVertex *pVertexBufferMirror, UINT *pInsertIndex, UINT pMaxBufferSize)
{
    if(!mControlVisible)
    {
        return;
    }

    if((NULL==pVertexBufferMirror) || (NULL==pInsertIndex))
    {
        return;
    }

#ifdef _DEBUG
    // Do we have enough room to put this control into the output buffer?
    int VertexCopyCount = GetOutputVertexCount();
    ASSERT( (pMaxBufferSize >= *pInsertIndex + VertexCopyCount), "Too many CPUT GUI controls for allocated GUI buffer. Allocated GUI vertex buffer is too small.\n\nIncrease CPUT_GUI_BUFFER_SIZE size." );
#endif
    switch(mControlState)
    {
    case CPUT_CONTROL_ACTIVE:
        // copy the active+idle button into the stream
        if(CPUT_CHECKBOX_UNCHECKED == mCheckboxState)
        {
            memcpy(&pVertexBufferMirror[*pInsertIndex], mpMirrorBufferActive, sizeof(CPUTGUIVertex)*6);
        }        

        // copy the pressed button into the stream
        if(CPUT_CHECKBOX_CHECKED == mCheckboxState)
        {
            memcpy(&pVertexBufferMirror[*pInsertIndex], mpMirrorBufferPressed, sizeof(CPUTGUIVertex)*6);
        }
        
        break;
    case CPUT_CONTROL_INACTIVE:
        // copy the inactive button into the stream
        memcpy(&pVertexBufferMirror[*pInsertIndex], mpMirrorBufferDisabled, sizeof(CPUTGUIVertex)*6);
        break;

    default:
        // error! unknown state
        ASSERT(0,"CPUTCheckbox: Control is in unknown state");
        return;
    }

    // move the index the correct number of floats to account
    // for 1 new quad, each quad with 6 verts in it (and each vert with 3+2 floats in it).
    *pInsertIndex+=6;
    if(mpCheckboxText)
    {
        mpCheckboxText->Draw(pVertexBufferMirror, pInsertIndex, pMaxBufferSize);
    }
}

// Recalculates the the control's image quads
//------------------------------------------------------------------------
void CPUTCheckbox::Recalculate()
{
    // active/idle
    float height = (float) mpCheckboxTextureSizeList[0].height * mScale;
    float width = (float) mpCheckboxTextureSizeList[0].width * mScale;
    float xPos = (float) mControlDimensions.x;
    float yPos = ((float)mControlDimensions.height - height)/2.0f + (float)mControlDimensions.y;
    AddQuadIntoMirrorBuffer(mpMirrorBufferActive, 0, xPos, yPos, width, height,
        mpUVCoordsCheckbox_active[0], mpUVCoordsCheckbox_active[3], mColor
                        );

    // pressed
    AddQuadIntoMirrorBuffer(mpMirrorBufferPressed, 0, xPos, yPos, width, height,
        mpUVCoordsCheckbox_pressed[0], mpUVCoordsCheckbox_pressed[3], mColor
                        );

    // disabled
    AddQuadIntoMirrorBuffer(mpMirrorBufferDisabled, 0, xPos, yPos, width, height,
        mpUVCoordsCheckbox_disabled[0], mpUVCoordsCheckbox_disabled[3], mColor
                        );

    // re-calculate the bounding box for the control used for hit-testing/sizing
    CalculateBounds();
}

CPUTCheckbox* CPUTCheckbox::Create(const std::string CheckboxText, CPUTControlID controlID, CPUTFont * mpFont, float scale)
{
	return new CPUTCheckbox(CheckboxText, controlID, mpFont, scale);
}
