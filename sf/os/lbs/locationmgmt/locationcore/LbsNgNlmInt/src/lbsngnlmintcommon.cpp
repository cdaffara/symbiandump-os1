/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <centralrepository.h>
#include "lbsngnlmintcommon.h"
#include "lbsrootcenrepdefs.h"


/* Utility function to simplify retrieving the CategoryUID of the given
   property from the LbsRoot central repository file.
*/
void LbsNgNlmIntCommon::GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid)
	{
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(aKey, category));
	aCategoryUid = TUid::Uid(category);
	CleanupStack::PopAndDestroy(rep);
	}
	
/* Utility function to simplify setting the CategoryUID of the given
   property in the LbsRoot central repository file.
*/
void LbsNgNlmIntCommon::SetCategoryUidL(TUint32 aKey, TUid aCategoryUid)
	{
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Set(aKey, TInt(aCategoryUid.iUid)));
	CleanupStack::PopAndDestroy(rep);
	}

