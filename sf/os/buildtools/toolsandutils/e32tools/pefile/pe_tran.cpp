// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <time.h>
#include <malloc.h>
#include <string.h>
#include "e32image.h"
#include <e32std.h>
#include <e32std_private.h>
#include "pe_defs.h"
#include "pe_file.h"
#include "h_ver.h"
#include "h_utl.h"

int gAlignConstSection=FALSE;
TUint gConstSectionAddressMask=0;
static TUint gRequiredConstPadding;

extern char* gX86imp;
extern int gX86num_imp_dlls;
extern int gX86imp_size;
extern int gX86num_imports;

E32ImageFile* E32ImageFile::New()
	{
	return new E32ImageFile_PE;
	}

E32ImageFile_PE::E32ImageFile_PE()
	{
	}

E32ImageFile_PE::~E32ImageFile_PE()
	{
	}

TUint E32ImageFile_PE::ImportAddressTableOffset()
//
// Return the offset of the iat
//
	{
	return iHdr->iTextSize;
	}

TUint E32ImageFile_PE::ConstOffset()
//
// return the offset of the const data
//
	{
	return iConstOffset;
	}

void E32ImageFile_PE::CreateExportDirectory(char *aPtr, PEFile &aPeFile)
//
// create a new format export directory
//
	{
	
	if (iHdr->iExportDirCount==0)
		return;
	TUint *src=(TUint *)aPeFile.iSectionData[KExportSection];
	TUint *dst=(TUint *)aPtr;
	PIMAGE_EXPORT_DIRECTORY dir=(PIMAGE_EXPORT_DIRECTORY)src;
	src+=(((TInt)dir->AddressOfFunctions)-((TInt)aPeFile.iSectionHeader[KExportSection]->VirtualAddress))/4;
	TUint i;
	for (i=0; i<dir->NumberOfFunctions; i++)
		{
		TUint va=*src++;
		dst[i]=va;
		}
	FixExportDirectory(dst, aPeFile);
	}

void E32ImageFile_PE::FixExportDirectory(TUint *aExportDir, PEFile &aPeFile)
//
// Fix the export directory
//
	{

	TUint lb = aPeFile.iLinkedBase;
	TUint *exportdir=aExportDir;
	TInt n;
	for (n=0; n<(TInt)iHdr->iExportDirCount; n++)
		{
		TUint va=*exportdir;
		if (!gLittleEndian) ByteSwap(va);

		// va is the address of an exported item, so assume it can't have been offset
		TInt i=aPeFile.FindSectionByVa(va+lb);
		if (i==KTextSection)
			va=va-aPeFile.iSectionHeader[i]->VirtualAddress;
		else if (i==KConstSection)
			va=va-aPeFile.iSectionHeader[i]->VirtualAddress+ConstOffset();
		else if (i==KDataSection)
			va=va-aPeFile.iSectionHeader[i]->VirtualAddress+DataOffset();
		else if (i==KBssSection)
			va=va-aPeFile.iSectionHeader[i]->VirtualAddress+BssOffset();
		else
			{
			if (va == 0)
				Print(EWarning, "No export specified for ordinal %d\n", n+1, va);
			else
				Print(EError, "Export %d (address %08x) is not from .text, .rdata, or data sections\n", n+1, va);
			}
		if (!gLittleEndian) ByteSwap(va);
		*exportdir++=va;
		}
	}

TInt E32ImageFile_PE::DoCodeHeader(PEFile &aPeFile)
//
// Calculate the code parts of the pefile
//
	{

	// .text
	TInt size=ALIGN4(aPeFile.iSectionHeader[KTextSection]->Misc.VirtualSize);

	// .rdata
	iConstOffset=0;
	if (gAlignConstSection)
		{
	    // Compute the amount of padding to put before the
	    // const section to align it correctly
	    TUint   oldAddressBits = aPeFile.iSectionHeader[KConstSection]->VirtualAddress & gConstSectionAddressMask;
	    TUint   oldConstAddress = size;
	    TUint   newConstAddress = oldConstAddress;
	    // slow but sure
	    while ((newConstAddress & gConstSectionAddressMask) != oldAddressBits)
	    	{
			newConstAddress++;
			}
	    gRequiredConstPadding = newConstAddress - oldConstAddress;
	    size += gRequiredConstPadding;
		}
	if (aPeFile.iSectionHeader[KConstSection])
		{
		iConstOffset = size;
		size += ALIGN4(aPeFile.iSectionHeader[KConstSection]->Misc.VirtualSize);
		}

	// .crt
	iCrtOffset=0;
	if (aPeFile.iSectionHeader[KCrtSection])
		{
		iCrtOffset = size;
		size += ALIGN4(aPeFile.iSectionHeader[KCrtSection]->Misc.VirtualSize);
		}

	iHdr->iTextSize=size; // The "text" part of the E32 code section combines PE's .text + .rdata + .crt.
						  // The remainder of the E32 code section is the IAT + export directory.

	// Import Address Table (IAT)
	TInt nimports=gX86imp?gX86num_imports:aPeFile.NumberOfImports();
	if (nimports!=0)
		size+=nimports*4+4; // null terminated
	
	// Export Dir
	if (iHdr->iExportDirCount)
		{
		iHdr->iExportDirOffset = iHdr->iCodeOffset + size;
		size += ALIGN4(iHdr->iExportDirCount*4);
		}
	iHdr->iCodeSize=size;
	return size;
	}

TInt E32ImageFile_PE::DoDataHeader(PEFile &aPeFile, TUint aDataBase)
//
//
//
	{

	if (aDataBase==NULL)
		aDataBase=iHdr->iCodeBase+iHdr->iCodeSize;
	TInt size=0;
	if (PEFile::HasInitialisedData(aPeFile.iSectionHeader[KDataSection]))
		{
		size=ALIGN4(aPeFile.iSectionHeader[KDataSection]->Misc.VirtualSize);
		iHdr->iDataBase=aDataBase;
		iHdr->iDataOffset = iHdr->iCodeOffset + iHdr->iCodeSize;
		TInt bsssize=aPeFile.iSectionHeader[KDataSection]->Misc.VirtualSize-aPeFile.iSectionHeader[KDataSection]->SizeOfRawData;
		// drop any uninitialised data
		if (bsssize>0)
			{
			iHdr->iBssSize+=bsssize;
			size=ALIGN4(aPeFile.iSectionHeader[KDataSection]->SizeOfRawData);
			}
		iHdr->iDataSize=size;
		}
	else if (aPeFile.iSectionHeader[KDataSection])
		{ // just .bss
		iHdr->iDataBase=aDataBase;
		TInt bsssize=aPeFile.iSectionHeader[KDataSection]->Misc.VirtualSize;
		iHdr->iBssSize+=bsssize;
		}
	if (aPeFile.iSectionHeader[KBssSection])
		{
		iHdr->iBssSize+=ALIGN4(aPeFile.iSectionHeader[KBssSection]->Misc.VirtualSize);
		if (iHdr->iDataBase==0) // .bss but no .data
			iHdr->iDataBase=aDataBase;
		}
	return size;
	}

TInt E32ImageFile_PE::CopyCode(char *p, PEFile &aPeFile)
//
// Copies the files code sections to p
// returns the number of bytes copied or KErrGeneral
//
	{

	// text
	TInt size=aPeFile.iSectionHeader[KTextSection]->Misc.VirtualSize;
	memcpy(p, aPeFile.iSectionData[KTextSection], size);
	TInt text_offset=ALIGN4(size);
	p+=text_offset;

	// rdata
	if (aPeFile.iSectionData[KConstSection])
		{
		if (gAlignConstSection)
			{
			// add padding ahead of const section
			p += gRequiredConstPadding;
			}
		TInt size=ALIGN4(aPeFile.iSectionHeader[KConstSection]->Misc.VirtualSize);
		memcpy(p, aPeFile.iSectionData[KConstSection], size);
		p+=size;
		}
	if (aPeFile.iSectionData[KCrtSection])
		{
		TInt size=ALIGN4(aPeFile.iSectionHeader[KCrtSection]->Misc.VirtualSize);
		memcpy(p, aPeFile.iSectionData[KCrtSection], size);
		p+=size;
		}

	// iat
	TInt nimports=gX86imp?gX86num_imports:aPeFile.NumberOfImports();
//	TInt nimports=aPeFile.NumberOfImports();
	if (nimports)
		{
		if (gX86imp)
			{
			TUint *rdata=(TUint*)aPeFile.iSectionData[KConstSection];
			int i;
			int* s=(int*)gX86imp;
			s++;
			for (i=0; i<gX86num_imp_dlls; ++i)
				{
				++s;
				int n=*s++;
				while (n--)
					{
					*(int*)p=rdata[(*s)>>2]&0x7fffffffu;	// rdata offset to ordinal
					++s;
					p+=4;
					}
				}
			*(int*)p=0;
			p+=4;
			}
		else
			{
			TInt r=CopyImportAddrTable(p, aPeFile);
			p+=ALIGN4(nimports*4+4);
			if (r!=KErrNone)
				return Print(EError, "%s is importing symbols by name.\n", iFileName);
			}
		}
	// export dir
	CreateExportDirectory(p, aPeFile);
	p+=iHdr->iExportDirCount*4;
	return iHdr->iCodeSize;
	}

TInt E32ImageFile_PE::CopyData(char *p, PEFile &aPeFile)
	{
	
	if (iHdr->iDataSize)
		memcpy(p, aPeFile.iSectionData[KDataSection], iHdr->iDataSize);
	return iHdr->iDataSize;
	}

TInt E32ImageFile_PE::Translate(const char* aFileName, TUint aDataBase, TBool aAllowDllData, TBool /*aSymLkupEnabled*/)
//
// Translate a PE format file to a E32Image file
//
	{
	iSource = EPeFile;
	PEFile pefile;
	if (!pefile.Init((const TText * const)aFileName))
		return KErrGeneral;
	TInt r=pefile.ReadSectionHeaders();
	if (r!=KErrNone) return r;
	r=pefile.ReadData();
	if (r!=KErrNone) return r;
	pefile.Close();
	r=pefile.Normalise();
	if (r!=KErrNone) return r;
	iFileName = strdup(aFileName);

	Adjust(ALIGN4(sizeof(E32ImageHeaderV)));	// fixed for now because holes not supported
	SetDefaultHeader();
	if (gX86imp)
		iHdr->iDllRefTableCount=gX86num_imp_dlls;
	else
		iHdr->iDllRefTableCount=pefile.NumberOfImportDlls();
	iHdr->iExportDirCount=pefile.NumberOfExports();
	iHdr->iCodeBase=pefile.iLinkedBase;
	TInt nimports=gX86imp?gX86num_imports:pefile.NumberOfImports();

	TInt importSectionSize;
	char *newImportSection=CreateImportSection(pefile, importSectionSize);

	TInt size = ALIGN4(sizeof(E32ImageHeaderV));	// fixed for now because holes not supported
	iHdr->iCodeOffset = size;
	TInt pos = size;
	size+=DoCodeHeader(pefile);
	TInt t=DoDataHeader(pefile, aDataBase);
	if (t>0)
		{
		iHdr->iDataOffset = size;
		size += t;
		}
	if (importSectionSize!=0)
		{
		iHdr->iImportOffset = size;
		size += importSectionSize;
		}

	char *newCodeRelocs=NULL;
	char *newDataRelocs=NULL;
	TInt codeRelocSize=0, dataRelocSize=0;
	TInt nrelocs=pefile.NumberOfRelocs();
	if (nrelocs)
		{
		TUint *relocs=new TUint [nrelocs];
		TUint *relocsection=new TUint [nrelocs];
		pefile.GetRelocs(relocs, relocsection, nrelocs);
		FixRelocs(pefile, relocs, relocsection, nrelocs);
		newCodeRelocs=CreateCodeRelocs(relocs, relocsection, nrelocs, codeRelocSize);
		newDataRelocs=CreateDataRelocs(relocs, relocsection, nrelocs, dataRelocSize);
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
		delete [] relocs;
		delete [] relocsection;
		}

	Adjust(size);
	t=CopyCode(iData + pos, pefile);
	if (t<0)
		return KErrGeneral;
	pos += t;
	pos += CopyData(iData + pos, pefile);
	if (nimports)
		{
		memcpy(iData + pos, newImportSection, importSectionSize);
		pos += importSectionSize;
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
	// entry point must be in the text section
	TInt entryPointSectionIndex=pefile.FindSectionByVa(pefile.iEntryPoint+pefile.iLinkedBase);
	TUint entryPointOffset=pefile.iEntryPoint-pefile.iSectionHeader[entryPointSectionIndex]->VirtualAddress;
	if (entryPointSectionIndex!=KTextSection)
		return Print(EError, "Entry Point not in code section\n");

	// Arrange a header for this E32 Image
	switch (pefile.iCpu)
		{
	case IMAGE_FILE_MACHINE_I386:
		iHdr->iCpuIdentifier = (TUint16)ECpuX86;
		break;
	case 0x0a00:
		iHdr->iCpuIdentifier = (TUint16)ECpuArmV4;
		break;
	case 0x0b00:
		iHdr->iCpuIdentifier = (TUint16)ECpuMCore;
		break;
	default:
		iHdr->iCpuIdentifier = (TUint16)ECpuUnknown;
		break;
		}

	// Import format is PE-derived without redundant ordinal lists
	// ABI is GCC98r2 ABI (on ARM)
	iHdr->iFlags |= KImageImpFmt_PE2;

	if (pefile.iImageIsDll)
		{
		iHdr->iFlags|=KImageDll;
		if (iHdr->iDataSize && !aAllowDllData)
			return Print(EError, "Dll '%s' has initialised data.\n", iFileName);
		if (iHdr->iBssSize  && !aAllowDllData)
			return Print(EError, "Dll '%s' has uninitialised data.\n", iFileName);
		}
	iHdr->iHeapSizeMin=pefile.iHeapCommittedSize;
	iHdr->iHeapSizeMax=pefile.iHeapReservedSize;
	iHdr->iStackSize=pefile.iStackCommittedSize;
	iHdr->iEntryPoint=entryPointOffset;
	r = DetermineEntryPointType();
	if (r == KErrCorrupt)
		return Print(EError, "File '%s': Bad Entry Point.\n", iFileName);
	else if (r == KErrNotSupported)
		return Print(EError, "File '%s': Bad Entry Point Type.\n", iFileName);

	delete [] newImportSection;
	delete [] newCodeRelocs;
	delete [] newDataRelocs;

	return KErrNone;
	}

TBool E32ImageFile_PE::Translate(PEFile &aPeFile)
//
//
//
	{

	return Translate((const char*)aPeFile.iFileName, NULL, EFalse, EFalse);
	}

