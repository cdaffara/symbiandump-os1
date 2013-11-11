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
#include "h_utl.h"
#include "pe_file.h"

TInt E32ImageFile_PE::CopyImportAddrTable(char *aPtr, PEFile &aPeFile)
//
// Copy the import address table entries
//
	{

	TUint *ptr=(TUint *)aPtr;
	char *importsection=aPeFile.iSectionData[KImportSection];
	TUint *src=(TUint *)importsection;
	while (*src)
		{
		TUint vaoffset=src[4];
		if (!gLittleEndian) ByteSwap(vaoffset);
		TUint offset=vaoffset-aPeFile.iSectionHeader[KImportSection]->VirtualAddress; // find the offset into the section of import addr table
		vaoffset=src[3];
		if (!gLittleEndian) ByteSwap(vaoffset);
		TUint exportername=vaoffset-aPeFile.iSectionHeader[KImportSection]->VirtualAddress;
		TUint *p=(TUint *)(importsection+offset);
		while (*p)
			{
			if ((*p&0x80000000)==0)
				{
				Print(EError, "%s exporting symbol by name\n", importsection+exportername);
				return KErrGeneral;
				}
			*ptr++=(*p++)&0x7fffffff; // mask out the high bit (- indicates export by ordinal)
			}
		src+=5;
		}
	*ptr++=0;
	return KErrNone;
	}

extern char* gX86imp;
extern int gX86num_imports;
extern int gX86num_imp_dlls;
extern int gX86imp_size;
int* gX86imp_relocs=NULL;

TInt CrunchImportSection(TAny* aSection, TInt aNumImpDlls, TInt aNumImports)
	{
	// Remove lists of ordinals from import section
	TInt* d = (TInt*)aSection;
	TInt orig_size = *d;
	TInt offset_correction = aNumImports*sizeof(TUint);
	*d -= offset_correction;	// reduce total section size
	TInt *dd = d+1;
	TInt *ss = d+1;

	TInt i;
	for (i=0; i<aNumImpDlls; ++i)
		{
		*dd++ = *ss++ - offset_correction;	// copy name offset and reduce it appropriately
		TInt nimp = *ss++;					// number of imports from this DLL
		*dd++ = nimp;						// copy it
		ss += nimp;							// skip the ordinals in the original list
		}
	TInt used_size = (ss - d) * sizeof(TInt);
	memcpy(dd, ss, orig_size - used_size);	// copy name strings

	return *d;	// return new total section size
	}

char *E32ImageFile_PE::CreateImportSection(const PEFile &aPeFile, TInt &aSize)
//
// Create a new format import section
//
	{

	int total_imports = 0;
	if (gX86imp)
		{
		TUint *rdata=(TUint*)aPeFile.iSectionData[KConstSection];
		TInt bytecount=gX86imp_size;
		char* section=new char[bytecount];
		memcpy(section,gX86imp,bytecount);
		int i;
		int j=0;
		int* s=(int*)section;
		s++;
		gX86imp_relocs=new int[gX86num_imports+gX86num_imp_dlls];
		for (i=0; i<gX86num_imp_dlls; ++i)
			{
			++s;
			int n=*s++;
			total_imports += n;
			while (n--)
				{
				int rdata_int_offset=*s>>2;
				*s=rdata[rdata_int_offset]&0x7fffffffu;	// rdata offset to ordinal
				gX86imp_relocs[rdata_int_offset]=j<<2;
				++j;
				++s;
				}
			}
		*(TInt*)section = bytecount;
		aSize = CrunchImportSection(section, gX86num_imp_dlls, total_imports);
		return section;
		}
	PIMAGE_SECTION_HEADER aHeader=aPeFile.iSectionHeader[KImportSection];
	TUint *aSrc=(TUint *)aPeFile.iSectionData[KImportSection];

	TInt nimportdlls=aPeFile.NumberOfImportDlls();
	if (nimportdlls==0)
		{
		aSize=0;
		return NULL;
		}
	E32ImportBlock *block=new E32ImportBlock [nimportdlls];
	char **name=new char* [nimportdlls];
	TUint **import=new TUint* [nimportdlls];

	TInt bytecount=sizeof(E32ImportSection)+sizeof(E32ImportBlock)*nimportdlls;
	TUint *src=aSrc;
	TInt i;
	for (i=0; i<nimportdlls; i++)
		{
		TUint vaoffset=src[4];
		if (!gLittleEndian) ByteSwap(vaoffset);
		TUint offset=vaoffset-aHeader->VirtualAddress; // find the offset into the section of import addr table
		TUint *p=aSrc+offset/4;
		block[i].iNumberOfImports=0;
		while (*p++)
			block[i].iNumberOfImports++;
		total_imports += block[i].iNumberOfImports;
		import[i]=new TUint [block[i].iNumberOfImports];
		TInt j;
		p=aSrc+offset/4;
		for (j=0; j<block[i].iNumberOfImports; j++)
			{
			import[i][j]=(*p++)&0x7fffffffu;
			bytecount+=4;
			}
		// name
		vaoffset=src[3];
		if (!gLittleEndian) ByteSwap(vaoffset);
		offset=vaoffset-aHeader->VirtualAddress;
		name[i]=((char *)aSrc)+offset;
		bytecount+=strlen(name[i])+1;
		src+=5;
		}

	bytecount=ALIGN4(bytecount);
	char *section=new char [bytecount];
	char *s=section+sizeof(E32ImportSection);
	for (i=0; i<nimportdlls; i++)
		{
		memcpy(s, (char *)&block[i], sizeof(E32ImportBlock));
		s+=sizeof(E32ImportBlock);
		memcpy(s, (char *)import[i], block[i].iNumberOfImports*4);
		s+=block[i].iNumberOfImports*4;
		}
	char *t=section+sizeof(E32ImportSection);
	for (i=0; i<nimportdlls; i++)
		{
		((E32ImportBlock *)t)->iOffsetOfDllName=s-section;
		strcpy(s, name[i]);
		s+=strlen(name[i])+1;
		t += ((E32ImportBlock *)t)->iNumberOfImports * sizeof(TUint) + sizeof(E32ImportBlock);
		}
	while ((s-section)<bytecount)
		*s++=0;

	// free mem
	for (i=0; i<nimportdlls; i++)
		delete import[i];
	delete block;
	delete import;
	delete name;

	*(TInt*)section = bytecount;
	aSize = CrunchImportSection(section, nimportdlls, total_imports);
	return section;
	}

TUint E32ImageFile_PE::FixImportThunk(PEFile &aPeFile, TUint va)
//
// Fix an access to the import address table
//
	{

	TUint *imports=(TUint *)aPeFile.iSectionData[KImportSection];
	TUint n=0;
	TUint importoffset=imports[4];
	if (!gLittleEndian) ByteSwap(importoffset);
	TUint iat=importoffset-aPeFile.iSectionHeader[KImportSection]->VirtualAddress;

	while (iat<va)
		{
		if (*((TUint *)(aPeFile.iSectionData[KImportSection]+iat))==0)
			{
			imports+=5;
			importoffset=imports[4];
			if (!gLittleEndian) ByteSwap(importoffset);
			iat=importoffset-aPeFile.iSectionHeader[KImportSection]->VirtualAddress;
			}
		else
			{
			n++;
			iat+=4;
			}
		}

	// Flag errors brought about by a corrupt input binary
	if (iat>va)
		{
		Print(EError, "%s is corrupt - problem processing import address table.\n", this->iFileName);
		return (TUint)KErrGeneral;
		}
		
	return iHdr->iTextSize+n*sizeof(TUint);
	}

