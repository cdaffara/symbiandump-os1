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

#if !defined(_PL_ELFEXECUTABLE_H_)
#define _PL_ELFEXECUTABLE_H_

#include "pl_common.h"
#include <list>
#include <hash_map>
#include "pl_elfimports.h"
#include "pl_elfrelocations.h"
#include "pl_elfexports.h"

using std::list;
using std::hash_map;

class DllSymbol;
class Symbol;
class ElfRelocations;
class ElfExports;
class ParameterListInterface;
class ElfLocalRelocation;
/**
This class is for ELF object carrying the elf header, sections, segments.
@internalComponent
@released
*/
class ElfExecutable
{

public:
	ElfExecutable(ParameterListInterface *aParameterListInterface);
	virtual ~ElfExecutable();
	
	PLUINT32 ProcessElfFile(Elf32_Ehdr *aElfHdr);
	void DumpElfFile(char* aFile);

	PLUINT32 ProcessSymbols();
	ElfImports::ImportMap GetImports();
	ElfExports* GetExports();
	DllSymbol* AddToExports(char* aDll, DllSymbol* aSymbol);
	void AddToImports(ElfImportRelocation* aReloc);
	PLUINT32 AddToRelocations(ElfRelocation* aReloc);
	void AddToLocalRelocations(ElfRelocation* aReloc);
	void ProcessVerInfo();

	Elf32_Sym* FindSymbol(char* aSymName);

	PLUINT32 GetSymbolOrdinal( char* aSymName);
	PLUINT32 GetSymbolOrdinal( Elf32_Sym* );
	char* GetSymbolName( PLUINT32 aSymIdx);
	Elf32_Word GetRelocationOffset(ElfRelocation * aReloc);
	Elf32_Word *GetRelocationPlace(ElfRelocation * aReloc);

	Elf32_Word GetROBase();
	Elf32_Word GetRWBase();
	size_t GetROSize();
	size_t GetRWSize();
	size_t GetBssSize();
	MemAddr GetRawROSegment();
	MemAddr GetRawRWSegment();
	Elf32_Word EntryPointOffset();
	ESegmentType SegmentType(Elf32_Addr aAddr);
	Elf32_Phdr* Segment(Elf32_Addr aAddr);
	ElfRelocations::RelocationList & GetCodeRelocations();
	ElfRelocations::RelocationList & GetDataRelocations();
	ElfRelocations& GetLocalRelocations();
	bool ExeceptionsPresentP();
	ElfExports::ExportList& GetExportsInOrdinalOrder();
	Elf32_Sym* LookupStaticSymbol(char * aName);
public:
	/**
	 * The elf header pointer which points to the base of the file records
	 */
	Elf32_Ehdr	*iElfHeader;
	Elf32_Addr	iEntryPoint;
	/**
	 * The dynamic program header of the elf file
	 */
	Elf32_Phdr	*iProgHeader;

	PLUINT32	iSONameOffset;
	char		*iSOName;
	/**
	 * This member points to the base of the section header table.
	 */
	Elf32_Shdr		*iSections;
	Elf32_Verdef	*iVersionDef;
	PLUINT32		iVerDefCount;
	Elf32_Verneed	*iVersionNeed;
	PLUINT32		iVerNeedCount;
	Elf32_Half		*iVersionTbl;
	PLUINT32		iRelSize;
	PLUINT32		iRelEntSize;
	PLUINT32		iNRelocs;
	Elf32_Rel		*iRel;
	PLUINT32		iRelaSize;
	PLUINT32		iRelaEntSize;
	Elf32_Rela		*iRela;
	char			*iStringTable;
	char			*iSectionHdrStrTbl;

	list<char*>		iNeeded;
	VersionInfo		*iVerInfo;
	/**
	 * The dynamic symbol array.
	 */
	Elf32_Sym		*iElfDynSym;//The ELF symbol
	hash_map<PLUINT32, DllSymbol*>	iSymbolTable; //The representation

	/**
	 * The static symbol table.
	 */
	Elf32_Sym *iSymTab; 
	char *iStrTab;
	Elf32_Sym *iLim;

	PLUINT32		iNSymbols;
	Elf32_HashTable	*iHashTbl;
	Elf32_Phdr		*iDynSegmentHdr;
	Elf32_Phdr		*iDataSegmentHdr;
	MemAddr			iDataSegment;
	size_t			iDataSegmentSize;
	PLUINT32		iDataSegmentIdx;
	Elf32_Phdr		*iCodeSegmentHdr;
	MemAddr			iCodeSegment;
	size_t			iCodeSegmentSize;
	PLUINT32		iCodeSegmentIdx;
	ElfImports		iImports;
	ElfExports		*iExports;
	ElfRelocations  iLocalRelocations;
	ParameterListInterface *iParameterListInterface;
	PLUINT32		iPltGotBase;
	PLUINT32		iPltGotLimit;
	PLUINT32		iStrTabSz;
	PLUINT32		iSymEntSz;
	Elf32_Word		*iPltGot;
	PLUINT32		iPltRelType;
	Elf32_Rel		*iPltRel;
	PLUINT32		iPltRelaSz;
	Elf32_Rela		*iPltRela;
	PLUINT32		iPltRelSz;
	PLUINT32		iJmpRelOffset;

	PLUINT32	ValidateElfFile();
	PLUINT32	ProcessDynamicEntries();
	void	ProcessRelocations();
	template <class T> void	ProcessRelocations(T *aElfRel, size_t aSize);

	VersionInfo* GetVersionInfo(PLUINT32 aIndex);
	char*		SymbolDefinedInDll(PLUINT32 aSymbolIndex);
	void SetVersionRecord( ElfRelocation* aReloc );

/** This function says if the Symbol is a global symbol
@return It returns true if the Symbol is a global one.
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool GlobalSymbol(Elf32_Sym* aSym);

/** This function says if the Symbol is Exported
@return It returns true if the Symbol is an exported one.
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool ExportedSymbol(Elf32_Sym* aSym);

/** This function says if the Symbol is Imported
@return It returns true if the Symbol is an imported one.
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool ImportedSymbol(Elf32_Sym* aSym);

/** This function says if the Symbol is a Code Symbol
@return It returns true if the Symbol is of Code type.
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool FunctionSymbol(Elf32_Sym* aSym);

/** This function says if the Symbol is a Data Symbol
@return It returns true if the Symbol is of Data type.
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool DataSymbol(Elf32_Sym* aSym);

/** This function says if the Symbol is Defined
@return It returns true if the Symbol is defined.
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool DefinedSymbol(Elf32_Sym* aSym);

/** This function says if the Symbol has a default Visibility
@return It returns true if the Symbol has a default Visibility
@param aSym The reference to the Symbol whose attribute is being checked
*/
bool VisibleSymbol(Elf32_Sym* aSym);

/** This function finds the segment in which this address belongs
@return the segment type
@param aAddr The address within the executable
*/

Elf32_Word Addend(Elf32_Rel* aRel);
Elf32_Word Addend(Elf32_Rela* aRel);

char* SymbolFromDSO(PLUINT32  aSymbolIndex);
Elf32_Word* GetFixupLocation(ElfLocalRelocation* aReloc, Elf32_Addr aPlace);
ESegmentType Segment(Elf32_Sym *aSym);
Elf32_Phdr* SegmentFromAbs(Elf32_Addr aAddr);
Elf32_Phdr* Segment(ESegmentType aType);

/** This function processes the linker generated Veneer symbols and
 *  creates a relocation entry.
 */
void ProcessVeneers();
/** This function processes the ELF file to find the static symbol table.
*/
void FindStaticSymbolTable();
/** This function finds the .comment section
@return the pointer to the comment section
*/
char* FindCommentSection();
/** This function finds the value at the address passed in
@return the value at the address passed in
@param aAddr The address within the executable
*/
Elf32_Word FindValueAtLoc(Elf32_Addr aOffset);
};




#endif // !defined(_PL_ELFEXECUTABLE_H_)
