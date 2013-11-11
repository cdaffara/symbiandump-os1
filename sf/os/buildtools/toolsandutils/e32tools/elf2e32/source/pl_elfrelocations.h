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
// Implementation of the Class ElfRelocations for the elf2e32 tool
// @internalComponent
// @released
// 
//

#if !defined(_PL_ELFRELOCATIONS_H)
#define _PL_ELFRELOCATIONS_H

#include "pl_common.h"
#include <list>

class ElfRelocation;
class ElfLocalRelocation;

/**
This class is for Elf relocations.

@internalComponent
@released
*/
class ElfRelocations
{
public:
	typedef std::list<ElfLocalRelocation*> RelocationList;
	
	ElfRelocations();
	~ElfRelocations();
	PLUINT32 Add(ElfLocalRelocation* aReloc);

	RelocationList & GetCodeRelocations();
	RelocationList & GetDataRelocations();
	struct Cmp {
		bool operator()(ElfRelocation * x, ElfRelocation * y);
	};
private:
	bool iCodeSortedP;
	RelocationList iCodeRelocations;
	bool iDataSortedP;
	RelocationList iDataRelocations;

};




#endif // !defined(_PL_ELFRELOCATIONS_H)
