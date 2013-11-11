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
// Implementation of the Class ElfLocalRelocation for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "pl_elflocalrelocation.h"
#include "pl_elfexecutable.h"
#include "e32imagedefs.h"

/**
Constructor for class ElfLocalRelocation
@param aElfExec - Instance of class ElfExecutable
@param aAddr    - location where the relocation refers to.
@param aAddend  - addend for the relocation entry
@param aIndex   - symbol index
@param aRelType - Relocation type
@param aRel     - Elf relocation entry
@internalComponent
@released
*/
ElfLocalRelocation::ElfLocalRelocation(ElfExecutable *aElfExec, PLMemAddr32 aAddr, \
			PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType, \
			Elf32_Rel* aRel, bool aVeneerSymbol): \
		ElfRelocation(aElfExec, aAddr, aAddend, aIndex, aRelType, aRel)
{
	iSegment = aElfExec->Segment( iAddr );
	iSegmentType = aElfExec->SegmentType( iAddr );
	iSymbol = &(aElfExec->iElfDynSym[iSymNdx]);
	iDelSym = false;
	iVeneerSymbol = aVeneerSymbol;
}

/**
Constructor for class ElfLocalRelocation
@param aElfExec - Instance of class ElfExecutable
@param aAddr    - location where the relocation refers to.
@param aAddend  - addend for the relocation entry
@param aIndex
@param aRelType - Relocation type
@param aRel     - Elf relocation entry
@param aSegmentType - Segment type
@param aSym			- Elf symbol
@param aDelSym		- indicate if the symbol is to be deleted by this relocation.
@internalComponent
@released
*/
ElfLocalRelocation::ElfLocalRelocation(ElfExecutable *aElfExec, PLMemAddr32 aAddr, \
			PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType, \
			Elf32_Rel* aRel, ESegmentType aSegmentType, Elf32_Sym* aSym,bool aDelSym, bool aVeneerSymbol): \
		ElfRelocation(aElfExec, aAddr, aAddend, aIndex, aRelType, aRel)
{
	iSegmentType = aSegmentType;
	iSegment = aElfExec->Segment(aSegmentType);
	//iSymbol = &(aElfExec->iElfDynSym[iSymNdx]);
	iSymbol = aSym;
	iDelSym = aDelSym;
	iVeneerSymbol = aVeneerSymbol;
}


/**
Destructor for class ElfLocalRelocation
@internalComponent
@released
*/
ElfLocalRelocation::~ElfLocalRelocation()
{
	if(iDelSym)
	{
		DELETE_PTR(iSymbol);
	}

}


/**
This function overrides the virtual function to indicate that this
relocation is not an import relocation.
@return false
@internalComponent
@released
*/
bool ElfLocalRelocation::IsImportRelocation(){
	return false;
}

/**
Function to add local relocations
@internalComponent
@released
*/
void ElfLocalRelocation::Add() {
	iElfExec->AddToLocalRelocations(this);
}

/**
This function adjusts the fixup for the relocation entry.
@return - Relocation type
@internalComponent
@released
*/
PLUINT16 ElfLocalRelocation::Fixup()
{
	if(!ExportTableReloc() && !iVeneerSymbol)
	{
		Elf32_Word * aLoc = iElfExec->GetFixupLocation(this, iAddr);
		Elf32_Word aLocVal = * aLoc;

		if (iRelType == R_ARM_ABS32 || iRelType == R_ARM_GLOB_DAT ) 
		{

			Elf32_Word aFixedVal = aLocVal + iSymbol->st_value;
			*aLoc = aFixedVal;
		}
	}

	ESegmentType aType;
	if( iSymbol )
		aType = iElfExec->Segment(iSymbol);
	else
		aType = iSegmentType;

	if (aType == ESegmentRO)
		return KTextRelocType;
	else if (aType == ESegmentRW)
		return KDataRelocType;

	// maybe this should be an error
	return KInferredRelocType;
}

/**
Function for export table relocation
@return - True
@internalComponent
@released
*/
bool ElfLocalRelocation::ExportTableReloc()
{
	return iRel == 0;
}



