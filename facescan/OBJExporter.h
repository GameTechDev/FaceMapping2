/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OBJExporter_H_
#define OBJExporter_H_
// ====================================================================================================================

#include <string>
#include <iomanip>
#include <functional>

#include <codecvt>
#include <wincodec.h>
#include <dxgiformat.h>
#include "../CPUT/middleware/DirectXTex/DirectXTex/DirectXTex.h"
#include <CPUTMath.h>
#include <CPUTMesh.h>
#include <vector>
#include <CPUTRenderParams.h>

using namespace DirectX;

enum class OBJIllumination {
    COLOR_ON_AMBIENT_OFF,
    COLOR_ON_AMBIENT_ON,
    HIGHLIGHT_ON,
    REFLECTION_RAYTRACE_ON,
    GLASS_ON_RAYTRACE_ON,
    FRESNEL_ON_RAYTRACE_ON,
    REFRACTION_ON_FRESNEL_OFF_RAYTRACE_ON,
    REFRACTION_ON_FRESNEL_ON_RAYTRACE_ON,
    REFLECTION_ON_RAYTRACE_OFF,
    GLASS_ON_RAYTRACE_OFF,
    CASTS_SHADOWS

    // Illumination Model
    // ----------------------------------------------
    //  0	Color on and Ambient off
    //  1	Color on and Ambient on
    //  2	Highlight on
    //  3	Reflection on and Ray trace on
    //
    //  4	Transparency: Glass on
    //  	Reflection: Ray trace on
    //
    //  5	Reflection: Fresnel on and Ray trace on
    //  6	Transparency: Refraction on
    //  	Reflection: Fresnel off and Ray trace on
    //
    //  7	Transparency: Refraction on
    //  	Reflection: Fresnel on and Ray trace on
    //
    //  8	Reflection on and Ray trace off
    //  9	Transparency: Glass on
    //  	Reflection: Ray trace off
    //
    //  10	Casts shadows onto invisible surfaces
};

enum class OBJTextureType {
    Ambient, Diffuse, Specular, Reflectivity, Dissolve, Displacement, Decal, Bump
};


class OBJTexture {
public:
    OBJTexture(std::string const& fileName, OBJTextureType type)
    : mFileName(fileName)
    , mType(type) {
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const OBJTexture& mat);

public:

    std::string mFileName;
    OBJTextureType mType;
    // TODO: Arguments and options
};



class OBJMaterial {
public:
    OBJMaterial()
        : mAmbient(1.0f)
        , mDiffuse(1.0f)
        , mSpectral(0.0f)
        , mIllumination( OBJIllumination::COLOR_ON_AMBIENT_ON)
        , mDissolve(1.0f)
        , Ambient("", OBJTextureType::Ambient)
        , Diffuse("", OBJTextureType::Diffuse)
        , Specular("", OBJTextureType::Specular)
        , Reflectivity("", OBJTextureType::Reflectivity)
        , Dissolve("", OBJTextureType::Dissolve)
        , Displacement("", OBJTextureType::Displacement)
        , Decal("", OBJTextureType::Decal)
        , Bump("", OBJTextureType::Bump) {
    }

    void AssignTexture( std::string fileName,  OBJTextureType type) {

    switch(type) {
        case OBJTextureType::Ambient:
            Ambient.mFileName = fileName;
            break;
        case OBJTextureType::Diffuse:
            Diffuse.mFileName = fileName;
            break;
        case OBJTextureType::Specular:
            Specular.mFileName = fileName;
            break;
        case OBJTextureType::Reflectivity:
            Reflectivity.mFileName = fileName;
            break;
        case OBJTextureType::Dissolve:
            Dissolve.mFileName = fileName;
            break;
        case OBJTextureType::Displacement:
            Displacement.mFileName = fileName;
            break;
        case OBJTextureType::Decal:
            Decal.mFileName = fileName;
            break;
        case OBJTextureType::Bump:
            Bump.mFileName = fileName;
            break;
        default: break;
    }
    }
private:

    friend std::ostream& operator<<(std::ostream& os, const OBJMaterial& mat);

public:
    std::string materialName; // Names may be any length but cannot contain spaces
    // TODO: Change spaces to underscores
    float3 mAmbient;
    float3 mDiffuse;
    float3 mSpectral;
    OBJIllumination mIllumination;
    float mDissolve;

    OBJTexture Ambient, Diffuse, Specular, Reflectivity, Dissolve, Displacement, Decal, Bump;
};


class OBJExporter {

public:
    std::vector<std::pair<CPUTMesh*, int>> modelMeshes;
    std::vector<OBJMaterial> objMaterials;
    CPUTRenderParameters renderParameters;
    std::vector<std::string> RawObjects;
    int currentNumVerts;

    OBJExporter(std::string const& filePath)
        : currentNumVerts(0), mClosed(false)
        , mFilePath(filePath)
        , mBaseName(CPUTFileSystem::basename(filePath, true)) {
        fileOut.rdbuf()->pubsetbuf(Buffer, 4096);
        fileOut.open(filePath, std::ios::out);
    }

    ~OBJExporter() {
        if(!mClosed) {
            Close();
        }
    }

	bool ExportModel( CPUTModel* cputModel, CPUTRenderParameters const& renderParams, int materialPass );

	void Close() {

        std::string MaterialPath = CPUTFileSystem::dirname(mFilePath) + "\\" + CPUTFileSystem::basename(mFilePath, true) + ".mtl";
        std::string MaterialName = CPUTFileSystem::basename(MaterialPath, true) + ".mtl";

        // Write the materials
        {
            std::ofstream materialFile(MaterialPath, std::ios::out);

            for (auto& item : objMaterials) {
                materialFile << item << std::endl;
            }
        }

        std::ofstream outFile;
        outFile.open(mFilePath, std::ios::out);

        outFile << "mtllib " << MaterialName << std::endl;

        for(auto obj : RawObjects) {
            outFile << obj;
        }

        mClosed = true;
    }

	void ExportObject( D3D11_MAPPED_SUBRESOURCE verts, D3D11_MAPPED_SUBRESOURCE indices, int numVerts, int numInd, OBJMaterial objMaterial, const std::string& name, CPUTMesh* mesh );
private:

    std::string mFilePath;
    std::string mBaseName;
    bool mClosed;
    char Buffer[4096];
    std::ofstream fileOut;

};


// ====================================================================================================================
#endif
