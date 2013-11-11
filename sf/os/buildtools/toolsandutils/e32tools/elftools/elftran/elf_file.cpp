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
//


#include <e32def.h>
#include <e32def_private.h>
#include <e32std.h>
#include <e32std_private.h>
#include "elftran.h"
#include <tools/elfdefs.h>
#include "elffile.h"
#include "elfdll.h"
#include "h_utl.h"
#include <string.h>
#include <stdlib.h>

TBool hadText, hadReloc = EFalse;

ELFFile::ELFFile() 
	: iFileName(0), iFileHandle(-1) , iElfFile(0),
	iHeapCommittedSize(0x1000), iHeapReservedSize(0x100000), iStackCommittedSize(0),
	iCodeSegmentHdr(0), iDataSegmentHdr(0), iDynamicSegmentHdr(0),
	iCodeSegmentIdx(0), iDataSegmentIdx(0),
	iDynamicSegmentIdx(0), iDllData(0), iCpu(ECpuUnknown)
	{}




ELFFile::~ELFFile()
//
// Destructor
//
	{

	delete [] iFileName;
	delete iElfFile;
	delete iDllData;
	}


TBool ELFFile::Init(const TText * const aFileName)
//
// Read the ELF file into memory
//
 	{

 	delete [] iFileName;	
	iFileName = new TText[strlen((const char *)aFileName)+1];
	strcpy ((char *)iFileName, (const char *)aFileName);

	TInt error = HFile::Open(iFileName, &iFileHandle);
	if (error!=0)
		return EFalse;

	TInt flength = HFile::GetLength(iFileHandle);

	iElfFile = (Elf32_Ehdr *)HMem::Alloc(0,flength);
	if (!iElfFile)
		{
		Print(EPeError,"Failed to allocate memory to read in file.\n");
		Close();
		return EFalse;
		}

	if (!HFile::Read(iFileHandle,iElfFile,flength))
		{
		Print(EPeError,"Unable to read file %s.\n",iFileName);
		Close();
		return EFalse;
		}

        Close();

	if (!IsValidFileHeader(iElfFile))
		{
		Print(EPeError,"Invalid file header.\n");
		return EFalse;
		}
	// we only support this....for the moment
	iCpu = ECpuArmV4;

	if (!InitHeaders()) return EFalse;
	
	if (!InitDllData()) return EFalse;

	iEntryPoint = iElfFile->e_entry;

	iCodeSize = GetCodeSize();
	iDataSize = GetDataSize();
	iBssSize = GetBssSize();

	iStackReservedSize = 0x2000;
	iStackCommittedSize = 0x2000;

 	iLinkedBase = iCodeSegmentHdr->p_vaddr;
	
	iImageIsDll = iDllData->ImageIsDll();

	return ETrue;
	}

char * ELFFile::CreateImportSection(TInt &aSize)
//
// get ELFDLLData to do it
//
	{
	TInt size;
	char * newSection = iDllData->CreateImportSection(size);
	aSize = size;
	return newSection;
	}

TUint ELFFile::GetExportTableOffset(void)
        {
	return iDllData->GetExportTableOffset();
	}

TBool ELFFile::InitHeaders(void)
        {
	TInt nphdrs = iElfFile->e_phnum;
	if (nphdrs)
	       {
	       // Find the dynamic segment
	       Elf32_Phdr * aPhdr = ELFADDR(Elf32_Phdr, iElfFile, iElfFile->e_phoff);
	       iPhdr = aPhdr;
	       for (TInt idx = 0; idx < nphdrs; idx++)
					{
					Elf32_Word ptype = aPhdr[idx].p_type;
					if (ptype == PT_DYNAMIC)
							{
							iDynamicSegmentHdr = &aPhdr[idx];
							iDynamicSegmentIdx = idx;
							}
      				else if (ptype == PT_LOAD && 
							 (aPhdr[idx].p_flags & (PF_X + PF_ARM_ENTRY)))
							{
							iCodeSegmentHdr = &aPhdr[idx];
							iCodeSegmentIdx = idx;
							}
      				else if (ptype == PT_LOAD && 
							 (aPhdr[idx].p_flags & (PF_W + PF_R)))
							{
							iDataSegmentHdr = &aPhdr[idx];
							iDataSegmentIdx = idx;
							}
					}
	       }
	
	// cache pointer to symbol table

	// Get section header table
	Elf32_Shdr * s = ELFADDR(Elf32_Shdr, iElfFile, iElfFile->e_shoff);
	// Index of section header for section header string table
	TInt stIdx = iElfFile->e_shstrndx;
	TInt symIdx = -1;
	// Section name string table
	char * stringtable = ELFADDR(char, iElfFile, s[stIdx].sh_offset);
	// the index at which we find '.symtab' is the index of the symtab section
	for (TInt idx = 0; idx < iElfFile->e_shnum; idx++)
	    {
		if (idx != stIdx)
		       {
		       if (!strcmp(&stringtable[s[idx].sh_name], ".symtab"))
			      {
			      symIdx = idx;
			      break;
			      }
		       }
		}
	if (symIdx == -1) return EFalse;

	// save section header table
	iSectionHeaderTable = s;
	// save the index
	iSymIdx = symIdx;	
	// here's the symbol table
	iSymTab = ELFADDR(Elf32_Sym, iElfFile, s[symIdx].sh_offset);
	return ETrue;
	}

TBool ELFFile::InitDllData(void)
	{
	if (!iDynamicSegmentHdr)
	       {
#if defined(_DEBUG)
	       Print(EWarning, "Image '%s' has no import/export data.\n", iFileName);
#endif
	       return ETrue;
	       }
	iDllData = new ELFDllData(this);
	if (!iDllData)
	       {
	       Print(EPeError, "Out of memory allocating DLL data\n");
	       return EFalse;
	       }

	Elf32_Dyn * dyn = ELFADDR(Elf32_Dyn, iElfFile, iDynamicSegmentHdr->p_offset);
	TInt idx = 0;
	TInt soNameOffset = 0;
	while(dyn[idx].d_tag != DT_NULL) // best to make it explicit
	       {
	       switch (dyn[idx].d_tag)
		      {
		      case DT_HASH:
			   iDllData->iHashTable = ELFADDR(Elf32_HashTable, dyn, dyn[idx].d_val);
			   break;
		      case DT_STRTAB:
			   iDllData->iDynStrTab = ELFADDR(char, dyn, dyn[idx].d_val);
			   break;
		      case DT_SYMTAB:
			   iDllData->iDynSymTab = ELFADDR(Elf32_Sym, dyn, dyn[idx].d_val);
			   break;
		      case DT_RELA:
			   iDllData->iRela = ELFADDR(Elf32_Rela, dyn, dyn[idx].d_val);
			   break;
		      case DT_RELASZ:
			   iDllData->iRelaSz = dyn[idx].d_val;
			   break;
		      case DT_RELAENT:
			   iDllData->iRelaSz = dyn[idx].d_val;
			   break;
		      case DT_STRSZ:
			   iDllData->iDynStrTabSize = dyn[idx].d_val;
			   break;
		      case DT_ARM_SYMTABSZ_21: //For RVCT2.1
			   //iDllData->iDynSymTabSize = dyn[idx].d_val;
		      case DT_ARM_SYMTABSZ:
			  /* This is same as DT_ARM_SYMTABSZ_21, but for RVCT 2.2
			   * The tag value has been changed for RVC2.2 from RVCT2.1.
			   * We just ignore this. i.e., we get the symbol table size
			   * from the nchain field of the hash table as noted in section
			   * 3.2.2.2 of the BPABI.
			   */
			   break;
		      case DT_SYMENT:
			   iDllData->iSymSize = dyn[idx].d_val;
			   break;
		      case DT_SONAME:
			   soNameOffset = dyn[idx].d_val;
			   break;
		      case DT_REL:
			   iDllData->iRel = ELFADDR(Elf32_Rel, dyn, dyn[idx].d_val);
			   break;
		      case DT_RELSZ:
			   iDllData->iRelSz = dyn[idx].d_val;
			   break;
		      case DT_RELENT:
			   iDllData->iRelEnt = dyn[idx].d_val;
			   break;
		      case DT_NEEDED:
			   iDllData->AddToDependency(dyn[idx].d_val);
			   break;
		      case DT_PLTRELSZ:
		      case DT_PLTGOT:
		      case DT_INIT:
		      case DT_FINI:
		      case DT_RPATH:
		      case DT_SYMBOLIC:
		      case DT_PLTREL:
		      case DT_DEBUG:
		      case DT_TEXTREL:
		      case DT_JMPREL:
		      case DT_BIND_NOW:
			   break;
		      default:
		           Print(EPeError,"Unrecognized Dyn Array tag in image '%s'.\n", iFileName);
			   return EFalse;
		      }
		      idx++;
	       }
	return iDllData->Init();
	}


void ELFFile::Close()
//
// close the ELF file
//
	{
	HFile::Close(iFileHandle);
	}



TInt ELFFile::NumberOfImports() const
//
// Count the total number of imports for this image
//
	{
	return iDllData->NumberOfImports();
	}

TInt ELFFile::NumberOfImportDlls() const
//
// Count the number of referenced Dlls
//
	{
	return iDllData->NumberOfImportDlls();
	}

TInt ELFFile::NumberOfExports() const
//
// Count the number of exported symbols
//
	{
	return iDllData->NumberOfExports();
	}

TInt ELFFile::NumberOfCodeRelocs() 
	{ 
	return iDllData->NumberOfCodeRelocs(); 
	}

TInt ELFFile::NumberOfDataRelocs() 
	{ 
	return iDllData->NumberOfDataRelocs(); 
	}

Elf32_Phdr * ELFFile::GetSegmentFromAddr(Elf32_Addr addr)
	{
	TInt nphdrs = iElfFile->e_phnum;
	for (TInt idx = 0; idx < nphdrs; idx++)
	        {
		// take advantage of unsignedness 
		if ((addr - iPhdr[idx].p_vaddr) < iPhdr[idx].p_memsz) return &iPhdr[idx];
		}
	return NULL;
	}


TInt ELFFile::NumberOfRelocs()
	{
	return iDllData->NumberOfRelocs();
	}

TUint16 ELFFile::GetRelocType(Elf32_Rel *aReloc)
    {
	// We work out the type by figuring out the segment of the reloc
	TInt segmentIdx = ELF32_R_SYM(aReloc->r_info);
	
	// check to see if its a reserved or special index.
	if ((!segmentIdx) || ((segmentIdx >= SHN_LORESERVE) && (segmentIdx <= SHN_HIRESERVE)))
		// up until now these have been treated as KInferredRelocType, so lets continue...
		return KInferredRelocType;
			
	// need to see if this section is executable or writable
	if (iPhdr[segmentIdx-1].p_flags & PF_X) 
		return KTextRelocType;
	if (iPhdr[segmentIdx-1].p_flags & PF_W) 
		return KDataRelocType;
	// perhaps we should error here.
	return KInferredRelocType;
	}

TBool ELFFile::GetRelocs(Elf32_Rel **aCodeRelocs, Elf32_Rel **aDataRelocs)
        {
	return iDllData->GetRelocs(aCodeRelocs, aDataRelocs);
	}

TUint ELFFile::GetCodeSize()
    {
	return iCodeSegmentHdr->p_filesz;
	}

TBool ELFFile::HasInitialisedData()
	{
	return iDataSegmentHdr != NULL && iDataSegmentHdr->p_filesz != 0;
	}

TUint ELFFile::GetDataSize()
    {
	return iDataSegmentHdr != NULL ? iDataSegmentHdr->p_filesz : 0;
	}

TBool ELFFile::HasBssData()
	{
	return iDataSegmentHdr != NULL && (iDataSegmentHdr->p_memsz - iDataSegmentHdr->p_filesz) != 0;
	}

TUint ELFFile::GetBssSize()
    {
	return iDataSegmentHdr != NULL ? iDataSegmentHdr->p_memsz - iDataSegmentHdr->p_filesz: 0;
	}






TBool ELFFile::IsValidFileHeader(Elf32_Ehdr * iElfFile)
 	{
        if (!(iElfFile->e_ident[EI_MAG0] == ELFMAG0 &&
	      iElfFile->e_ident[EI_MAG1] == ELFMAG1 &&
	      iElfFile->e_ident[EI_MAG2] == ELFMAG2 &&
	      iElfFile->e_ident[EI_MAG3] == ELFMAG3))
		{
                Print(EPeError,"Invalid ELF magic.\n");
		return EFalse;
		}

        if (iElfFile->e_ident[EI_CLASS] != ELFCLASS32)
		{
                Print(EPeError,"File is not a 32 bit object file.\n");
		return EFalse;
		}
        if (iElfFile->e_ident[EI_DATA] != ELFDATA2LSB)
		{
                Print(EPeError,"File data encoding is not Little Endian.\n");
		return EFalse;
		}

  	if (iElfFile->e_machine != EM_ARM)
		{
		Print(EPeError,"File does not target ARM/THUMB processors.\n");
		return EFalse;
		}

  	if (!(iElfFile->e_type == ET_EXEC || iElfFile->e_type == ET_DYN))
		{
		Print(EPeError,"File is neither an executable nor a shared object\n");
		return EFalse;
		}

	return ETrue;
	}


// Get details of the next import to fix-up in the current file. Fill in the name of the dll 
//it is imported from, the ordinal number and the address to write back to.
#define ORDINAL_DONE 0x40000000


// The following static functions are passed an array of PE files to operate on

Elf32_Sym * ELFFile::FindSymbol(const TText *aName)
    {
	Elf32_Shdr * s = ELFADDR(Elf32_Shdr, iElfFile, iElfFile->e_shoff);	
	TInt symIdx = iSymIdx;
	Elf32_Sym * sym = iSymTab;
	TInt nSyms = s[symIdx].sh_size / s[symIdx].sh_entsize;
	char * symStringtable = ELFADDR(char, iElfFile, s[s[symIdx].sh_link].sh_offset);
	for (TInt jdx = 0; jdx < nSyms; jdx++)
	    {
		if (!strcmp(&symStringtable[sym[jdx].st_name], (char *)aName)) 
			return &sym[jdx];
		}
	return (Elf32_Sym *)0;
    }

TBool ELFFile::SymbolPresent(TText *aName)
    {
	return (FindSymbol(aName) != 0);
    }

TBool ELFFile::GetExceptionIndexInfo(TUint32 &aOffset)
    {
    const TText * aBase = (TText *)".ARM.exidx$$Base";
    const TText * aLimit = (TText *)".ARM.exidx$$Limit";
	Elf32_Sym * exidxBase = FindSymbol(aBase);
	Elf32_Sym * exidxLimit = FindSymbol(aLimit);
	if (exidxBase && exidxLimit && (exidxLimit->st_value - exidxBase->st_value)) 
	    {
		const TText * aExceptionDescriptor = (TText *)"Symbian$$CPP$$Exception$$Descriptor";
		Elf32_Sym * aED = FindSymbol(aExceptionDescriptor);
		if (aED) 
			{
			// Set bottom bit so 0 in header slot means an old binary.
			// The decriptor is always aligned on a 4 byte boundary.
			aOffset = (aED->st_value - iLinkedBase) | 0x00000001;
			return ETrue;
			}
		else
			{
			Print(EPeError,"Executable has exception table but no exception descriptor\n");
			exit(666);
			}
		}
	return EFalse;
    }

TBool ELFFile::SetUpLookupTable()
{
	if(!iDllData->CreateSymLookupTable() ) {
		Print(EPeError,"Failed to create named symbol lookup information\n");
		return FALSE;
	}
	if(!iDllData->CreateDependency()){
		Print(EPeError,"Failed to create dependency ordering for named symbol lookup\n");
		return FALSE;
	}

	iDllData->SetExportSymInfo();
	return TRUE;
}

void ELFFile::GetExportSymInfoHeader(E32EpocExpSymInfoHdr& aSymInfoHdr)
{
	iDllData->GetExportSymInfoHeader(aSymInfoHdr);
}

void ELFFile::SetLookupTblBase(TInt aBaseOffset)
{
	iDllData->SetLookupTblBase(aBaseOffset);
}

TInt ELFFile::GetLookupTblSize()
{
	return iDllData->GetLookupTblSize();
}

TUint ELFFile::GetSymLookupSection(char* aBuff)
{
	return iDllData->GetSymLookupSection(aBuff);
}

