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
// Implementation of the Class LibraryTarget for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "librarytarget.h"
#include "pl_dso_handler.h"
#include "deffile.h"
#include <iostream>

/**
Constructor for the Library Class

@internalComponent
@released
*/
LibraryTarget::LibraryTarget(ParameterListInterface* aParameterListInterface):
UseCaseBase(aParameterListInterface)
{
	iElfIfc = new DSOHandler(aParameterListInterface);
	iDefFile = new DefFile();
}


/**
Destructor for the Library Class

@internalComponent
@released
*/
LibraryTarget::~LibraryTarget()
{
	delete iElfIfc;
	delete iDefFile;
}

/**
Execute Function for the library target creation. The symbols are read from the input
DEF file. The DSO file is generated on passing the symbols.
 
@internalComponent
@released

@return EXIT_SUCCESS, on successful creation of library.
*/
int LibraryTarget::Execute()
{
	SymbolList *aSymList;
	aSymList = ReadInputDefFile();
	GenerateOutput(aSymList);
	return EXIT_SUCCESS;

}

/**
Function to read the symbols from the DEF file.
 
@internalComponent
@released

@return the list of symbols read from the DEF file.
*/
SymbolList* LibraryTarget::ReadInputDefFile()
{
	char * aDEFFileName = UseCaseBase::DefInput();

	return iDefFile->ReadDefFile(aDEFFileName);
}

/**
Function to generate the output DSO File.
 
@internalComponent
@released
*/
void LibraryTarget::GenerateOutput(SymbolList* aSymList)
{
	char * aLinkAs = UseCaseBase::LinkAsDLLName();
	char * aDSOName = UseCaseBase::DSOOutput();
	char * aDSOFileName = UseCaseBase::FileName(aDSOName);

	iElfIfc->WriteElfFile( aDSOName, aDSOFileName, aLinkAs, *aSymList);	
}
