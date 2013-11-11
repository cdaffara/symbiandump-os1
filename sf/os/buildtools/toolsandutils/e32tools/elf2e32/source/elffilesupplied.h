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
// ElfFileSupplied class for elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef __ELFFILESUPPLIED_H_
#define __ELFFILESUPPLIED_H_

#include "usecasebase.h"
#include "e32exporttable.h"
#include <list>

class Symbol;
class DllSymbol;
class DSOHandler;
class ParameterManager;

typedef std::list<Symbol*> SymbolList;

/**
This class is derived from the base class UseCaseBase and handles case for elf file supplied.

@internalComponent
@released
*/
class ElfFileSupplied : public UseCaseBase
{

public:
	ElfFileSupplied(ParameterListInterface* aParameterListInterface);
	~ElfFileSupplied();
	int Execute();

	void ReadElfFile();
	virtual void ProcessExports();
	virtual void GenerateOutput();
	void ValidateExports(SymbolList* aDefExports);
	void CreateExports();
	void WriteDefFile();
	void WriteDSOFile();
	void WriteE32();
	virtual bool ImageIsDll();
	virtual bool WarnForNewExports();
	SymbolType SymbolTypeF(char * aName);
	E32ImageHeaderV * AllocateE32ImageHeader();

	virtual void CreateExportTable();
	void CreateExportBitMap();
	virtual size_t GetNumExports();
	virtual size_t GetExportOffset();
	virtual bool AllocateExportTableP();
	virtual char * GetExportTable();
	virtual size_t GetExportTableSize();

	PLUINT16 GetExportDescSize();
	PLUINT8 GetExportDescType();
	size_t GetExportTableAddress();
	//Method to check whether a symbol name is unwanted
	int UnWantedSymbolp(const char * aSymbol);

protected:
	SymbolList iSymList;
	DSOHandler	*iElfIfc;

	E32ExportTable iExportTable;
	int iNumAbsentExports;
	PLUINT8 * iExportBitMap;

	E32ImageFile * iE32ImageFile;
	ElfExecutable *iElfExecutable;
	PLUINT16 iExportDescSize;
	PLUINT8 iExportDescType;
};


#endif

