// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CStoreableOverrideSettings class inline functions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef __CDBSTORE_INL__
#define __CDBSTORE_INL__

#include <cdbstore.h>

inline CBufStore* CStoreableOverrideSettings::GetStore() const
/** 
Accesses the buffer store.
@deprecated 7.0 There is no need for this store - none of the above functions use this
store. If a client needs a store it should create its own.
@return The pointer to the memory store object.
*/
	{
	return iStore;
	}

#endif

