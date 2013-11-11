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
//

#include "CommDbConnPref.h"
#include "Commsdat_Log.h"

EXPORT_C TCommDbConnPref::TCommDbConnPref()
: TConnPref(TConnPref::EConnPrefCommDb)
/**
Constructor
*/
	{
	SetIapId(0);
	SetNetId(0);
	SetDialogPreference(ECommDbDialogPrefUnknown);
	SetDirection(ECommDbConnectionDirectionUnknown);
	SetBearerSet(0);
	}

EXPORT_C TVersion TCommDbConnPref::Version()
/**
Returns the version number of this class

@return The version information.
*/
	{
	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("TCommDbConnPref::Version -> Major=%d, Minor=%d, Build=%d"), KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	return TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	}

EXPORT_C void TCommDbConnPref::SetIapId(TUint32 aIapId)
/**
Sets the CommDb ID of the IAP to use for this connection

@param aIapId The IAP Id to be set
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::SetIapId -> IAP Id=%d"), aIapId);
	PrefPtr()->iIapId = aIapId;
	}

EXPORT_C TUint32 TCommDbConnPref::IapId() const
/**
Retrieves the CommDb ID of the IAP to use in this connection

@return CommDb ID of the IAP
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::IapId -> IAP Id=%d"), PrefPtr()->iIapId);
	return PrefPtr()->iIapId;
	}
	
EXPORT_C void TCommDbConnPref::SetNetId(TUint32 aNetId)
/**
Sets the CommDb ID of the Network Id to use in this connection

@param aNetId Network Id to use in this connection
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::SetNetId -> NetId=%d"), aNetId);
	PrefPtr()->iNetId = aNetId;
	}

EXPORT_C TUint32 TCommDbConnPref::NetId() const
/**
Retrieves the CommDb ID of the Network Id to use in this connection

@return CommDb ID of the Network Id to use in this connection
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::NetId -> NetId=%d"), PrefPtr()->iNetId);
	return PrefPtr()->iNetId;
	}
	
EXPORT_C void TCommDbConnPref::SetDialogPreference(TCommDbDialogPref aDialogPref)
/**
Sets the dialog preference to use in this connection

@param aDialogPref Dialog preference to use in this connection
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::SetDialogPreference -> DialogPreference=%d"), aDialogPref);
	PrefPtr()->iDialogPref = aDialogPref;
	}

EXPORT_C TCommDbDialogPref TCommDbConnPref::DialogPreference() const
/**
Retrieves the dialog preference to use in this connection

@return Dialog preference
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::DialogPreference -> DialogPreference=%d"), PrefPtr()->iDialogPref);
	return PrefPtr()->iDialogPref;
	}
	
EXPORT_C void TCommDbConnPref::SetDirection(TCommDbConnectionDirection aDirection)
/**
Sets the connection direction

@param aDirection Connection direction
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::SetDirection -> Direction=%d"), aDirection);
	PrefPtr()->iDirection = aDirection;
	}

EXPORT_C TCommDbConnectionDirection TCommDbConnPref::Direction() const
/**
Retrieves the connection direction

@return Connection direction
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::Direction -> Direction=%d"), PrefPtr()->iDirection);
	return PrefPtr()->iDirection;
	}
	
EXPORT_C void TCommDbConnPref::SetBearerSet(TUint32 aBearerSet)
/**
Sets the bearer set applicable for the connection

@param aBearerSet The bearer set applicable for the connection
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::SetBearerSet -> BearerSet=%d"), aBearerSet);
	PrefPtr()->iBearerSet = aBearerSet;
	}

EXPORT_C TUint32 TCommDbConnPref::BearerSet() const
/**
Retrieves the bearer set applicable for the connection

@return The bearer set 
*/
	{
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("TCommDbConnPref::BearerSet -> BearerSet=%d"), PrefPtr()->iBearerSet);
	return PrefPtr()->iBearerSet;
	}

EXPORT_C TInt TCommDbConnPref::Compare(const TCommDbConnPref& aPref) const
/**
Compares two sets of connection preferences
The returned value gives a measure of the differences between the preferences
If either value has not been set then the comparison is not made

@param aPref The connection preferences to compare
@return The measure of the differences between the preferences
*/
	{

	TInt retVal(0);

	if(BearerSet() != 0 && aPref.BearerSet() != 0 &&
		aPref.BearerSet() != BearerSet())
		{
		--retVal;
		}

	if(DialogPreference() != ECommDbDialogPrefUnknown && aPref.DialogPreference() != ECommDbDialogPrefUnknown && 
		aPref.DialogPreference() != DialogPreference())
		{
		--retVal;
		}

	if(Direction() != ECommDbConnectionDirectionUnknown && aPref.Direction() != ECommDbConnectionDirectionUnknown &&
		aPref.Direction() != Direction())
		{
		--retVal;
		}

	if(IapId() != 0 && aPref.IapId() != 0 &&
		aPref.IapId() != IapId())
		{
		--retVal;
		}

	if(NetId() != 0 && aPref.NetId() != 0 && 
		aPref.NetId() != NetId())
		{
		--retVal;
		}
	
	//__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("Compare Dialog Preferences -> Result=%d"), retVal);
	return retVal;
	}

EXPORT_C TCommDbMultiConnPref::TCommDbMultiConnPref()
: TConnPref(TConnPref::EConnPrefCommDbMulti)
/**
Constructor
*/
	{
	SetConnectionAttempts(KMaxMultiConnPrefCount);
	}

EXPORT_C TVersion TCommDbMultiConnPref::Version()
/**
Returns the version number of this class

@return The version information.
*/
	{
	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("TCommDbMultiConnPref::Version -> Major=%d, Minor=%d, Build=%d"), KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	return TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	}

EXPORT_C TInt TCommDbMultiConnPref::SetPreference(TInt aIndex, const TCommDbConnPref& aPref)
/**
Sets a connection preference to the array

@param aIndex Index of the array
@param aPref Connection preference settings
@return KErrNone if successful else KErrArgument
*/
	{

	if(aIndex<1 || aIndex>KMaxMultiConnPrefCount)
		{
		return KErrArgument;
		}

	PrefPtr()->iPrefs[aIndex-1] = *(aPref.PrefPtr());
	return KErrNone;
	}

EXPORT_C TInt TCommDbMultiConnPref::GetPreference(TInt aIndex, TCommDbConnPref& aPref)
/**
Retrieves a connection preference from the array

@param aIndex Index of the array
@param aPref On return, the connection preferences
@return KErrNone if successful else KErrArgument
*/
	{

	if(aIndex<1 || aIndex>KMaxMultiConnPrefCount)
		{
		return KErrArgument;
		}

	*(aPref.PrefPtr()) = PrefPtr()->iPrefs[aIndex-1];
	return KErrNone;
	}

EXPORT_C void TCommDbMultiConnPref::SetConnectionAttempts(TInt aNumAttempts)
/**
Sets the number of times to attempt the connection

@param aNumAttempts Number of attempts to be made for a connection
*/
	{
	if(aNumAttempts<1 || aNumAttempts>KMaxMultiConnPrefCount)
		{
		return;
		}

	PrefPtr()->iNumAttempts = aNumAttempts;
	}

EXPORT_C TInt TCommDbMultiConnPref::ConnectionAttempts()
/**
Retrieves the number of times to attempt the connection

@internalTechnology
@return Number of times to attempt the connection
*/
	{

	return PrefPtr()->iNumAttempts;
	}
