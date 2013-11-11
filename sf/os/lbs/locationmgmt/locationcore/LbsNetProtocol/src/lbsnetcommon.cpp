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
// Implentation of functions shared between all implementations 
// of the LBS Network Protocol Module interface/ECom plugin.
// 
//

#include <e32base.h>
#include <ecom/ecom.h>

#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsnetcommon.h>

//------------------------------------------------------------------------------
// TLbsNetClassTypeBase 
//------------------------------------------------------------------------------

/**
 * Default constructor for TLbsNetClassTypeBase
 *
 * 
 * 
 */
EXPORT_C TLbsNetClassTypeBase::TLbsNetClassTypeBase()
:	iClassType(0),
	iClassSize(0)
	{}

/**
 * @return The size of the TPositionClassTypeBase-derived class
 *
 * 
 * 
 */
EXPORT_C TUint TLbsNetClassTypeBase::ClassSize() const
	{
	return iClassSize;
	}

/**
 * @return The type of the TPositionClassTypeBase-derived class
 *
 * 
 * 
 */
EXPORT_C TUint32 TLbsNetClassTypeBase::ClassType() const
	{
	return iClassType;
	}

//------------------------------------------------------------------------------
// TLbsNetSessionId
//------------------------------------------------------------------------------

/**
Default constructor for TLbsNetSessionId

Sets the session owner to TUid::Null() and the session request number to 0
*/
EXPORT_C TLbsNetSessionId::TLbsNetSessionId()
	:
	iSessionOwner(TUid::Null()),
	iSessionNum(0)
	{
	iClassType = ELbsNetSessionIdClass;
	iClassSize = sizeof(TLbsNetSessionId);
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

/**
Constructor for TLbsNetSessionId
@param aSessionOwner The session owner
@param aSessionNum The request number
*/
EXPORT_C TLbsNetSessionId::TLbsNetSessionId(TUid aSessionOwner, TSessionNum aSessionNum)
	:
	iSessionOwner(aSessionOwner),
	iSessionNum(aSessionNum)
	{
	iClassType = ELbsNetSessionIdClass;
	iClassSize = sizeof(TLbsNetSessionId);
	}
	
/**
Get the owner of the session
@return The session owner
*/
EXPORT_C TUid TLbsNetSessionId::SessionOwner() const
	{
	return iSessionOwner;
	}

/**
Get the request number
@return The request number
*/
EXPORT_C TLbsNetSessionId::TSessionNum TLbsNetSessionId::SessionNum() const
	{
	return iSessionNum;
	}

/**
Set the session owner
@param aSessionOwner The session owner
*/
EXPORT_C void TLbsNetSessionId::SetSessionOwner(TUid aSessionOwner)
	{
	iSessionOwner = aSessionOwner;
	}

/**
Set the request number
@param aSessionNum The request number
*/
EXPORT_C void TLbsNetSessionId::SetSessionNum(TSessionNum aSessionNum)
	{
	iSessionNum = aSessionNum;
	}

/**
Increments the request number
*/
EXPORT_C void TLbsNetSessionId::IncrSession()
	{
	iSessionNum++;
	}

/**
Tests if two session Ids are equal
@return True if session Ids are equal
*/
EXPORT_C TBool TLbsNetSessionId::operator==(const TLbsNetSessionId& aSessionId) const
	{
	return (iSessionOwner == aSessionId.SessionOwner()) &&
			 (iSessionNum   == aSessionId.SessionNum());
	}

/**
Tests if two session Ids are not equal
@return True if session Ids are not equal
*/
EXPORT_C TBool TLbsNetSessionId::operator!=(const TLbsNetSessionId& aSessionId) const
	{
	return (iSessionOwner != aSessionId.SessionOwner()) ||
			 (iSessionNum   != aSessionId.SessionNum());
	}


//------------------------------------------------------------------------------
// TLbsNetSessionIdArray
//------------------------------------------------------------------------------

/**
Default constructor for TLbsNetSessionIdArray

Sets the session owner to TUid::Null() and the session request number to 0
*/
EXPORT_C TLbsNetSessionIdArray::TLbsNetSessionIdArray()
	:
	iNumSessions(0)
	{
	iClassType = ELbsNetSessionIdArrayClass;
	iClassSize = sizeof(TLbsNetSessionIdArray);
	}

	
/**
Returns the number of @see TLbsNetSessionId objects.
Currently not supported.

@return An integer indicating the number of @see TLbsNetSessionId
   session ids.
*/
EXPORT_C TInt TLbsNetSessionIdArray::NumSessions() const
	{
	return KErrNotSupported;
#pragma message("TBD")
	}

/**
Returns the specified session id.
Currently not supported.

@param aIndex [In] Specifies the index of session id returned.
@param aSessionId [Out] On successful completion, contains the session id.

@return KErrNone if the aIndex is within range and aSessionId has been assigned a 
   valid session id; KErrArgument if the aIndex is out of range.
*/
EXPORT_C TInt TLbsNetSessionIdArray::GetSession
	(
	TInt /*aIndex*/, 
	TLbsNetSessionId& /*aSessionId*/
	) const
	{
	return KErrNotSupported;
#pragma message("TBD")
	}


/**
Assigns the array of session ids.
Currently not supported.

@param aSessions [In] The array of session ids..
@param aNumSessions [In] The number of session ids in the array.

@return KErrNone if the assigment has been completed successfully; KErrArgument if the 
   parameter aNumSessions was out of range.
*/	
EXPORT_C TInt TLbsNetSessionIdArray::SetSessions
	(
	const TLbsNetSessionId /*aSessions*/[], 
	TInt /*aNumSessions*/
	)
	{
	return KErrNotSupported;
#pragma message("TBD")
	}
	

//-----------------------------------------------------------------------------
// TLbsNetPosRequestPrivacy
//-----------------------------------------------------------------------------

/**
 * Constructor for the TLbsNetPosRequestPrivacy
 *
 * 
 * 
 * @see CLbsLocationSourceGpsBase
 */
EXPORT_C TLbsNetPosRequestPrivacy::TLbsNetPosRequestPrivacy()
	:
	iAction(ERequestActionNotUsed),
	iAdvice(ERequestAdviceNotUsed)
	{
 	iClassType = ELbsNetPosRequestPrivacyClass;
	iClassSize = sizeof(TLbsNetPosRequestPrivacy);
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}
	

/**
Returns the network's recomendation on whether the location request should be
accepted or rejected. In the situation where the user is consulted, the
action is the default behaviour when the user does not respond.



@see RequestAdvice
@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest
*/
EXPORT_C TLbsNetPosRequestPrivacy::TLbsRequestAction TLbsNetPosRequestPrivacy::RequestAction() const
	{
	return iAction;
	}

/**
Returns the networks recomendation on whether the user is to be consulted,
simply notified, or potentially not advised about the request for their
location. 



@see RequestAction
@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest
*/
EXPORT_C TLbsNetPosRequestPrivacy::TLbsRequestAdvice TLbsNetPosRequestPrivacy::RequestAdvice() const
	{
	return iAdvice;
	}


/**
Assigns which action (reject/accept) the network has recomended for a location request.

@param aAction contains the recomended action.



@see RequestAction
@see SetRequestAdvice
@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest
*/
EXPORT_C void TLbsNetPosRequestPrivacy::SetRequestAction(TLbsRequestAction aAction)
	{
	iAction = aAction;
	}

/**
Assigns which advice (verify/notify/silent) the network has recomended for a location request.

@param aAdvice contains the recomended advice.



@see RequestAdvice
@see SetRequestAction
@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest
*/
EXPORT_C void TLbsNetPosRequestPrivacy::SetRequestAdvice(TLbsRequestAdvice aAdvice)
	{
	iAdvice = aAdvice;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosRequestQuality
//-----------------------------------------------------------------------------

/**
 * Constructor for the TLbsNetPosRequestQuality
 *
 * 
 * 
 * @see CLbsLocationSourceGpsBase
 */
EXPORT_C TLbsNetPosRequestQuality::TLbsNetPosRequestQuality()
	:
	iMinHorizontalAccuracy(0),
	iMinVerticalAccuracy(0),
	iMaxFixTime(0),
	iMaxFixAge(0)
	{
 	iClassType = ELbsNetPosRequestQualityClass;
	iClassSize = sizeof(TLbsNetPosRequestQuality);
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

/**
The maximum time the network is prepared to wait while
lbs subsystem calculates the current location.



@see SetMaxFixTime

@return The maximum time that a data source should spend calculating
the current location.
*/
EXPORT_C TTimeIntervalMicroSeconds  TLbsNetPosRequestQuality::MaxFixTime() const
	{
	return iMaxFixTime;
	}

/**
The maximum age of a location fix that is acceptable to the network.



@see SetMaxFixAge

@return The maximum age of a location fix that can be returned to the
network.
*/
EXPORT_C TTimeIntervalMicroSeconds  TLbsNetPosRequestQuality::MaxFixAge() const
	{
	return iMaxFixAge;
	}

/**
 * The desired minimum horizontal accuracy for a location update.
 *
 * 
 * 
 * @see SetMaxFixTime
 *
 * @return The desired minimum horizontal accuracy for a location update in metres.
 */

EXPORT_C TReal32 TLbsNetPosRequestQuality::MinHorizontalAccuracy() const
	{
	return iMinHorizontalAccuracy;
	}

/**
 * The desired minimum vertical accuracy for a location update.
 *
 * 
 * 
 * @see SetMaxFixTime
 *
 * @return The desired minimum vertical accuracy for a location update in metres.
 */
EXPORT_C TReal32 TLbsNetPosRequestQuality::MinVerticalAccuracy() const
	{
	return iMinVerticalAccuracy;
	}

/**
 * The maximum time a data source should spend calculating
 * the current location.
 * When used in conjunction with SetMinHorizontalAccuracy and/or
 * SetMinVerticalAccuracy the Lbs subsystem will produce an update
 * either when the desired accuracy has be met or when the time
 * limit is reached.
 *
 * 
 * 
 * @see MaxFixTime
 *
 * @param aTime specifies the maximum time that the data source should spend
 * calculating the current location.
 */
EXPORT_C void TLbsNetPosRequestQuality::SetMaxFixTime(TTimeIntervalMicroSeconds aTime)
	{
	iMaxFixTime = aTime;
	}

/**
Specifies the maximum age of a location fix that can be returned to the
network. 


@see MaxFixAge

@param aTime specifies the maximum time that the data source should spend
calculating the current location.
*/
EXPORT_C void TLbsNetPosRequestQuality::SetMaxFixAge(TTimeIntervalMicroSeconds aTime)
	{
	iMaxFixAge = aTime;
	}


/**
 * The desired minimum horizontal accuracy for a location update.
 * When used in conjunction with SetMaxFixTime the data source will
 * produce an update either when the desired accuracy has be met or
 * when the time limit is reached.
 *
 * 
 * 
 *
 * @see SetMaxFixTime
 *
 * @param aAccuracy specifies the desired minimum horizontal accuracy
 * (in metres) for a location update produced by the data source. 
 */
EXPORT_C void TLbsNetPosRequestQuality::SetMinHorizontalAccuracy(TReal32 aAccuracy)
	{
	iMinHorizontalAccuracy = aAccuracy;
	}

/**
 * The desired minimum vertical accuracy for a location update.
 * When used in conjunction with SetMaxFixTime the data source will
 * produce an update either when the desired accuracy has be met or
 * when the time limit is reached.
 *
 * 
 * 
 *
 * @see SetMaxFixTime
 *
 * @param aAccuracy specifies the desired minimum vertical accuracy
 * (in metres) for a location update produced by the data source. 
 */
EXPORT_C void TLbsNetPosRequestQuality::SetMinVerticalAccuracy(TReal32 aAccuracy)
	{
	iMinVerticalAccuracy = aAccuracy;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosRequestOptions and Family
//-----------------------------------------------------------------------------

/**
Constructor for the TLbsNetPosRequestOptionsBase



@see TLbsNetPosRequestOptions
 */
EXPORT_C TLbsNetPosRequestOptionsBase::TLbsNetPosRequestOptionsBase()
	{
	iClassType = ELbsNetPosRequestOptionsBaseClass;
	iClassSize = sizeof(TLbsNetPosRequestOptionsBase);
	}


/**
Constructor for the TLbsNetPosRequestOptions




This class is used to encapsulated the common parameters for the terminal
issuing a network and a self location request.

@see TLbsNetPosRequestOptionsAssistance
@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TLbsNetPosRequestOptions::TLbsNetPosRequestOptions()
	{
	iClassType = ELbsNetPosRequestOptionsClass;
	iClassSize = sizeof(TLbsNetPosRequestOptions);	
	}

/**
SetNewClientConnected should be set to ETrue is there is at least one
local client application that is requesting location information for
the first time.




@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptions::SetNewClientConnected
	(
	TBool aNewClient
	)
	{
	iNewClient = aNewClient;
	}

/**
Specifies the desired quality for the location request.
Default values for each of the quality fields indicate no preference
has been set by the LBS subsystem. 

Currently the LBS subsystem does not set any quality requirements.




@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptions::SetRequestQuality
	(
	const TLbsNetPosRequestQuality& aQuality
	)
	{
	iQuality = aQuality;	
	}


/**
NewClientConnected returns ETrue is there is at least one local client
application that is requesting location information for the first time.




@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TBool TLbsNetPosRequestOptions::NewClientConnected() const
	{
	return iNewClient;
	}

/**
Obtains the desired quality for the location request.
Default values for each of the quality fields indicate no preference
has been specified by the LBS subsystem.

Currently the LBS subsystem does not set any quality requirements.




@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptions::GetRequestQuality
	(
	TLbsNetPosRequestQuality& aQuality
	) const
	{
	aQuality = iQuality;
	}

/**
Constructor for the TLbsNetPosRequestOptionsAssistance




This class is used to encapsulated the common parameters for the terminal
issuing a self location request.

@see TLbsNetPosRequestOptions
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TLbsNetPosRequestOptionsAssistance::TLbsNetPosRequestOptionsAssistance()
	{
	iClassType = ELbsNetPosRequestOptionsAssistanceClass;
	iClassSize = sizeof(TLbsNetPosRequestOptionsAssistance);		
	}

/**
Specifies the desired set of assistance data that should be obtained
from the network. A bit mask of zero indicates that no assistance
data is requried.




@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsAssistance::SetDataRequestMask
	(
	TLbsAsistanceDataGroup aDataRequestMask
	)
	{
	iDataRequestMask = aDataRequestMask;
	}

/**
Specifies the current positioning mode for the location being requested
by the LBS subsystem




@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsAssistance::SetPosMode(TPositionModuleInfo::TTechnologyType aPosMode)
	{
	iPosMode = aPosMode;
	}

/**
Returns the desired set of assistance data that should be obtained
from the network. A bit mask of zero indicates that no assistance
data is requried.




@see CLbsNetworkProtocolBase::RequestSelfLocation
*/

EXPORT_C TLbsAsistanceDataGroup TLbsNetPosRequestOptionsAssistance::DataRequestMask() const
	{
	return iDataRequestMask;
	}

/**
Returns the current mode of the module requesting location.




@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TPositionModuleInfo::TTechnologyType TLbsNetPosRequestOptionsAssistance::PosMode() const
	{
	return iPosMode;
	}

/**
Constructor for the TLbsNetPosRequestOptionsTechnology




This class is used to encapsulate the common parameters for the terminal
issuing a self location request.

@see TLbsNetPosRequestOptions
@see CLbsNetworkProtocolBase::RequestTransmitLocation
*/
EXPORT_C TLbsNetPosRequestOptionsTechnology::TLbsNetPosRequestOptionsTechnology()
	{
	iClassType = ELbsNetPosRequestOptionsTechnologyClass;
	iClassSize = sizeof(TLbsNetPosRequestOptionsTechnology);
	}

/**
Specifies the current positioning mode for the location being requested
by the LBS subsystem




@see CLbsNetworkProtocolBase::RequestTransmitLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsTechnology::SetPosMode(TPositionModuleInfo::TTechnologyType aPosMode)
	{
	iPosMode = aPosMode;
	}

/**
Returns the current mode of the module requesting location.




@see CLbsNetworkProtocolBase::RequestTransmitLocation
*/
EXPORT_C TPositionModuleInfo::TTechnologyType TLbsNetPosRequestOptionsTechnology::PosMode() const
	{
	return iPosMode;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosMethod
//-----------------------------------------------------------------------------

/**
Default Constructor for the TLbsNetPosMethod

This class is used by specify one positioning method that the LBS susbsystem
should use to obtain location information. It comprises of a positioning "means"
and an associated "mode".

The positioning "means" specifies the general mechanism that should be employed.
For example, GPS, Cell or EOTD. Currently only GPS is supported.

The positioning "mode" refines the specified mechanism. For example, it indicates
where the calculation should be performed and if assistance information should be used
or requested. In particular, the calculation can either be performed on the terminal
or on the network. 

For autonomous GPS, the positioning "means" should be set to KLbsPositioningMeansGps
and ETechnologyTerminal specified as the "mode".

For terminal based GPS, the positioning "means" should be set to KLbsPositioningMeansGps
and "ETechnologyTerminal + ETechnologyAssisted" specified as the "mode".

For terminal assisted GPS, the positioning "means" should be set to KLbsPositioningMeansGps
and "ETechnologyNetwork + ETechnologyAssisted" specified as the "mode".



*/
EXPORT_C TLbsNetPosMethod::TLbsNetPosMethod()
	:
	iPosMeans(TUid::Null()),
	iPosMode(TPositionModuleInfo::ETechnologyUnknown)
	{
 	iClassType = ELbsNetPosMethodClass;
	iClassSize = sizeof(TLbsNetPosMethod);
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

/**
The positioning technology (means)

Currently only GPS is support. This is indicated by the value KLbsPositioningMeansGps

@return The positioning technology (means)



*/
EXPORT_C TUid TLbsNetPosMethod::PosMeans() const
	{
	return iPosMeans;
	}

/**
The positioning mode of operation for the given technology. The indicates where
the calculation should be performed and if assistance information should be used
or requested.

For GPS a mode of ETechnologyTerminal indicates that autonomous mode should be used
by the terminal.

A mode of "ETechnologyTerminal + ETechnologyAssisted" specifies Terminal Based
calculations. Whereas, "ETechnologyNetwork + ETechnologyAssisted" represents
Terminal Assisted mode.

@see SetPosMode()
@return The positioning operational mode.



*/
EXPORT_C TPositionModuleInfo::TTechnologyType TLbsNetPosMethod::PosMode() const
	{
	return iPosMode;
	}

/**
Assigns a positioning mechanism and mode for a location request.

Only GPS position technology is currently supported. Parameter aPosMeans must
be set to the value KLbsPositioningMeansGps.

Parameter "aPosMode" specifies the positioning mode of operation for the given
technology. This parameter indicates where the calculation should be performed
and if assistance information should be used or requested.

For GPS, a mode of ETechnologyTerminal indicates that GPS autonomous mode should
be used by the terminal.

A mode of "ETechnologyTerminal + ETechnologyAssisted" specifies Terminal Based
calculations. Whereas, "ETechnologyNetwork + ETechnologyAssisted" represents
Terminal Assisted mode.

@see TLbsNetPosMethod()
@see PosMode()
@see PosMeans()
@param aPosMeans The positioning technology. This must be KLbsPositioningMeansGps
@param aPosMode  The positioning operational mode



*/
EXPORT_C void TLbsNetPosMethod::SetPosMethod(TUid aPosMeans, TPositionModuleInfo::TTechnologyType aPosMode)
	{
	iPosMeans = aPosMeans;
	iPosMode = aPosMode;
	}


//-----------------------------------------------------------------------------
// TLbsNetDeviceCapabilities
//-----------------------------------------------------------------------------

/**
Constructor for the TLbsNetPosCapabilities



@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TLbsNetPosCapabilities::TLbsNetPosCapabilities()
	:
	iNumPosProtocols(0),
	iNumPosMethods(0)
	{
	iClassType = ELbsNetPosCapabilitiesClass;
	iClassSize = sizeof(TLbsNetPosCapabilities);	
	}


/**
Returns the number of positioning protocols supported by the device.

@return The number of positioning protocols supported by the device.



@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TInt TLbsNetPosCapabilities::NumPosProtocols() const
	{
	return iNumPosProtocols;
	}


/**
Retrieves the details of a positioning protocol supported by the device.

@param aIndex Specifies the index of the positioning protocol to be
	retrieved.

@param aPosMethod On a successful return, aPosMethod contains
	the details of the positioning protocol at the index specified
	by aIndex

@return KErrNone if the assigment has been completed successfully.
@return KErrArgument if the parameter aIndex was out of range.
	


@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TInt TLbsNetPosCapabilities::GetPosProtocol
	(
	TInt aIndex,
	TUid& aPosProtocol
	) const
	{
	if (aIndex < 0 || aIndex >= iNumPosProtocols)
		{
		return KErrArgument;
		}
	
	aPosProtocol = iPosProtocol[aIndex];
	return KErrNone;
	}


/**
Returns the number of positioning methods supported by the device.

@return The number of positioning methods supported by the device.



@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TInt TLbsNetPosCapabilities::NumPosMethods() const
	{
	return iNumPosMethods;
	}


/**
Retrieves the details of a positioning method supported by the device.

@param aIndex Specifies the index of the positioning method to be
	retrieved.

@param aPosMethod On a successful return, aPosMethod contains
	the details of a positioning method at the index specified by
	aIndex

@return KErrNone if the assigment has been completed successfully.
@return KErrArgument if the parameter aIndex was out of range.
	


@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TInt TLbsNetPosCapabilities::GetPosMethod
	(
	TInt aIndex,
	TLbsNetPosMethod& aPosMethod
	) const
	{
	if (aIndex < 0 || aIndex >= iNumPosMethods)
		{
		return KErrArgument;
		}
	
	aPosMethod = iPosMethod[aIndex];
	return KErrNone;
	}


/**
Assigns the list of positioning protocols supported by the device.

@param aPosMethods The list of positioning protocols support by the device.
@param aNumPosMethods The number of protocols in the list.

@return KErrNone if the assigment has been completed successfully.
@return KErrArgument if the parameter aNumPosMethods was out of range.
	


@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TInt TLbsNetPosCapabilities::SetPosProtocols
	(
	const TUid aPosProtocols[],
	TInt aNumPosProtocols
	)
	{
	if (aNumPosProtocols < 0 || aNumPosProtocols > KLbsMaxNetPosProtocols)
		{
		return KErrArgument;
		}
	
	for (TInt index = 0; index < aNumPosProtocols; index++)
		{
		iPosProtocol[index] = aPosProtocols[index];
		}
		
	iNumPosProtocols = aNumPosProtocols;
	return KErrNone;
	}


/**
Assigns the list of positioning methods supported by the device.

@param aPosMethods The list of positioning methods support by the device.
@param aNumPosMethods The number of methods in the list.

@return KErrNone if the assigment has been completed successfully.
@return KErrArgument if the parameter aNumPosMethods was out of range.
	


@see MLbsLocationSourceObserver::GetCurrentCapabilities
*/
EXPORT_C TInt TLbsNetPosCapabilities::SetPosMethods
	(
	const TLbsNetPosMethod aPosMethods[],
	TInt aNumPosMethods
	)
	{
	if (aNumPosMethods < 0 || aNumPosMethods > KLbsMaxNetPosMethods)
		{
		return KErrArgument;
		}
	
	for (TInt index = 0; index < aNumPosMethods; index++)
		{
		iPosMethod[index] = aPosMethods[index];
		}
		
	iNumPosMethods = aNumPosMethods;
	return KErrNone;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosRequestMethod
//-----------------------------------------------------------------------------

/**
Constructor for the TLbsNetPosRequestMethod



@see MLbsLocationSourceObserver::ProcessLocationRequest
*/
EXPORT_C TLbsNetPosRequestMethod::TLbsNetPosRequestMethod()
	:
	iNumPosMethods(0),
	iGpsTimingOfCellFramesRequested(EFalse)
	{
 	iClassType = ELbsNetPosRequestMethodClass;
	iClassSize = sizeof(TLbsNetPosRequestMethod);
	Mem::FillZ(&iReserved, sizeof(iReserved));
	}

/**
Returns the number of positioning methods allowed by the network

@return An integer indicating the number of position methods
	that the network has indicated can be used in carrying out
	the position calculation. 
	


@see MLbsLocationSourceObserver::ProcessLocationRequest
*/
EXPORT_C TInt TLbsNetPosRequestMethod::NumPosMethods() const
	{
	return iNumPosMethods;
	}

/**
Returns the specified positioning method allowed by the network

@param aIndex Specifies the index of the position method to returned/
@param aPosMethod On successful completion, contains the details of
	the positioning methods held at the specified index.

@return KErrNone if the aIndex is within range and aPosMethod
	has been assigned a valid positioning method.
@return KErrArgument if the aIndex is out of range.
	


@see MLbsLocationSourceObserver::ProcessLocationRequest
*/
EXPORT_C TInt TLbsNetPosRequestMethod::GetPosMethod
	(
	TInt aIndex,
	TLbsNetPosMethod& aPosMethod
	) const
	{
	if (aIndex < 0 || aIndex >= iNumPosMethods)
		{
		return KErrArgument;
		}
	
	aPosMethod = iPosMethod[aIndex];
	return KErrNone;	
	}

/**
Assigns the prioritised list of positioning methods stipulated
by the network.

@param aPosMethods The prioritised list of positioning methods.
@param aNumPosMethods The number of methods in the prioritised list.

@return KErrNone if the assigment has been completed successfully.
@return KErrArgument if the parameter aNumPosMethods was out of range.
	


@see MLbsLocationSourceObserver::ProcessLocationRequest
*/
EXPORT_C TInt TLbsNetPosRequestMethod::SetPosMethods
	(
	const TLbsNetPosMethod aPosMethods[],
	TInt aNumPosMethods
	)
	{
	if (aNumPosMethods < 0 || aNumPosMethods > KLbsMaxNetPosMethods)
		{
		return KErrArgument;
		}
	
	for (TInt index = 0; index < aNumPosMethods; index++)
		{
		iPosMethod[index] = aPosMethods[index];
		}
		
	iNumPosMethods = aNumPosMethods;
	return KErrNone;
	}

/**
Returns flag indicating that the Gps Timing Of Cell Frames information has
been requeseted by the network.

@return Bool indicating if the data has been requested
*/
EXPORT_C TBool TLbsNetPosRequestMethod::GpsTimingOfCellFramesRequested() const
    {
    return iGpsTimingOfCellFramesRequested;
    }

/**
Sets flag indicating that the Gps Timing Of Cell Frames information has
been requeseted by the network.

@param aRequested Bool indicating if the data has been requested
*/
EXPORT_C void TLbsNetPosRequestMethod::SetGpsTimingOfCellFramesRequested(TBool aRequested)
    {
    iGpsTimingOfCellFramesRequested = aRequested;
    }

