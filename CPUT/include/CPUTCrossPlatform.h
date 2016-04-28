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
