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

//
// Obtain a single number for the complete GCC version. For example, if the GCC version is 3.2.0
// this macro will expand into 30200
//
// Example:
// #if GCC_VERSION > 30200
//
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

//
// Macros for memory alignment of variables. Will not work for structures
//
// Example
// MEMALIGN(int alignedVar, 16);
//
#ifdef __GNUC__
#define MEMALIGN(variable, alignment) variable __attribute__ ((aligned(alignment)))
#elif _MSC_VER
#define MEMALIGN(variable, alignment) __declspec(align(alignment)) variable
#else
#define MEMALIGN(variable, alignment)
#endif

//
// Macros for memory alignment of structures. MSVCC and GCC require alignment directives on opposite
// ends of a structure definition.
//
// MSVCC Example:
// __declspec(align(16)) struct float4
// {
//   float x, y, z, w;
// };
//
// GCC Example:
// struct float4
// {
//   float x, y, z, w;
// } __attribute__ ((aligned(16)));
//
// MSVCC + GCC with macros Example:
// ALIGNEDSTRUCT_START struct float4
// {
//   float x, y, z, w;
// } ALIGNEDSTRUCT_END;
//
#ifdef __GNUC__
#define ALIGNEDSTRUCT_START(alignment)
#define ALIGNEDSTRUCT_END(alignment) __attribute__ ((aligned(alignment)))
#elif _MSC_VER
#define ALIGNEDSTRUCT_START(alignment) __declspec(align(alignment))
#define ALIGNEDSTRUCT_END(alignment) 
#else
#define ALIGNEDSTRUCT_START(alignment)
#define ALIGNEDSTRUCT_END(alignment)
#endif

//
// MSFT supports _snprintf but not snprintf while GCC does not support sprintf_s
//
#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

//
// MSFT supports sscanf_s while GCC does not. They take the same arguments.
//
#if defined(_MSC_VER) && _MSC_VER < 1900
#define sscanf sscanf_s
#endif

//
// MSFT supports strtok_s instead of strtok_r. They take the same arguments.
//
#ifdef _MSC_VER
#define strtok_r strtok_s
#endif
