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
#ifndef __CPUTCONFIGBLOCK_H__
#define __CPUTCONFIGBLOCK_H__

/*
    The ConfigFile class and associated subclasses provides the functionality to read data
    from any "configuration file" that follows the format:

    [Config Block 1]
    keyName1 = value
    keyName2 = value
    
    [Config Block 2]
    keyName1 = value
    keyName2 = value

    The ConfigFile consists of 1 or more ConfigBlocks which are named by the text in square
    brackets []. Each block consists of 0 or more ConfigEntry entries. These consist of a
    Name and a Value. These are read and store as text. Accessor functions are provided
    to read this data as different formats. The ConfigFile class does not make any determiniation
    as to what type of data it is. It stores it as text.
*/

#include "CPUT.h"
#include <algorithm> // for std::transform

//-----------------------------------------------------------------------------
class CPUTConfigEntry
{
private:
    std::string szName;
    std::string szValue;

    friend class CPUTConfigBlock;
    friend class CPUTConfigFile;

public:
    CPUTConfigEntry() {}
    CPUTConfigEntry(const std::string &name, const std::string &value): szName(name), szValue(value){};

    static const CPUTConfigEntry  &sNullConfigValue;

    const std::string & NameAsString(void){ return  szName;};
    const std::string & ValueAsString(void){ return szValue; }
    bool IsValid(void){ return !szName.empty(); }

    float ValueAsFloat(void)
    {
        float fValue=0;
        int retVal;
        retVal = sscanf(szValue.c_str(), "%g", &fValue ); // float (regular float, or E exponentially notated float)
        ASSERT(0!=retVal, "ValueAsFloat - value specified is not a float");
        return fValue;
    }
    int ValueAsInt(void)
    {
        int nValue=0;
        int retVal;
        retVal = sscanf(szValue.c_str(), "%d", &nValue ); // signed int (NON-hex)
        ASSERT(0!=retVal, "ValueAsInt - value specified is not a signed int");
        return nValue;
    }
    UINT ValueAsUint(void)
    {
        UINT nValue=0;
        int retVal;
        retVal = sscanf(szValue.c_str(), "%u", &nValue ); // unsigned int
        ASSERT(0!=retVal, "ValueAsUint - value specified is not a UINT");
        return nValue;
    }
    bool ValueAsBool(void)
    {
        return  (szValue.compare("true") == 0) || 
                (szValue.compare("1") == 0) || 
                (szValue.compare("t") == 0);
    }
    UINT ValueAsHex32(void)
    {
        UINT nValue = 0;
        int retVal;
        retVal = sscanf(szValue.c_str(), "%x", &nValue ); // unsigned int
        ASSERT(0!=retVal, "ValueAsUint - value specified is not a hex");
        return nValue;
    }
    
    void ValueAsString(std::string *stringOut)
    {
        *stringOut = szValue;
    }

    void ValueAsFloatArray(float *pFloats, int count);
};

//-----------------------------------------------------------------------------
class CPUTConfigBlock
{
    friend class CPUTConfigFile;

public:
    CPUTConfigBlock();
    ~CPUTConfigBlock();

    CPUTConfigEntry *AddValue(const std::string &szName, const std::string &szValue);
    CPUTConfigEntry *GetValue(int nValueIndex);
    CPUTConfigEntry *GetValueByName(const std::string &szName);
    const std::string &GetName(void);
    int GetNameValue(void);
    int ValueCount(void);
    bool IsValid();
private:
    CPUTConfigEntry mpValues[64];
    CPUTConfigEntry mName;
    std::string     mszName;
    int             mnValueCount;
};

//-----------------------------------------------------------------------------
class CPUTConfigFile
{
public:
    CPUTConfigFile();
    ~CPUTConfigFile();

    CPUTResult LoadFile(const std::string &szFilename);
    CPUTConfigBlock *GetBlock(int nBlockIndex);
    CPUTConfigBlock *GetBlockByName(const std::string &szBlockName);
    int BlockCount(void);

private:
    CPUTConfigFile(const CPUTConfigFile &);
    CPUTConfigFile & operator=(const CPUTConfigFile &);

    CPUTConfigBlock    *mpBlocks;
    int                 mnBlockCount;
};

#endif //#ifndef __CPUTCONFIGBLOCK_H__
