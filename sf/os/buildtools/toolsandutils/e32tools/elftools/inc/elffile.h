// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__ELFFILE_H__)
#define __ELFFILE_H__
#include <e32rom.h>
#include "e32ldfmt.h"
#include <tools/elfdefs.h>

#define ELFADDR(rtype, p, o) (rtype *)(((char *)p) + o)

//
enum TImportStat {EImpError, EImpSuccess, EImpDone};
//
class ELFFile
	{
public:
	ELFFile();
	~ELFFile();
	TBool Init(const TText * const aFileName);
	void Close(void);

	char *CreateImportSection(TInt &aSize);

	TBool GetRelocs(Elf32_Rel **aCodeRelocs, Elf32_Rel **aDataRelocs);
	TUint16 GetRelocType(Elf32_Rel* aReloc);

	TInt NumberOfImports() const;
	TInt NumberOfImportDlls() const;
	TInt NumberOfExports() const;
	TInt NumberOfRelocs();

	

	TInt NumberOfCodeRelocs();
	TInt NumberOfDataRelocs();
	
public:
	TUint iCodeSize;
	TUint iDataSize;
	TUint iBssSize;


	TUint GetCodeSize();
	TBool HasInitialisedData();
	TUint GetDataSize();
	TBool HasBssData();
	TUint GetBssSize();
	

	Elf32_Phdr * GetSegment(TInt idx) {return &iPhdr[idx];}
	Elf32_Phdr * GetSegmentFromAddr(Elf32_Addr addr);

	char * GetCode() { return ELFADDR(char, iElfFile, iCodeSegmentHdr->p_offset); }
	char * GetData() { return ELFADDR(char, iElfFile, iDataSegmentHdr->p_offset); }
	TBool CodeSegmentP(Elf32_Phdr * s) { return (TBool)(s == iCodeSegmentHdr); }
	TBool CodeSegmentP(TInt idx) { return (TBool)(idx == iCodeSegmentIdx); }
	TInt CodeSegmentIndex() {return iCodeSegmentIdx;}
	TBool DataSegmentP(Elf32_Phdr * s) { return (TBool)(s == iDataSegmentHdr); }
	TBool DataSegmentP(TInt idx) { return (TBool)(idx == iDataSegmentIdx); }
	TInt DataSegmentIndex() {return iDataSegmentIdx;}

	TUint * CodePtrFromAddr(Elf32_Addr addr) { return PtrInSegment(iCodeSegmentHdr, addr); }
	TUint * DataPtrFromAddr(Elf32_Addr addr) { return PtrInSegment(iDataSegmentHdr, addr); }


	char * ELFFileBase() { return (char *) iElfFile; }

	TUint GetExportTableOffset(void);
	TUint GetEntryPointOffset(void) { return iElfFile->e_entry - iCodeSegmentHdr->p_vaddr; }

	TBool SymbolPresent(TText *s);
	Elf32_Sym * FindSymbol(const TText *);

	TBool GetExceptionIndexInfo(TUint32 &aOffset);
	TBool SetUpLookupTable();
	void SetLookupTblBase(TInt);
	TInt GetLookupTblSize();
	void GetExportSymInfoHeader(E32EpocExpSymInfoHdr& aSymInfoHdr);
	TUint GetSymLookupSection(char* aBuff);
private:
	TBool InitHeaders(void);
	TBool InitDllData(void);

	TUint * PtrInSegment(Elf32_Phdr * phdr, Elf32_Addr addr) 
	        { 
		return ELFADDR(TUint, ELFADDR(TUint, iElfFile, phdr->p_offset), (addr - phdr->p_vaddr));
		}


	TBool IsValidFileHeader(Elf32_Ehdr * aElfHdr);
	void CopySectionData(TAny *source, TAny *dest, TUint32 fileLength, TUint32 memLength);
	TBool ProcessRelocData(TAny *relocData,TInt dataSize);
	ELFFile(const ELFFile&);
	const ELFFile & operator = (const ELFFile&);
public:
	TBool	iImageIsDll;
	TBool	ImageIsDll() { return iImageIsDll; }
	Elf32_Addr iLinkedBase;

	TUint iEntryPoint;
	TUint iHeapCommittedSize;
	TUint iHeapReservedSize;
	TUint iStackCommittedSize;
	TUint iStackReservedSize;
private:
	friend class E32ImageFile;
	friend class E32ImageFile_ELF;

	TText * iFileName;
	TInt32 iFileHandle;
	Elf32_Ehdr * iElfFile;
	Elf32_Phdr * iPhdr;
	Elf32_Phdr * iDynamicSegmentHdr;
	TInt iDynamicSegmentIdx;
	Elf32_Phdr * iCodeSegmentHdr;
	TInt iCodeSegmentIdx;
	Elf32_Phdr * iDataSegmentHdr;
	TInt iDataSegmentIdx;

	Elf32_Shdr * iSectionHeaderTable;
	TInt iSymIdx;
	Elf32_Sym * iSymTab;

	class ELFDllData;
	ELFDllData * iDllData;

	TCpu iCpu;


	};
#endif













