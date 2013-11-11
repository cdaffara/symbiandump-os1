// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines all the patchable constants in fbscli.dll.
// The definition of the constants must not be in the same file as the usage,
// or the compiler will optimise them out, so put them all in here.
// Note that existing patchable constants are declared as global,
// i.e. not in a namespace, so I have continued the trend.
// 
//

#include <e32def.h>


/**
@SYMPatchable
@publishedPartner
@released

This constant determines the granularity of the section of contiguous committed memory
at the beginning of the large bitmap chunk. This granularity equals the memory page size
multiplied by 2 to the power of the value of the constant.
@note	Default value: 1 (2x)
*/
EXPORT_C extern const TInt KFbServLargeChunkGrowByShifter = 1;

/**
@SYMPatchable
@publishedPartner
@released

This constant, together with <code>KFbServLargeChunkMinVirtualSize</code> and
<code>KFbServLargeChunkMaxVirtualSize</code>, determines the size of the virtual
address range reserved for the large bitmap chunk. Between a minimum and a maximum,
the size of this virtual address range equals the amount of physical RAM memory
multiplied by 2 to the power of the value of the constant.
@note	Default value: 2 (4x)
*/
EXPORT_C extern const TInt KFbServLargeChunkSizeShifter = 2;

/**
@SYMPatchable
@publishedPartner
@released

This constant defines the minimum amount of physical memory committed to the large bitmap chunk.
@note	Default value: 16KB (0x4000)
*/
EXPORT_C extern const TInt KFbServLargeChunkMinPhysicalSize = 0x4000;

/**
@SYMPatchable
@publishedPartner
@released

This constant defines the minimum size of the virtual address range reserved for the large bitmap chunk.
@note	Default value: 64MB (0x04000000)
@note   Emulator value 32MB (0x02000000)
*/
#ifdef __WINS__
EXPORT_C extern const TInt KFbServLargeChunkMinVirtualSize = 0x02000000;
#else
EXPORT_C extern const TInt KFbServLargeChunkMinVirtualSize = 0x04000000;
#endif

/**
@SYMPatchable
@publishedPartner
@released

This constant defines the maximum size of the virtual address range reserved for the large bitmap chunk.
@note	Default value: 256MB (0x10000000)
@note   Emulator value 48MB (0x03000000)
*/
#ifdef __WINS__
EXPORT_C extern const TInt KFbServLargeChunkMaxVirtualSize = 0x03000000;
#else
EXPORT_C extern const TInt KFbServLargeChunkMaxVirtualSize = 0x10000000;
#endif

// Header files declaring these constants are included to confirm that the definitions match.
// They are included at the end because otherwise the ARM compiler refuses to initialise the values.

#if (__ARMCC_VERSION >= 310000)
//#	include "UTILS.H"	// Commented out to prevent warning dllexport/dllimport conflict
#else
#	include "UTILS.H"
#endif



