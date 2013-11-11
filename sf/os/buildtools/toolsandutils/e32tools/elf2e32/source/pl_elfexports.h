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

#if !defined(_PL_ELFEXPORTS_H_)
#define _PL_ELFEXPORTS_H_

#include "pl_common.h"
#include <vector>
#include <functional>
#include <algorithm>

using std::vector;
using std::binary_function;

class ElfExecutable;
class DllSymbol;
class Symbol;

/**
This class is for exports coming from elf file.
(Corresponding to the exported symbols defined in the given DLL. These exports are filtered
by individual targets as per their requirements(relating to class impedimenta). While 
processing these exports, they are also to update the ordinal numbers before these exports
are written into the dso file.
@internalComponent
@released
*/
class ElfExports
{

public:
	typedef std::vector<DllSymbol*>	ExportList;

	
	struct PtrELFExportNameCompare : 
		binary_function<const Symbol *, const Symbol *, bool>
	{
		bool operator()(const Symbol * lhs, const Symbol * rhs) const;
	};

	struct PtrELFExportOrdinalCompare : 
		binary_function<const Symbol *, const Symbol *, bool>
	{
		bool operator()(const Symbol * lhs, const Symbol * rhs) const;
	};

	struct PtrELFExportNameCompareUpdateAttributes : 
		binary_function<const Symbol *, const Symbol *, bool>
	{
		bool operator()(const Symbol * lhs, const Symbol * rhs) const;
	};
	

	ElfExports();
	~ElfExports();
	
	bool ValidExportP(ElfExecutable * aExecutable, DllSymbol * aSym);
	void FilterExports();
	DllSymbol* Add(char *aDll, ElfExecutable * aExecutable, DllSymbol *aSym);
	void Add(char *aDll, DllSymbol *aSym);
	void Sort();
	void ExportsFilteredP(bool aExportsFilteredP) 
	{
		iExportsFilteredP = aExportsFilteredP;
	}
	bool ExportsFilteredP() {return iExportsFilteredP;} 

	char* DllName();
	ExportList& GetExports(bool) ;
	ExportList& GetExportsInOrdinalOrder();
	size_t GetNumExports();
	ExportList iFilteredExports;

private:
	ExportList	iExportList;
	char		*iDllName;
	bool iSorted;
	bool iExportsFilteredP;
};




#endif // !defined(_PL_ELFEXPORTS_H_)
