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
// Implementation of LBS Host Settings class types
// 
//

/**
 @file
 @publishedPartner
 @deprecated 
*/

#include <lbs/lbshostsettingsclasstypes.h>


const TLbsHostSettingsId KLbsUnknownSettingsId = {-1};
const TLbsHostCreatorId  KLbsUnknownCreatorId  = {-1};


//=============================================================================
// TLbsHostSettingsClassTypeBase
//=============================================================================

/**
Default constructor
*/  
EXPORT_C TLbsHostSettingsClassTypeBase::TLbsHostSettingsClassTypeBase() :
	iClassType(ELbsHostSettingsUnknownClass),
	iClassSize(sizeof(TLbsHostSettingsClassTypeBase))
	{
	}

/**
Retrieves the class type
@return The class type
*/ 
EXPORT_C TUint32 TLbsHostSettingsClassTypeBase::ClassType() const 
	{ 
	return iClassType; 
	}

/**
Retrieves the class size
@return The class size
*/
EXPORT_C TUint TLbsHostSettingsClassTypeBase::ClassSize() const   
	{ 
	return iClassSize; 
	}

//=============================================================================
// TLbsHostSettingsBase
//=============================================================================

/**
 * Default constructor
*/  
EXPORT_C TLbsHostSettingsBase::TLbsHostSettingsBase() : 
	iHostSettingsId(KLbsUnknownSettingsId),
	iCreatorId(KLbsUnknownCreatorId)
	{
	iClassSize = sizeof(TLbsHostSettingsBase);
	iClassType |= ELbsHostSettingsBaseClass;
	}

/**
Sets the creator id
@param aCreatorId Id to set.
*/  
void TLbsHostSettingsBase::SetCreatorId(TLbsHostCreatorId aCreatorId)
	{
	iCreatorId  = aCreatorId;
	}

/**
Sets the host settings id
@param aId Id to set.
*/  
void TLbsHostSettingsBase::SetHostSettingsId(TLbsHostSettingsId& aId)
	{
	iHostSettingsId  = aId;
	}

/**
Update the last modified date of the account to be the current time.
*/  
void TLbsHostSettingsBase::UpdateLastModified()
	{
	iLastModified.UniversalTime();
	}

/**
Set the last modified date of the account.

The Date must be in the format YYYYMMDD:HHMMSS.MMMMMM
*/  
void TLbsHostSettingsBase::SetLastModifiedL(const TDesC& aTime)
	{
	//As there are inconsistencies in the TTime library (zero indexed days and months!) we must 
	//manipulate the dates first.
	TDateTime time;
	TInt val=0;

	val=val; //shuts up armv5 compiler warning

	// Year
	TLex16 lex(aTime.Mid(0, 4));
	User::LeaveIfError(lex.Val(val));
	User::LeaveIfError(time.SetYear(val));
	
	// Month
	lex = aTime.Mid(4, 2);
	User::LeaveIfError(lex.Val(val));
	//Subtracting 1 from Month value read from buffer, as Month in TTime starts from 0
	User::LeaveIfError(time.SetMonth(static_cast<TMonth>(val - 1)));
	
	// Day
	lex = aTime.Mid(6, 2);
	User::LeaveIfError(lex.Val(val));
	//Subtracting 1 from Day value read from buffer, as Day in TTime starts from 0
	User::LeaveIfError(time.SetDay(val - 1));
	
	// Hour
	lex = aTime.Mid(9, 2);
	User::LeaveIfError(lex.Val(val));
	User::LeaveIfError(time.SetHour(val));

	// Minute
	lex = aTime.Mid(11, 2);
	User::LeaveIfError(lex.Val(val));
	User::LeaveIfError(time.SetMinute(val));

	// Second
	lex = aTime.Mid(13, 2);
	User::LeaveIfError(lex.Val(val));
	User::LeaveIfError(time.SetSecond(val));

	// Milli-Second
	lex = aTime.Mid(16, 6);
	User::LeaveIfError(lex.Val(val));
	User::LeaveIfError(time.SetMicroSecond(val));
	
	iLastModified = TTime(time);
	}

/**
Gets the ID of the creator of the host settings.
@return The creator id
*/  
EXPORT_C TLbsHostCreatorId TLbsHostSettingsBase::CreatorId() const
	{
	return iCreatorId;
	}

/**
Retrieves the host setting id
@return The host setting id
*/  
EXPORT_C TLbsHostSettingsId TLbsHostSettingsBase::HostSettingsId() const
	{
	return iHostSettingsId;
	}

/**
Returns the last modification date/time of the account
@return The last modification time of the account
*/  
EXPORT_C TTime TLbsHostSettingsBase::LastModified() const
	{
	return iLastModified;
	}

//=============================================================================
// TLbsHostSettingsSupl
//=============================================================================
/**
Default constructor.
*/
EXPORT_C TLbsHostSettingsSupl::TLbsHostSettingsSupl():
	iConnectionId(KLbsSuplUseDefaultConRefId), iConnectionType(TLbsHostSettingsSupl::ELbsConnectionTypeNap),
	iPortId(KLbsSuplUseDefaultPortId), iAuthModesMOLR(EAuthTls|EAuthPskTls), iAuthModesMTLR(EAuthTls|EAuthPskTls)
	{
	iClassSize = sizeof(TLbsHostSettingsSupl);		
	iClassType |= ELbsHostSettingsSuplClass;
	}

/**
Gets the host address of this SUPL host setting entry.
@param aHostAddress On return it contains the host address.
*/
EXPORT_C void TLbsHostSettingsSupl::GetHostNameAddress(TDes8& aHostAddress) const
	{
	aHostAddress = iHostAddress;
	}
/**
Sets the host address of this SUPL host setting
@param aHostAddress The host address to be set for this SUPL host setting entry.
*/
EXPORT_C void TLbsHostSettingsSupl::SetHostNameAddress(const TDesC8& aHostAddress)
	{
	iHostAddress = aHostAddress;
	}

/**
Retrieves the host name of the SUPL server
@param aName On return, contains the name of the SUPL server.
*/
EXPORT_C void TLbsHostSettingsSupl::GetName(TDes8& aName) const
	{
	aName = iName;
	}

/**
Sets the host name of the SUPL server
@param aName The name to use.
*/
EXPORT_C void TLbsHostSettingsSupl::SetName(const TDesC8& aName)
	{
	iName = aName;
	}

/**
Retrieves the connection details of the SUPL account.
Note that connection point types may be added in the future, code using this API should support
graceful handling of unexpected connection types.
@param aConnectionId The id of connection point to use for SUPL.
@param aConnectionType The type of connection point that the id refers to.
*/
EXPORT_C void TLbsHostSettingsSupl::GetConnectionPoint(TInt& aConnectionId, TLbsConnectionType& aConnectionType) const
	{
	aConnectionId = iConnectionId;
	aConnectionType = iConnectionType;
	}


/**
Sets the connection details of the SUPL account.
Note that connection point types may be added in the future, code using this API should support
graceful handling of unexpected connection types.
@param aConnectionId The id of connection point to use for SUPL.
@param aConnectionType The type of connection point that the id refers to.
*/
EXPORT_C void TLbsHostSettingsSupl::SetConnectionPoint(const TInt aConnectionId, const TLbsConnectionType aConnectionType)
	{
	iConnectionId = aConnectionId;
	iConnectionType = aConnectionType;
	}

/**
Retrieves the read only status of the SUPL account.
@param aReadOnly If the account is read only ETrue, else EFalse.
*/
EXPORT_C void TLbsHostSettingsSupl::GetReadOnly(TBool& aReadOnly) const
	{
	aReadOnly = iReadOnly;
	}


/**
Sets the read only status of the SUPL account.
@param aReadOnly If the account should be read only ETrue, else EFalse.
*/
EXPORT_C void TLbsHostSettingsSupl::SetReadOnly(const TBool aReadOnly)
	{
	iReadOnly = aReadOnly;
	}



/**
Retrieves the port id of the SUPL server.
@param aPortId On return, contains the port id.
*/
EXPORT_C void TLbsHostSettingsSupl::GetPortId(TInt& aPortId) const
	{
	aPortId = iPortId;
	}


/**
Sets the port id of the SUPL server.
@param aPortId The port id to use.
*/
EXPORT_C void TLbsHostSettingsSupl::SetPortId(const TInt aPortId)
	{
	iPortId = aPortId;
	}

/**
Retrieves the identifier for the location service provider
@param aProviderId On return, contains the identifier.
*/
EXPORT_C void TLbsHostSettingsSupl::GetProviderId(TDes8& aProviderId) const
	{
	aProviderId = iProviderId;
	}
	
/**
Sets the the identifier for the location service provider
@param aProviderId The identifier to use.
*/
EXPORT_C void TLbsHostSettingsSupl::SetProviderId(const TDesC8& aProviderId)
	{
	iProviderId = aProviderId;
	}
	
/**
Get the authentication modes that this SUPL host setting can use for MO-LRs (terminal 
initiated location requests).
@param aAuthModes On return it contains the authentication modes associated with this SUPL host setting.
*/
EXPORT_C void TLbsHostSettingsSupl::GetAuthModesMOLR
	(
	TAuthModes& aAuthModes
	) const
	{
	aAuthModes = iAuthModesMOLR;
	}


/**
Set the authentication modes that this SUPL host setting can use for MO-LRs (terminal 
initiated location requests).
@param aAuthModes The authentication modes associated with this SUPL host setting.
*/
EXPORT_C void TLbsHostSettingsSupl::SetAuthModesMOLR
	(
	TAuthModes aAuthModes
	)
	{
	iAuthModesMOLR = aAuthModes;
	}


/**
Get the authentication modes that this SUPL host setting can use for MT-LRs (network 
initiated location requests).
@param aAuthModes On return it contains the authentication modes associated with this SUPL host setting.
*/
EXPORT_C void TLbsHostSettingsSupl::GetAuthModesMTLR
	(
	TAuthModes& aAuthModes
	) const
	{
	aAuthModes = iAuthModesMTLR;
	}

/**
Set the authentication modes that this SUPL host setting can use for MT-LRs (network 
initiated location requests).
@param aAuthModes The authentication modes associated with this SUPL host setting.
*/
EXPORT_C void TLbsHostSettingsSupl::SetAuthModesMTLR
	(
	TAuthModes aAuthModes
	)
	{
	iAuthModesMTLR = aAuthModes;
	}
	

