// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "queryconnsettingstiermgrimpl.h"


CQueryConnSettingsTierMgrImpl* CQueryConnSettingsTierMgrImpl::NewL(TUint aIap)
	{
	CQueryConnSettingsTierMgrImpl* self = new (ELeave)CQueryConnSettingsTierMgrImpl();
	self->iIapView = CCommsDatIapView::NewL(aIap);
	return self;
	}

TInt CQueryConnSettingsTierMgrImpl::GetInt(CommsDat::TMDBElementId aElementId, TUint32& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aPlatsecApiExt;
	return iIapView->GetInt(aElementId, aValue);
	}
	
TInt CQueryConnSettingsTierMgrImpl::GetInt(CommsDat::TMDBElementId aElementId, TInt& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aPlatsecApiExt;
	return iIapView->GetInt(aElementId, aValue);
	}
	
TInt CQueryConnSettingsTierMgrImpl::GetBool(CommsDat::TMDBElementId aElementId, TBool& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aPlatsecApiExt;
	return iIapView->GetBool(aElementId, aValue);
	}
	
TInt CQueryConnSettingsTierMgrImpl::GetText(CommsDat::TMDBElementId aElementId, HBufC8*& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aPlatsecApiExt;
	return iIapView->GetText(aElementId, aValue);
	}
	
TInt CQueryConnSettingsTierMgrImpl::GetText(CommsDat::TMDBElementId aElementId, HBufC16*& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aPlatsecApiExt;
	return iIapView->GetText(aElementId, aValue);
	}

