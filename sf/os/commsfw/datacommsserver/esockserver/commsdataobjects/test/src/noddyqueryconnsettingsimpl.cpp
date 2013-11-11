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

#include "noddyqueryconnsettingsimpl.h"


TInt CNoddyQueryConnSettingsImpl::GetInt(CommsDat::TMDBElementId aElementId, TUint32& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aElementId;
	(void)aPlatsecApiExt;
	aValue = 31;
	return KErrNone;
	}
	
TInt CNoddyQueryConnSettingsImpl::GetInt(CommsDat::TMDBElementId aElementId, TInt& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aElementId;
	(void)aPlatsecApiExt;
	(void)aValue;
	return KErrNotFound;
	}
	
TInt CNoddyQueryConnSettingsImpl::GetBool(CommsDat::TMDBElementId aElementId, TBool& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aElementId;
	(void)aPlatsecApiExt;
	aValue = ETrue;
	return KErrNone;
	}
	
TInt CNoddyQueryConnSettingsImpl::GetText(CommsDat::TMDBElementId aElementId, HBufC8*& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aElementId;
	(void)aPlatsecApiExt;
	TRAPD(err, aValue = HBufC8::NewL(32));
	*aValue = _L8("8:testing.8");
	return KErrNone;
	}
	
TInt CNoddyQueryConnSettingsImpl::GetText(CommsDat::TMDBElementId aElementId, HBufC16*& aValue, MPlatsecApiExt* aPlatsecApiExt)
	{
	(void)aElementId;
	(void)aPlatsecApiExt;
	TRAPD(err, aValue = HBufC16::NewL(32));
	*aValue = _L16("16.test.16");
	return KErrNone;
	}

