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

#include "pl_elfimports.h"
#include "pl_elfimportrelocation.h"

/**
Constructor for class ElfImports
@internalComponent
@released
*/
ElfImports::ElfImports(){

}

/**
Destructor for class ElfImports to release allocated memory
@internalComponent
@released
*/
ElfImports::~ElfImports()
{

	if(iImports.size())
	{
		ImportMap::iterator aItr = iImports.begin();
		ImportMap::iterator last = iImports.end();
		RelocationList *rlistTemp;

		while( aItr != last)
		{
			rlistTemp = &((*aItr).second);
			RelocationList::iterator aItr1 = rlistTemp->begin();
			RelocationList::iterator last1 = rlistTemp->end();
			ElfImportRelocation *temp;

			while( aItr1 != last1)
			{
				temp = *aItr1;
				aItr1++;
				delete temp;
			}
			aItr++;
			rlistTemp->clear();
		}
	}

	iImports.clear();
}


/**
Function to add imports
@param aDllName - Dll name
@param aReloc - Elf import relocation
@internalComponent
@released
*/
void ElfImports::Add(const char* aDllName, ElfImportRelocation *aReloc){
	iImports[aDllName].insert(iImports[aDllName].end(), aReloc);
}


/**
Function to get import size
@return import size
@internalComponent
@released
*/
PLUINT32 ElfImports::GetImportSize(){

	PLUINT32	aSize = 0;

	ImportMap::iterator aItr = iImports.begin();
	RelocationList aList;

	while(aItr != iImports.end()) {
		aList = ((*aItr).second);
		aSize += aList.size();
		aItr++;
	}
	return aSize;
}


/**
Overloaded operator comparing the symbol names.
@return True if lhs string is less then rhs, otherwise false
@internalComponent
@released
*/
bool ElfImports::StringPtrLess::operator() (const char * lhs, const char * rhs) const
{
	return strcmp(lhs, rhs) < 0;
}

/**
Function to get imports
@return imports
@internalComponent
@released
*/
ElfImports::ImportMap& ElfImports::GetImports()
{
	return iImports;
}

