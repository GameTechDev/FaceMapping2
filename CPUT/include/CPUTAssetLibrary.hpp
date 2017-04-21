/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////
#include <algorithm> // find_if
#include "CPUTOSServices.h"

// Find an asset in a specific library
// ** Does not Addref() returned items **
// Asset library doesn't care if we're using absolute paths for names or not, it
// just adds/finds/deletes the matching string literal.
//-----------------------------------------------------------------------------
template<typename T>
T* CPUTAssetLibrary::FindAsset( const std::string &name, std::vector<CPUTAsset<T>> const& pList, bool nameIsFullPathAndFilename ) {
	std::string absolutePathAndFilename;
	if (!nameIsFullPathAndFilename)
	{
		CPUTFileSystem::ResolveAbsolutePathAndFilename(mAssetSetDirectoryName + name, &absolutePathAndFilename);
	}
	else
	{
		absolutePathAndFilename = name;
	}

	UINT hash = CPUTComputeHash(absolutePathAndFilename);

	// Use a inline lambda function for the asset lookup. Pull this out into a method if lookup occurs elsewhere
	auto foundAsset = std::find_if(std::begin(pList), std::end(pList), [&](CPUTAsset<T> const& item) {
		return ((hash == item.hash)
			&& (absolutePathAndFilename == item.name));
	});

	if (foundAsset != std::end(pList)) {
		return (*foundAsset).pData;
	}
	return nullptr;
}

template<typename T>
void CPUTAssetLibrary::AddAsset(
	const std::string &name, 
	const std::string &prefixDecoration, 
	const std::string &suffixDecoration, 
	T* pAsset, 
	std::vector<CPUTAsset<T>>& pHead) {

#ifdef DEBUG
	// Do we already have one by this name?
	auto existingAsset = std::find_if(std::begin(pHead), std::end(pHead), [&name](CPUTAsset<T> const &currentAsset) {
		return (currentAsset.name == name);
	});

	if (existingAsset != std::end(pHead))  {
		DEBUG_PRINT("WARNING: Adding asset with duplicate name: %s\n", name.c_str());
	}
#endif

	std::string fileName = CPUTFileSystem::basename(name);

	CPUTAsset<T> newAsset;
	newAsset.hash = CPUTComputeHash(prefixDecoration + name + suffixDecoration);
	newAsset.name = prefixDecoration + name + suffixDecoration;
	newAsset.pData = pAsset;
	newAsset.fileName = fileName;

	((CPUTRefCount*)newAsset.pData)->AddRef();
	pHead.push_back(newAsset);
}
// 
// bool EndsWith(std::string &haystackString, std::string &needle)
// {
// 	size_t hLen = haystackString.length();
// 	size_t nLen = needle.length();
// 	if (hLen < nLen)
// 		return false;
// 	return haystackString.compare(hLen - nLen, nLen, needle) == 0;
// }

template <typename T>
T* CPUTAssetLibrary::FindAssetByName( std::string const& name, std::vector<CPUTAsset<T>>& assetList ) {

	auto existingAsset = std::find_if(std::begin(assetList), std::end(assetList), [&name](CPUTAsset<T> const &currentAsset) {
		return (currentAsset.name == name);
	});

	if (existingAsset != std::end(assetList)) {
		return (*existingAsset).pData;
	}

	return nullptr;
}

template <typename T>
T* CPUTAssetLibrary::FindAssetByNameNoPath(std::string const& name, std::vector<CPUTAsset<T>>& assetList) {

	auto existingAsset = std::find_if(std::begin(assetList), std::end(assetList), [&name](CPUTAsset<T> const &currentAsset) {
		const std::string &haystack = currentAsset.name;
		const std::string &needle = name;
		size_t hLen = haystack.length();
		size_t nLen = needle.length();
		if (hLen < nLen)
			return false;
		return haystack.compare(hLen - nLen, nLen, needle) == 0;
	});

	if (existingAsset != std::end(assetList)) {
		return (*existingAsset).pData;
	}

	return nullptr;
}