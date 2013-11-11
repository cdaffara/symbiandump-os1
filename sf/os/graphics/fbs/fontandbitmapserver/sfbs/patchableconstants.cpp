// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines all the patchable constants in fbserv.exe.
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

This constant determines the maximum size of the shared heap in the font and bitmap server.
Up to the value of the constant, the size of the virtual address range reserved for this heap
equals the amount of physical RAM memory. As available hardware RAM sizes increase in the future,
it may become impossible to reserve a virtual address range that big, hence the need for an upper limit.
@note	Default value: 64MB (0x04000000)
@note   Emulator value 2MB (0x0200000)
*/
#ifdef __WINS__
EXPORT_C extern const TInt KFbServSharedHeapMaxSize = 0x00200000;
#else
EXPORT_C extern const TInt KFbServSharedHeapMaxSize = 0x04000000;
#endif

/**
@SYMPatchable
@internalTechnology

This constant modifies the writable data paging mode used by the font and bitmap server.
It is configured at rom build time via the following definitions in fbserv.iby:
	SYMBIAN_GRAPHICS_FBSERV_PAGEDATA
	SYMBIAN_GRAPHICS_FBSERV_UNPAGEDDATA
	SYMBIAN_GRAPHICS_FBSERV_PAGE_BITMAP_DATA_ONLY
	SYMBIAN_GRAPHICS_FBSERV_PAGE_BITMAP_DATA_AND_SHARED_HEAP_ONLY
@note	Default value: (0x00) This does not modify the writable data paging mode.
*/
EXPORT_C extern const TInt KFbServWritableDataPagingMode = 0x00;

/**
@SYMPatchable
@publishedPartner
@prototype

This constant determines the maximum size of the GPU memory that the font and 
bitmap server's glyph atlas can use.
The glyph atlas will store glyphs in this memory.  When the glyph atlas reaches 
this limit, old glyphs will be evicted from the cache to make space for new glyphs.
@note	Default value: 0.5 MB (0x00080000).
*/
EXPORT_C extern const TInt KFbServGlyphAtlasCacheLimit = 0x00080000;

// Header files declaring these constants are included to confirm that the definition matches.
// They are included at the end because otherwise the ARM compiler refuses to initialise the values.
#if (__ARMCC_VERSION >= 310000)
//#	include "SERVER.H"	// Commented out to prevent warning dllexport/dllimport conflict
#else
#	include "SERVER.H"
#endif
