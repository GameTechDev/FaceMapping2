/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include "CPUTConfigBlock.h"
#include "CPUTOSServices.h"
//#include <regex>
#include <string>
//#include <string.h>

const CPUTConfigEntry  &CPUTConfigEntry::sNullConfigValue = CPUTConfigEntry("", "");

//----------------------------------------------------------------
static bool iswhite(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

template<typename T>
static void RemoveWhitespace(T &start, T &end)
{
    while (start < end && iswhite(*start))
    {
        ++start;
    }

    while (end > start && iswhite(*(end - 1)))
    {
        --end;
    }
}

//----------------------------------------------------------------
static bool ReadLine(const char **ppStart, const char **ppEnd, const char **ppCur)
{
    const char *pCur = *ppCur;
    if (!*pCur) // check for EOF
    {
        return false;
    }

    // We're at the start of a line now, skip leading whitespace
    while (*pCur == ' ' || *pCur == '\t')
    {
        ++pCur;
    }

    *ppStart = pCur;

    // Forward to the end of the line and keep track of last non-whitespace char
    const char *pEnd = pCur;
    for (;;)
    {
        char ch = *pCur++;
        if (!ch)
        {
            --pCur; // terminating NUL isn't consumed
            break;
        }
        else if (ch == '\n')
        {
            break;
        }
        else if (!iswhite(ch))
        {
            pEnd = pCur;
        }
    }

    *ppEnd = pEnd;
    *ppCur = pCur;
    return true;
}

//----------------------------------------------------------------
static const char *FindFirst(const char *start, const char *end, char ch)
{
    const char *p = start;
    while (p < end && *p != ch)
    {
        ++p;
    }
    return p;
}

static const char *FindLast(const char *start, const char *end, char ch)
{
    const char *p = end;
    while (--p >= start && *p != ch)
    {
    }

    return p;
}

//----------------------------------------------------------------
void CPUTConfigEntry::ValueAsFloatArray(float *pFloats, int count)
{
    char *szOrigValue = new char [szValue.length()+1];
	const char *pString = szValue.c_str();
    strcpy(szOrigValue, pString);

	char *szNewValue = NULL;
    char *szCurrValue = strtok_r(szOrigValue, " ", &szNewValue);
    for(int clear = 0; clear < count; clear++)
    {
        pFloats[clear] = 0.0f;
    }
    for(int ii=0;ii<count;++ii)
    {
        if(szCurrValue == NULL)
        {
			delete[] szOrigValue;
            return;
        }
        pFloats[ii] = (float) atof(szCurrValue);
        szCurrValue = strtok_r(NULL, " ", &szNewValue);

    }
    
	delete[] szOrigValue;
}

//----------------------------------------------------------------
CPUTConfigBlock::CPUTConfigBlock()
    : mnValueCount(0)
{
}
//----------------------------------------------------------------
CPUTConfigBlock::~CPUTConfigBlock()
{
}
//----------------------------------------------------------------
const std::string &CPUTConfigBlock::GetName(void)
{
    return mszName;
}
//----------------------------------------------------------------
int CPUTConfigBlock::GetNameValue(void)
{
    return mName.ValueAsInt();
}
//----------------------------------------------------------------
CPUTConfigEntry *CPUTConfigBlock::GetValue(int nValueIndex)
{
    if(nValueIndex < 0 || nValueIndex >= mnValueCount)
    {
        return NULL;
    }
    return &mpValues[nValueIndex];
}
//----------------------------------------------------------------
CPUTConfigEntry *CPUTConfigBlock::AddValue(const std::string &szName, const std::string &szValue )
{
    // TODO: What should we do if it already exists?
    CPUTConfigEntry *pEntry = &mpValues[mnValueCount++];
    pEntry->szName  = szName;
    pEntry->szValue = szValue;
    return pEntry;
}
//----------------------------------------------------------------
CPUTConfigEntry *CPUTConfigBlock::GetValueByName(const std::string &szName)
{
    for(int ii=0; ii<mnValueCount; ++ii)
    {
        const std::string &valName = mpValues[ii].szName;
        if(valName.size() != szName.size())
        {
            continue;
        }

        size_t j = 0;
        while (j < valName.size() && szName[j] == valName[j])
        {
            ++j;
        }

        if (j == valName.size()) // match
        {
            return &mpValues[ii];
        }
    }

    // not found - return an 'empty' object to avoid crashes/extra error checking
    return const_cast<CPUTConfigEntry*>(&CPUTConfigEntry::sNullConfigValue);
}
//----------------------------------------------------------------
int CPUTConfigBlock::ValueCount(void)
{
    return mnValueCount;
}
//----------------------------------------------------------------
CPUTConfigFile::CPUTConfigFile()
    : mnBlockCount(0)
    , mpBlocks(NULL)
{
}
//----------------------------------------------------------------
CPUTConfigFile::~CPUTConfigFile()
{
    if(mpBlocks)
    {
        delete [] mpBlocks;
        mpBlocks = 0;
    }
    mnBlockCount = 0;
}
//----------------------------------------------------------------
CPUTResult CPUTConfigFile::LoadFile(const std::string &szFilename)
{
    int nBytes = 0;
    char *pFileContents = NULL;
    CPUTResult result = CPUTFileSystem::ReadFileContents(szFilename, (UINT *)&nBytes, (void **)&pFileContents, true);
    if(CPUTFAILED(result))
    {
        DEBUG_PRINT("Failed to read file %s\n", szFilename.c_str());
        return result;
    }
    /*
    Not sure on compiler support across the board for regular expressions. This would simplify the parsing code

    std::string fileContents(pFileContents);
    std::smatch m;
    std::string blockTitle;
    auto dataStart = fileContents.cbegin();
    auto dataEnd = fileContents.cend();
    while( std::regex_search(dataStart, dataEnd, m, std::regex("\\[(.*)\\]([^\\[]*)")) ) {
        DEBUG_PRINT("match group 1: %s", m[1].str().c_str()); // m[1].str() is the block name
        DEBUG_PRINT("match group 2: %s", m[2].str().c_str()); // m[2].str() is the block contents
        dataStart = m.suffix().first;
//        mnBlockCount++;
//        blockTitle.assign(m[1]);
    }
    */
    CPUTConfigBlock    *pCurrBlock = NULL;
    int                 nCurrBlock = 0;
    
    /* Count the number of blocks */
    const char *pCur = pFileContents;
    const char *pStart, *pEnd;

    while(ReadLine(&pStart, &pEnd, &pCur))
    {
        const char *pOpen = FindFirst(pStart, pEnd, '[');
        const char *pClose = FindLast(pOpen + 1, pEnd, ']');
        if (pOpen < pClose)
        {
            // This line is a valid block header
            mnBlockCount++;
        }
    }

    // For files that don't have any blocks, just add the entire file to one block
    if(mnBlockCount == 0)
    {
        mnBlockCount   = 1;
    }

    pCur = pFileContents;
    mpBlocks = new CPUTConfigBlock[mnBlockCount];
    pCurrBlock = mpBlocks;

    /* Find the first block first */
    while(ReadLine(&pStart, &pEnd, &pCur))
    {
        // if the first character in the line is a '#' then skip that line.
        const char *pRemark = FindFirst(pStart, pEnd, '#');
        if (pRemark == pStart)
        {
            continue;
        }

        const char *pOpen = FindFirst(pStart, pEnd, '[');
        const char *pClose = FindLast(pOpen + 1, pEnd, ']');
        if (pOpen < pClose)
        {
            // This line is a valid block header
            pCurrBlock = mpBlocks + nCurrBlock++;
			pCurrBlock->mszName.assign(pOpen + 1, pClose);
        }
        else if (pStart < pEnd)
        {
            // It's a value
            if (pCurrBlock == NULL)
            {
                continue;
            }

            const char *pEquals = FindFirst(pStart, pEnd, '=');
            if (pEquals == pEnd)
            {
                // No value, just a key, save it anyway
                // Optimistically, we assume it's new
                std::string &name = pCurrBlock->mpValues[pCurrBlock->mnValueCount].szName;
				name.assign(pStart, pEnd);

                bool dup = false;
                for(int ii=0;ii<pCurrBlock->mnValueCount;++ii)
                {
                    if(!pCurrBlock->mpValues[ii].szName.compare(name))
                    {
                        dup = true;
                        break;
                    }
                }
                if(!dup)
                {
                    pCurrBlock->mnValueCount++;
                }
            }
            else
            {
                const char *pNameStart = pStart;
                const char *pNameEnd = pEquals;
                const char *pValStart = pEquals + 1;
                const char *pValEnd = pEnd;

                RemoveWhitespace(pNameStart, pNameEnd);
                RemoveWhitespace(pValStart, pValEnd);

                // Optimistically assume the name is new
                std::string &name = pCurrBlock->mpValues[pCurrBlock->mnValueCount].szName;
				name.assign(pNameStart, pNameEnd);

                bool dup = false;
                for(int ii=0;ii<pCurrBlock->mnValueCount;++ii)
                {
                    if(!pCurrBlock->mpValues[ii].szName.compare(name))
                    {
                        dup = true;
                        break;
                    }
                }
                if(!dup)
                {
					pCurrBlock->mpValues[pCurrBlock->mnValueCount].szValue.assign(pValStart, pValEnd);
                    pCurrBlock->mnValueCount++;
                }
            }
        }
    }

    delete[] pFileContents;
    return CPUT_SUCCESS;
}

//----------------------------------------------------------------
CPUTConfigBlock *CPUTConfigFile::GetBlock(int nBlockIndex)
{
    if(nBlockIndex >= mnBlockCount || nBlockIndex < 0)
    {
        return NULL;
    }

    return &mpBlocks[nBlockIndex];
}

//----------------------------------------------------------------
CPUTConfigBlock *CPUTConfigFile::GetBlockByName(const std::string &szBlockName)
{
    for(int ii=0; ii<mnBlockCount; ++ii)
    {
        if(mpBlocks[ii].mszName.compare(szBlockName) == 0)
        {
            return &mpBlocks[ii];
        }
    }
    return NULL;
}

//----------------------------------------------------------------
int CPUTConfigFile::BlockCount(void)
{
    return mnBlockCount;
}

