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
// A unit test wrapper over the RProperty class
// 
//


#include "Te_LbsSuplErrorPattern.h"
#include "Te_LbsSuplPushRProperty.h"

/**
A test wrapper over the RProperty::Define.

@see RProperty::Define
*/
TInt RTe_LbsSuplPushRProperty::Define(TUint aKey, TInt aAttr, const TSecurityPolicy &aReadPolicy, const TSecurityPolicy &aWritePolicy, TInt aPreallocated)
	{
	TInt err=RTe_LbsSuplErrorPattern::CheckNextError(Te_LbsSuplErrorId::ERProperty_Define);
	if(err==KErrNone)
		{
		return RProperty::Define(aKey, aAttr, aReadPolicy, aWritePolicy, aPreallocated);
		}
	return err;
	}

/**
A test wrapper over the RProperty::Get.

@see RProperty::Get
*/
TInt RTe_LbsSuplPushRProperty::Get(TUid aCategory, TUint aKey, TInt &aValue)
	{
	TInt err=RTe_LbsSuplErrorPattern::CheckNextError(Te_LbsSuplErrorId::ERProperty_Get);
	if(err==KErrNone)
		{
		return RProperty::Get(aCategory, aKey, aValue);
		}
	return err;
	}

/**
A test wrapper over the RProperty::Get.

@see RProperty::Get
*/
TInt RTe_LbsSuplPushRProperty::Get(TUid aCategory, TUint aKey, TDes8 &aValue)
	{
	TInt err=RTe_LbsSuplErrorPattern::CheckNextError(Te_LbsSuplErrorId::ERProperty_Get);
	if(err==KErrNone)
		{
		return RProperty::Get(aCategory, aKey, aValue);
		}
	return err;
	}

/**
A test wrapper over the RProperty::Set.

@see RProperty::Set
*/
TInt RTe_LbsSuplPushRProperty::Set(TUid aCategory, TUint aKey, TInt aValue)
	{
	TInt err=RTe_LbsSuplErrorPattern::CheckNextError(Te_LbsSuplErrorId::ERProperty_Set);
	if(err==KErrNone)
		{
		return RProperty::Set(aCategory, aKey, aValue);
		}
	return err;
	}

/**
A test wrapper over the RProperty::Set.

@see RProperty::Set
*/
TInt RTe_LbsSuplPushRProperty::Set(TUid aCategory, TUint aKey, const TDesC8 &aValue)
	{
	TInt err=RTe_LbsSuplErrorPattern::CheckNextError(Te_LbsSuplErrorId::ERProperty_Set);
	if(err==KErrNone)
		{
		return RProperty::Set(aCategory, aKey, aValue);
		}
	return err;
	}
