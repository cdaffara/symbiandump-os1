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

#include "pl_elfrelocation.h"
#include "pl_elfimportrelocation.h"
#include "pl_elflocalrelocation.h"
#include "pl_dllsymbol.h"

/**
Constructor for class ElfRelocation
@param aElfExec - Instance of class ELfExecutable
@param aAddr
@param aAddend
@param aIndex
@param aRelType - Relocation type
@param aRel
@internalComponent
@released
*/
ElfRelocation::ElfRelocation(ElfExecutable *aElfExec, PLMemAddr32 aAddr, \
			PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType, \
			Elf32_Rel* aRel) : iAddr(aAddr), iAddend(aAddend), \
			iSymNdx(aIndex), iRelType(aRelType), iRel(aRel),  iElfExec(aElfExec)
{
}



/**
Destructor for class ElfRelocation
@internalComponent
@released
*/
ElfRelocation::~ElfRelocation(){

}


/**
Function to create new relocation entry
@param aElfExec - Instance of class ElfExecutable
@param aAddr    - location where the relocation refers to.
@param aAddend  - addend for the relocation entry
@param aIndex   - symbol index
@param aRelType - Relocation type
@param aRel     - Elf relocation entry
@param aImportRel - Import relocation
@internalComponent
@released
*/
ElfRelocation* ElfRelocation::NewRelocEntry(ElfExecutable *aElfExec, PLMemAddr32 aAddr, \
				PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType, \
				void* aRel, bool aImportRel)
{
	ElfRelocation	*aReloc = NULL;
	if(aImportRel)
		aReloc = new ElfImportRelocation(aElfExec, aAddr, aAddend, aIndex, aRelType, (Elf32_Rel*)aRel);
	else
		aReloc = new ElfLocalRelocation(aElfExec, aAddr, aAddend, aIndex, aRelType, (Elf32_Rel*)aRel);

	return aReloc;
}


/**
This function verifies if the relocation entry is required to be
handled by the postlinker.
@param aType - type of relocation
@return - True if valid relocation type, otherwise false
@internalComponent
@released
*/
bool ElfRelocation::ValidRelocEntry(PLUCHAR aType) {
	
	switch(aType)
	{
	case R_ARM_ABS32:
	case R_ARM_GLOB_DAT:
	case R_ARM_JUMP_SLOT:
	case R_ARM_RELATIVE:
	case R_ARM_GOT_BREL:
		return true;
	case R_ARM_NONE:
		return false;
	default:
		return false;
	}
}


