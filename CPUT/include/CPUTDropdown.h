/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __CPUTDROPDOWN_H__
#define __CPUTDROPDOWN_H__

#include "CPUTControl.h"

#include <vector>


typedef enum CPUTDropdownGUIState
{
    CPUT_DROPDOWN_GUI_MOUSE_NEUTRAL,
    CPUT_DROPDOWN_GUI_MOUSE_PRESSED,
} CPUTDropdownGUIState;

// forward declarations
struct CPUTGUIVertex;
class CPUTFont;
class CPUTText;

#define CPUT_NUM_IMAGES_IN_DROPDOWN_ARRAY 12
#define CPUT_NUM_QUADS_IN_DROPDOWN_ARRAY 18
#define CPUT_NUM_QUADS_IN_CLOSED_DROPDOWN 9
#define CPUT_DROPDOWN_TEXT_PADDING 2

const int CLeftTop = 0;
const int CLeftMid = 1;
const int CLeftBot = 2;
const int CMidTop = 3;
const int CMidMid = 4;
const int CMidBot = 5;
const int CRightTop = 6;
const int CRightMid = 7;
const int CRightBot = 8;
const int CButtonUp = 9;
const int CButtonDown = 10;

#define CPUT_NUM_IMAGES_IN_DROPDOWN  12


// Dropdown base - common functionality for all dropdown controls
//-----------------------------------------------------------------------------
class CPUTDropdown:public CPUTControl
{
public:
    // button should self-register with the GuiController on create
    CPUTDropdown(CPUTDropdown& copy);
	static CPUTDropdown* Create(const std::string controlName, CPUTControlID controlID, CPUTFont * mpFont);

    virtual ~CPUTDropdown();

    // CPUTControl
    virtual void GetPosition(int &x, int &y);
    virtual unsigned int GetOutputVertexCount();

    //CPUTEventHandler
    virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key, CPUTKeyState state){UNREFERENCED_PARAMETER(key);return CPUT_EVENT_UNHANDLED;}  
    
    //CPUTEventHandler
    virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state, CPUTEventID message);

    //CPUTControl
    virtual void SetEnable(bool in_bEnabled);
    virtual bool IsEnabled();
    virtual bool ContainsPoint(int x, int y);
    virtual bool ContainsPointReadoutArea(int x, int y);
    virtual bool ContainsPointTrayArea(int x, int y);
    virtual void SetText(std::string ControlText) {;} // doesn't apply to this control
    virtual void GetText(std::string &ControlString) {UNREFERENCED_PARAMETER(ControlString);} // doesn't apply to this control
    virtual void GetDimensions(int &width, int &height);
    virtual void SetPosition(int x, int y);

    void NumberOfSelectableItems(int &count);
    void GetSelectedItem(int &index);
    void GetSelectedItem(std::string &Item);
    void SetSelectedItem(const unsigned int index);
    CPUTResult AddSelectionItem(const std::string Item, bool IsSelected=false);
    void DeleteSelectionItem(const unsigned int index);
    void DeleteSelectionItem(const std::string string);
    
    // Draw
    virtual void Draw(CPUTGUIVertex *pVertexBufferMirror, UINT *pInsertIndex, UINT pMaxBufferSize);

    // Register assets
    static CPUTResult RegisterStaticResources();
    static CPUTResult UnRegisterStaticResources();

    CPUTResult RegisterInstanceResources();
    CPUTResult UnRegisterInstanceResources();

protected:

	CPUTDropdown(const std::string controlName, CPUTControlID id, CPUTFont *pFont);

    CPUT_RECT               mControlDimensions;
    CPUTGUIControlState     mControlState;
    CPUTDropdownGUIState    mControlGuiState;
    void InitialStateSet();

    // uber-buffer adds
    CPUTFont *mpFont;
    CPUTText *mpButtonText;
    static CPUT_SIZE mpDropdownIdleImageSizeList[CPUT_NUM_IMAGES_IN_DROPDOWN];    
    static CPUT_SIZE mpDropdownDisabledSizeList[CPUT_NUM_IMAGES_IN_DROPDOWN];

    // uber-buffer per-instance
    CPUT_SIZE mpDropdownIdleSizeList[CPUT_NUM_IMAGES_IN_DROPDOWN];    
    CPUT_SIZE mpDropdownDisabledList[CPUT_NUM_IMAGES_IN_DROPDOWN];

    CPUTGUIVertex *mpMirrorBufferActive;
    CPUTGUIVertex *mpMirrorBufferDisabled;
    void Recalculate();
    
    // instance data
    
    CPUT_RECT mButtonRect;
    CPUT_RECT mReadoutRectInside;
    CPUT_RECT mReadoutRectOutside;

    CPUT_RECT mTrayDimensions;
    bool mbMouseInside;
    int mRevertItem;
    bool mbStartedClickInside;
    bool mbStartedClickInsideTray;

    // list of items in the dropdown
    int mSelectedItem;            //zero-based index that tells what item is currently highlighted (soft selected)
    int mHighlightedItem;   //zero-based index that tells what item was last confirm-selected (hard selected)
    std::vector<CPUTText*> mpListOfSelectableItems;
    CPUTText *mpSelectedItemCopy;

    bool mbSizeDirty;
    UINT mVertexStride; // stride and offsets of the image quads we're drawing on
    UINT mVertexOffset;

    // helper functions
    void CalculateButtonRect(CPUT_RECT& button);
    void CalculateReadoutRect(CPUT_RECT& inner, CPUT_RECT& outer);
    void CalculateTrayRect(CPUT_RECT& inner, CPUT_RECT& outer);

    void CalculateReadoutTextPosition(int &x, int &y);
    void CalculateMaxItemSize(int &width, int &height);

    // sets which item is to be highlighted on a dropdown list
    void SetHighlightedItem(const int index);

    bool TopContainsPoint(int x, int y);
    bool TrayContainsPoint(int x, int y);
    int GetItem(int x, int y);    
};

#endif //#ifndef __CPUTDROPDOWN_H__
