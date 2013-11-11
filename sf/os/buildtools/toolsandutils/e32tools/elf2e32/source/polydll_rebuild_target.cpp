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
// Implementation of the Class PolyDLL Rebuild Target for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "polydll_rebuild_target.h"
#include "deffile.h"
#include "errorhandler.h"

#include <iostream>

using std::list;

/**
Constructor for the POLYDLLRebuildTarget Class

@internalComponent
@released
*/
POLYDLLRebuildTarget::POLYDLLRebuildTarget(ParameterListInterface* aParameterListInterface) : ExportTypeRebuildTarget(aParameterListInterface) {
}

/**
Destructor for the POLYDLLRebuildTarget Class

@internalComponent
@released
*/
POLYDLLRebuildTarget::~POLYDLLRebuildTarget() 
{
}

/**
Function to process the symbols to be exported. In case of Poly DLL, there might be
predefined symbols passed to '--sydef' option, need to consider them alongwith the 
symbols coming from the DEF file and ELF file.
 
@internalComponent
@released
*/
void POLYDLLRebuildTarget::ProcessExports() 
{
	int count = iParameterListInterface->SysDefCount();
	ParameterListInterface::Sys aSysDefSymbols[10];

	int j=0, i=count;
	while (i)
	{
		aSysDefSymbols[j] = iParameterListInterface->SysDefSymbols(j);
		j++; i--;
	}

	typedef SymbolList::iterator Iterator;

	Symbol *aSymbolEntry;

	SymbolList *iSysDefExports = new SymbolList;

	iDefExports = iDefFile->ReadDefFile(iParameterListInterface->DefInput());
	
	for (int k=0; k < count; k++)
	{
		SymbolType aType = SymbolTypeCode;
		aSymbolEntry = new Symbol(aSysDefSymbols[k].iSysDefSymbolName, aType);
		aSymbolEntry->SetOrdinal(aSysDefSymbols[k].iSysDefOrdinalNum);
		iSysDefExports->push_back(aSymbolEntry);
	}

	// Check if the Sysdefs and the DEF file are matching.

	Iterator aBegin = iSysDefExports->begin();
	Iterator aEnd = iSysDefExports->end();

	Iterator aDefBegin = iDefExports->begin();
	Iterator aDefEnd = iDefExports->end();

	std::list<String> aMissingSysDefList;

	while ((aDefBegin != aDefEnd) && (aBegin != aEnd))
	{
		if (strcmp((*aBegin)->SymbolName(), (*aDefBegin)->SymbolName()))
			aMissingSysDefList.push_back((*aBegin)->SymbolName());
		aBegin++;
		aDefBegin++;
	}

	if( aMissingSysDefList.size() ) 
		throw SysDefMismatchError(SYSDEFSMISMATCHERROR, aMissingSysDefList, UseCaseBase::DefInput());
		
	ValidateExports(iDefExports);
	CreateExports();
}

/**
Function to generate the output E32 image. Incase of an output DEF file, then the 
DEF file and the corresponding DSO file should be generated.
  
@internalComponent
@released
*/
void POLYDLLRebuildTarget::GenerateOutput() {
	
	if( UseCaseBase::DefOutput() ) {
		WriteDefFile();
	}
	if(UseCaseBase::DSOOutput() && UseCaseBase::LinkAsDLLName()) {
		WriteDSOFile();
	}
	WriteE32();
}
