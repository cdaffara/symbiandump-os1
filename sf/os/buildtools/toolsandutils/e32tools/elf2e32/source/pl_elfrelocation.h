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
// Implementation of the Class ElfRelocation for the elf2e32 tool
// @internalComponent
// @released
// 
//

#if !defined(_PL_ELFRELOCATION_H_)
#define _PL_ELFRELOCATION_H_

#include "pl_common.h"

class DllSymbol;
class ElfExecutable;

/**
This class is for Elf relocation.

@internalComponent
@released
*/
class ElfRelocation
{

public:
	ElfRelocation(ElfExecutable *aElfExec, PLMemAddr32 aAddr, PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType, Elf32_Rel* aRel);
	virtual ~ElfRelocation();

	static bool ValidRelocEntry(PLUCHAR aType);
	static ElfRelocation* NewRelocEntry( ElfExecutable *aElfExec, PLMemAddr32 aAddr, \
		PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType,\
		void* aRel, bool aImportRel);

	virtual bool IsImportRelocation() =0;
	virtual void Add() = 0;

	PLMemAddr32 iAddr;
	PLUINT32	iAddend;
	PLUINT32	iSymNdx;
	PLUCHAR		iRelType;
	Elf32_Rel	*iRel;
	Elf32_Sym	*iSymbol;
	ElfExecutable *iElfExec;
	Elf32_Phdr	*iSegment;
	ESegmentType iSegmentType;
};




#endif // !defined(_PL_ELFRELOCATION_H_)
