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
// Implementation of the Class ElfImports for the elf2e32 tool
// @internalComponent
// @released
// 
//


#if !defined(_PL_ELFIMPORTS_H_)
#define _PL_ELFIMPORTS_H_

#include "pl_common.h"
#include <vector>
#include <map>

using std::vector;
using std::map;
using std::binary_function;

class ElfImportRelocation;

/**
This class get the list of symbols imported from a list of DSOs.
@internalComponent
@released
*/
class ElfImports
{

public:
	struct StringPtrLess : binary_function<const char *, const char *, bool>
	{
		bool operator() (const char * lhs, const char * rhs) const;
	};
	
	typedef std::vector<ElfImportRelocation*>			RelocationList;
	
	typedef std::map<const char*, RelocationList, StringPtrLess>	ImportMap;

	ElfImports();
	~ElfImports();

	void Add(const char* aDll, ElfImportRelocation *aReloc);
	PLUINT32 GetImportSize();
	ImportMap& GetImports();

private:
	ImportMap	iImports;

};




#endif // !defined(_PL_ELFIMPORTS_H_)
