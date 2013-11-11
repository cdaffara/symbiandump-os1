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
// Implementation of the Class PolyDllFB Target for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "polydll_fb_target.h"

/**
Constructor for the POLYDLLFBTarget Class

@internalComponent
@released
*/
POLYDLLFBTarget::POLYDLLFBTarget(ParameterListInterface* aParameterListInterface) : ExportTypeFBTarget(aParameterListInterface) {
}

/**
Destructor for the POLYDLLFBTarget Class

@internalComponent
@released
*/
POLYDLLFBTarget::~POLYDLLFBTarget()
{
	if(iDefExports)
	{
		SymbolList::iterator aItr = iDefExports->begin();
		SymbolList::iterator last = iDefExports->end();
		Symbol *temp;

		while( aItr != last)
		{
			temp = *aItr;
			aItr++;
			delete temp;
		}
		iDefExports->clear();
	}
}

/**
Function to process the symbols to be exported. In case of Poly DLL, there might be
predefined symbols passed to '--sydef' option, need to consider them alongwith the 
symbols coming from the ELF file.
 
@internalComponent
@released
*/
void POLYDLLFBTarget::ProcessExports() 
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

	iDefExports = new SymbolList;

	for (int k=0; k < count; k++)
	{
		SymbolType aType = SymbolTypeCode;
		aSymbolEntry = new Symbol(aSysDefSymbols[k].iSysDefSymbolName, aType);
		aSymbolEntry->SetOrdinal(aSysDefSymbols[k].iSysDefOrdinalNum);
		iDefExports->push_back(aSymbolEntry);
	}

	ValidateExports(iDefExports);
	CreateExports();
}

/**
Function to generate the output E32 image. Incase of an output DEF file, then the 
DEF file and the corresponding DSO file should be generated.
  
@internalComponent
@released
*/
void POLYDLLFBTarget::GenerateOutput() {
	
	if( UseCaseBase::DefOutput() ) {
		WriteDefFile();
	}
	if(UseCaseBase::DSOOutput() && UseCaseBase::LinkAsDLLName()) {
		WriteDSOFile();
	}
	WriteE32();
}



