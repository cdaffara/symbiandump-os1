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

#if !defined(__ELFDLL_H__)
#define __ELFDLL_H__
//#include <e32rom.h>
#include "e32ldfmt.h"
#include <tools/elfdefs.h>
#include "elffile.h"

#define DLLSYMPREFIX "#<DLL>"
#define DLLSYMPREFIX0 "#"
#define DLLSYMSUFFIX "#<\\DLL>"
#define ORDBASE 16
#define EXPORTTABLENAME "DLL##ExportTable"
#define EXPORTTABLESIZENAME "DLL##ExportTableSize"

class NamedExportSymbol;
class OrdZeroRecord;
struct NeededDLLsList;

const TUint		KNameLookupOffsetFlag32 = 0x0001;

class ELFFile::ELFDllData
        {
public:
	ELFFile * iElfFile;

	char * iDynStrTab;
	// bytesize of the dynamic string table
	TInt iDynStrTabSize; 
	Elf32_Sym * iDynSymTab;
	// size of an entry in the dynamic symbol table
	TInt iSymSize;
	// the rela relocation table
	Elf32_Rela * iRela;
	// size of rela relocation table in bytes
	TInt iRelaSz;
	// size of a rela entry in bytes
	TInt iRelaEnt;
	// the rel relocation table
	Elf32_Rel * iRel;
	// size of rel relocation table in bytes
	TInt iRelSz;
	// size of a rel entry in bytes
	TInt iRelEnt;

	Elf32_HashTable * iHashTable;

	// Exported symbols required for lookup via names.
	NamedExportSymbol			*iNamedExportSymbolHead;
	// Number of exported symbols - this doesn't nclude the absent symbols.
	TInt					iNamedExportCount;
	// header of the export symbol info table
	E32EpocExpSymInfoHdr	iSymInfoHdr;
	// String table size for the symbol names.
	TUint					iSymStringTableSize;
	TUint					iStringNameOffset;
	
	// Dependency list
	// Static Dependencies - An 'import relocation' entry is required to be created 
	// for each such record.
	OrdZeroRecord		*iDepRecords;
	OrdZeroRecord		*iDepRecordsTail;

	// List of names obtained from DT_NEEDED entries in ELF.
	// The order in which they appear is the order in which symbol
	// lookup shall be followed.
	NeededDLLsList		*iNeededDllNames;
	NeededDLLsList		*iNeededDllNamesTail;
	// Ordinal 0 of this binary. - A local relocation needs to be created for this
	// record.

	OrdZeroRecord		*iOrdZeroRec;		
	TInt			iNamedLookupEnabled;
	
	ELFDllData(ELFFile * f);
	~ELFDllData();
    TBool Init();

	TBool ParseDllSymbol(Elf32_Sym * s, char*& dll, TUint& len, TInt& ord);
	class DllSymbol
		{
	public:
		char * iDll;
		TUint iLen;
		TInt iOrd;
		Elf32_Phdr * iSegment;
		Elf32_Rel * iRel;
		DllSymbol * iNext;

		DllSymbol(char * n, TUint l, TInt o) 
		        : iDll(n), iLen(l), iOrd(o), iSegment(0),iRel(0),iNext(0) 
			{}
		~DllSymbol()
			{ 
			delete iNext; 
			}
		};
	DllSymbol * DllSymbolP(Elf32_Sym * s);
	TBool AddSymbol(DllSymbol * s);

	class DllRec
		{
	public:
	    char * iName;
	    TUint iLen;
	    TInt nImports;
	    DllSymbol * iHead;
	    DllSymbol * iTail;
	    DllRec * iNext;

		DllRec(char * n, TInt l, DllSymbol * s) :
			iName(n), iLen(l), nImports(1), iHead(s), iTail(s), iNext(0)
		    {}
	    ~DllRec() 
		    { 
		    delete iHead; 
		    delete iNext; 
		    }
	    void AddSymbol(DllSymbol * s);
	    };

	DllRec * iDllHead;
	DllRec * iDllTail;

	Elf32_Word iExportTableSymIdx;
	Elf32_Word iExportTableSizeSymIdx;

	TBool InitExportInfo();
	TBool iImageIsDll;
	TBool ImageIsDll() { return iImageIsDll; }

	Elf32_Word FindSymbolIndex(TText * s);

	TInt iNumberOfImports;
	TInt iNumberOfExports;
	TInt iNumberOfImportDlls;

	TInt iStringTableSize;

	TInt iNumberOfRelocs;
	TInt iNumberOfCodeRelocs;
	TInt iNumberOfDataRelocs;

	TInt NumberOfImports(void);
	TInt NumberOfExports(void);
	TInt NumberOfImportDlls(void);

	TInt NumberOfRelocs(void);

	TInt NumberOfCodeRelocs() { return iNumberOfCodeRelocs; }
	TInt NumberOfDataRelocs() { return iNumberOfDataRelocs; }
	
	TBool GetRelocs(Elf32_Rel **aCodeRelocs, Elf32_Rel **aDataRelocs);

	TUint GetExportTableOffset(void);
	char * CreateImportSection(TInt &aSize);
	TBool CreateSymLookupTable();
	TBool SetupSymbolValues();
	TBool SetupSymbolNames();
	void Sort(NamedExportSymbol** aDstList, NamedExportSymbol* aSrcList);
	void MergeSort(NamedExportSymbol** aDstList, NamedExportSymbol** aSrcList);
	void MergeSort(NamedExportSymbol** aDstList, NamedExportSymbol** aSrcList, \
									TUint aLeft, TUint aRight);
	TBool AddToDependency(TUint aOff);
	TBool CreateDependency();
	OrdZeroRecord* FindDependency(char* aName, TUint aLen);
	DllRec * SearchImports(char *aName);
	TInt GetLookupTblSize(){ return iSymInfoHdr.iSize;}
	void SetLookupTblBase(TInt);
	void GetExportSymInfoHeader(E32EpocExpSymInfoHdr& aSymInfoHdr);
	void SetExportSymInfo();
	TUint GetSymLookupSection(char* aBuff);
	};

// This class is used to create symbols to support named lookups.
// 
	class NamedExportSymbol
	{
	public:
		NamedExportSymbol(char* aName, Elf32_Addr aValue);
		~NamedExportSymbol()
		{
			delete iNext;
		}
		
		char* Name() { return iSymbolName;}
		void Name(char* aName) {iSymbolName = aName;}
		
		TUint NameOffset() {return iSymbolNameOffset;}
		void  NameOffset(TUint aOffset) {iSymbolNameOffset = aOffset;}
		
		Elf32_Addr Value() { return iValue;}
		void Value(Elf32_Addr aValue) { iValue = aValue;}
	
		NamedExportSymbol* Next() { return iNext;}
		void Next(NamedExportSymbol* aNext) { iNext = aNext;}

		Elf32_Rel	iReloc;	// relocation entry for the symbol address
		TUint		iSymRelocType;  // Whether it is a code or data relocation for this symbol
	private:
		char*		iSymbolName;	// Symbol name
		TInt		iSymIdx;
		TUint		iSymbolNameOffset;// offset of name in string table.
		Elf32_Addr	iValue;			// symbol value - this is the address of the symbol
		NamedExportSymbol *iNext;		// next symbol
	};

// This class is used to support symbol lookup of the static dependencies by linking in their 
// 0th ordinal.
	class OrdZeroRecord
	{
	public:
		OrdZeroRecord(char* aName): 
			iName(aName), iNext(0){}
		~OrdZeroRecord()
		{
			delete iNext;
		}
		char				*iName; // (linkas) name of the dependency
		Elf32_Rel			iReloc;	// a relocation entry for the 0th ordinal of each dependency
		OrdZeroRecord		*iNext; // next dependency
		TUint				iOffset;
	};
	struct NeededDLLsList
	{
		NeededDLLsList(TUint aOff) : iOffset(aOff), iNext(0)
		{}
		~NeededDLLsList()
		{
			delete iNext;
		}
		TUint iOffset;
		NeededDLLsList *iNext;
	};

#endif

