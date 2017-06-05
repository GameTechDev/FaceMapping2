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

#include "CPUTShaderOGL.h"
#include "CPUTConfigBlock.h"
#include "CPUTMaterial.h"
#include "CPUTOSServices.h"
#include "CPUT_OGL.h"
#include "CPUTAssetLibraryOGL.h"

std::string GenerateName(const std::vector<std::string> &fileNames)
{
    return fileNames[0];
}

CPUTShaderOGL *CPUTShaderOGL::CreateFromFiles(
        const std::vector<std::string> &fileNames,
        GLuint shaderType,
        const char* glslVersion,
        CPUT_SHADER_MACRO *pGlobalMacros,
        CPUT_SHADER_MACRO *pShaderMacros
    )
{
    GenerateName(fileNames);

    GLuint shader = 0;
    int maxLength = 0;
    int nBytes = 0;
    char *infoLog = NULL;
    //NOTE: This will specify shader version and shader type as first two lines of shader
    //TODO: Maybe use the PixelShaderProfile to determine version number or some other mechanism
    size_t files = fileNames.size();
    
    const int MACRO_FILES = 4; //one for version, one for system, one for material

    //fixme allocate dynamically
    const char* source[20];
    source[0] = glslVersion;
    //FIXME: remove these defines - support separate defines for separate shaders.
    if(shaderType == GL_FRAGMENT_SHADER)
    {
        source[1] = "\n#define GLSL_FRAGMENT_SHADER\n";
    }
    else if(shaderType == GL_VERTEX_SHADER)
    {
        source[1] = "\n#define GLSL_VERTEX_SHADER\n";
    }
    else if(shaderType == GL_TESS_CONTROL_SHADER)
	{
		source[1] = "\n#define GLSL_TESS_CONTROL_SHADER\n";
	}
	else if(shaderType == GL_TESS_EVALUATION_SHADER)
	{
		source[1] = "\n#define GLSL_TESS_EVALUATION_SHADER\n";
	}
	else if(shaderType == GL_GEOMETRY_SHADER)
	{
		source[1] = "\n#define GLSL_GEOMETRY_SHADER\n";
	}
	else
	{
        source[1] = NULL;
    }
    source[2] = ConvertShaderMacroToChar(pGlobalMacros);
    source[3] = ConvertShaderMacroToChar(pShaderMacros);
    // Read our shaders into the appropriate buffers
    for(unsigned int i = 0; i < files; i++)
    {
        CPUTResult result = CPUTFileSystem::ReadFileContents(fileNames[i].c_str(), (UINT *)&nBytes, (void **)&source[i+MACRO_FILES], true);
        if(CPUTFAILED(result))
        {
            DEBUG_PRINT("Failed to read file %s\n", fileNames[i].c_str());
		    ASSERT(false, "Fragment shader failed to compile");
            return NULL;
        }
    }
    // Create an empty fragment shader handle
    shader = glCreateShader(shaderType);

    // Send the fragment shader source code to GL
    // Note that the source code is NULL character terminated.
    // GL will automatically detect that therefore the length info can be 0 in this case (the last parameter)
    glShaderSource(shader, (GLsizei)(files+MACRO_FILES), (const GLchar**)&source, 0);
 
    
    // Compile the fragment shader
    GL_CHECK(glCompileShader(shader));
    GLint isCompiled;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
    if(isCompiled == false)
    {
        GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));
 
        // The maxLength includes the NULL character
        infoLog = (char *)malloc(maxLength*40);
 
        glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);
 
        // Handle the error in an appropriate way such as displaying a message or writing to a log file.
        // In this simple program, we'll just leave
        DEBUG_PRINT("Failed to compile shader: %s\n%s\n", fileNames[0].c_str(), infoLog);
		ASSERT(false, "Compile frag shader failed");

        free(infoLog);
    }

    SAFE_DELETE(source[2]);
    SAFE_DELETE(source[3]);
    for(unsigned int i = 0; i < files; i++)
    {
        delete source[i+MACRO_FILES];
    } 

    CPUTShaderOGL *pNewCPUTShader = new CPUTShaderOGL( shader );
    
    // add shader to library
    CPUTAssetLibraryOGL *pAssetLibrary = (CPUTAssetLibraryOGL*)CPUTAssetLibrary::GetAssetLibrary();
    
    return pNewCPUTShader;

}

CPUTShaderOGL *CPUTShaderOGL::CreateFromMemory(
        const std::vector<char*>     &source,
        const std::string     &shaderProfile,
        CPUT_SHADER_MACRO *pShaderMacros
    )
{
    return NULL;
}

std::string CPUTShaderOGL::CreateShaderName(const std::vector<std::string> &fileNames,
                                            GLuint shaderType,
                                            CPUT_SHADER_MACRO *pShaderMacros)
{
    
    std::string libName(cput_to_string(shaderType)); 
    for(unsigned int i = 0; i < fileNames.size(); i++)
    {
        libName += fileNames[i];
    }
   
    char* pMacroString = ConvertShaderMacroToChar(pShaderMacros);
    libName += std::string(pMacroString);
    SAFE_DELETE(pMacroString);
    return libName;
}

//-----------------------------------------------------------------------------
bool CPUTShaderOGL::ShaderRequiresPerModelPayload( CPUTConfigBlock &properties )
{
    return false;
}

char* ConvertShaderMacroToChar(CPUT_SHADER_MACRO *pShaderMacros)
{
	char* pOutput = NULL;
	const char* pDefineText = "#define ";
	CPUT_SHADER_MACRO* pCurrentMacro = pShaderMacros;
	//first find string length
	size_t length = 0;
	while(pShaderMacros != NULL && pCurrentMacro->Name != NULL)
	{
		length += strlen(pDefineText);
		length += strlen(pCurrentMacro->Name);
		length += strlen(pCurrentMacro->Definition);
		length += 2; // for " " and "\n"
		pCurrentMacro++;
	}
	length += 1; // for NULL terminator
	pOutput = new char[length];
	char* pEnd = pOutput;
	pCurrentMacro = pShaderMacros;
	while(pShaderMacros != NULL && pCurrentMacro->Name != NULL)
	{
		strcpy(pEnd, pDefineText);
		pEnd += strlen(pDefineText);
		strcpy(pEnd, pCurrentMacro->Name);
		pEnd += strlen(pCurrentMacro->Name);
		*pEnd = ' ';
		pEnd++;
		strcpy(pEnd, pCurrentMacro->Definition);
		pEnd += strlen(pCurrentMacro->Definition);
		*pEnd = '\n';
		pEnd++;
		pCurrentMacro++;
	}
	*pEnd = 0;
	return pOutput;
}
