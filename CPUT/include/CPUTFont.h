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
#ifndef __CPUTFONT_H__
#define __CPUTFONT_H__

#include "CPUT.h"
#include "CPUTRefCount.h"
class CPUTTexture;
#define CPUT_MAX_NUMBER_OF_CHARACTERS 256

struct BMFontInfo {
    int16_t  fontSize;
    uint8_t  bitField;
    uint8_t  charSet;
    uint16_t stretchH;
    uint8_t  aa;
    uint8_t  paddingUp;
    uint8_t  paddingRight;
    uint8_t  paddingDown;
    uint8_t  paddingLeft;
    uint8_t  spacingHoriz;
    uint8_t  spacingVert;
    uint8_t  outline;
    char     fontName[1];
};

struct BMFontCommon {
    uint16_t lineHeight;
    uint16_t base;
    uint16_t scaleW;
    uint16_t scaleH;
    uint16_t pages;
    uint8_t  bitField;
    uint8_t  alphaChannel;
    uint8_t  redChannel;
    uint8_t  greenChannel;
    uint8_t  blueChannel;
};

struct BMFontPages {
    char pageNames[1];
};


struct BMFontChars {
    uint32_t id;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    int16_t  xoffset;
    int16_t  yoffset;
    int16_t  xadvance;
    uint8_t  page;
    uint8_t  channel;
};

struct BMFontKerningPairs {
    uint32_t first;
    uint32_t second;
    int16_t  amount;
};

//
// Because of padding, the sizeof the structure may not match the memory. The memory will be laid out properly (no padding).
// For example, to access the 3rd element of an array of BMFontChars you should do:
// (BMFontChars *)(((uint8_t *)(mFont.mpFontChars)) + 60)
//
// This is also true for mpFontKerningPairs
//
struct CPUTGUIVertex;

class CPUTFont : public CPUTRefCount
{
public:

    static CPUTFont *Create(const std::string& FontName, const std::string& AbsolutePathAndFilename);
    static CPUTFont *LoadBMFont(const std::string& FontName, const std::string& AbsolutePathAndFilename);

    void LayoutText(CPUTGUIVertex *pVtxBuffer, int *pWidth, int *pHeight, const std::string& text, int tlx, int tly);

    void SetFontScale(float scale) { mFontScale = scale; }

protected:
    BMFontInfo         *mpFontInfo;
    BMFontCommon       *mpFontCommon;
    BMFontPages        *mpFontPages;
    BMFontChars        *mpFontChars;
    BMFontKerningPairs *mpFontKerningPairs;
    uint32_t            mNumChars;
    uint32_t            mNumKerningPairs;
    float               mFontScale;

    ~CPUTFont();

private:
    CPUTFont();
    CPUTFont(const CPUTFont &);
    CPUTFont & operator=(const CPUTFont &);
};

#endif // #ifndef __CPUTFONT_H__