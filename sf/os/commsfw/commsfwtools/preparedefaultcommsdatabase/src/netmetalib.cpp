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
// Cut down version of NetMeta.lib to allow Commsdat to build on the TOOLS2 platform
// 
//

/**
 @file NetMetaLib.cpp
 @internalTechnology
*/
#ifdef __TOOLS2__
// For the TOOLS2 platform
 
#include "netmetalib.h"
 
using namespace Meta;


EXPORT_C TMetaVTableIterator::TMetaVTableIterator(MMetaData const* aMetaData) :
	iEntry(aMetaData->GetVDataTable() + 1)
	{
	}

	
EXPORT_C TMetaVTableIterator::~TMetaVTableIterator()
	{
	}

	
TBool TMetaVTableIterator::IsNextEntryPresent()
	{
	if (iEntry->iOffset == 0)
		{
		return EFalse;
		}
	if (iEntry->iMetaNewL == NULL)
		{
		typedef SVDataTableEntry* (*TMetaBaseTableNewL)();
		iEntry = reinterpret_cast<TMetaBaseTableNewL>(iEntry->iOffset)();
		iEntry++;
		return IsNextEntryPresent();
		}
	return ETrue;
	}

	
EXPORT_C  SVDataTableEntry const* TMetaVTableIterator::operator++(TInt /*aDummy*/)
	{
	if (!IsNextEntryPresent())
	    return NULL;

	SVDataTableEntry const* entry = iEntry;
	iEntry++;
	return entry;
	}

#endif
