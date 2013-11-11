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
// Implementation of the Class ElfExports for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "pl_elfexports.h"
#include "pl_elfexecutable.h"
#include "pl_dllsymbol.h"

using std::set_difference;

/**
Constructor for class ElfExports
@internalComponent
@released
*/
ElfExports::ElfExports() : iDllName(NULL), iSorted(false), iExportsFilteredP(false){
}

/**
Destructor for class ElfExports
@internalComponent
@released
*/
ElfExports::~ElfExports()
{
	if(iExportList.size())
	{
		ExportList::iterator aItr = iExportList.begin();
		ExportList::iterator last = iExportList.end();
		DllSymbol *temp;

		while( aItr != last)
		{
			temp = *aItr;
			aItr++;
			delete temp;
		}
	}
	iExportList.clear();

}

/**
This function validates exported symbols. The typeinfo name strings
are not valid export symbols and are discarded.
@param aExecutable - Instance of class ElfExecutable
@param aSym	- DllSymbol
@return True if symbol is valid, otherwise false
@internalComponent
@released
*/
bool ElfExports::ValidExportP(ElfExecutable * aExecutable, DllSymbol * aSym)
{
	char * aSymName = aExecutable->GetSymbolName(aSym->iSymbolIndex);
	int result = strncmp(aSymName, "_ZTS", strlen("_ZTS"));
	return ( result != 0);
}

/**
This function adds export symbols into exports list.
@param aDll - Dll name
@param aExecutable - Instance of class ElfExecutable
@param aSym - Dll symbol
@return Dll Symbol if its valid, otherwise NULL
@internalComponent
@released
*/
DllSymbol* ElfExports::Add(char *aDll, ElfExecutable * aExecutable, DllSymbol *aSym)
{
	if (ValidExportP(aExecutable, aSym))
	{
		if( !iDllName )
			iDllName = aDll;

		iExportList.push_back(aSym);
		iSorted = false;
		return aSym;
	}
	return NULL;
}

/**
Function to add elf exports
@param aDll - Dll name
@param aSym - Dll symbol
@internalComponent
@released
*/
void ElfExports::Add(char *aDll, DllSymbol *aSym)
{
	if( !iDllName )
		iDllName = aDll;
	iExportList.push_back(aSym);
	iSorted = false;
}

/**
Function to sort elf exports. Sorting may be done based on the symbol
name or on the ordinal number depending on the usecase.
@internalComponent
@released
*/
void ElfExports::Sort() 
{
	if (!iSorted) {
		if(iExportsFilteredP) {
			std::sort(iFilteredExports.begin(), iFilteredExports.end(), PtrELFExportNameCompare());
		}
		else {
			std::sort(iExportList.begin(), iExportList.end(), PtrELFExportNameCompare());
		}
		iSorted = true;
	}
}

/**
Function to get exports list.
@param aSorted - sort before returning the exports.
@return export list
@internalComponent
@released
*/
ElfExports::ExportList & ElfExports::GetExports(bool aSorted)
{
	if (aSorted) Sort();
	
	if(iExportsFilteredP) 
		return iFilteredExports;
	else
		return iExportList;
}

/**
Function to get exports in ordinal order
@return export list
@internalComponent
@released
*/
ElfExports::ExportList & ElfExports::GetExportsInOrdinalOrder()
{
	if (iExportsFilteredP)
	{
		std::sort(iFilteredExports.begin(), iFilteredExports.end(), PtrELFExportOrdinalCompare());
		return iFilteredExports;
	}
	else
	{
		std::sort(iExportList.begin(), iExportList.end(), PtrELFExportOrdinalCompare());
		return iExportList;
	}
}

/**
Function to process the filtered exports
@internalComponent
@released
*/
void ElfExports::FilterExports()
{
	std::sort(iExportList.begin(), iExportList.end(), PtrELFExportNameCompare());
	std::sort(iFilteredExports.begin(), iFilteredExports.end(), PtrELFExportNameCompare());

	ExportList aNewList(iExportList.size());
	ExportList::iterator aNewListBegin = aNewList.begin(); 

	ExportList::iterator aNewListEnd = set_difference(iExportList.begin(), iExportList.end(), \
		iFilteredExports.begin(), iFilteredExports.end(), aNewListBegin, PtrELFExportNameCompare());

	iFilteredExports.clear();
	while (aNewListBegin != aNewListEnd)
	{
		iFilteredExports.push_back(*aNewListBegin);
		aNewListBegin++;
	}
}

/**
Function to get number of exports
@return size of export list
@internalComponent
@released
*/
size_t ElfExports::GetNumExports()
{
	return iExportList.size();
}

/**
Function to get Dll name
@return Dll name
@internalComponent
@released
*/
char* ElfExports::DllName()
{ 
	return iDllName;
}

/**
Overloaded operator to compare ELF export names.
@return True if lhs symbol name < rhs symbol name, otherwise false
@internalComponent
@released
*/
bool ElfExports::PtrELFExportNameCompare::operator()(const Symbol * lhs, const Symbol * rhs) const
{
	return strcmp( lhs->SymbolName(), rhs->SymbolName()) < 0;
}

/**
Overloaded operator to compare ordinal numbers of symbols.
@return True if lhs symbol name < rhs symbol name, otherwise false
@internalComponent
@released
*/
bool ElfExports::PtrELFExportOrdinalCompare::operator()(const Symbol * lhs, const Symbol * rhs) const
{
	return lhs->OrdNum() < rhs->OrdNum();
}

/**
Overloaded operator to compare update symbol attributes that are 
being compared. The comparision is done on the symbol names.
@return True if lhs symbol name < rhs symbol name, otherwise false
@internalComponent
@released
*/
bool ElfExports::PtrELFExportNameCompareUpdateAttributes::operator()(const Symbol * lhs, const Symbol * rhs) const
{
	int result = strcmp(lhs->SymbolName(), rhs->SymbolName());
	if (!result) 
	{
		if (lhs->OrdNum() > 0)
			((Symbol*)rhs)->SetOrdinal( lhs->OrdNum());
		else if (rhs->OrdNum() > 0)
			((Symbol*)lhs)->SetOrdinal( rhs->OrdNum());
				
		if( ((Symbol*)lhs)->Absent() )
			((Symbol*)rhs)->SetAbsent(true);
		else if ( ((Symbol*)rhs)->Absent() )
			((Symbol*)lhs)->SetAbsent(true);

		if( ((Symbol*)lhs)->SymbolSize() )
			((Symbol*)rhs)->SetSymbolSize(((Symbol*)lhs)->SymbolSize());
		else if( ((Symbol*)rhs)->SymbolSize() )
			((Symbol*)lhs)->SetSymbolSize(((Symbol*)rhs)->SymbolSize());
	}
	return result < 0;
}

