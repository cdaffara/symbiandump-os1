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
//

/**
 @file 
 @internalTechnology
*/

#if (!defined METAITER_H)
#define METAITER_H

#include <e32base.h>
#include <e32std.h>

namespace Meta
{

class MMetaData;
struct SVDataTableEntry;
class TMetaVTableIterator
/**

A simple iterator used to walk through the virtual data tables

@internalComponent
@released since v9.0 */
	{
public:
	IMPORT_C explicit TMetaVTableIterator(MMetaData const* aMetaData);
	IMPORT_C ~TMetaVTableIterator();

	//the only defined method - the postfix operator
	IMPORT_C SVDataTableEntry const* operator++(TInt aDummy);

protected:
    TBool IsNextEntryPresent(); //Recursive helper function

private:
	SVDataTableEntry const* iEntry;
	};

} //namespace Meta



#endif //METAITER_H


