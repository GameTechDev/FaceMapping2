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
#include "CPUTOSServices.h"

// Retrieves the current working directory
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::GetWorkingDirectory(std::string *pPath)
{
    char pPathAsTchar[CPUT_MAX_PATH];
    DWORD result = GetCurrentDirectoryA(CPUT_MAX_PATH, pPathAsTchar);
    ASSERT( result, "GetCurrentDirectory returned 0." );
    UNREFERENCED_PARAMETER(result);
    *pPath = pPathAsTchar;
    return CPUT_SUCCESS;
}

// Sets the current working directory
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::SetWorkingDirectory(const std::string &path)
{
    BOOL result = SetCurrentDirectoryA(path.c_str());
    ASSERT( 0 != result, "Error setting current directory." );
    UNREFERENCED_PARAMETER(result);
    return CPUT_SUCCESS;
}

// Gets the location of the executable's directory
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::GetExecutableDirectory(std::string *pExecutableDir)
{
    char   pFilename[CPUT_MAX_PATH];
    DWORD result = GetModuleFileNameA(NULL, pFilename, CPUT_MAX_PATH);
    ASSERT( 0 != result, "Unable to get executable's working directory.");

    // strip off the executable name+ext
    std::string ResolvedPathAndFilename;
    ResolveAbsolutePathAndFilename(pFilename, &ResolvedPathAndFilename);
    std::string Drive, Dir, Filename, Ext;
    SplitPathAndFilename(ResolvedPathAndFilename, &Drive, &Dir, &Filename, &Ext);

    // store and return
    *pExecutableDir = Drive + Dir;

    return CPUT_SUCCESS;
}

CPUTResult CPUTFileSystem::GetMediaDirectory(std::string *pPath)
{
	GetExecutableDirectory(pPath);
	StripDirectoriesFromPath(pPath, 4);
	CombinePath(*pPath, "Media", pPath);
	return CPUT_SUCCESS;

}

// Split up the supplied path+fileName into its constituent parts
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::SplitPathAndFilename(const std::string &sourceFilename, std::string *pDrive, std::string *pDir, std::string *pFileName, std::string *pExtension)
{
    char pSplitDrive[CPUT_MAX_PATH];
    char pSplitDirs[CPUT_MAX_PATH];
    char pSplitFile[CPUT_MAX_PATH];
    char pSplitExt[CPUT_MAX_PATH];

    errno_t result = _splitpath_s(sourceFilename.c_str(), pSplitDrive, CPUT_MAX_PATH, pSplitDirs, CPUT_MAX_PATH, pSplitFile, CPUT_MAX_PATH, pSplitExt, CPUT_MAX_PATH);
    ASSERT( 0 == result, "Error splitting path" );

    // return the items the user wants
	if ( pDrive != NULL )
		*pDrive     = pSplitDrive;
	if ( pDir != NULL )
		*pDir       = pSplitDirs;
	if ( pFileName != NULL )
		*pFileName  = pSplitFile;
	if ( pExtension != NULL )
		*pExtension = pSplitExt;

    return CPUT_SUCCESS;
}

CPUTResult CPUTFileSystem::CombinePath(const std::string &p1, const std::string &p2, std::string *pOut)
{
	if (p2.size() == 0)
	{
		*pOut = p1;
		return CPUT_SUCCESS;
	}
	// if p1 ends or p2 starts with / or \\ , strip it
	char lastCharP1 = p1.back();
	char firstCharP2 = p2.front();
	bool stripSlashP2 = (firstCharP2 == '\\' || firstCharP2 == '/');
	
	int p1Size = p1.size() + (lastCharP1 == '\\' || lastCharP1 == '/') ? -1 : 0;
	std::string outPath = p1.substr(0, p1Size);

	int p2Size = p2.size() + stripSlashP2 ? -1 : 0;
	int p2Idx = stripSlashP2 ? 1 : 0;
	outPath.push_back('\\');
	outPath.append(p2.substr(p2Idx, p2Size));

	*pOut = outPath;

	return CPUT_SUCCESS;
}

CPUTResult CPUTFileSystem::GetDirectoryFromPath(const std::string &filename, std::string *pOutDirectory)
{
	int lastIndexSlash1 = (int)filename.find_last_of('\\');
	int lastIndexSlash2 = (int)filename.find_last_of('/');
	int count = lastIndexSlash1 > lastIndexSlash2 ? lastIndexSlash1 : lastIndexSlash2;
	count = (count < 0) ? 0 : count;
	*pOutDirectory = filename.substr(0, count);
	return CPUT_SUCCESS;
}

CPUTResult CPUTFileSystem::StripDirectoriesFromPath(std::string *dir, int levelsToRemove, std::string *pResult )
{
	pResult = (pResult == NULL) ? dir : pResult;

	const char *cStr = dir->c_str();
	int lastChar = (int)dir->size() - 2; // an ending / doesn't count
	int removed = 0;
	while (lastChar >= 0 && removed < levelsToRemove)
	{
		if (cStr[lastChar] == '\\' || cStr[lastChar] == '/')
		{
			removed++;
		}
		lastChar--;
	}
	*pResult = dir->substr(0, lastChar+1);
	return (removed == levelsToRemove) ? CPUT_SUCCESS : CPUT_ERROR;
}

// Takes a relative/full path+fileName and returns the absolute path with drive
// letter, absolute path, fileName and extension of this file.
// Truncates total path/file length to CPUT_MAX_PATH. Both source and destination may be the same string.
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::ResolveAbsolutePathAndFilename(const std::string &fileName, std::string *pResolvedPathAndFilename)
{
    char pFullPathAndFilename[CPUT_MAX_PATH];
    DWORD result = GetFullPathNameA(fileName.c_str(), CPUT_MAX_PATH, pFullPathAndFilename, NULL);
    ASSERT( 0 != result, "Error getting full path name" );
    *pResolvedPathAndFilename = pFullPathAndFilename;
    UNREFERENCED_PARAMETER(result);

    return CPUT_SUCCESS;
}

// Verifies that file exists at specified path
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::DoesFileExist(const std::string &pathAndFilename)
{
    // check for file existence
    // attempt to open it where they said it was
    FILE *pFile = NULL;
    errno_t err = fopen_s(&pFile, pathAndFilename.c_str(), "r");
    if(0 == err && pFile != NULL)
    {
        // yep - file exists
        fclose(pFile);
        return CPUT_SUCCESS;
    }

    // not found, translate the file error and return it
    return TranslateFileError(err);
}

// Verifies that directory exists.
// Returns success if the directory exists and is readable (failure may mean
// it's busy or permissions denied on win32)
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::DoesDirectoryExist(const std::string &path)
{
    DWORD fileAttribs = GetFileAttributesA(path.c_str());
    ASSERT( INVALID_FILE_ATTRIBUTES != fileAttribs, "Failed getting file attributes" );

    return CPUT_SUCCESS;
}

// Open a file and return file pointer to it
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::OpenFile(const std::string &fileName, FILE **ppFilePointer)
{
    errno_t err = fopen_s(ppFilePointer, fileName.c_str(), "r");

    return TranslateFileError(err);
}

// Read the entire contents of a file and return a pointer/size to it
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::ReadFileContents(const std::string &fileName, UINT *pSizeInBytes, void **ppData, bool bAddTerminator, bool bLoadAsBinary)
{
    FILE *pFile = NULL;

    errno_t err;
    if (bLoadAsBinary) {
        err = fopen_s(&pFile, fileName.c_str(), "rb");
    } else {
        err = fopen_s(&pFile, fileName.c_str(), "r");
    }

    if(0 == err)
    {
        // get file size
        fseek(pFile, 0, SEEK_END);
        *pSizeInBytes = ftell(pFile);
        fseek (pFile, 0, SEEK_SET);

        // allocate buffer
        *ppData = (void*) new char[*pSizeInBytes + 1];
        ASSERT( *ppData, "Out of memory" );

        // read it all in
        UINT numBytesRead = (UINT) fread(*ppData, sizeof(char), *pSizeInBytes, pFile);
        if (bAddTerminator)
        {
            ((char *)(*ppData))[numBytesRead++] = '\0';
            (*pSizeInBytes)++;
        }
        //fixme this is off by something for windows at least.
        //ASSERT( numBytesRead == *pSizeInBytes, "File read byte count mismatch." );
        UNREFERENCED_PARAMETER(numBytesRead);

        // close and return
        fclose(pFile);
        return CPUT_SUCCESS;
    }

    // some kind of file error, translate the error code and return it
    return TranslateFileError(err);
}


// Translate a file operation error code
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::TranslateFileError(errno_t err)
{
    if(0==err)
    {
        return CPUT_SUCCESS;
    }

    // see: http://msdn.microsoft.com/en-us/library/t3ayayh1.aspx
    // for list of all error codes
    CPUTResult result = CPUT_ERROR_FILE_ERROR;

    switch(err)
    {
    case ENOENT: result = CPUT_ERROR_FILE_NOT_FOUND;                 break; // file/dir not found
    case EIO:    result = CPUT_ERROR_FILE_IO_ERROR;                  break;
    case ENXIO:  result = CPUT_ERROR_FILE_NO_SUCH_DEVICE_OR_ADDRESS; break;
    case EBADF:  result = CPUT_ERROR_FILE_BAD_FILE_NUMBER;           break;
    case ENOMEM: result = CPUT_ERROR_FILE_NOT_ENOUGH_MEMORY;         break;
    case EACCES: result = CPUT_ERROR_FILE_PERMISSION_DENIED;         break;
    case EBUSY:  result = CPUT_ERROR_FILE_DEVICE_OR_RESOURCE_BUSY;   break;
    case EEXIST: result = CPUT_ERROR_FILE_EXISTS;                    break;
    case EISDIR: result = CPUT_ERROR_FILE_IS_A_DIRECTORY;            break;
    case ENFILE: result = CPUT_ERROR_FILE_TOO_MANY_OPEN_FILES;       break;
    case EFBIG:  result = CPUT_ERROR_FILE_TOO_LARGE;                 break;
    case ENOSPC: result = CPUT_ERROR_FILE_DEVICE_FULL;               break;
    case ENAMETOOLONG: result = CPUT_ERROR_FILE_FILENAME_TOO_LONG;   break;
    default:
        // unknown file error type - assert so you can add it to the list
        ASSERT(0,"Unkown error code");
    }
    return result;
}

// Open a system dialog box
//-----------------------------------------------------------------------------
CPUTResult CPUTOSServices::OpenMessageBox(std::string title, std::string text)
{
    ::MessageBoxA(NULL, text.c_str(), title.c_str(), MB_OK);

    return CPUT_SUCCESS;
}



CPUTLog Log;

void CPUTLog::SetDestination(std::ostream* output)
{
    if (output->fail()) {

    }

    os = output;
}

void CPUTLog::Log(int priority, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vLog(priority, format, args);
    va_end(args);
}

void CPUTLog::vLog(int priority, const char *format, va_list args)
{
    std::string prefix;

    switch (priority) {
    case LOG_FATAL:
        prefix = "FATAL:   ";
        break;
    case LOG_ERROR:
        prefix = "ERR:     ";
        break;
    case LOG_WARNING:
        prefix = "WARNING: ";
        break;
    case LOG_INFO:
        prefix = "INFO:    ";
        break;
    case LOG_DEBUG:
        prefix = "DEBUG:   ";
        break;
    case LOG_VERBOSE:
        prefix = "VERBOSE: ";
        break;
    default:
        prefix = "UNKNOWN: ";
        break;
    }

    char *sOut = NULL;
    int len;

    len = vsnprintf(NULL, 0, format, args) + 1;

    int outputBufferSize = (int)prefix.size() + len;
    sOut = (char *)malloc(outputBufferSize * sizeof(char));
    int numWritten = sprintf_s(sOut, outputBufferSize, "%s", prefix.c_str());
    vsprintf_s(sOut + numWritten, outputBufferSize - numWritten, format, args);

    if (os && sOut) {
        *os << sOut;
    }

    // Always send to VS debug output window. Function does nothing if no attached debugger
    OutputDebugStringA(sOut);

    free(sOut);
}

void DEBUG_PRINT(const char *pData, ...)
{
    va_list args;

    va_start(args, pData);
    Log.vLog(LOG_DEBUG, pData, args);
    va_end(args);
}
