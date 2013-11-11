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
// CCommDbOverrideSettings inline functions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#if !defined(__CDBOVER_INL__)
#define __CDBOVER_INL__

#include <cdbover.h>
#include <commdb.h>

inline CCommDbOverrideSettings::TParamList CCommDbOverrideSettings::PartialFull() const
/**
Returns the value set in the constructor.

@return An enumeration which determines whether this set of override values is full or partial. 
*/
	{
	return iPartialFull;
	}

#endif

