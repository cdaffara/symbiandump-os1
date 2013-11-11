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
// Implementation of the Header file for Class UseCaseBase of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef USECASEBASE_H
#define USECASEBASE_H

#include "pl_symbol.h"
#include "e32imagefile.h"
#include "parameterlistinterface.h"

class E32ImageFile;

/**
This class is the base class for the use cases. The appropriate usecases like LibraryTarget,
DLLTarget, EXETarget are derived from this class. 

Other classes like ParameterManager are dependant on this class.

@internalComponent
@released
*/
class UseCaseBase
{

public:
	UseCaseBase(ParameterListInterface* aParameterListInterface);
	virtual ~UseCaseBase();
	char * DefInput();
	char * DSOOutput();
	char * LinkAsDLLName();
	char * FileName(char *aFileName);

//const char * DefOutput();
	char *DefOutput();

	char *LogFile();
	bool LogFileOption();

char * InputElfFileName();

const char * OutputDSOFileName();

const char * OutputE32FileName();

bool AllowDllData();

size_t HeapCommittedSize();

size_t HeapReservedSize();

size_t StackCommittedSize();

unsigned int GetUid1();

unsigned int GetUid2();

unsigned int GetUid3();

unsigned int GetSecureId();
bool GetSecureIdOption();

unsigned int GetVendorId();

unsigned int GetVersion();

bool GetCallEntryPoints();

SCapabilitySet GetCapability();
bool Unfrozen();

TProcessPriority GetPriority();

bool GetFixedAddress();

bool GetCompress();
unsigned int GetCompressionMethod();

unsigned int GetFPU();

ParameterListInterface::LibSearchPaths &  GetLibSearchPaths();


bool GetIgnoreNonCallable();

char GetDirectorySeparator();

bool IsCodePaged();
bool IsCodeUnpaged();
bool IsCodeDefaultPaged();

bool IsDataPaged();
bool IsDataUnpaged();
bool IsDataDefaultPaged();

bool ExcludeUnwantedExports();
bool IsCustomDllTarget();
bool GetNamedSymLookup();

bool IsDebuggable();

bool IsSmpSafe();

SymbolType SymbolTypeF(char * aName);
/**
This function creates the appropriate target.

@internalComponent
@released

@return EXIT_SUCCESS if the generation of the target is successful, else EXIT_FAILURE
*/
	virtual int Execute() = 0;

protected:
	/** Pointer to the ParameterListInterface */
	ParameterListInterface *iParameterListInterface;

	
};




#endif // USECASEBASE_H
