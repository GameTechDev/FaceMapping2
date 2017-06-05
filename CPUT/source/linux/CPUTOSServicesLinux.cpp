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
#ifdef CPUT_OS_ANDROID
#include "CPUTWindowAndroid.h"
#elif CPUT_OS_LINUX
#include "CPUTWindowX.h"
#endif
#include "CPUT_OGL.h"

#include <iostream>

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdarg.h>

// Retrieves the current working directory
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::GetWorkingDirectory(std::string *pPath)
{
	assert(false);
	/*
    wchar_t pPathAsTchar[CPUT_MAX_PATH];
    DWORD result = GetCurrentDirectory(CPUT_MAX_PATH, pPathAsTchar);
    ASSERT( result, "GetCurrentDirectory returned 0." );
    *pPath = pPathAsTchar;
	* */
    return CPUT_SUCCESS;
}

// Sets the current working directory
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::SetWorkingDirectory(const std::string &path)
{
	assert(false);
	/*
    bool result = SetCurrentDirectory(path.c_str());
    ASSERT( 0 != result, "Error setting current directory." );
    return CPUT_SUCCESS;
	 * */
}

// Gets the location of the executable's directory
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::GetExecutableDirectory(std::string *pExecutableDir)
{
    CPUTResult result = CPUT_SUCCESS;
    
#ifdef CPUT_OS_ANDROID
    // In Android, the assets come from a /assets/ folder in the apk
    // So this doesn't really make sense.
    // We'll just stub it with a local dir
    pExecutableDir->assign("");
#else
    // get full path and executable
    char pathBuffer[CPUT_MAX_PATH];
    ssize_t count = readlink("/proc/self/exe", pathBuffer, CPUT_MAX_PATH);
    if (count == -1) {
        DEBUG_ERROR(strerror(errno));
        result = CPUT_ERROR;
    } else if (count >= CPUT_MAX_PATH) {
        DEBUG_ERROR("path is too long");
        result = CPUT_ERROR;
    }
    pathBuffer[count] = '\0';
    
    // extract path to executable
    char *dirPath;
    dirPath = dirname(pathBuffer);
    if (dirPath == NULL) {
        DEBUG_ERROR(strerror(errno));
        result = CPUT_ERROR;
    }
    
    pExecutableDir->assign(dirPath);
    pExecutableDir->append("/");
#endif    
    return result;
}

// Split up the supplied path+fileName into its constituent parts
// 
//  this doesn't really make sense in linux as there are no drive letters or necessarily any extensions
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::SplitPathAndFilename(const std::string &sourceFilename, std::string *pDrive, std::string *pDir, std::string *pFileName, std::string *pExtension)
{
	assert(false);
	/*
    wchar_t pSplitDrive[CPUT_MAX_PATH];
    wchar_t pSplitDirs[CPUT_MAX_PATH];
    wchar_t pSplitFile[CPUT_MAX_PATH];
    wchar_t pSplitExt[CPUT_MAX_PATH];
#if defined (UNICODE) || defined(_UNICODE)
    #define SPLITPATH _wsplitpath_s
#else
    #define SPLITPATH _splitpath_s
#endif
    int result = SPLITPATH(sourceFilename.c_str(), pSplitDrive, CPUT_MAX_PATH, pSplitDirs, CPUT_MAX_PATH, pSplitFile, CPUT_MAX_PATH, pSplitExt, CPUT_MAX_PATH);
    ASSERT( 0 == result, "Error splitting path" );

    // return the items the user wants
    *pDrive     = pSplitDrive;
    *pDir       = pSplitDirs;
    *pFileName  = pSplitFile;
    *pExtension = pSplitExt;
*/
    return CPUT_SUCCESS;
}

// Takes a relative/full path+fileName and returns the absolute path with drive
// letter, absolute path, fileName and extension of this file.
// Truncates total path/file length to CPUT_MAX_PATH
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::ResolveAbsolutePathAndFilename(const std::string &fileName, std::string *pResolvedPathAndFilename)
{
    CPUTResult  result   = CPUT_SUCCESS;
#ifdef CPUT_OS_ANDROID
    pResolvedPathAndFilename->assign(fileName);
#else
    char       *fullPath = NULL;

    fullPath = realpath(fileName.c_str(), NULL);
    if (fullPath == NULL) {
        DEBUG_ERROR(fileName);
        DEBUG_ERROR(strerror(errno));
        result = CPUT_ERROR;
        return result;
    }
    
    pResolvedPathAndFilename->assign(fullPath);
    
    free(fullPath); // realpath() mallocs the memory required to hold the path name, so we need to free it
#endif
    return result;
}

// Verifies that file exists at specified path
// // #### this should probably just return a bool
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::DoesFileExist(const std::string &pathAndFilename)
{
#ifdef CPUT_OS_ANDROID
    DEBUG_PRINT("DoesFileExist stubbed");
#else
    struct stat fileAttributes;
    
    int result = stat(pathAndFilename.c_str(), &fileAttributes);
    if (result == -1) {
        DEBUG_ERROR(strerror(errno));
        result = CPUT_ERROR; 
    }
	
    if (S_ISREG(fileAttributes.st_mode)) {
        return CPUT_SUCCESS;
    }
#endif    
    return CPUT_ERROR;
}

// Verifies that directory exists.
// Returns success if the directory exists and is readable (failure may mean
// it's busy or permissions denied on win32)
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::DoesDirectoryExist(const std::string &path)
{
#ifdef CPUT_OS_ANDROID
    // On Android, all files are in the APK and are compressed.
    // We do not have access to the standard file system, so
    // all files need streaming from memory through the android asset manager
    AAssetManager* assetManager = CPUTWindowAndroid::GetAppState()->activity->assetManager;
    
    AAssetDir* AssetDir = AAssetManager_openDir(assetManager, path.c_str());
    if (AssetDir)
        return CPUT_SUCCESS;
#else
    struct stat fileAttributes;
    
    int result = stat(path.c_str(), &fileAttributes);
    if (result == -1) {
        DEBUG_ERROR(strerror(errno));
        result = CPUT_ERROR; 
    }
	
    if (S_ISDIR(fileAttributes.st_mode)) {
        return CPUT_SUCCESS;
    }
#endif
    return CPUT_ERROR;
}

// Open a file and return file pointer to it
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::OpenFile(const std::string &fileName, FILE **ppFilePointer)
{
#ifdef CPUT_OS_ANDROID
    DEBUG_PRINT("OpenFile stubbed");
#else
    *ppFilePointer = fopen(fileName.c_str(), "r");
    
    if (*ppFilePointer == NULL) {
        return CPUT_ERROR;
    }
#endif    
    return CPUT_SUCCESS;
}

// Read the entire contents of a file and return a pointer/size to it
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::ReadFileContents(const std::string &fileName, UINT *pSizeInBytes, void **ppData, bool bAddTerminator, bool bLoadAsBinary)
{
#ifdef CPUT_USE_ANDROID_ASSET_MANAGER
    // Extract the file and dir
    int length = fileName.length();
    int index = fileName.find_last_of("\\/");
    std::string file = std::string("");
    std::string dir = std::string("");
    if(index == -1)
    {
        file = fileName;
    }
    else
    {
        file = fileName.substr(index + 1, (length - 1 - index));
        dir  = fileName.substr(0, index);
    }
    DEBUG_PRINT("ReadFileContents(%s) : dir [%s], file [%s]", fileName.c_str(), dir.c_str(), file.c_str());
    
    
    // On Android, all files are in the APK and are compressed.
    // We do not have access to the standard file system, so
    // all files need streaming from memory through the android asset manager
    AAssetManager* assetManager = CPUTWindowAndroid::GetAppState()->activity->assetManager;
    
    AAssetDir* assetDir = AAssetManager_openDir(assetManager, dir.c_str());
    if (!assetDir)
        DEBUG_PRINT("Failed to load asset Dir");

    const char* assetFileName = NULL;
    while ((assetFileName = AAssetDir_getNextFileName(assetDir)) != NULL) 
    {
        if (strcmp(file.c_str(), assetFileName) == 0)
        {
            // For some reason we need to pass in the fully pathed filename here, rather than the relative filename
            // that we have just been given. This feels like a bug in Android!
            AAsset* asset = AAssetManager_open(assetManager, fileName.c_str()/*assetFileName*/, AASSET_MODE_STREAMING);
            if (!asset)
            {
                DEBUG_PRINT("Asset failed to load file %s", fileName.c_str());
                return CPUT_ERROR;
            }
            
            *pSizeInBytes = AAsset_getLength(asset);
            
            // allocate buffer
            if (bAddTerminator)
                *ppData = (void*) new char[*pSizeInBytes + 1];
            else
                *ppData = (void*) new char[*pSizeInBytes];
            
            if (!*ppData)
            {
                DEBUG_PRINT("Out of memory loading %s", fileName.c_str());
                return CPUT_ERROR;
            }
                
            // read it all in
            int numBytesRead = AAsset_read(asset, *ppData, *pSizeInBytes);
            if (bAddTerminator)
            {
                ((char *)(*ppData))[numBytesRead++] = '\0';
                *pSizeInBytes++;
            }
            ASSERT( numBytesRead == *pSizeInBytes, "File read byte count mismatch." );
            
            AAsset_close(asset);
            AAssetDir_close(assetDir);    
            return CPUT_SUCCESS;
        }
    }
    AAssetDir_close(assetDir); 

#else // CPUT_USE_ANDROID_ASSET_MANAGER
    FILE *pFile = NULL;

    DEBUG_PRINT("ReadFileContents: %s", fileName.c_str());

    if (bLoadAsBinary) {
#if defined (UNICODE) || defined(_UNICODE)
    	_wfopen_s(&pFile, fileName.c_str(), "rb");
#else
    	pFile = fopen(fileName.c_str(), "rb");
#endif
    } else {
#if defined (UNICODE) || defined(_UNICODE)
    	_wfopen_s(&pFile, fileName.c_str(), "r");
#else
    	pFile = fopen(fileName.c_str(), "r");
#endif
    }

    if(pFile)
    {
        // get file size
        fseek(pFile, 0, SEEK_END);
        *pSizeInBytes = ftell(pFile);
        fseek (pFile, 0, SEEK_SET);

        // allocate buffer
        if (bAddTerminator)
            *ppData = (void*) new char[*pSizeInBytes + 1];
        else
            *ppData = (void*) new char[*pSizeInBytes];
        ASSERT( *ppData, "Out of memory" );

        // read it all in
        UINT numBytesRead = (UINT) fread(*ppData, sizeof(char), *pSizeInBytes, pFile);
        if (bAddTerminator)
        {
            ((char *)(*ppData))[numBytesRead++] = '\0';
            *pSizeInBytes++;
        }
        ASSERT( numBytesRead == *pSizeInBytes, "File read byte count mismatch." );

        // close and return
        fclose(pFile);
        return CPUT_SUCCESS;
    }

#endif // CPUT_USE_ANDROID_ASSET_MANAGER 
    // some kind of file error, translate the error code and return it
    return CPUT_ERROR;
//    return TranslateFileError(err);

}

// Open a system dialog box
//-----------------------------------------------------------------------------
CPUTResult CPUTOSServices::OpenMessageBox(std::string title, std::string text)
{
	assert(false);
	//::MessageBox(NULL, text.c_str(), title.c_str(), MB_OK);

    return CPUT_SUCCESS;
}
// Translate a file operation error code
//-----------------------------------------------------------------------------
CPUTResult CPUTFileSystem::TranslateFileError(int err)
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

#ifdef CPUT_GPA_INSTRUMENTATION
// Allows you to get the global/domain-wide instrumentation markers needed
// to mark events in GPA
//-----------------------------------------------------------------------------
void CPUTOSServices::GetInstrumentationPointers(__itt_domain **ppGPADomain, CPUT_GPA_INSTRUMENTATION_STRINGS eString, __itt_string_handle **ppGPAStringHandle)
{
    *ppGPADomain = mpGPADomain;
    *ppGPAStringHandle = mppGPAStringHandles[eString];
}

// Set the global/domain-wide instrumtation markers needed to mark events
// in GPA
//-----------------------------------------------------------------------------
void CPUTOSServices::SetInstrumentationPointers(__itt_domain *pGPADomain, CPUT_GPA_INSTRUMENTATION_STRINGS eString, __itt_string_handle *pGPAStringHandle)
{
    mpGPADomain = pGPADomain;
    mppGPAStringHandles[eString] = pGPAStringHandle;
}
#endif


#ifdef CPUT_USE_ANDROID_ASSET_MANAGER
CPUTFileSystem::CPUTandroidifstream::CPUTandroidifstream(const std::string &fileName, std::ios_base::openmode mode) : iCPUTifstream(fileName, mode)
{
    mpAsset = NULL;
    mpAssetDir = NULL;
    mbEOF = true;
    
    // Extract the file and dir
    int length = fileName.length();
    int index = fileName.find_last_of("\\/");
    std::string file = fileName.substr(index + 1, (length - 1 - index));
    std::string dir  = fileName.substr(0, index);
    
    // On Android, all files are in the APK and are compressed.
    // We do not have access to the standard file system, so
    // all files need streaming from memory through the android asset manager
    AAssetManager* assetManager = CPUTWindowAndroid::GetAppState()->activity->assetManager;
    
    mpAssetDir = AAssetManager_openDir(assetManager, dir.c_str());
    if (!mpAssetDir)
        DEBUG_PRINT("Failed to load asset Dir");
    
    const char* assetFileName = NULL;
    while ((assetFileName = AAssetDir_getNextFileName(mpAssetDir)) != NULL) 
    {
        if (strcmp(file.c_str(), assetFileName) == 0)
        {
            // For some reason we need to pass in the fully pathed filename here, rather than the relative filename
            // that we have just been given. This feels like a bug in Android!
            mpAsset = AAssetManager_open(assetManager, fileName.c_str()/*assetFileName*/, AASSET_MODE_STREAMING);
            if (mpAsset)
                mbEOF = false;
            return;
        }
    }
}
        
CPUTFileSystem::CPUTandroidifstream::~CPUTandroidifstream()
{
    if (mpAsset)
        AAsset_close(mpAsset);
    if (mpAssetDir)
        AAssetDir_close(mpAssetDir);    
}

void CPUTFileSystem::CPUTandroidifstream::read(char* s, int64_t n)
{
    if (mbEOF || !mpAsset)
        return;
    
    int numBytesRead = AAsset_read(mpAsset, s, n);
    if (numBytesRead != n)
        mbEOF = true;
}

void CPUTFileSystem::CPUTandroidifstream::close()
{
    if (mpAsset)
        AAsset_close(mpAsset);
    if (mpAssetDir)
        AAssetDir_close(mpAssetDir);    
    mpAsset = NULL;
    mpAssetDir = NULL;
}

#endif // CPUT_USE_ANDROID_ASSET_MANAGER

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

    char *sOut;
    int len;

    len = vsnprintf(NULL, 0, format, args) + 1;

    int outputBufferSize = prefix.size() + len;
    sOut = (char *)malloc(outputBufferSize * sizeof(char));
    int numWritten = snprintf(sOut, outputBufferSize, "%s", prefix.c_str());
    vsnprintf(sOut + numWritten, outputBufferSize - numWritten, format, args);

    if (os) {
        *os << prefix << sOut;
    }

#ifdef CPUT_OS_ANDROID
    // CPUT Priorities should match androids
    __android_log_print(priority, "CPUT", "%s", sOut);
#endif

    // Always send to output
    cout << sOut;
    free(sOut);
}

void DEBUG_PRINT(const char *pData, ...)
{
    va_list args;

    va_start(args, pData);
    Log.vLog(LOG_DEBUG, pData, args);
    va_end(args);
}