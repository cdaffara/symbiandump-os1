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
#include "elftran.h"
#include "elffile.h"
#include "h_utl.h"
#include <assert.h>

TInt sizeOfRelocs(Elf32_Rel **relocs, TInt nrelocs)
	{

	TInt bytecount=0;
	TInt page=-1;
	TInt i;
	for (i=0; i<nrelocs; i++)
		{
		TInt p=relocs[i]->r_offset&0xfffff000;
		if (page!=p)
			{
			if (bytecount%4!=0)
				bytecount+=2;
			bytecount+=8; // page, block size
			page=p;
			}
		bytecount+=2;
		}
	if (bytecount%4!=0)
		bytecount+=2;
	return bytecount;
	}

char* E32ImageFile_ELF::CreateRelocs(ELFFile& aElfFile, Elf32_Rel **relocs, TInt nrelocs, TInt &aSize, TUint aBase)
	{

	TInt bytecount=sizeOfRelocs(relocs, nrelocs);
	aSize=0;
	if (bytecount==0)
		return NULL;
	aSize=bytecount+sizeof(E32RelocSection);

	char *section=new char [bytecount+sizeof(E32RelocSection)];
	char *data=section+sizeof(E32RelocSection);
	char *startofblock=data;

	TInt page=-1;
	TInt pagesize=8;
	TInt i;
	for (i=0; i<nrelocs; i++)
		{
		TInt p=relocs[i]->r_offset&0xfffff000;
		if (page!=p)
			{
			if (pagesize%4!=0)
				{
				*(TUint16 *)data=0;
				data+=2;
				pagesize+=2;
				}
			if (page == -1) page = p;
			*(TUint *)startofblock=page - aBase;
			*(TUint *)(startofblock+4)=pagesize;
			pagesize=8;
			page=p;
			startofblock=data;
			data+=8;
			}
		TUint16 relocType = aElfFile.GetRelocType(relocs[i]);
		*(TUint16 *)data=(TUint16)((relocs[i]->r_offset&0xfff)|relocType);
		data+=2;
		pagesize+=2;
		}
	if (pagesize%4!=0)
		{
		*(TUint16 *)data=0;
		data+=2;
		pagesize+=2;
		}
	*(TUint *)startofblock=page - aBase;
	*(TUint *)(startofblock+4)=pagesize;
	((E32RelocSection *)section)->iNumberOfRelocs=nrelocs;
	((E32RelocSection *)section)->iSize=bytecount;
	return section;
	}

void checkreloc(ELFFile& aElfFile, TUint va, TUint reloc)
	{
	// This will warn if appears to be outside any known segment.
   	// NB. Such relocations can occur as the result of 'optimizations'
   	// and aren't 'bugs'.
   	// It turns out that the released version of RVCT makes such widespread
   	// use of the optimization that this warning is disconcerting and
   	// unhelpful.
 	(void) aElfFile;
 	(void)va;
 	(void)reloc;
#if 0
	if (!aElfFile.GetSegmentFromAddr(va)) 
		Print(EWarning, "dubious relocation:  [%08x] = %08x\n", reloc, va);
#endif
	}

void E32ImageFile_ELF::FixRelocs(ELFFile &aElfFile, Elf32_Rel **codeRelocs, Elf32_Rel **dataRelocs)
	{
	TUint *data;
	TInt numberOfCodeRelocs = aElfFile.NumberOfCodeRelocs();
	TInt numberOfDataRelocs = aElfFile.NumberOfDataRelocs();
	for (TInt i=0; i<numberOfCodeRelocs; i++)
	        {
		data=aElfFile.CodePtrFromAddr(codeRelocs[i]->r_offset);
		checkreloc(aElfFile, *data, codeRelocs[i]->r_offset);
		//*data=FixAddress(aElfFile, *data, codeRelocs[i]);
		}
// fix defect
	for (TInt j=0; j<numberOfDataRelocs; j++)
	        {
		data=aElfFile.DataPtrFromAddr(dataRelocs[j]->r_offset);
		checkreloc(aElfFile, *data, dataRelocs[j]->r_offset);
		//*data=FixAddress(aElfFile, *data, dataRelocs[j]);
		}
	}

TUint E32ImageFile_ELF::FixAddress(ELFFile &aElfFile, TUint va, Elf32_Rel * rel)
//
// Fix the given virtual address for the new headers
//
	{
	Elf32_Phdr * segment = aElfFile.GetSegmentFromAddr(rel->r_offset);
	assert(aElfFile.CodeSegmentP(segment) || aElfFile.DataSegmentP(segment));
	TUint offset = va - segment->p_vaddr;
	if (aElfFile.CodeSegmentP(segment))
	      return (TUint)aElfFile.GetCode() + offset;
	else 
	      return (TUint)aElfFile.GetData() + offset;
	
	}

