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
// Implementation of the Class ElfImportRelocation for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "pl_elfimportrelocation.h"
#include "pl_elfexecutable.h"


/**
Constructor for class ElfImportRelocation
@param aElfExec - Instance of class ElfExecutable
@param aAddr
@param aAddend
@param aIndex
@param aRelType
@param aRel
@internalComponent
@released
*/
ElfImportRelocation::ElfImportRelocation(ElfExecutable *aElfExec,PLMemAddr32 aAddr, \
			PLUINT32 aAddend, PLUINT32 aIndex, PLUCHAR aRelType, \
			Elf32_Rel* aRel): \
			ElfRelocation(aElfExec, aAddr, aAddend, aIndex, aRelType, aRel), \
			iVerRecord(NULL)
{
	iSymbol = &(aElfExec->iElfDynSym[iSymNdx]);
	iSegment = aElfExec->Segment(iAddr);
	iSegmentType = aElfExec->SegmentType(iAddr);
}



/**
Destructor for class ElfImportRelocation
@internalComponent
@released
*/
ElfImportRelocation::~ElfImportRelocation(){

}


/**
Function to know is import relocation
@internalComponent
@released
*/
bool ElfImportRelocation::IsImportRelocation(){
	return true;
}

/**
Function to add import relocation
@internalComponent
@released
*/
void ElfImportRelocation::Add() {
	iElfExec->AddToImports(this);
}


