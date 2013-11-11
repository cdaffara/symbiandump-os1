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
// Private header for functions internal to the 
// Location Based Services Net Internal Interfaces API
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSNETINTERNALCOMMON_H
#define LBSNETINTERNALCOMMON_H

#include <e32cmn.h>

/* Static utility functions used internally to LbsInternalApi
*/
NONSHARABLE_CLASS(LbsNetInternalCommon)
	{
public:
	static void GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid);
	
	static void SetCategoryUidL(TUint32 aKey, TUid aCategoryUid);
	};


#endif // LBSNETINTERNALCOMMON_H