// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Class SymbolAttrib for the elf2e32 tool
// @internalComponent
// @released
// 
//

#if !defined _PL_COMMON_H_
#define _PL_COMMON_H_

#ifdef _MSC_VER 
	#pragma warning(disable: 4786) // identifier was truncated to '255' characters in the debug information
	#pragma warning(disable: 4514) // unreferenced inline function has been removed
	#pragma warning(disable: 4702) // unreachable code
	#pragma warning(disable: 4710) // function not inlined
#endif

#include <tools/elfdefs.h>

typedef unsigned long	PLULONG;
typedef unsigned int	PLUINT32;
typedef unsigned short	PLUINT16;
typedef unsigned char	PLUCHAR;
typedef	int				PLINT32;
typedef short			PLINT16;
typedef unsigned char	PLUINT8;
typedef char			PLCHAR;
typedef unsigned int	PLMemAddr32;
typedef unsigned int	PLOffset32;
typedef unsigned short	PLOffset16;

#define DELETE_PTR(aPtr) delete aPtr; aPtr = NULL;
#define DELETE_PTR_ARRAY(aPtr) delete[] aPtr; aPtr = NULL;

#define ELF_ENTRY_PTR(ptype, base, offset) \
	((ptype*)((char*)base + offset))

//enum for version category
enum VER_CATEGORY{
	VER_CAT_NONE = 0,
	VER_CAT_DEFINED,
	VER_CAT_NEEDED

};

/**
Class for general utility
@internalComponent
@released
*/class Util {
public:
	static unsigned long elf_hash(const unsigned char *name);
};

/**
class for Version info
@internalComponent
@released
*/
class VersionInfo {
public:
	VersionInfo();
	~VersionInfo();

	char*	iSOName;
	char*	iLinkAs;
	char	iVerCategory;
};

//enum for e32 dump flag and dumping asm file flag
enum TDumpFlags
{
	EDumpHeader = 1<<0,
	EDumpSecurityInfo = 1<<1,
	EDumpCode = 1<<2,
	EDumpData = 1<<3,
	EDumpExports = 1<<4,
	EDumpImports = 1<<5,
	EDumpAsm = 1<<6, //Added (Only)option for generating assembly code for deffile input.//DumpAsm
	EDumpSymbols = 1<<7,//Not added as a default option.
	EDumpDefaults = EDumpHeader|EDumpCode|EDumpData|EDumpExports|EDumpImports
};

#endif //_PL_COMMON_H_

