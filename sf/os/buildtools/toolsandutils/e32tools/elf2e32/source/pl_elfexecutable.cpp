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
// Implementation of the Class ElfExecutable for the elf2e32 tool
// @internalComponent
// @released
// 
//


#include "pl_elfexecutable.h"
#include "errorhandler.h"
#include <stdio.h>
#include "parameterlistinterface.h"
#include "pl_elfimportrelocation.h"
#include "pl_dllsymbol.h"
#include "messagehandler.h"
#include "pl_elflocalrelocation.h"


/**
Constructor for class ElfExecutable
@param aParameterListInterface - Instance of class ParameterListInterface
@internalComponent
@released
*/
ElfExecutable::ElfExecutable(ParameterListInterface *aParameterListInterface) :\
	iElfHeader(NULL), \
	iEntryPoint(0),\
	iProgHeader(NULL), \
	iSONameOffset(0) ,\
	iSections (NULL) , \
	iVersionDef (NULL) , iVerDefCount(0), \
	iVersionNeed (NULL) , iVerNeedCount(0), \
	iVersionTbl (NULL) ,iRelSize(0),iRelEntSize(0), \
	iNRelocs(0),
	iRel (NULL) ,iRelaSize(0), iRelaEntSize(0), \
	iRela(NULL), 
	iStringTable (NULL) , \
	iSectionHdrStrTbl(NULL), \
	iVerInfo(NULL),	iElfDynSym (NULL), \
	iSymTab (NULL), \
	iStrTab (NULL), \
	iLim (NULL), \
	iNSymbols(0), \
	iHashTbl (NULL) , \
	iDynSegmentHdr (NULL) , \
	iDataSegmentHdr (NULL) ,iDataSegment(NULL), iDataSegmentSize(0), iDataSegmentIdx(0), \
	iCodeSegmentHdr (NULL) , iCodeSegment(NULL), iCodeSegmentSize(0), iCodeSegmentIdx(0), \
	iExports (NULL), \
	iParameterListInterface(aParameterListInterface),\
	iPltGotBase(0), iPltGotLimit(0), iStrTabSz(0), iSymEntSz(0), \
	iPltGot(NULL), iPltRel(NULL),iPltRelaSz(0), iPltRela(NULL), iPltRelSz(0)  \

{
}


/**
Destructor for class ElfExecutable
@internalComponent
@released
*/
ElfExecutable::~ElfExecutable()
{
	delete iExports;
	delete [] iVerInfo;
	/* 
	all of these were getting deleted, they are not allocated by 
	ElfExecutable, they simply refer to a linear array of images 
	in an ElfImage, hence they shouldn't be de-allocated 
	
	delete iRela;
	delete iPltRel;
	delete iPltRela; */ 

	iNeeded.clear();
	iSymbolTable.clear();
}


/**
Function to process Elf file
@param aElfHdr - pointer to Elf header
@return 0 if its valid ELF file
@internalComponent
@released
*/
PLUINT32  ElfExecutable::ProcessElfFile(Elf32_Ehdr *aElfHdr) {

	iElfHeader = aElfHdr;
	iEntryPoint = aElfHdr->e_entry;

	ValidateElfFile();

	/* A valid ELF file so far..*/

	/* Get the Section base..*/
	if(iElfHeader->e_shnum) {
		iSections = ELF_ENTRY_PTR(Elf32_Shdr, iElfHeader, iElfHeader->e_shoff); 
	}

	/* Get the program header..*/
	if(iElfHeader->e_phnum) {
		iProgHeader = ELF_ENTRY_PTR(Elf32_Phdr, iElfHeader, iElfHeader->e_phoff);
	}

	/* Get the section-header-string table..*/
	if(iElfHeader->e_shstrndx != SHN_UNDEF) {

		if(iElfHeader->e_shstrndx > iElfHeader->e_shnum ) {
			throw ELFFormatError(ELFSHSTRINDEXERROR,iParameterListInterface->ElfInput());
		}

		iSectionHdrStrTbl = ELF_ENTRY_PTR(char, iElfHeader, iSections[iElfHeader->e_shstrndx].sh_offset);
	}

	if( iProgHeader ) {
		PLUINT32 aIdx = 0;

		while( aIdx < iElfHeader->e_phnum) {
			switch( iProgHeader[aIdx].p_type ) {
			case PT_DYNAMIC:
				{
					iDynSegmentHdr = &iProgHeader[aIdx];
				}
				break;
			case PT_LOAD:
				{
					if( (iProgHeader[aIdx].p_flags) & (PF_X | PF_ARM_ENTRY) ) {
						iCodeSegmentHdr = &iProgHeader[aIdx];
						iCodeSegmentIdx = aIdx;
						iCodeSegment = ELF_ENTRY_PTR(char, iElfHeader, iCodeSegmentHdr->p_offset);
						iCodeSegmentSize = iCodeSegmentHdr->p_filesz;
					}
					else if( (iProgHeader[aIdx].p_flags) & (PF_W | PF_R) ) {
						iDataSegmentHdr = &iProgHeader[aIdx];
						iDataSegmentIdx = aIdx;
						iDataSegment = ELF_ENTRY_PTR(char, iElfHeader, iDataSegmentHdr->p_offset);
						iDataSegmentSize = iDataSegmentHdr->p_filesz;
					}
				}
				break;
			default:
				break;

			}
			aIdx++;
		}

		if( iDynSegmentHdr ) {
			ProcessDynamicEntries();
		}

		ProcessSymbols();
		ProcessRelocations();
	}

	return 0;
}

/**
Function to Find the Static Symbol Table
@internalComponent
@released
*/
void ElfExecutable::FindStaticSymbolTable()
{
	size_t nShdrs = iElfHeader->e_shnum;

	if (nShdrs)
	{
		// Find the static symbol table and string table
		for (PLUINT32 i = 0; i < nShdrs; i++)
		{
			if (iSections[i].sh_type == SHT_SYMTAB)
			{
				iSymTab = ELF_ENTRY_PTR(Elf32_Sym, iElfHeader, iSections[i].sh_offset);
				iLim = ELF_ENTRY_PTR(Elf32_Sym, iSymTab, iSections[i].sh_size);
				if (iStrTab) break;
			}
			else if (iSections[i].sh_type == SHT_STRTAB)
			{
				char * aSectionName = iSectionHdrStrTbl + iSections[i].sh_name;
				if (!strcmp(aSectionName, ".strtab"))
				{
					iStrTab = ELF_ENTRY_PTR(char, iElfHeader, iSections[i].sh_offset);
					if (iSymTab) break;
				}
			}
		}
	}
}

/**
Function to Find the Comment Section
@return aComment - Pointer to Comment Section
@internalComponent
@released
*/
char* ElfExecutable::FindCommentSection()
{
	size_t nShdrs = iElfHeader->e_shnum;
	char *aCommentSection = ".comment";
	char *aComment;

	if (nShdrs)
	{
		// find the comment section
		for (PLUINT32 i = 0; i < nShdrs; i++)
		{
			if (iSections[i].sh_type == SHT_PROGBITS)
			{
				char * aSectionName = iSectionHdrStrTbl + iSections[i].sh_name;
				int length = strlen(aCommentSection);
				if (!strncmp(aSectionName, aCommentSection, length))
				{
					aComment = ELF_ENTRY_PTR(char, iElfHeader, iSections[i].sh_offset);
					return aComment;
				}
			}
		}
	}
	return NULL;
}

/**
Function to process the ARM to Thumb veneers
@internalComponent
@released
*/
void ElfExecutable::ProcessVeneers()
{
	if (iSymTab && iStrTab)
	{	
		ElfRelocations::RelocationList & iLocalCodeRelocs = GetCodeRelocations();

		Elf32_Sym *aSymTab = iSymTab;
		int length = strlen("$Ven$AT$L$$");

		// Process the symbol table to find Long ARM to Thumb Veneers
		// i.e. symbols of the form '$Ven$AT$L$$'
		for(; aSymTab < iLim; aSymTab++)
		{
			if (!aSymTab->st_name) continue;
			char * aSymName = iStrTab + aSymTab->st_name;
			Elf32_Sym	*aSym;

			if (!strncmp(aSymName, "$Ven$AT$L$$", length))
			{
				aSym = aSymTab;
				Elf32_Addr r_offset = aSym->st_value;
				Elf32_Addr aOffset = r_offset + 4;
				Elf32_Word	aInstruction = FindValueAtLoc(r_offset);
				bool aRelocEntryFound = false;

				ElfRelocations::RelocationList::iterator r;
				for (r = iLocalCodeRelocs.begin(); r != iLocalCodeRelocs.end(); r++)
				{
					ElfLocalRelocation * aReloc = *r;
					// Check if there is a relocation entry for the veneer symbol
					if (aReloc->iAddr == aOffset)
					{
						aRelocEntryFound = true;
						break;
					}
				}

				Elf32_Word aPointer = FindValueAtLoc(aOffset);

				/* If the symbol addresses a Thumb instruction, its value is the
				 * address of the instruction with bit zero set (in a
				 * relocatable object, the section offset with bit zero set).
				 * This allows a linker to distinguish ARM and Thumb code symbols 
				 * without having to refer to the map. An ARM symbol will always have 
				 * an even value, while a Thumb symbol will always have an odd value.
				 * Reference: Section 4.5.3 in Elf for the ARM Architecture Doc
				 * aIsThumbSymbol will be 1 for a thumb symbol and 0 for ARM symbol
				 */
				int aIsThumbSymbol = aPointer & 0x1;

				/* The relocation entry should be generated for the veneer only if
				 * the following three conditions are satisfied:
				 * 1) Check if the instruction at the symbol is as expected 
				 *    i.e. has the bit pattern 0xe51ff004 == 'LDR pc,[pc,#-4]'
				 * 2) There is no relocation entry generated for the veneer symbol
				 * 3) The instruction in the location provided by the pointer is a thumb symbol
				 */
				if (aInstruction == 0xE51FF004 && !aRelocEntryFound && aIsThumbSymbol)
				{
					ElfLocalRelocation	*aRel;
					PLUCHAR	aType = R_ARM_NONE;

					aRel = new ElfLocalRelocation(this, aOffset, 0, 0, aType, NULL, ESegmentRO, aSym, false, true);
					if(aRel) 
					{
						aRel->Add();
					}
				}
			}
		}
	}
}

/**
Function to find the content of the address passed in
@param aOffset - Address
@return aLocVal - The content of the address, like instruction or a pointer
@internalComponent
@released
*/
Elf32_Word	ElfExecutable::FindValueAtLoc(Elf32_Addr aOffset)
{
	Elf32_Phdr  *aHdr = Segment(aOffset);
	PLUINT32 aLoc = aHdr->p_offset + aOffset - aHdr->p_vaddr;
	Elf32_Word	*aLocVal = ELF_ENTRY_PTR(Elf32_Word, iElfHeader, aLoc);
	return *aLocVal;
}

/**
Function to process Elf symbols
@internalComponent
@released
*/
PLUINT32  ElfExecutable::ProcessSymbols(){
	PLUINT32	aSymIdx = 0;
	DllSymbol	*aSymbol;
	char		*aDllName;
	char		*aSymName, *aNewSymName;
	SymbolType	aType;
	
	while( aSymIdx < iNSymbols ) {
		
		aSymName = ELF_ENTRY_PTR(char, iStringTable, iElfDynSym[aSymIdx].st_name );

		if( ExportedSymbol( &iElfDynSym[aSymIdx] ) ){

			if( FunctionSymbol( &iElfDynSym[aSymIdx] ))
				aType = SymbolTypeCode;
			else
				aType = SymbolTypeData;

			aSymName = ELF_ENTRY_PTR(char, iStringTable, iElfDynSym[aSymIdx].st_name );
			aDllName = iVerInfo[iVersionTbl[aSymIdx]].iLinkAs;
			aNewSymName = new char[strlen(aSymName)+1];
			strcpy(aNewSymName, aSymName);
			aSymbol = new DllSymbol( aNewSymName, aType, &iElfDynSym[aSymIdx], aSymIdx);
			aSymbol->SetSymbolSize(iElfDynSym[aSymIdx].st_size);
			
			//Putting the symbols into a hash table - Used later while processing relocations
			iSymbolTable[aSymIdx] = aSymbol ;
			if( !AddToExports( aDllName, aSymbol ))
			{
				//Not a valid export... delete it..
				delete aSymbol;
			}
		}
		else if( ImportedSymbol( &iElfDynSym[aSymIdx] ) ){

			if( FunctionSymbol( &iElfDynSym[aSymIdx] ))
				aType = SymbolTypeCode;
			else
				aType = SymbolTypeData;

			aSymName = ELF_ENTRY_PTR(char, iStringTable, iElfDynSym[aSymIdx].st_name );

			/*
			 * All imported symbols must be informed via the version needed information.
			 */
			if( iVerInfo[iVersionTbl[aSymIdx]].iVerCategory != VER_CAT_NEEDED ) {
				throw UndefinedSymbolError(UNDEFINEDSYMBOLERROR, iParameterListInterface->ElfInput(), aSymName);
			}
			aDllName = iVerInfo[iVersionTbl[aSymIdx]].iLinkAs;
			//aSymbol = new DllSymbol( aSymName, aType, &iElfDynSym[aSymIdx], aSymIdx);
			
			//Putting the symbols into a hash table
			//iSymbolTable[aSymIdx] = aSymbol ;
		}
		aSymIdx++;
	}

	return 0;
}

/**
This function Dump all the sections with their section details (i.e., the section name, type,
size and linked section if any)
@param aFile - ELF file name
@internalComponent
@released
*/
void ElfExecutable::DumpElfFile(char* aFile){
	aFile = aFile;
}


/**
This function adds exports into the export list
@param aDll - Dll name
@param aSymbol - Symbol
@return
@internalComponent
@released
*/
DllSymbol* ElfExecutable::AddToExports(char* aDll, DllSymbol* aSymbol){
	if( !iExports ) {
		iExports = new ElfExports();
	}
	return iExports->Add( aDll, this, aSymbol );
}


/**
This function adds imports into the map
@param aReloc - Instance of class ElfImportRelocation
@internalComponent
@released
*/
void  ElfExecutable::AddToImports(ElfImportRelocation* aReloc){
	SetVersionRecord(aReloc);
	//char *aDll = iVerInfo[iVersionTbl[aReloc->iSymNdx]].iLinkAs;
	char *aDll = aReloc->iVerRecord->iLinkAs;
	iImports.Add( (const char*)aDll, aReloc );
		
}

/**
This function adds local relocation into a list
@param aReloc - Instance of class ElfImportRelocation
@internalComponent
@released
*/
void ElfExecutable::AddToLocalRelocations(ElfRelocation* aReloc) {
	iLocalRelocations.Add((ElfLocalRelocation*)aReloc);
}

/**
This function records the version of an imported symbol
@param aReloc - Instance of class ElfImportRelocation
@internalComponent
@released
*/
void ElfExecutable::SetVersionRecord( ElfRelocation* aReloc ) {
	if( !aReloc )
		return;
	((ElfImportRelocation*)aReloc)->iVerRecord = &iVerInfo[ iVersionTbl[aReloc->iSymNdx]];
}

/**
This function validates the ELF file
@internalComponent
@released
*/
PLUINT32  ElfExecutable::ValidateElfFile() {

	/*Check if the ELF-Magic is correct*/
	if(!(iElfHeader->e_ident[EI_MAG0] == ELFMAG0) &&
		(iElfHeader->e_ident[EI_MAG1] == ELFMAG1) &&
		(iElfHeader->e_ident[EI_MAG2] == ELFMAG2) &&
		(iElfHeader->e_ident[EI_MAG3] == ELFMAG3) ) {
			throw ELFFormatError(ELFMAGICERROR, iParameterListInterface->ElfInput());
	}

	/*32-bit ELF file*/
	if(iElfHeader->e_ident[EI_CLASS] != ELFCLASS32) {
		throw ELFFormatError(ELFCLASSERROR, iParameterListInterface->ElfInput());
	}

	/* Check if the ELF file is in Little endian format*/
	if(iElfHeader->e_ident[EI_DATA] != ELFDATA2LSB) {
		throw ELFFormatError(ELFLEERROR, iParameterListInterface->ElfInput());
	}

	/* The ELF executable must be a DLL or an EXE*/
	if( iElfHeader->e_type != ET_EXEC && iElfHeader->e_type != ET_DYN) {
		throw ELFFormatError(ELFEXECUTABLEERROR, iParameterListInterface->ElfInput());
	}

	return 0;
}


/**
This function processes the dynamic table.
@internalComponent
@released
*/
PLUINT32  ElfExecutable::ProcessDynamicEntries(){

	PLUINT32 aIdx = 0;
	bool aSONameFound = false;
	bool aPltRelTypeSeen = false, aJmpRelSeen = false;
	list<PLUINT32>	aNeeded;
	Elf32_Dyn *aDyn = ELF_ENTRY_PTR(Elf32_Dyn, iElfHeader, iDynSegmentHdr->p_offset);

	while( aDyn[aIdx].d_tag != DT_NULL ) {
		switch (aDyn[aIdx].d_tag) {
		case DT_NEEDED:
			aNeeded.push_back( aDyn[aIdx].d_val );
			break;
		case DT_HASH:
			iHashTbl = ELF_ENTRY_PTR(Elf32_HashTable, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_STRTAB:
			iStringTable = ELF_ENTRY_PTR(char, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_SYMTAB:
			iElfDynSym = ELF_ENTRY_PTR(Elf32_Sym, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_RELA:
			iRela = ELF_ENTRY_PTR(Elf32_Rela, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_RELASZ:
			iRelaSize = aDyn[aIdx].d_val;
			break;
		case DT_RELAENT:
			iRelaEntSize = aDyn[aIdx].d_val;
			break;
		case DT_SONAME:
			aSONameFound = true;
			iSONameOffset = aDyn[aIdx].d_val;
			break;
		case DT_REL:
			iRel = ELF_ENTRY_PTR(Elf32_Rel, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_RELSZ:
			iRelSize = aDyn[aIdx].d_val;
			break;
		case DT_RELENT:
			iRelEntSize = aDyn[aIdx].d_val;
			break;
		case DT_VERSYM:
			iVersionTbl = ELF_ENTRY_PTR(Elf32_Half, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_VERDEF:
			iVersionDef = ELF_ENTRY_PTR(Elf32_Verdef, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_VERDEFNUM:
			iVerDefCount = aDyn[aIdx].d_val;
			break;
		case DT_VERNEED:
			iVersionNeed = ELF_ENTRY_PTR(Elf32_Verneed, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_VERNEEDNUM:
			iVerNeedCount = aDyn[aIdx].d_val;
			break;
		case DT_STRSZ:
			iStrTabSz = aDyn[aIdx].d_val;
			break;
		case DT_SYMENT:
			iSymEntSz = aDyn[aIdx].d_val;
			break;
		case DT_PLTRELSZ:
			iPltRelSz = aDyn[aIdx].d_val;
			break;
		case DT_PLTGOT:
			iPltGot = ELF_ENTRY_PTR(Elf32_Word, iElfHeader, aDyn[aIdx].d_val);
			break;
		case DT_RPATH:
			break;
		case DT_SYMBOLIC:
			break;
		case DT_INIT:
			break;
		case DT_FINI:
			break;
		case DT_PLTREL:
			aPltRelTypeSeen = true;
			iPltRelType = aDyn[aIdx].d_val;
			break;
		case DT_DEBUG:
			break;
		case DT_TEXTREL:
			break;
		case DT_JMPREL:
			aJmpRelSeen = true;
			iJmpRelOffset = aDyn[aIdx].d_val;
			break;
		case DT_BIND_NOW:
			break;
		case DT_INIT_ARRAY:
			break;
		case DT_FINI_ARRAY:
			break;
		case DT_INIT_ARRAYSZ:
			break;
		case DT_FINI_ARRAYSZ:
			break;
		case DT_RELCOUNT:
			break;
		case DT_ARM_PLTGOTBASE:
			iPltGotBase = aDyn[aIdx].d_val;
			break;
		case DT_ARM_PLTGOTLIMIT:
			iPltGotLimit = aDyn[aIdx].d_val;
			break;
		case DT_ARM_SYMTABSZ:
			iNSymbols = aDyn[aIdx].d_val;
			break;
		default:
			//cout << "Unknown entry in dynamic table Tag=0x%x Value=0x%x",aDyn[aIdx].d_tag, aDyn[aIdx].d_val);
			break;
		}
		aIdx++;
	}

	//String table is found, so get the strings...
	if(aSONameFound) {
		iSOName = ELF_ENTRY_PTR(char, iStringTable, iSONameOffset);
	}

	std::list<PLUINT32>::iterator aItr = aNeeded.begin();
	char *aStr;
	for( ; aItr != aNeeded.end();aItr++ ) {
		aStr = ELF_ENTRY_PTR(char, iStringTable, *aItr);
		iNeeded.push_back( aStr );
	}

	if(iVerNeedCount || iVerDefCount) {
		ProcessVerInfo();
	}

	if(iHashTbl)
	{
		//The number of symbols should be same as the number of chains in hashtable
		if (iNSymbols && (iNSymbols != iHashTbl->nChains))
			throw ELFFormatError(SYMBOLCOUNTMISMATCHERROR,(char*)iParameterListInterface->ElfInput());
		else
		//The number of symbols is same as the number of chains in hashtable	
			iNSymbols = iHashTbl->nChains;
	}

	if( aPltRelTypeSeen  && aJmpRelSeen) {

		if (iPltRelType == DT_REL)
		{
			iPltRel = ELF_ENTRY_PTR(Elf32_Rel, iElfHeader, iJmpRelOffset);
			// check to see if PltRels are included in iRel. If they are
			// ignore them since we don't care about the distinction
			if (iRel <= iPltRel && iPltRel < ELF_ENTRY_PTR(Elf32_Rel, iRel, iRelSize))
				iPltRel = 0;
		}
		else
		{
			iPltRela = ELF_ENTRY_PTR(Elf32_Rela, iElfHeader, iJmpRelOffset);
			// check to see if PltRels are included in iRel.  If they are
			// ignore them since we don't care about the distinction
			if (iRela <= iPltRela && iPltRela < ELF_ENTRY_PTR(Elf32_Rela, iRela, iRelaSize))
				iPltRela = 0;
		}
	}

	return 0;
}

/**
This function processes version information
@internalComponent
@released
*/
void ElfExecutable::ProcessVerInfo() {
	PLUINT32 aSz = iVerNeedCount + iVerDefCount + 1;
	iVerInfo = new VersionInfo[aSz];

	Elf32_Verdef	*aDef;
	Elf32_Verdaux	*aDaux;
	Elf32_Verneed	*aNeed;
	Elf32_Vernaux	*aNaux;
	char			*aSoName;
	char			*aLinkAs;
	
	aDef = iVersionDef;
	
	while( aDef ) {
		aDaux = ELF_ENTRY_PTR( Elf32_Verdaux, aDef, aDef->vd_aux);
		aLinkAs = ELF_ENTRY_PTR(char, iStringTable, aDaux->vda_name );
		aSoName = iSOName;
		iVerInfo[aDef->vd_ndx].iLinkAs = aLinkAs;
		iVerInfo[aDef->vd_ndx].iSOName = aSoName;
		iVerInfo[aDef->vd_ndx].iVerCategory = VER_CAT_DEFINED;

		if( !aDef->vd_next ) {
			break;
		}
		aDef = ELF_ENTRY_PTR(Elf32_Verdef, aDef, aDef->vd_next);
	}

	aNeed = iVersionNeed;

	while( aNeed ) {
		aNaux = ELF_ENTRY_PTR(Elf32_Vernaux, aNeed, aNeed->vn_aux);
		aLinkAs = ELF_ENTRY_PTR(char, iStringTable, aNaux->vna_name);
		aSoName = ELF_ENTRY_PTR(char, iStringTable, aNeed->vn_file);

		iVerInfo[aNaux->vna_other].iLinkAs = aLinkAs;
		iVerInfo[aNaux->vna_other].iSOName = aSoName;
		iVerInfo[aNaux->vna_other].iVerCategory = VER_CAT_NEEDED;

		if( !aNeed->vn_next ) {
			break;
		}
		aNeed = ELF_ENTRY_PTR(Elf32_Verneed, aNeed, aNeed->vn_next);
	}
}

/**
This function processes Elf relocations
@internalComponent
@released
*/
void ElfExecutable::ProcessRelocations(){
	ProcessRelocations(iRel, iRelSize);
	ProcessRelocations(iRela, iRelaSize);
	ProcessRelocations(iPltRel, iPltRelSz);
	ProcessRelocations(iPltRela, iPltRelaSz);
}

/**
Template Function to process relocations
@param aElfRel - relocation table
@param aSize - relocation table size
@internalComponent
@released
*/
template <class T> 
void ElfExecutable::ProcessRelocations(T *aElfRel, size_t aSize){
	if( !aElfRel ) 
		return;

	T * aElfRelLimit = ELF_ENTRY_PTR(T, aElfRel, aSize);

	PLUINT32		aSymIdx;
	PLUCHAR			aType;
	ElfRelocation	*aRel;
	bool			aImported;
	Elf32_Word		aAddend;

	while( aElfRel < aElfRelLimit) {
		
		aType = ELF32_R_TYPE(aElfRel->r_info );
		
		if(ElfRelocation::ValidRelocEntry(aType)) {

			aSymIdx = ELF32_R_SYM(aElfRel->r_info);
			aImported = ImportedSymbol( &iElfDynSym[aSymIdx] );
			aAddend = Addend(aElfRel);
			aRel = ElfRelocation::NewRelocEntry(this, aElfRel->r_offset, aAddend, \
				aSymIdx, aType, aElfRel, aImported);

			if(aRel) {
				aRel->Add();
			}
		}
		aElfRel++;
	}
}

/**
This function finds the addend associated with a relocation entry.
@param aRel - relocation entry
@return location in the elf image
@internalComponent
@released
*/
Elf32_Word ElfExecutable::Addend(Elf32_Rel* aRel) {
	PLUINT32 aOffset;
	Elf32_Word	*aAddendPlace;
	Elf32_Phdr  *aHdr = Segment(aRel->r_offset);
	aOffset = aHdr->p_offset + aRel->r_offset - aHdr->p_vaddr;
	aAddendPlace = ELF_ENTRY_PTR(Elf32_Word, iElfHeader, aOffset);
	return *aAddendPlace;
}

/**
This function returns the addend for a relocation entry
@param aRel - relocation entry
@return location in the elf image
@internalComponent
@released
*/
Elf32_Word ElfExecutable::Addend(Elf32_Rela* aRel) {
	return aRel->r_addend;
}

/**
This function gets the version info at an index
@param aIndex - index into the version table
@return version record
@internalComponent
@released
*/
VersionInfo* ElfExecutable::GetVersionInfo(PLUINT32  aIndex){
	return &iVerInfo[ iVersionTbl[aIndex]];
}


/**
This function returns the Dll name in which an imported symbol is 
defined by looking in the version required section.
@param aSymbolIndex - Index of symbol
@return Dll name
@internalComponent
@released
*/
char* ElfExecutable::SymbolDefinedInDll(PLUINT32  aSymbolIndex){

	VersionInfo *aVInfo = GetVersionInfo(aSymbolIndex);
	return aVInfo ? aVInfo->iLinkAs : NULL;
}

/**
This function returns the DSO(import library) name where the Symbol information can be found.
This DSO is then looked up for the ordinal number of this symbol.
@param aSymbolIndex - Index of symbol
@return DSO name
@internalComponent
@released
*/
char* ElfExecutable::SymbolFromDSO(PLUINT32  aSymbolIndex){

	VersionInfo *aVInfo = GetVersionInfo(aSymbolIndex);
	return aVInfo ? aVInfo->iSOName : NULL;
}

/**
This function returns the segment type
@param aAddr - Address
@return Segment type
@internalComponent
@released
*/
ESegmentType ElfExecutable::SegmentType(Elf32_Addr aAddr) {
	
	try {
		Elf32_Phdr *aHdr = Segment(aAddr);
		if( !aHdr )
			return ESegmentUndefined;

		if( aHdr == iCodeSegmentHdr)
			return ESegmentRO;
		else if(aHdr == iDataSegmentHdr)
			return ESegmentRW;
		else
			return ESegmentUndefined;
	}
	catch(...)
	{
	}

	return ESegmentUndefined;
}

/**
This function returns the segment type
@param aType
@return Segment header
@internalComponent
@released
*/
Elf32_Phdr* ElfExecutable::Segment(ESegmentType aType) {
	
	switch(aType)
	{
	case ESegmentRO:
		return iCodeSegmentHdr;
	case ESegmentRW:
		return iDataSegmentHdr;
	default:
		return NULL;
	}
}

/**
Function to get segment header
@param aAddr - Address
@return Segment header
@internalComponent
@released
*/
Elf32_Phdr* ElfExecutable::Segment(Elf32_Addr aAddr) {
	
	if(iCodeSegmentHdr) {
		PLUINT32 aBase = iCodeSegmentHdr->p_vaddr;
		if( aBase <= aAddr && aAddr < (aBase + iCodeSegmentHdr->p_memsz) ) {
			return iCodeSegmentHdr;
		}
	}
	if(iDataSegmentHdr) {
		PLUINT32 aBase = iDataSegmentHdr->p_vaddr;
		if( aBase <= aAddr && aAddr < (aBase + iDataSegmentHdr->p_memsz) ) {
			return iDataSegmentHdr;
		}
	}

	throw int(0);
}

/**
Thsi function returns the segment header to which the address refers.
@param aAddr - location
@return Segment header.
@internalComponent
@released
*/
Elf32_Phdr* ElfExecutable::SegmentFromAbs(Elf32_Addr aAddr) {

	if(iCodeSegmentHdr) {
		PLUINT32 aBase = iCodeSegmentHdr->p_vaddr;
		if( aBase <= aAddr && aAddr <= (aBase + iCodeSegmentHdr->p_memsz) ) {
			return iCodeSegmentHdr;
		}
	}
	if(iDataSegmentHdr) {
		PLUINT32 aBase = iDataSegmentHdr->p_vaddr;
		if( aBase <= aAddr && aAddr <= (aBase + iDataSegmentHdr->p_memsz) ) {
			return iDataSegmentHdr;
		}
	}
	return NULL;
}

/**
This function says if the symbol is global.
@param aSym - Symbol
@return True if symbol is global, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::GlobalSymbol(Elf32_Sym* aSym)
{
	return (ELF32_ST_BIND(aSym->st_info) == STB_GLOBAL);
}

/**
This function says if the symbol is exported.
@param aSym - Symbol
@return True if symbol is exported, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::ExportedSymbol(Elf32_Sym* aSym)
{
	PLUINT32 aIdx = aSym->st_shndx;

	if(GlobalSymbol(aSym) && VisibleSymbol(aSym) && DefinedSymbol(aSym) && \
		(aIdx != SHN_UNDEF) && (FunctionSymbol(aSym) || DataSymbol(aSym) ) && aIdx < SHN_ABS )
		return true;
	return false;
}

/**
This function says if the symbol is imported.
@param aSym - Symbol
@return True if symbol is imported, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::ImportedSymbol(Elf32_Sym* aSym)
{
	PLUINT32 aIdx = aSym->st_shndx;

	if( (aIdx == SHN_UNDEF) && GlobalSymbol(aSym) && VisibleSymbol(aSym) && (!DefinedSymbol(aSym)) )
		return true;
	return false;
}

/**
This function says if the symbol refers to code or data.
@param aSym - Symbol
@return True if symbol refers to code, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::FunctionSymbol(Elf32_Sym* aSym)
{
	return (STT_FUNC == ELF32_ST_TYPE(aSym->st_info));
}

/**
This function says if the symbol refers to code or data.
@param aSym - Symbol
@return True if symbol refers to data, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::DataSymbol(Elf32_Sym* aSym)
{
	return (STT_OBJECT == ELF32_ST_TYPE(aSym->st_info));
}

/**
This function says if the symbol is defined in the Elf executable.
@param aSym - Symbol
@return True if symbol is defined, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::DefinedSymbol(Elf32_Sym* aSym)
{
	if( aSym->st_shndx == SHN_UNDEF )
		return false;
	ESegmentType aType = SegmentType(aSym->st_value);
	return ((aType == ESegmentRO) || (aType == ESegmentRW));
}

/**
This function says if the visibility of the symbol is default.
@param aSym - Symbol
@return True if symbol has default visibility, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::VisibleSymbol(Elf32_Sym* aSym)
{
	return (STV_DEFAULT == ELF32_ST_VISIBILITY(aSym->st_other) || STV_PROTECTED == ELF32_ST_VISIBILITY(aSym->st_other));
}

/**
This function finds symbol using the hash table
@param aName - Symbol name
@return elf symbol.
@internalComponent
@released
*/
Elf32_Sym* ElfExecutable::FindSymbol(char* aName) {
	if(!aName )
		return NULL;

	PLULONG aHashVal = Util::elf_hash((const PLUCHAR*) aName );
	
	Elf32_Sword* aBuckets = ELF_ENTRY_PTR(Elf32_Sword, iHashTbl, sizeof(Elf32_HashTable) );
	Elf32_Sword* aChains = ELF_ENTRY_PTR(Elf32_Sword, aBuckets, sizeof(Elf32_Sword)*(iHashTbl->nBuckets) );

	Elf32_Sword aIdx = aHashVal % iHashTbl->nBuckets;
	aIdx = aBuckets[aIdx];
	
	char	*aSymName;
	do {
		aSymName = ELF_ENTRY_PTR(char, iStringTable, iElfDynSym[aIdx].st_name);
		if( !strcmp(aSymName, aName) ) {
			return &iElfDynSym[aIdx];
		}
		aIdx = aChains[aIdx];
	}while( aIdx > 0 );

	return NULL;
}

/**
Function to get symbol name
@param aSymIdx - Index of symbol
@return Symbol name
@internalComponent
@released
*/
char* ElfExecutable::GetSymbolName( PLUINT32 aSymIdx) {
	return ELF_ENTRY_PTR(char, iStringTable, iElfDynSym[aSymIdx].st_name);
}

/**
Function to get symbol ordinal
@param aSymName - Symbol name
@return Symbol ordinal
@internalComponent
@released
*/
PLUINT32 ElfExecutable::GetSymbolOrdinal( char* aSymName) {
	Elf32_Sym	*aSym = FindSymbol(aSymName);
	if( !aSym )
		return (PLUINT32)-1;
	return GetSymbolOrdinal( aSym );
	
}

/**
Function to get symbol ordinal
@param aSym - Symbol
@return Symbol ordinal
@internalComponent
@released
*/
PLUINT32 ElfExecutable::GetSymbolOrdinal( Elf32_Sym* aSym) {
	PLUINT32 aOrd = (PLUINT32)-1;
	if( aSym->st_shndx == ESegmentRO) {
		Elf32_Word *aLocation, aOffset;

		aOffset = iCodeSegmentHdr->p_offset + aSym->st_value - iCodeSegmentHdr->p_vaddr;
		aLocation = ELF_ENTRY_PTR(Elf32_Word, iElfHeader, aOffset);
		aOrd = *aLocation;
	}
	return aOrd;
}

/**
Function to get relocation offset
@param aReloc - Instance of class ElfRelocation
@return offset
@internalComponent
@released
*/
Elf32_Word ElfExecutable::GetRelocationOffset(ElfRelocation * aReloc)
{
	Elf32_Phdr * aHdr = Segment(aReloc->iAddr);
	unsigned int aOffset = aReloc->iAddr - aHdr->p_vaddr;
	return aOffset;
}

/**
Function to get relocation place address
@param aReloc - Instance of class ElfRelocation
@return address to place relocation
@internalComponent
@released
*/
Elf32_Word * ElfExecutable::GetRelocationPlace(ElfRelocation * aReloc)
{
	Elf32_Phdr * aHdr = Segment(aReloc->iAddr);
	unsigned int aOffset = aHdr->p_offset + aReloc->iAddr - aHdr->p_vaddr;
	Elf32_Word * aPlace = ELF_ENTRY_PTR(Elf32_Word, iElfHeader, aOffset);
	return aPlace;
}

/**
Function to get local relocation
@return local relocation
@internalComponent
@released
*/
ElfRelocations& ElfExecutable::GetLocalRelocations()
{
	return iLocalRelocations;
}

/**
Function to get code relocation
@return code relocation list
@internalComponent
@released
*/
ElfRelocations::RelocationList & ElfExecutable::GetCodeRelocations()
{
	return GetLocalRelocations().GetCodeRelocations();
}

/**
Function to get data relocation
@return data relocation list
@internalComponent
@released
*/
ElfRelocations::RelocationList & ElfExecutable::GetDataRelocations()
{
	return GetLocalRelocations().GetDataRelocations();
}

/**
Function to get RO base address
@return RO base virtual address
@internalComponent
@released
*/
Elf32_Word ElfExecutable::GetROBase()
{
	if (iCodeSegmentHdr) return iCodeSegmentHdr->p_vaddr;
	return 0;
}

/**
Function to get RO segment
@return code segment
@internalComponent
@released
*/
MemAddr ElfExecutable::GetRawROSegment()
{
	return iCodeSegment;
}

/**
Function to get RW segment virtual address
@return RW base address
@internalComponent
@released
*/
Elf32_Word ElfExecutable::GetRWBase()
{
	if (iDataSegmentHdr) return iDataSegmentHdr->p_vaddr;
	return 0;
}

/**
Function to get Raw RW segment
@return data segment address
@internalComponent
@released
*/
MemAddr ElfExecutable::GetRawRWSegment()
{
	return iDataSegment;
}

/**
Function to get RO segment size
@return code segment size
@internalComponent
@released
*/
size_t ElfExecutable::GetROSize()
{
	return iCodeSegmentHdr->p_filesz;
}

/**
Function to get RW segment size
@return data segment size
@internalComponent
@released
*/
size_t ElfExecutable::GetRWSize()
{
	if (iDataSegmentHdr)
		return iDataSegmentHdr->p_filesz;;
	return 0;
}

/**
Function to get Bss segment size
@return Bss segment size, if data segment, otherwise 0
@internalComponent
@released
*/
size_t ElfExecutable::GetBssSize()
{
	if (iDataSegmentHdr)
		return iDataSegmentHdr->p_memsz - iDataSegmentHdr->p_filesz;
	return 0;
}

/**
Function returns entry point location in Elf image.
@return entry point offset if valid, warning if undefined, otherwise throw error
@internalComponent
@released
*/
Elf32_Word ElfExecutable::EntryPointOffset()
{
	if (!(iElfHeader->e_entry) && !(iCodeSegmentHdr->p_vaddr))
	{
		MessageHandler::GetInstance()->ReportMessage(WARNING, UNDEFINEDENTRYPOINTERROR,(char*)iParameterListInterface->ElfInput());
		return 0;
	}
	else if (!(iElfHeader->e_entry))
		throw ELFFormatError(ENTRYPOINTNOTSETERROR, (char*)iParameterListInterface->ElfInput());
	else 
		return iElfHeader->e_entry - iCodeSegmentHdr->p_vaddr;
}

/**
Function to check exception is present in the Elf image.
@return True if exception present, otherwise false
@internalComponent
@released
*/
bool ElfExecutable::ExeceptionsPresentP()
{	
	size_t nShdrs = iElfHeader->e_shnum;
	if (nShdrs)
	{
		// Find the exception index table section
		Elf32_Shdr * aShdr = ELF_ENTRY_PTR(Elf32_Shdr, iElfHeader, iElfHeader->e_shoff);
		char * aShStrTab = ELF_ENTRY_PTR(char, iElfHeader, aShdr[iElfHeader->e_shstrndx].sh_offset);

		for (PLUINT32 i = 0; i < nShdrs; i++)
		{
			if (aShdr[i].sh_type == SHT_ARM_EXIDX)
			{
				char * aSectionName = aShStrTab + aShdr[i].sh_name;
				if (!strcmp(aSectionName, ".ARM.exidx"))
				{
					return true;
				}
			}
		}

	}
	else
		throw ELFFileError(NEEDSECTIONVIEWERROR, (char*)iParameterListInterface->ElfInput());
	
	return false;
}

/**
Function to get the exports in ordinal number order.
@return ordered exports
@internalComponent
@released
*/
ElfExports::ExportList &ElfExecutable::GetExportsInOrdinalOrder() {
	return iExports->GetExportsInOrdinalOrder();
}

/**
This function looks up for a symbol in the static symbol table.
@return Elf symbol.
@internalComponent
@released
*/
Elf32_Sym * ElfExecutable::LookupStaticSymbol(char * aName) {
	size_t nShdrs = iElfHeader->e_shnum;
	if (nShdrs)
	{
		// find the static symbol table and string table
		Elf32_Shdr * aShdr = ELF_ENTRY_PTR(Elf32_Shdr, iElfHeader, iElfHeader->e_shoff);
		char * aShStrTab = ELF_ENTRY_PTR(char, iElfHeader, aShdr[iElfHeader->e_shstrndx].sh_offset);
		Elf32_Sym * aSymTab = 0;
		Elf32_Sym * aLim = 0;
		char * aStrTab = 0;
		for (PLUINT32 i = 0; i < nShdrs; i++)
		{
			if (aShdr[i].sh_type == SHT_SYMTAB)
			{
				aSymTab = ELF_ENTRY_PTR(Elf32_Sym, iElfHeader, aShdr[i].sh_offset);
				aLim = ELF_ENTRY_PTR(Elf32_Sym, aSymTab, aShdr[i].sh_size);
				if (aStrTab) break;
			}
			else if (aShdr[i].sh_type == SHT_STRTAB)
			{
				char * aSectionName = aShStrTab + aShdr[i].sh_name;
				if (!strcmp(aSectionName, ".strtab"))
				{
					aStrTab = ELF_ENTRY_PTR(char, iElfHeader, aShdr[i].sh_offset);
					if (aSymTab) break;
				}
			}
		}

		/*if(aHashTbl && aSymTab && aStrTab)
		{
			PLULONG aHashVal = Util::elf_hash((const PLUCHAR*)aName);
			Elf32_Sword* aBuckets = ELF_ENTRY_PTR(Elf32_Sword, aHashTbl, sizeof(Elf32_HashTable) );
			Elf32_Sword* aChains = ELF_ENTRY_PTR(Elf32_Sword, aBuckets, sizeof(Elf32_Sword)*(aHashTbl->nBuckets) );

			PLUINT32 aIdx = aHashVal % aHashTbl->nBuckets;
			aIdx = aBuckets[aIdx];
			
			char	*aSymName;
			do {
				aSymName = ELF_ENTRY_PTR(char, aStrTab, aSymTab[aIdx].st_name);
				if( !strcmp(aSymName, aName) ) {
					return &aSymTab[aIdx];
				}
				aIdx = aChains[aIdx];
			}while( aIdx > 0 );

			return NULL;
		}
		else */ 

		if (aSymTab && aStrTab)
		{	
			for(; aSymTab < aLim; aSymTab++)
			{
				if (!aSymTab->st_name) continue;
				char * aSymName = aStrTab + aSymTab->st_name;
				if (!strcmp(aSymName, aName))
					return aSymTab;
			}
			return 0;
		}
		else
		{
			throw ELFFileError(NOSTATICSYMBOLSERROR, (char*)iParameterListInterface->ElfInput());
		}
	}
	else
	{
			throw ELFFileError(NOSTATICSYMBOLSERROR, (char*)iParameterListInterface->ElfInput());
	}
}

/**
Function to get imports
@return imports
@internalComponent
@released
*/
ElfImports::ImportMap ElfExecutable::GetImports() {
	return iImports.GetImports();
}

/**
Function to get exports
@return exports
@internalComponent
@released
*/
ElfExports* ElfExecutable::GetExports() {
	return iExports;
}

/**
Function to get fixup location
@param aReloc - Instance of class ElfLocalRelocation
@param aPlace -
@return addres of position for relocation
@internalComponent
@released
*/
Elf32_Word* ElfExecutable::GetFixupLocation(ElfLocalRelocation* aReloc, Elf32_Addr aPlace)
{
	Elf32_Phdr * aPhdr = aReloc->ExportTableReloc() ? 
		iCodeSegmentHdr :
		Segment(aPlace);
	Elf32_Word offset = aPhdr->p_offset + aPlace - aPhdr->p_vaddr;
	return ELF_ENTRY_PTR(Elf32_Word, iElfHeader, offset);
}

/**
Function to get the segment type
@param aSym - Symbol
@return Segment type
@internalComponent
@released
*/
ESegmentType ElfExecutable::Segment(Elf32_Sym *aSym)
{
	Elf32_Phdr * aHdr;

	try {

		bool limitSymbolFound = false;

		// If Symbol is absolute then assume it came from linker and is a
		// limit symbol.
		if (aSym->st_shndx == SHN_ABS)
		{
			aHdr = SegmentFromAbs(aSym->st_value);
		}
		else
		{
			if( (iCodeSegmentHdr && aSym->st_value == (iCodeSegmentHdr->p_vaddr + iCodeSegmentHdr->p_memsz)) ||
				(iDataSegmentHdr && aSym->st_value == (iDataSegmentHdr->p_vaddr + iDataSegmentHdr->p_memsz)) )
			{
				//If Symbol is a $$Limit symbol, then consider the open boundary.
				String limitstr = iStringTable + aSym->st_name;
				if (limitstr.rfind("$$Limit",limitstr.length()) != String::npos)
				{
					aHdr = SegmentFromAbs(aSym->st_value);
					limitSymbolFound = true;
				}
			}

			if(!limitSymbolFound )
			{
				aHdr = Segment(aSym->st_value);
			}

		}
		
		if (aHdr == iCodeSegmentHdr)
		{
			return ESegmentRO;
		} 
		else if (aHdr == iDataSegmentHdr)
		{
			return ESegmentRW;
		}
	}
	catch(...)
	{
	}
	return ESegmentUndefined;
}

