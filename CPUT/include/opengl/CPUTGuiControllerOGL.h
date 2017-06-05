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
#ifndef __CPUTGUICONTROLLEROGL_H__
#define __CPUTGUICONTROLLEROGL_H__

#include "CPUTGuiController.h"

class CPUTMaterial;
class CPUTBufferOGL;
class CPUTMeshOGL;


// the GUI controller class that dispatches the rendering calls to all the buttons
//--------------------------------------------------------------------------------
class CPUTGuiControllerOGL:public CPUTGuiController
{   
public:
    static CPUTGuiControllerOGL *GetController();
    static CPUTResult DeleteController();

    // initialization
    CPUTResult Initialize(const std::string& materialName, const std::string& fontName);
    
    // draw routines    
    void  Draw();
    void  UpdateConstantBuffer();

private:
    static CPUTGuiControllerOGL *mguiController; // singleton object

    CPUTMaterial *mpMaterial;

    CPUTBuffer *mpConstantBufferVS;
    CPUTMeshOGL *mpUberBuffer;
    
    CPUTResult UpdateUberBuffers();

    CPUTGuiControllerOGL();    // singleton
    ~CPUTGuiControllerOGL();
};

#endif // #ifndef __CPUTGUICONTROLLEROGL_H__
