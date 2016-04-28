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
#ifndef __CPUTCHECKBOX_H__
#define __CPUTCHECKBOX_H__

#include "CPUTControl.h" 

// forward declarations
struct CPUTGUIVertex;
class CPUTFont;
class CPUTText;


typedef enum CPUTCheckboxState
{
    CPUT_CHECKBOX_UNCHECKED,
    CPUT_CHECKBOX_CHECKED,
} CPUTCheckboxState;

typedef enum CPUTCheckboxGUIState
{
    CPUT_CHECKBOX_GUI_NEUTRAL,
    CPUT_CHECKBOX_GUI_PRESSED,
} CPUTCheckboxGUIState;

const int CPUT_CHECKBOX_NUM_IMAGES_IN_CHECKBOX=3;
const int CPUT_CHECKBOX_PADDING=5;          // padding (in pixels) between checkbox image and the label

// Checkbox 
//-----------------------------------------------------------------------------
class CPUTCheckbox:public CPUTControl
{
public:
	static CPUTCheckbox* Create(const std::string CheckboxText, CPUTControlID controlID, CPUTFont * mpFont, float scale);
    virtual ~CPUTCheckbox();

    // CPUTControl
    virtual void GetPosition(int &x, int &y);
    virtual unsigned int GetOutputVertexCount();

    // CPUTCheckboxDX11
    virtual CPUTCheckboxState GetCheckboxState();
    void SetCheckboxState(CPUTCheckboxState State);
    
    
    //CPUTEventHandler
    virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state);
    virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);

    //CPUTControl
    virtual bool ContainsPoint(int x, int y);
    virtual void SetPosition(int x, int y);
    virtual void GetDimensions(int &width, int &height);
    virtual void SetText(const std::string String);
    virtual void GetText(std::string &TextString);
    virtual void SetEnable(bool bEnabled);

    // Register assets
    static CPUTResult RegisterStaticResources();
    static CPUTResult UnRegisterStaticResources();

    CPUTResult RegisterInstanceResources();
    CPUTResult UnRegisterInstanceResources();

    // CPUTCheckboxDX11    
    virtual void Draw(CPUTGUIVertex *pVertexBufferMirror, UINT *pInsertIndex, UINT pMaxBufferSize);


protected:
    CPUT_RECT               mControlDimensions;
    CPUTCheckboxState       mCheckboxState;
    CPUTCheckboxGUIState    mCheckboxGuiState;
    float                   mScale;
    // helper functions
    void InitialStateSet();

    CPUTCheckbox(const std::string ControlText, CPUTControlID id, CPUTFont *pFont, float scale = 1.0);
	
    // button should self-register with the GuiController on create
    CPUTCheckbox(CPUTCheckbox& copy);


    // new for uber-buffer version
    CPUTFont *mpFont;
    CPUTGUIVertex *mpMirrorBufferActive;;
    CPUTGUIVertex *mpMirrorBufferPressed;
    CPUTGUIVertex *mpMirrorBufferDisabled;
    void Recalculate();

    // static varibles used by ALL checkbox controls    
    static CPUT_SIZE mpCheckboxTextureSizeList[CPUT_CHECKBOX_NUM_IMAGES_IN_CHECKBOX];

    // GUI state
    bool mbMouseInside;
    bool mbStartedClickInside;

    // instance variables for this particular checkbox
    UINT mVertexStride;
    UINT mVertexOffset;    
    CPUTText *mpCheckboxText;

    // helper functions
    void GetTextPosition(int &x, int &y);
    void CalculateBounds();
};

#endif //#ifndef __CPUTCHECKBOX_H__
