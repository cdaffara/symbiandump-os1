// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Derived in part from E32IMAGE.H
// 
//

#ifndef __ELFTRAN_H__
#define __ELFTRAN_H__

#include <e32std.h>
#include "e32image.h"
#include "e32ldfmt.h"
#include <tools/elfdefs.h>

class ELFFile;

//
class E32ImageFile_ELF : public E32ImageFile
	{
public:
	E32ImageFile_ELF();
	virtual ~E32ImageFile_ELF();
	virtual TBool Translate(const char* aFileName, TUint aDataBase, TBool aAllowDllData, \
		TBool aSymLkupEnabled = FALSE);
	TBool Translate(ELFFile& aElfFile);
	TBool ImageIsDll(ELFFile& aElfFile);
private:
	TInt DoCodeHeader(ELFFile &aElfFile);
	TInt DoDataHeader(ELFFile &aElfFile, TUint aDataBase);
	TInt CopyCode(char *aPtr, ELFFile &aElfFile);
	TInt CopyData(char *aPtr, ELFFile &aElfFile);

	char *CreateImportSection(ELFFile &aElfFile, TInt &aSize);
	void CreateExportSection(char *aPtr, ELFFile &aElfFile);

	void FixRelocs(ELFFile &aElfFile, Elf32_Rel **codeRelocs, Elf32_Rel **dataRelocs);
	char *CreateRelocs(ELFFile& aElfFile, Elf32_Rel **relocs, TInt nrelocs, TInt &aSize, TUint aBase);

	TUint FixAddress(ELFFile &aElfFile, TUint va, Elf32_Rel * rel);

	void SetUpExceptions(ELFFile &aElfFile);
	void SetSymNameLookup(TInt aSymNameLkupEnabled);
	TBool IsNamedLookupEnabled();
	TBool SetUpLookupTable(ELFFile &aElfFile);
	TInt DoSymbolLookupHeader(ELFFile &aElfFile, TInt aBaseOffset);
	TUint CopyExportSymInfo(char *aPtr, ELFFile &aElfFile);
	};

#endif



