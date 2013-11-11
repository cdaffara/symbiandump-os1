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

#include <elements/metaiterator.h>
#include <elements/metadata.h>
#include <elements/metatype.h>


using namespace Meta;

EXPORT_C TMetaVTableIterator::TMetaVTableIterator(MMetaData const* aMetaData) :
    iEntry(aMetaData->GetVDataTable() + 1)
/**
 * Constructor
 */
	{
	}

EXPORT_C TMetaVTableIterator::~TMetaVTableIterator()
/**
 * Destructor
 */
	{
	}

TBool TMetaVTableIterator::IsNextEntryPresent()
/**
 * Recursive helper function.
 * Protected and not exported to be used only by TMetaVTableIterator objects.
 * Recursively traverses Virtual Data Tables in the derivation chain until
 * the next entry or the end of the last table has been reached, updating
 * TMetaVTableIterator's internal state (iEntry) at each step of the way.
 * Returns ETrue if there is another entry present or EFalse if there is not.
 */
	{
	//are we at the end of a table?
	if (iEntry->iOffset == 0)
		{//this is the last table
		return EFalse; //no more entries present
		}
	if (iEntry->iMetaNewL == NULL)
		{//the end of the table, go for the superclass table
		typedef SVDataTableEntry* (*TMetaBaseTableNewL)();
		iEntry = reinterpret_cast<TMetaBaseTableNewL>(iEntry->iOffset)();
		iEntry++; //skip the first entry of the base class table containing TypeId
		return IsNextEntryPresent();
		}
	return ETrue; //next entry (possibly in the base class) is present
	}

EXPORT_C  SVDataTableEntry const* TMetaVTableIterator::operator++(TInt /*aDummy*/)
/**
 * Postfix increment operator.
 * Walks through virtual data tables each time returning an entry.
 * When the end of the last lable is reached returns NULL.
 */
	{
	if (!IsNextEntryPresent())
	    return NULL;

	SVDataTableEntry const* entry = iEntry;
	iEntry++;
	return entry;
	}


