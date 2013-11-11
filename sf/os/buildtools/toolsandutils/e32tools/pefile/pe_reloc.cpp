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

#include <stdlib.h>
#include <string.h>
#include <e32std.h>
#include <e32std_private.h>
#include "pe_file.h"
#include "h_utl.h"
#include <stdio.h>

extern char* gX86imp;
extern int gX86num_imp_dlls;
extern int gX86imp_size;
extern int gX86num_imports;
extern int* gX86imp_relocs;

TInt sizeOfCodeRelocs(TUint *relocs, TUint *relocsection, TInt nrelocs)
	{

	TInt bytecount=0;
	TInt page=-1;
	TInt i;
	for (i=0; i<nrelocs; i++)
		{
		if ((TInt)relocsection[i]==KTextSection || (TInt)relocsection[i]==KConstSection || (TInt)relocsection[i]==KCrtSection)
			{
			TInt p=relocs[i]&0xfffff000;
			if (page!=p)
				{
				if (bytecount%4!=0)
					bytecount+=2;
				bytecount+=8; // page, block size
				page=p;
				}
			bytecount+=2;
			}
		}
	if (bytecount%4!=0)
		bytecount+=2;
	return bytecount;
	}

TInt sizeOfDataRelocs(TUint *relocs, TUint *relocsection, TInt nrelocs)
	{

	TInt bytecount=0;
	TInt page=-1;
	TInt i;
	for (i=0; i<nrelocs; i++)
		{
		if ((TInt)relocsection[i]==KDataSection)
			{
			TInt p=relocs[i]&0xfffff000;
			if (page!=p)
				{
				if (bytecount%4!=0)
					bytecount+=2;
				bytecount+=8; // page, block size
				page=p;
				}
			bytecount+=2;
			}
		}
	if (bytecount%4!=0)
		bytecount+=2;
	return bytecount;
	}

void reorderRelocs(TUint aReloc[], TUint aRelocSection[], TInt aNumberOfRelocs)
//
// sort the relocations in section order
//
	{
	TUint *temp=new TUint [aNumberOfRelocs];
	TUint *tempsection=new TUint [aNumberOfRelocs];
	TInt idx=0;
	TUint section=0;
	while (idx<aNumberOfRelocs)
		{
		for (TInt i=0; i<aNumberOfRelocs; i++)
			{
			if (aRelocSection[i]==section)
				{
				temp[idx]=aReloc[i];
				tempsection[idx]=aRelocSection[i];
				idx++;
				}
			}
		section++;
		}
	memcpy((char *)aReloc, (char *)temp, aNumberOfRelocs*sizeof(TUint));
	memcpy((char *)aRelocSection, (char *)tempsection, aNumberOfRelocs*sizeof(TUint));
	delete [] temp;
	delete [] tempsection;
	}

char *E32ImageFile_PE::CreateCodeRelocs(TUint *relocs, TUint *relocsection, TInt nrelocs, TInt &aSize)
	{

	TInt bytecount=sizeOfCodeRelocs(relocs, relocsection, nrelocs);
	aSize=0;
	if (bytecount==0)
		return NULL;
	aSize=bytecount+sizeof(E32RelocSection);

	char *section=new char [bytecount+sizeof(E32RelocSection)];
	char *data=section+sizeof(E32RelocSection);
	char *startofblock=data;

	TInt ncoderelocs=0;
	TInt page=-1;
	TInt pagesize=8;
	TInt i;
	for (i=0; i<nrelocs; i++)
		{
		if ((TInt)relocsection[i]==KTextSection || (TInt)relocsection[i]==KConstSection || (TInt)relocsection[i]==KCrtSection)
			{
			TInt p=relocs[i]&0xfffff000;
			if (page!=p)
				{
				if (pagesize%4!=0)
					{
					*(TUint16 *)data=0;
					data+=2;
					pagesize+=2;
					}
				*(TUint *)startofblock=page;
				*(TUint *)(startofblock+4)=pagesize;
				pagesize=8;
				page=p;
				startofblock=data;
				data+=8;
				}
			*(TUint16 *)data=(TUint16)((relocs[i]&0xfff)|0x3000);
			data+=2;
			pagesize+=2;
			ncoderelocs++;
			}
		}
	if (pagesize%4!=0)
		{
		*(TUint16 *)data=0;
		data+=2;
		pagesize+=2;
		}
	*(TUint *)startofblock=page;
	*(TUint *)(startofblock+4)=pagesize;
	((E32RelocSection *)section)->iNumberOfRelocs=ncoderelocs;
	((E32RelocSection *)section)->iSize=bytecount;
	return section;
	}

char *E32ImageFile_PE::CreateDataRelocs(TUint *relocs, TUint *relocsection, TInt nrelocs, TInt &aSize)
	{

	TInt bytecount=sizeOfDataRelocs(relocs, relocsection, nrelocs);
	aSize=0;
	if (bytecount==0)
		return NULL;
	aSize=bytecount+sizeof(E32RelocSection);

	char *section=new char [bytecount+sizeof(E32RelocSection)];
	char *data=section+sizeof(E32RelocSection);
	char *startofblock=data;

	TInt ndatarelocs=0;
	TInt page=-1;
	TInt pagesize=8;
	TInt i;
	for (i=0; i<nrelocs; i++)
		{
		if ((TInt)relocsection[i]==KDataSection)
			{
			TInt p=relocs[i]&0xfffff000;
			if (page!=p)
				{
				if (pagesize%4!=0)
					{
					*(TUint16 *)data=0;
					data+=2;
					pagesize+=2;
					}
				*(TUint *)startofblock=page;
				*(TUint *)(startofblock+4)=pagesize;
				pagesize=8;
				page=p;
				startofblock=data;
				data+=8;
				}
			*(TUint16 *)data=(TUint16)((relocs[i]&0xfff)|0x3000);
			data+=2;
			pagesize+=2;
			ndatarelocs++;
			}
		}
	if (pagesize%4!=0)
		{
		*(TUint16 *)data=0;
		data+=2;
		pagesize+=2;
		}
	*(TUint *)startofblock=page;
	*(TUint *)(startofblock+4)=pagesize;

	((E32RelocSection *)section)->iNumberOfRelocs=ndatarelocs;
	((E32RelocSection *)section)->iSize=bytecount;
	return section;
	}

void checkreloc(PEFile &aPeFile, TUint va, TUint reloc)
	{

	// Allow the section find routine to use heuristics to resolve addresses
	// which have been offset by the compiler
	TInt s = aPeFile.FindSectionByVa(va, 1);
	switch(s)
		{
		case KTextSection:
		case KConstSection:
		case KDataSection:
		case KCrtSection:
		case KBssSection:
		case KImportSection:
			return;
		default:
			break;
		}
	Print(EAlways, "bad relocation:  [%08x] = %08x\n", reloc, va);
	}

void E32ImageFile_PE::FixRelocs(PEFile &aPeFile, TUint *relocation, TUint *relocsection, TInt aNumberOfRelocs)
	{

	TUint *data;
	TInt i;
#if 0
	Print(EAlways, "Linked base %08x\n", aPeFile.iLinkedBase);
	for (i=0; i<KNumberOfSections; i++)
		{
		if (!aPeFile.iSectionHeader[i])
			continue;
		TUint start = aPeFile.iSectionHeader[i]->VirtualAddress;
		TUint finish = start + aPeFile.iSectionHeader[i]->Misc.VirtualSize;
		Print(EAlways, "Section %d %08x-%08x\n", i, start, finish);
		}
#endif
	for (i=0; i<aNumberOfRelocs; i++)
		{
		switch (relocsection[i])
			{
		case KTextSection:
			relocation[i]-=aPeFile.iSectionHeader[KTextSection]->VirtualAddress;
			data=(TUint *)(aPeFile.iSectionData[KTextSection]+relocation[i]);
			if (!gLittleEndian) ByteSwap(*data);
			checkreloc(aPeFile, *data, relocation[i]+aPeFile.iSectionHeader[KTextSection]->VirtualAddress);
			*data=FixAddress(aPeFile, *data);
			if (!gLittleEndian) ByteSwap(*data);
			break;
		case KConstSection:
			relocation[i]-=aPeFile.iSectionHeader[KConstSection]->VirtualAddress;
			data=(TUint *)(aPeFile.iSectionData[KConstSection]+relocation[i]);
			if (!gLittleEndian) ByteSwap(*data);
			checkreloc(aPeFile, *data, relocation[i]+aPeFile.iSectionHeader[KConstSection]->VirtualAddress);
			relocation[i]+=ConstOffset();
			*data=FixAddress(aPeFile, *data);
			if (!gLittleEndian) ByteSwap(*data);
			break;
		case KCrtSection:
			relocation[i]-=aPeFile.iSectionHeader[KCrtSection]->VirtualAddress;
			data=(TUint *)(aPeFile.iSectionData[KCrtSection]+relocation[i]);
			if (!gLittleEndian) ByteSwap(*data);
			checkreloc(aPeFile, *data, relocation[i]+aPeFile.iSectionHeader[KCrtSection]->VirtualAddress);
			relocation[i]+=iCrtOffset;
			*data=FixAddress(aPeFile, *data);
			if (!gLittleEndian) ByteSwap(*data);
			break;
		case KDataSection:
			relocation[i]-=aPeFile.iSectionHeader[KDataSection]->VirtualAddress;
			data=(TUint *)(aPeFile.iSectionData[KDataSection]+relocation[i]);
			if (!gLittleEndian) ByteSwap(*data);
			checkreloc(aPeFile, *data, relocation[i]+aPeFile.iSectionHeader[KDataSection]->VirtualAddress);
			*data=FixAddress(aPeFile, *data);
			if (!gLittleEndian) ByteSwap(*data);
			break;
		default:
			Print(EWarning, "Relocation in invalid section.\n");
			break;
			}
		}
	reorderRelocs(relocation, relocsection, aNumberOfRelocs);
	}

TUint E32ImageFile_PE::FixAddress(PEFile &aPeFile, TUint va)
//
// Fix the given virtual address for the new headers
//
	{

	// Allow the section find routine to use heuristics to resolve addresses
	// which have been offset by the compiler
	TInt section=aPeFile.FindSectionByVa(va, 1);
	switch(section)
		{
		case KTextSection:
			va-=aPeFile.iLinkedBase;
			va-=aPeFile.iSectionHeader[KTextSection]->VirtualAddress;
			va+=iHdr->iCodeBase;
			break;
		case KConstSection:
			va-=aPeFile.iLinkedBase;
			va-=aPeFile.iSectionHeader[KConstSection]->VirtualAddress;
			if (gX86imp)
				{
				TUint old_iat_size=(gX86num_imports+gX86num_imp_dlls)<<2;
				if (va<old_iat_size)
					{
					TInt offset=iHdr->iTextSize;
//					fprintf(stderr,"IAT OFF %x ",va);
					va=gX86imp_relocs[va>>2]+iHdr->iCodeBase+offset;
//					fprintf(stderr,"-> %x\n",va);
					break;
					}
				}
			va+=iHdr->iCodeBase+ConstOffset();
//			fprintf(stderr,"const reloc -> %x\n",va);
			break;
		case KDataSection:
			va-=aPeFile.iLinkedBase;
			va-=aPeFile.iSectionHeader[KDataSection]->VirtualAddress;
			va+=iHdr->iDataBase; //DataOffset();
			break;
		case KCrtSection:
			va-=aPeFile.iLinkedBase;
			va-=aPeFile.iSectionHeader[KCrtSection]->VirtualAddress;
			va+=iHdr->iCodeBase+iCrtOffset;
			break;
		case KBssSection:
			va-=aPeFile.iLinkedBase;
			va-=aPeFile.iSectionHeader[KBssSection]->VirtualAddress;
			va+=iHdr->iDataBase+iHdr->iDataSize;
			break;
		case KImportSection:
			va-=aPeFile.iLinkedBase;
			va=FixImportThunk(aPeFile, va-aPeFile.iSectionHeader[KImportSection]->VirtualAddress);
			va+=iHdr->iCodeBase;
			break;
		default:
			if (va < 0x10000u)
				{
				// assume it's a relocation relative to an omitted section
				break;
				}
			Print(EWarning, "Address to relocate cannot be resolved to .text, .rdata, .idata or data sections\n");
			Print(EWarning, "Problem address = %08x (section %d)\n", va, section);
			break;
		}
	// va is now an offset from the start of the text
	return va;
	}


