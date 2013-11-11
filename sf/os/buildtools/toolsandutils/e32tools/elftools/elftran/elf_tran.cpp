// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// derived from pe_tran.cpp
// 
//

#include <time.h>
#include <malloc.h>
#include <string.h>
#include "elftran.h"
#include <e32std.h>
#include <e32std_private.h>
#include <tools/elfdefs.h>
#include "elffile.h"
#include "h_ver.h"
#include "h_utl.h"

extern TUid gUid1, gUid2, gUid3;
extern int gSetUid1, gSetUid2, gSetUid3;

int gAlignConstSection=FALSE;
TUint gConstSectionAddressMask=0;
static TUint gRequiredConstPadding;

E32ImageFile* E32ImageFile::New()
	{
	return new E32ImageFile_ELF;
	}

E32ImageFile_ELF::E32ImageFile_ELF()
	{
	}

E32ImageFile_ELF::~E32ImageFile_ELF()
	{
	}

TInt E32ImageFile_ELF::DoCodeHeader(ELFFile &aElfFile)
//
// Calculate the code parts of the ELFFile
//
	{

	TInt size=ALIGN4(aElfFile.GetCodeSize());
	iHdr->iCodeSize = iHdr->iTextSize = size;
	// make it the offset from the beginning of the file.....
	if(iHdr->iExportDirCount==0)
		iHdr->iExportDirOffset = 0;
	else
		iHdr->iExportDirOffset = aElfFile.GetExportTableOffset() + iHdr->iCodeOffset;

	return size;
	}

TInt E32ImageFile_ELF::DoDataHeader(ELFFile &aElfFile, TUint aDataBase)
	{

	if (aDataBase==0 && aElfFile.iDataSegmentHdr)
		aDataBase=aElfFile.iDataSegmentHdr->p_vaddr;
	TInt size=0;

	iHdr->iDataBase=aDataBase;

	if (aElfFile.HasInitialisedData())
		{
	        size=ALIGN4(aElfFile.GetDataSize());
		iHdr->iDataOffset = iHdr->iCodeOffset + iHdr->iCodeSize;
		iHdr->iDataSize = size;
		}
	if (aElfFile.HasBssData())
		{
		iHdr->iBssSize = ALIGN4(aElfFile.GetBssSize());
		}
	return size;
	}

TInt E32ImageFile_ELF::CopyCode(char *p, ELFFile &aElfFile)
//
// Copies the files code sections to p
// returns the number of bytes copied or KErrGeneral
//
	{
	TInt size=aElfFile.GetCodeSize();
	memcpy(p, (char *)aElfFile.GetCode(), size);
	p+=ALIGN4(size);
	return iHdr->iCodeSize = size;
	}

TInt E32ImageFile_ELF::CopyData(char *p, ELFFile &aElfFile)
	{
	TInt size=aElfFile.GetDataSize();
	if (size) memcpy(p, (char *)aElfFile.GetData(), size);
	return size;
	}

TInt E32ImageFile_ELF::Translate(const char* aFileName, TUint aDataBase, TBool aAllowDllData, \
								 TBool aSymLkupEnabled)
//
// Translate a ELF format file to a E32Image file
//
	
{
	iSource = EElfFile;
	ELFFile elffile;
	if (!elffile.Init((const TText * const)aFileName)) return KErrGeneral;
		
	iFileName = strdup(aFileName);

	Adjust(ALIGN4(sizeof(E32ImageHeaderV)));	// fixed for now because holes not supported
	SetDefaultHeader();
	iHdr->iDllRefTableCount = elffile.NumberOfImportDlls();
	iHdr->iExportDirCount = elffile.NumberOfExports();
	iHdr->iCodeBase = elffile.iLinkedBase;

	if(aSymLkupEnabled)
	{
		if( !SetUpLookupTable(elffile) )
			return KErrGeneral;
	}


	TInt size = ALIGN4(sizeof(E32ImageHeaderV));	// fixed for now because holes not supported
	iHdr->iCodeOffset = size;
	TInt pos = size;
	size+=DoCodeHeader(elffile);

	size += DoSymbolLookupHeader(elffile, size - pos);

	TInt nimports=elffile.NumberOfImports();
	TInt importSectionSize;
	char *newImportSection=CreateImportSection(elffile, importSectionSize);

	TInt t=DoDataHeader(elffile, aDataBase);
	if (t>0)
		{
		iHdr->iDataOffset = size;
		size += t;
		}
	if (importSectionSize!=0)
		{
		iHdr->iImportOffset=size;
		size+=ALIGN4(importSectionSize);
		}

	char *newCodeRelocs=NULL;
	char *newDataRelocs=NULL;
	TInt codeRelocSize=0, dataRelocSize=0;
	TInt nCodeRelocs=elffile.NumberOfCodeRelocs();
	TInt nDataRelocs=elffile.NumberOfDataRelocs();
	if (nCodeRelocs + nDataRelocs)
		{
		Elf32_Rel **codeRelocs=new Elf32_Rel * [nCodeRelocs];
		Elf32_Rel **dataRelocs=new Elf32_Rel * [nDataRelocs];
		if (!elffile.GetRelocs(codeRelocs, dataRelocs)) return KErrGeneral;
		       
		FixRelocs(elffile, codeRelocs, dataRelocs);
		if (elffile.iCodeSegmentHdr)
			newCodeRelocs=CreateRelocs(elffile, codeRelocs, nCodeRelocs, codeRelocSize, elffile.iCodeSegmentHdr->p_vaddr);
		if (elffile.iDataSegmentHdr)
			newDataRelocs=CreateRelocs(elffile, dataRelocs, nDataRelocs, dataRelocSize, elffile.iDataSegmentHdr->p_vaddr);
		if (codeRelocSize)
			{
			iHdr->iCodeRelocOffset = size;
			size += codeRelocSize;
			}
		if (dataRelocSize)
			{
			iHdr->iDataRelocOffset = size;
			size += dataRelocSize;
			}
		delete [] codeRelocs;
		delete [] dataRelocs;
		}

	Adjust(size);
	t=CopyCode(iData + pos, elffile);
	if (t<0)
		return KErrGeneral;
	pos += t;
	t = CopyExportSymInfo(iData+pos, elffile);
	if (t<0)
		return KErrGeneral;
	pos += t;

	pos += CopyData(iData + pos, elffile);
	if (nimports)
		{
		memcpy(iData + pos, newImportSection, importSectionSize);
		pos += ALIGN4(importSectionSize);
		}
	if (codeRelocSize)
		{
		memcpy(iData + pos, newCodeRelocs, codeRelocSize);
		pos += codeRelocSize;
		}
	if (dataRelocSize)
		{
		memcpy(iData + pos, newDataRelocs, dataRelocSize);
		pos += dataRelocSize;
		}

	// locate the entry point
	TUint entryPointOffset=elffile.GetEntryPointOffset();

	// Arrange a header for this E32 Image
	iHdr->iCpuIdentifier = (TUint16)ECpuArmV4;
	// Import format is ELF-derived
	iHdr->iFlags |= KImageImpFmt_ELF;
	// ABI is ARM EABI
	iHdr->iFlags |= KImageABI_EABI;
	if (ImageIsDll(elffile))
		{
		iHdr->iFlags |= KImageDll;
		if (iHdr->iDataSize && !aAllowDllData)
			return Print(EError, "Dll '%s' has initialised data.\n", iFileName);
		if (iHdr->iBssSize  && !aAllowDllData)
			return Print(EError, "Dll '%s' has uninitialised data.\n", iFileName);
		}
	iHdr->iHeapSizeMin = elffile.iHeapCommittedSize;
	iHdr->iHeapSizeMax = elffile.iHeapReservedSize;
	iHdr->iStackSize = elffile.iStackCommittedSize;
	iHdr->iEntryPoint = entryPointOffset;
	TInt r = DetermineEntryPointType();
	if (r == KErrCorrupt)
		return Print(EError, "File '%s': Bad Entry Point.\n", iFileName);
	else if (r == KErrNotSupported)
		return Print(EError, "File '%s': Bad Entry Point Type.\n", iFileName);

	SetUpExceptions(elffile);

	delete [] newImportSection;
	delete [] newCodeRelocs;
	delete [] newDataRelocs;

	return KErrNone;
	}

TBool E32ImageFile_ELF::Translate(ELFFile &aElfFile)
    {
    // VT fix for warning
    // fix warning in Linux
    return Translate((const char*)aElfFile.iFileName, 0, EFalse);
    }

TBool E32ImageFile_ELF::ImageIsDll(ELFFile& aElfFile)
    {
    return aElfFile.ImageIsDll();
    }

void E32ImageFile_ELF::SetUpExceptions(ELFFile &aElfFile)
    {
    aElfFile.GetExceptionIndexInfo(iHdr->iExceptionDescriptor);
    }

void E32ImageFile_ELF::SetSymNameLookup(TInt aSymNameLkupEnabled)
	{
	if(aSymNameLkupEnabled)
		iHdr->iFlags |= KImageNmdExpData;
	else
		iHdr->iFlags &= ~KImageNmdExpData;
	}
TBool E32ImageFile_ELF::IsNamedLookupEnabled()
{
	return (iHdr->iFlags & KImageNmdExpData);
}

TBool E32ImageFile_ELF::SetUpLookupTable(ELFFile &aElfFile)
// Symbol lookup by name is enabled. Create the symbol table with names
// and their values.
{
	if(!aElfFile.SetUpLookupTable() )
		return FALSE;
	SetSymNameLookup(TRUE);
	return TRUE;
}

// Build the infrastructure for symbol lookup via name
TInt E32ImageFile_ELF::DoSymbolLookupHeader(ELFFile &aElfFile, TInt aBaseOffset)
	{
	if(!IsNamedLookupEnabled())
		return 0;
	
	aElfFile.SetLookupTblBase(aBaseOffset);
	return aElfFile.GetLookupTblSize(); 
	}
TUint E32ImageFile_ELF::CopyExportSymInfo(char *p, ELFFile &aElfFile)
	{
	iHdr->iCodeSize += aElfFile.GetLookupTblSize();
	iHdr->iTextSize = iHdr->iCodeSize ;
	return aElfFile.GetSymLookupSection(p);
	}
