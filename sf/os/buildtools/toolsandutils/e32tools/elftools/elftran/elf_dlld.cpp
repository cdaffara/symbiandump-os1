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
// ELFDLLD.CPP
// 
//


#include <string.h>
#include <stdlib.h>
#include <e32std.h>
#include <e32std_private.h>
#include <tools/elfdefs.h>
#include "elfdll.h"
#include "elffile.h"
#include "h_utl.h"
#include <e32ldr.h>

ELFFile::ELFDllData::ELFDllData(ELFFile * f) :
        iElfFile(f), iDynStrTab(0), iDynStrTabSize(0), iDynSymTab(0),
		iSymSize(0), iRela(0), iRelaSz(0), iRelaEnt(0), 
		iRel(0), iRelSz(0), iRelEnt(0),
		iHashTable(0),iDllHead(0), iDllTail(0), 
		iNumberOfImports(0), iNumberOfExports(0), iNumberOfImportDlls(0),
		iStringTableSize(0), iNumberOfRelocs(0), iNumberOfCodeRelocs(0), 
		iNumberOfDataRelocs(0), iNamedLookupEnabled(0), iNamedExportSymbolHead(0), 
		iNamedExportCount(0), iSymStringTableSize(0),iStringNameOffset(0), iOrdZeroRec(0), 
		iDepRecords(0), iDepRecordsTail(0),iNeededDllNames(0),
		iNeededDllNamesTail(0)
		{
		}

ELFFile::ELFDllData::~ELFDllData()
	{
		delete iOrdZeroRec;
		delete iDepRecords;
		delete iNeededDllNames;
	}

TBool ELFFile::ELFDllData::Init()
	{
	// process imported symbols
	// There will be at least one relocation (assumed to be Elf32_Rel) for each such symbol.
	// S iterate over relocations looking for DLL symbols and add them to the record of
	// of import info.
	TInt nrelocs = iRelSz/iRelEnt;
	TInt SrcSegIdx = -1;
	TInt errors = 0;

	for (TInt idx = 0; idx < nrelocs; idx++)
			{
			Elf32_Rel * rel = &iRel[idx];
			TUint relType = ELF32_R_TYPE(rel->r_info);

			if (relType == R_ARM_ABS32)      
					{	
					TInt symIdx = ELF32_R_SYM(rel->r_info);
					Elf32_Sym * sym = &iDynSymTab[symIdx];
					DllSymbol * dllSym = DllSymbolP(sym);
					if (dllSym) 
							{
							dllSym->iRel = rel;
							dllSym->iSegment = iElfFile->GetSegment(SrcSegIdx);
							if (!AddSymbol(dllSym)) return EFalse;
							}
					else errors++;
					}
			else if (relType == R_ARM_RABS32)
					{
					iNumberOfRelocs++;
					if (iElfFile->CodeSegmentP(SrcSegIdx)) iNumberOfCodeRelocs++;
					else iNumberOfDataRelocs++;
					}
			else if (relType == R_ARM_RBASE)
				    {
					SrcSegIdx = ELF32_R_SYM(rel->r_info);
					if (SrcSegIdx) SrcSegIdx--;
					}
			else 
				    {
					// Gives error with all other Relocation types..
					TInt symIdx = ELF32_R_SYM(rel->r_info);
					Elf32_Sym * s = &iDynSymTab[symIdx];
					char * symName = ELFADDR(char,iDynStrTab,s->st_name);
					Print(EPeError, "Unresolved symbol: %s\n", symName);
					errors++;
					}
			}
	// Set up export info
	if (InitExportInfo())
	       {
	       TText * sym = (TText *)"_E32Startup";
	       // If _E32Startup is defined then this is not a DLL
	       iImageIsDll = !iElfFile->SymbolPresent(sym);
	       }
	if (errors > 0) return EFalse;
	return ETrue;
	}

TBool ELFFile::ELFDllData::ParseDllSymbol(Elf32_Sym * s, char *& dll, TUint& len, TInt& ord)
    {
	char * sym = ELFADDR(char,iDynStrTab,s->st_name);
	if (!strncmp(sym, DLLSYMPREFIX, strlen(DLLSYMPREFIX)))
			{
			dll = sym + strlen(DLLSYMPREFIX);
			TUint lim = strlen(dll);
			TUint index = strcspn(dll, DLLSYMPREFIX0);
			if ((index + strlen(DLLSYMSUFFIX)) < lim)
					{
					len = index;
					char * dllSuffix = dll+index;
					char * match = DLLSYMSUFFIX;
					int suflen = strlen(DLLSYMSUFFIX);
					if (!strncmp(dllSuffix, match, suflen))
							{
							char * ordString = dll+index+suflen;
							char * final;
							TUint X = strtoul(ordString, &final, ORDBASE);
							if (ordString != final)
									{
									ord = (TInt)X;
									return ETrue;
									}
			     
							}
					}
			}
	dll = 0;
	len = 0;
	ord = -1;
	return EFalse;
	}

ELFFile::ELFDllData::DllSymbol * ELFFile::ELFDllData::DllSymbolP(Elf32_Sym * s)
    {
	char * dllName;
	TUint nameLen;
	TInt symOrd;
	if (ParseDllSymbol(s, dllName, nameLen, symOrd))
	       {
	       DllSymbol * ds = new DllSymbol(dllName, nameLen, symOrd);
	       if (ds) return ds;
	       Print(EPeError, "Out of memory.\n");
	       return NULL;
	       }
	// If we get here its not a valid 'dll symbol' and so it is an unresolved symbol
	char * sym = ELFADDR(char,iDynStrTab,s->st_name);
	Print(EPeError, "Unresolved symbol: %s\n", sym);
    return NULL;
	}

TBool ELFFile::ELFDllData::AddSymbol(ELFFile::ELFDllData::DllSymbol * s)
    {
	DllRec * aDll = NULL;
     
	for (DllRec * r = iDllHead; r != NULL; r = r->iNext)
			{
			if (!strncmp(s->iDll, r->iName, s->iLen))
					{
					aDll = r;
					break;
					}
			}
	if (aDll)
			{
		    iNumberOfImports++;
		    aDll->AddSymbol(s);
			return ETrue;
			}
	else
			{
			aDll = new DllRec(s->iDll, s->iLen, s);
			if (aDll)
					{
					if (!iDllHead) 
							{
							iDllHead = iDllTail = aDll;
							}
					else
							{
							iDllTail->iNext = aDll;
							iDllTail = aDll;
							}
					iStringTableSize += (s->iLen + 1);
	      			iNumberOfImportDlls++;
					iNumberOfImports++;
					return ETrue;
					}
			else
					{
					Print(EPeError, "Out of memory.\n");
					return EFalse;
					}
			}
	}

void ELFFile::ELFDllData::DllRec::AddSymbol(ELFFile::ELFDllData::DllSymbol * s)
	{
	nImports++;
    iTail->iNext = s;
    iTail = s;
    }

static unsigned long elf_hash(const unsigned char *name)
    {
    unsigned long h, g;
    for (h = 0; *name != 0; ++name)
	        {
			h = (h << 4) + *name;
			g = h & 0xf0000000;
			if (g != 0) h ^= g >> 24;
			h &= ~g;
			}
	return h;
    }

Elf32_Word ELFFile::ELFDllData::FindSymbolIndex(TText * s)
    {
	TUint h = elf_hash(s);
	TUint nb = iHashTable[0].nBuckets;
	TUint probe = h%nb;
	Elf32_Sword * bucket = ELFADDR(Elf32_Sword, iHashTable, sizeof(Elf32_HashTable));
	Elf32_Sword * chain = ELFADDR(Elf32_Sword, bucket, nb * sizeof(Elf32_Sword));
	Elf32_Sword idx = bucket[probe];

	do 	
			{
			if (!strcmp(ELFADDR(char, iDynStrTab, iDynSymTab[idx].st_name), (char *)s)) return idx;
			idx = chain[idx];
			} while (idx > 0);
	if (idx == 0) idx = -1;
	return idx;
	}

TBool ELFFile::ELFDllData::InitExportInfo()
	{
	memset(&iSymInfoHdr, 0, sizeof(iSymInfoHdr));
	TText * exp = (TText *)EXPORTTABLENAME;
	if ((iExportTableSymIdx = FindSymbolIndex(exp)) != -1)
			{
			TText * exps = (TText *)EXPORTTABLESIZENAME;
			iExportTableSizeSymIdx = FindSymbolIndex(exps);
			//TUint offset = iDynSymTab[iExportTableSizeSymIdx].st_value - (TUint)code;
			Elf32_Word * pNumberOfExports = iElfFile->CodePtrFromAddr(iDynSymTab[iExportTableSizeSymIdx].st_value);
			iNumberOfExports = * pNumberOfExports;
			iImageIsDll = ETrue;
			return ETrue;
			}
	else iImageIsDll = EFalse;
	return EFalse;
	}

TInt ELFFile::ELFDllData::NumberOfImports(void)
    {
	return iNumberOfImports;
	}

TInt ELFFile::ELFDllData::NumberOfExports(void)
    {
	return iNumberOfExports;
	}

TInt ELFFile::ELFDllData::NumberOfImportDlls(void)
    {
	return iNumberOfImportDlls;
	}

TInt ELFFile::ELFDllData::NumberOfRelocs()
	{
	return iNumberOfRelocs;
	}

char * ELFFile::ELFDllData::CreateImportSection(TInt &aSize)
	{
	if (!iNumberOfImports)
			{
			aSize = 0;
			return 0;
			}

	TInt byteSize = sizeof(E32ImportSection) + 
	                (sizeof(E32ImportBlock) * iNumberOfImportDlls) +
					(sizeof(TUint) * iNumberOfImports) +
					iStringTableSize;
	if(iNamedLookupEnabled)
	{
		// 0th ordinal of each DLL imported from.
		byteSize += (sizeof(TUint) * iNumberOfImportDlls);
	}
	char * newSection = new char[byteSize];

	if (!newSection)
	       {
	       Print(EPeError, "Failed to allocate new import section.\n");
	       aSize = 0;
	       return 0;
	       }

	((E32ImportSection *)newSection)->iSize = byteSize;

	// used to point to current ImportBlock
	E32ImportBlock * bp = ELFADDR(E32ImportBlock, newSection, sizeof(E32ImportSection));
	// used to point to current import relocation entry
	TUint * rp;
	// used to point to current location in string table
	char * sp = ELFADDR(char, newSection, (byteSize - iStringTableSize));
	OrdZeroRecord *aDep = iDepRecords;

	for (DllRec * dll = iDllHead; dll != NULL ; dll = dll->iNext, bp = (E32ImportBlock *)rp )
	        {
			// set up the offset from E32ImportBlock.iOffsetOfDllName to the
			// corresponding  string table location
			bp->iOffsetOfDllName = (sp - (char *)newSection);
			bp->iNumberOfImports = dll->nImports;
			if(iNamedLookupEnabled)
			{
				bp->iNumberOfImports++;
			}

			// copy the DLL name to the string table
			memcpy(sp , dll->iName, dll->iLen);
			sp[dll->iLen] = 0;
			sp = sp + dll->iLen + 1;

			// sort out the import 'relocs'
			rp = ELFADDR(TUint, bp, sizeof(E32ImportBlock));
		
			for (DllSymbol * sym = dll->iHead; sym != NULL; sym = sym->iNext, rp++)
				    {
					Elf32_Phdr * segment = sym->iSegment;
					Elf32_Addr segBase = segment->p_vaddr;

					// sanity check: segment should be the code segment
					if (!iElfFile->CodeSegmentP(segment))
							{
							Print(EPeError, "Import relocation does not refer to code segment.\n");
							aSize = 0;
							return 0;
							}

					// This field is misnamed because it is actually given as a virtual address in 
					// dynamic relocations
					Elf32_Addr dynOffset = sym->iRel->r_offset;
					//So this is the 'real' offset of the reloc in  the segment in which it occurs
					Elf32_Addr segOffset = dynOffset - segBase;
					TUint * relocAddr = ELFADDR(TUint, iElfFile->ELFFileBase(), (segment->p_offset + segOffset));
					// The only non-zero reloc vals we expect are for vtables.
					// We store there reloc offset in the top 16 bits of the 'reloc info'.
					// NB the ELF reloc values should only ever be multiples of 4. So we could optimize here,
					// but we won't.
					TUint relocVal = *relocAddr;
					TUint importOrdinal = sym->iOrd;
					if (relocVal > 0xFFFF)
							Print(EPeError, "ELF relocation exceeds E32Image limit of 64K.\n");
					if (importOrdinal > 0xFFFF)
							Print(EPeError, "Import ordinal exceeds E32Image limit of 64K.\n");
   
   					*rp = segOffset;
   			
   					// eek !! surgery on the code segment....
					*relocAddr = (relocVal<<16) | importOrdinal;
					}
			
				if(iNamedLookupEnabled)
				{
					aDep = FindDependency(dll->iName, dll->iLen);
					if( aDep )
					{
						*rp = aDep->iOffset;
						rp++;
					}
				}
			}
	
	aSize = byteSize;
	return newSection;
	}

TUint ELFFile::ELFDllData::GetExportTableOffset(void)
    {
	Elf32_Sym * et = &iDynSymTab[iExportTableSymIdx];
	Elf32_Phdr * segment = iElfFile->GetSegment(et->st_shndx - 1);
	
	return et->st_value - segment->p_vaddr;
	}

TBool ELFFile::ELFDllData::GetRelocs(Elf32_Rel **aCodeRelocs, Elf32_Rel **aDataRelocs)
	//	
	// load the relocs from the reloc section into relocation and relocsection arrays
	//
	{
	TInt nrelocs = iRelSz/iRelEnt;

	TInt SrcSegIdx = -1;

	for (TInt idx = 0, cidx = 0, didx = 0; idx < nrelocs; idx++)
	        {
			Elf32_Rel * rel = &iRel[idx];

			if (ELF32_R_TYPE(rel->r_info) == R_ARM_RABS32)
					{
					if (iElfFile->CodeSegmentP(SrcSegIdx))
							aCodeRelocs[cidx++]=rel;
					else if (iElfFile->DataSegmentP(SrcSegIdx))
							aDataRelocs[didx++]=rel;
					}
			else if (ELF32_R_TYPE(rel->r_info) == R_ARM_RBASE)
					{
					SrcSegIdx = ELF32_R_SYM(rel->r_info);
					if (!(iElfFile->CodeSegmentP(SrcSegIdx-1) || iElfFile->DataSegmentP(SrcSegIdx-1)))
							{
							Print(EPeError, "Source segment for relocations is neither Code or Data.\n");
							return EFalse;
							}
					SrcSegIdx--;
					}
			}
	if(!iNamedLookupEnabled)
		return ETrue;

	// Add the 0th ordinal of this binary - The relocation info is already setup.
	aCodeRelocs[cidx++] = &iOrdZeroRec->iReloc;

	// add relocation entries for each of the symbols
	NamedExportSymbol *aSym = iNamedExportSymbolHead;
	while(aSym)
	{
		// The symbol name info is part of the code section hence all relocations
		// are collected as part of Code relocations.
		aCodeRelocs[cidx++] = &aSym->iReloc;
		aSym = aSym->Next();
	}

	// Since we have added a few relocations, lets make sure
	// they are still sorted on addresses they refer to.
	
	//Sorting the code relocs
	TInt aIdx1, aIdx2;
	Elf32_Rel *aTmp;
	for (aIdx1 = 0; aIdx1 < cidx; aIdx1++) {
		for (aIdx2 = aIdx1; aIdx2 < cidx; aIdx2++) {
			if(aCodeRelocs[aIdx1]->r_offset > aCodeRelocs[aIdx2]->r_offset) {
				aTmp = aCodeRelocs[aIdx2];
				aCodeRelocs[aIdx2] = aCodeRelocs[aIdx1];
				aCodeRelocs[aIdx1] = aTmp;
			}
		}
	}
	
	//Sorting the data relocs
	for (aIdx1 = 0; aIdx1 < didx; aIdx1++) {
		for (aIdx2 = aIdx1; aIdx2 < didx; aIdx2++) {
			if(aDataRelocs[aIdx1]->r_offset > aDataRelocs[aIdx2]->r_offset) {
				aTmp = aDataRelocs[aIdx2];
				aDataRelocs[aIdx2] = aDataRelocs[aIdx1];
				aDataRelocs[aIdx1] = aTmp;
			}
		}
	}

	return ETrue;
	}

NamedExportSymbol::NamedExportSymbol(char* aName, Elf32_Addr aValue) : \
	iSymbolName(aName), iValue(aValue), iNext(NULL)
{
}

TBool ELFFile::ELFDllData::CreateSymLookupTable()
{
	if( !SetupSymbolValues() )
		return FALSE;

	if( !SetupSymbolNames() )
		return FALSE;

	return TRUE;
}

TBool ELFFile::ELFDllData::SetupSymbolValues()
{
	NamedExportSymbol *aSym, *aPrevSym;

	if( iExportTableSymIdx == -1 || iExportTableSizeSymIdx == -1)
		return FALSE;

	// Fetch the 'export table' symbol from the dynamic symbol table.
	Elf32_Sym *aElfExpTbl = &iDynSymTab[iExportTableSymIdx];

	// Fetch the 'export table size' symbol from the dynamic symbol table.
	Elf32_Sym *aElfExpTblSz = &iDynSymTab[iExportTableSizeSymIdx];

	if((aElfExpTbl->st_value - aElfExpTblSz->st_value) != 4)
	{
		// Check that the word prior to the export table is not the export table size
		// This is to make sure that there is space for the 0th ordinal and
		// we dont overwrite the 'export table size' entry.
		iNamedLookupEnabled = 1;
	}
	else 
		return FALSE;

	// Fetch the export table contents
	Elf32_Word * aExpEntries = iElfFile->CodePtrFromAddr(aElfExpTbl->st_value);

	if(!aExpEntries)
		return FALSE;

	aSym = aPrevSym = NULL;
	TInt idx;
	// Create symbols corresponding to export table entries.
	for(idx = 0;  idx < iNumberOfExports; idx++)
	{
		//Symbols marked Absent are ignored.
		if( aExpEntries[idx] == iElfFile->iEntryPoint)
			continue;

		aSym = new NamedExportSymbol(0, aExpEntries[idx]);
		iNamedExportCount++;
		if(aPrevSym)
		{
			aPrevSym->Next(aSym);
		}
		else
		{
			iNamedExportSymbolHead = aSym;
		}

		aPrevSym = aSym;
	}

	return TRUE;
}

TBool ELFFile::ELFDllData::SetupSymbolNames()
{
	char *aSymName = NULL;
	NamedExportSymbol *aSym;
	TUint aDynSymbolTblCount = iHashTable->nChains;
	TInt aCount = 0;
	TInt aCodeSegIdx = iElfFile->CodeSegmentIndex() + 1;
	TInt aDataSegIdx = iElfFile->DataSegmentIndex() + 1;

	// Traverse the dynamic symbol table
	for(TUint idx = 0; idx < aDynSymbolTblCount; idx++)
	{
		//Consider only the the defined symbols
		if( ELF32_ST_TYPE(iDynSymTab[idx].st_info) == STT_OBJECT || 
			ELF32_ST_TYPE(iDynSymTab[idx].st_info) == STT_FUNC )
		{
			aSym = iNamedExportSymbolHead;
			while(aSym)
			{
				// Name already set
				if(aSym->Name())
				{
					aSym = aSym->Next();
					continue;
				}
				Elf32_Addr aAddr = aSym->Value();

				// If the exported symbol and the dynamic symbol table entry have the 
				// same values, setup the name
				if(iDynSymTab[idx].st_value == aAddr)
				{
					aSymName = ELFADDR(char, iDynStrTab, iDynSymTab[idx].st_name);
					aSym->Name(aSymName);
					
					if(iElfFile->CodeSegmentP (iElfFile->GetSegmentFromAddr(aAddr)) ) {
						aSym->iReloc.r_info = aCodeSegIdx << 8 | R_ARM_RABS32;
						aSym->iSymRelocType = KTextRelocType;
					}
					else {
						aSym->iReloc.r_info = aDataSegIdx << 8 | R_ARM_RABS32;
						aSym->iSymRelocType = KDataRelocType;
					}
					
					iNumberOfCodeRelocs++;
					iNumberOfRelocs++;

					// The offset to the name is always 4 byte aligned.
					iStringNameOffset = iSymStringTableSize >> 2;
					aSym->NameOffset( iSymStringTableSize );
					// These are NULL-terminated strings
					iSymStringTableSize += (strlen(aSymName) + 1);
					iSymStringTableSize = ALIGN4(iSymStringTableSize);

					aCount++;
					break;
				}
				aSym = aSym->Next();
			}
		}
	}

	if(aCount != iNamedExportCount)
		return FALSE;

	// Sort symbols on their names...
	if(iNamedExportCount > 1)
	{
		NamedExportSymbol **aTmpStart = &iNamedExportSymbolHead;
		Sort(aTmpStart, iNamedExportSymbolHead);
	}

	return TRUE;
}

void ELFFile::ELFDllData::SetLookupTblBase(TInt aBaseOffset)
{
	Elf32_Addr	aBaseAddr = iElfFile->iLinkedBase + iElfFile->GetCodeSize();

	Elf32_Addr	aAddr;

	// setup relocations of each of the exported symbols.
	aAddr = aBaseAddr + iSymInfoHdr.iSymbolTblOffset;
	NamedExportSymbol *aSym = iNamedExportSymbolHead;
	while(aSym)
	{
		aSym->iReloc.r_offset = aAddr;
		aAddr += sizeof(Elf32_Addr);
		aSym = aSym->Next();
	}

	// setup relocations for the 0th ordinal of this binary. 

	iOrdZeroRec = new OrdZeroRecord(0);
	Elf32_Sym * et = &iDynSymTab[iExportTableSymIdx];
	iOrdZeroRec->iReloc.r_offset = et->st_value - 4; // The word prior ro the first entry
											// of the export table is the 0th ordinal entry.

	//At the 0th ordinal, write the address of the start of symbol info	
	TUint32 aZeroOrdOff = et->st_value - 4 - iElfFile->iLinkedBase;
	aZeroOrdOff += (iElfFile->GetSegment(et->st_shndx - 1))->p_offset;
	TUint32 *aZeroOrdLocation = ELFADDR(TUint32, iElfFile->ELFFileBase(), aZeroOrdOff);
	*aZeroOrdLocation = aBaseAddr;

	iOrdZeroRec->iReloc.r_info = ELF32_R_INFO(et->st_shndx, R_ARM_RABS32);
	iNumberOfCodeRelocs++;
	iNumberOfRelocs++;

	TInt aOffset = aBaseOffset + iSymInfoHdr.iDepDllZeroOrdTableOffset;

	OrdZeroRecord *aDepRecord = iDepRecords;

	while( aDepRecord )
	{
		// Setup the offset - This offset (relative code segment) is filled in the 
		// import table to point to this dependency record.
		aDepRecord->iOffset = aOffset;

		aOffset += 4;
		aDepRecord = aDepRecord->iNext;
	}
}

TBool ELFFile::ELFDllData::AddToDependency(TUint aOff)
{
	// Add the name found in DT_NEEDED into a list.
	// The dynamic string table might not have been found in dynamic table yet.
	// So store the offset (wrt base of dynamic string table) for now.
	NeededDLLsList *aNeeded = new NeededDLLsList(aOff);
	if(!aNeeded)
		return FALSE;

	if ( iNeededDllNames ) {
	 iNeededDllNamesTail->iNext= aNeeded;
	 iNeededDllNamesTail = aNeeded; 
	}
	else {
		iNeededDllNames = iNeededDllNamesTail = aNeeded;
	}
	
	return TRUE;
}

TBool ELFFile::ELFDllData::CreateDependency()
{
	OrdZeroRecord *aDep;
	NeededDLLsList *aNeeded = iNeededDllNames;
	char *aDllName = NULL;
	DllRec *aRec;
	TInt aNeededFound;

	for(aNeededFound = 0; (aNeededFound < iNumberOfImportDlls) && aNeeded;)
	{
		aRec = 0;
		while(aNeeded) {
			aDllName = iDynStrTab + aNeeded->iOffset;
			// aNeeded is just a guess that this binary might be dependent on aDllName
			// Search through the import table to find if the guess was correct.
			aRec = SearchImports(aDllName);
			if(aRec && FindDependency(aRec->iName, aRec->iLen) == NULL) {
				// Check if aDllName is listed in import table and it
				// not added already in the depedency records.
				aNeededFound++;
				break;
			}
			// Bad guess...go to the next aNeeded
			aNeeded = aNeeded->iNext;
		}

		if( !aRec )
			return FALSE;

		aDep = new OrdZeroRecord(aDllName);
		if(!iDepRecords)
		{
			iDepRecords = iDepRecordsTail = aDep;
		}
		else
		{
			iDepRecordsTail->iNext = aDep;
			iDepRecordsTail = aDep;
		}
		aNeeded = aNeeded->iNext;
	}

	return (aNeededFound == iNumberOfImportDlls);
}

ELFFile::ELFDllData::DllRec* ELFFile::ELFDllData::SearchImports(char *aName)
{
	DllRec *aRec = iDllHead;
	while (aRec)
	{
		if(strncmp(aRec->iName, aName, aRec->iLen) == 0)
			return aRec;
		aRec = aRec->iNext;
	}
	return NULL;
}

OrdZeroRecord* ELFFile::ELFDllData::FindDependency(char* aName, TUint aLen)
{
	OrdZeroRecord* aDep = iDepRecords;
	while(aDep)
	{
		if(strncmp(aName, aDep->iName, aLen) == 0)
			return aDep;
		aDep = aDep->iNext;
	}
	return NULL;
}

void ELFFile::ELFDllData::GetExportSymInfoHeader(E32EpocExpSymInfoHdr& aSymInfoHdr)
{
	memcpy(&aSymInfoHdr, &iSymInfoHdr, sizeof(E32EpocExpSymInfoHdr));
}

void ELFFile::ELFDllData::SetExportSymInfo()
{
	iSymInfoHdr.iSymCount = (TUint16)iNamedExportCount;
	iSymInfoHdr.iSymbolTblOffset = sizeof(E32EpocExpSymInfoHdr);
	iSymInfoHdr.iStringTableSz = iSymStringTableSize;
	TInt aSymTabSz;
	if( iStringNameOffset > 0xffff){
		iSymInfoHdr.iFlags = KNameLookupOffsetFlag32;  // Flag indicating 32 bit offsets 
													   // for symbol names
		aSymTabSz = iNamedExportCount* sizeof(TUint32);// symbol addresses
		aSymTabSz += iNamedExportCount* sizeof(TUint32);// symbol name 32-bit offsets
	}
	else
	{
		iSymInfoHdr.iFlags &= ~KNameLookupOffsetFlag32;// Flag indicating 16-bit offsets 
													   // for symbol names
		aSymTabSz = iNamedExportCount* sizeof(TUint32);	// symbol addresses
		aSymTabSz += iNamedExportCount* sizeof(TUint16);// symbol name 16-bit offsets
		aSymTabSz = ALIGN4(aSymTabSz);
	}
	iSymInfoHdr.iStringTableOffset = iSymInfoHdr.iSymbolTblOffset + aSymTabSz;
	iSymInfoHdr.iDllCount = iNumberOfImportDlls;
	iSymInfoHdr.iDepDllZeroOrdTableOffset = iSymInfoHdr.iStringTableOffset + \
								iSymInfoHdr.iStringTableSz;

	iSymInfoHdr.iSize = iSymInfoHdr.iDepDllZeroOrdTableOffset + \
						iSymInfoHdr.iDllCount * sizeof(Elf32_Addr);
}

TUint ELFFile::ELFDllData::GetSymLookupSection(char* aBuff)
{
	if( !iNamedLookupEnabled)
		return 0;

	memcpy(aBuff, &iSymInfoHdr, sizeof(iSymInfoHdr));
	
	// Name offsets start after the end of symbol addresses.
	TUint32 aNameOffsetStart = iSymInfoHdr.iSymbolTblOffset + \
								iNamedExportCount* sizeof(TUint32);
																
	TUint32 *aAddrPtr = (TUint32*)(aBuff + iSymInfoHdr.iSymbolTblOffset);
	TUint32 aStringTabOff = 0;
	char *aStringTab = aBuff + iSymInfoHdr.iStringTableOffset;//Start of the string table.
	NamedExportSymbol *aSym = iNamedExportSymbolHead;
	while(aSym)
	{
		*aAddrPtr = aSym->Value();
		aStringTabOff = aSym->NameOffset(); // Get the offset of symbol name (which is wrt 
											// string table base).
		if( iSymInfoHdr.iFlags & KNameLookupOffsetFlag32 )
		{
			TUint32 *aNameOffPtr = (TUint32*)(aBuff + aNameOffsetStart);
			*aNameOffPtr = (aStringTabOff >> 2);//write the offset of the name
			strcpy(aStringTab + aStringTabOff, aSym->Name());//write the symbol name
			aNameOffsetStart +=4;
		}
		else
		{
			TUint16 *aNameOffPtr = (TUint16*)(aBuff + aNameOffsetStart);
			*aNameOffPtr = (TUint16)(aStringTabOff >> 2);//write the offset of the name
			strcpy(aStringTab + aStringTabOff, aSym->Name());//write the symbol name
			aNameOffsetStart +=2;
		}
		aAddrPtr++;
		aSym = aSym->Next();
	}

	OrdZeroRecord *aRec = iDepRecords;
	TUint32* aDepsTable = (TUint32*)(aBuff + iSymInfoHdr.iDepDllZeroOrdTableOffset);
	while(aRec)
	{
		*aDepsTable++ = 0;
		aRec = aRec->iNext;
	}
	return iSymInfoHdr.iSize;
}

void ELFFile::ELFDllData::Sort(NamedExportSymbol** aDstList, NamedExportSymbol* aSrcList)
{
	NamedExportSymbol *aSym = aSrcList;
	NamedExportSymbol **aSymbols = new NamedExportSymbol*[iNamedExportCount];
	
	TInt pos;
	for (pos = 0; pos < iNamedExportCount; pos++) {
		aSymbols[pos] = aSym;
		aSym = aSym->Next();
	}
	
	NamedExportSymbol **aResult = new NamedExportSymbol*[iNamedExportCount];
	MergeSort(aResult, aSymbols);
	iNamedExportSymbolHead = aResult[0];
	for (pos = 0; pos < iNamedExportCount; pos++) {
		aSym = aResult[pos];
		if( pos == iNamedExportCount-1)
			aSym->Next(NULL);
		else
			aSym->Next(aResult[pos+1]);
	}
	*aDstList = aResult[0];
	delete [] aResult;
	delete [] aSymbols;
}

void ELFFile::ELFDllData::MergeSort(NamedExportSymbol** aDstList, NamedExportSymbol** aSrcList)
{
	MergeSort(aDstList, aSrcList, 0, iNamedExportCount);
}

void ELFFile::ELFDllData::MergeSort(NamedExportSymbol** aDstList, NamedExportSymbol** aSrcList, \
									TUint aLeft, TUint aRight)
{
	if( (aRight - aLeft) <= 1)
		return;

	TUint aSize = aRight - aLeft;
	TUint aCenter = aLeft + aSize/2;

	MergeSort(aDstList, aSrcList, aLeft, aCenter);
	MergeSort(aDstList, aSrcList, aCenter, aRight);

	TUint aLPos, aRPos, aCnt;
	aLPos = aLeft;
	aRPos = aCenter;
	for(aCnt = 0; aCnt < aSize; aCnt++)
	{
		if( (aLPos < aCenter) &&
			(aRPos == aRight || (strcmp(aSrcList[aLPos]->Name(), aSrcList[aRPos]->Name()) < 0) )
		  )
		{
			// Compare the left half with the right and add the lesser one.
			// The comparision is done on the topmost element on each half.
			// if aRPos is past the last element of the right half, the left element has 
			// nothing to compare with. Just add it to the result list.
			aDstList[aCnt] = aSrcList[aLPos];
			aLPos++;
		}
		else
		{
			// Add the greater one into the list.
			// if aLPos is past the element at the center, it anyway belongs to the
			// right half. Add it to the result list.
			aDstList[aCnt] = aSrcList[aRPos];
			aRPos++;
		}
	}

	// Once the sublist is sorted, put it back to the source list
	// so that the parent has its left and right sublists are sorted.
	for(aCnt = 0; aCnt < aSize; aCnt++)
	{
		aSrcList[aLeft+aCnt] = aDstList[aCnt];
	}
}

