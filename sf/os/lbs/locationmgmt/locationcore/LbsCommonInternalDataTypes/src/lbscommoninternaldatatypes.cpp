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


#include "lbscommoninternaldatatypes.h"

/**
@file
@internalTechnology
@released
*/

//------------------------------------------------------------------------------
// TLbsNetClassTypeBaseInt 
//------------------------------------------------------------------------------

/**
 * Default constructor for TLbsNetClassTypeBaseInt
 */
EXPORT_C TLbsNetClassTypeBaseInt::TLbsNetClassTypeBaseInt()
:	iClassType(0),
	iClassSize(0)
	{}

/**
 * @return The size of the TPositionClassTypeBase-derived class
 */
EXPORT_C TUint TLbsNetClassTypeBaseInt::ClassSize() const
	{
	return iClassSize;
	}

/**
 * @return The type of the TPositionClassTypeBase-derived class
 */
EXPORT_C TUint32 TLbsNetClassTypeBaseInt::ClassType() const
	{
	return iClassType;
	}

//------------------------------------------------------------------------------
// TLbsNetSessionIdInt
//------------------------------------------------------------------------------

/**
Default constructor for TLbsNetSessionIdInt

Sets the session owner to TUid::Null() and the session request number to 0
*/
EXPORT_C TLbsNetSessionIdInt::TLbsNetSessionIdInt()
	:
	iSessionOwner(TUid::Null()),
	iSessionNum(0)
	{
	iClassType = ELbsNetSessionIdClassInt;
	iClassSize = sizeof(TLbsNetSessionIdInt);
	}

/**
Constructor for TLbsNetSessionIdInt
@param aSessionOwner The session owner
@param aSessionNum The request number
*/
EXPORT_C TLbsNetSessionIdInt::TLbsNetSessionIdInt(TUid aSessionOwner, TSessionNum aSessionNum)
	:
	iSessionOwner(aSessionOwner),
	iSessionNum(aSessionNum)
	{
	iClassType = ELbsNetSessionIdClassInt;
	iClassSize = sizeof(TLbsNetSessionIdInt);
	}
	
/**
Get the owner of the session
@return The session owner
*/
EXPORT_C TUid TLbsNetSessionIdInt::SessionOwner() const
	{
	return iSessionOwner;
	}

/**
Get the request number
@return The request number
*/
EXPORT_C TLbsNetSessionIdInt::TSessionNum TLbsNetSessionIdInt::SessionNum() const
	{
	return iSessionNum;
	}

/**
Set the session owner
@param aSessionOwner The session owner
*/
EXPORT_C void TLbsNetSessionIdInt::SetSessionOwner(TUid aSessionOwner)
	{
	iSessionOwner = aSessionOwner;
	}

/**
Set the request number
@param aSessionNum The request number
*/
EXPORT_C void TLbsNetSessionIdInt::SetSessionNum(TSessionNum aSessionNum)
	{
	iSessionNum = aSessionNum;
	}

/**
Increments the request number
*/
EXPORT_C void TLbsNetSessionIdInt::IncrSession()
	{
	iSessionNum++;
	}

/**
Tests if two session Ids are equal
@return True if session Ids are equal
*/
EXPORT_C TBool TLbsNetSessionIdInt::operator==(const TLbsNetSessionIdInt& aSessionId) const
	{
	return (iSessionOwner == aSessionId.SessionOwner()) &&
			 (iSessionNum   == aSessionId.SessionNum());
	}

/**
Tests if two session Ids are not equal
@return True if session Ids are not equal
*/
EXPORT_C TBool TLbsNetSessionIdInt::operator!=(const TLbsNetSessionIdInt& aSessionId) const
	{
	return (iSessionOwner != aSessionId.SessionOwner()) ||
			 (iSessionNum   != aSessionId.SessionNum());
	}


//------------------------------------------------------------------------------
// TLbsNetSessionIdArrayInt
//------------------------------------------------------------------------------

/**
Default constructor for TLbsNetSessionIdArrayInt

Sets the session owner to TUid::Null() and the session request number to 0
*/
EXPORT_C TLbsNetSessionIdArrayInt::TLbsNetSessionIdArrayInt()
	:
	iNumSessions(0)
	{
	iClassType = ELbsNetSessionIdArrayClassInt;
	iClassSize = sizeof(TLbsNetSessionIdArrayInt);
	}

	
/**
Returns the number of @see TLbsNetSessionId objects.
Currently not supported.

@return An integer indicating the number of @see TLbsNetSessionId
   session ids.
*/
EXPORT_C TInt TLbsNetSessionIdArrayInt::NumSessions() const
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
EXPORT_C TInt TLbsNetSessionIdArrayInt::GetSession
	(
	TInt /*aIndex*/, 
	TLbsNetSessionIdInt& /*aSessionId*/
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
EXPORT_C TInt TLbsNetSessionIdArrayInt::SetSessions
	(
	const TLbsNetSessionIdInt /*aSessions*/[], 
	TInt /*aNumSessions*/
	)
	{
	return KErrNotSupported;
#pragma message("TBD")
	}
	

//-----------------------------------------------------------------------------
// TLbsNetPosRequestPrivacyInt
//-----------------------------------------------------------------------------

/**
 * Constructor for the TLbsNetPosRequestPrivacyInt
 * @see CLbsLocationSourceGpsBase
 */
EXPORT_C TLbsNetPosRequestPrivacyInt::TLbsNetPosRequestPrivacyInt()
	:
	iAction(ERequestActionNotUsed),
	iAdvice(ERequestAdviceNotUsed)
	{
 	iClassType = ELbsNetPosRequestPrivacyClassInt;
	iClassSize = sizeof(TLbsNetPosRequestPrivacyInt);
	}
	

/**
Returns the network's recomendation on whether the location request should be
accepted or rejected. In the situation where the user is consulted, the
action is the default behaviour when the user does not respond.

@see RequestAdvice
@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest
*/
EXPORT_C TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt TLbsNetPosRequestPrivacyInt::RequestAction() const
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
EXPORT_C TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt TLbsNetPosRequestPrivacyInt::RequestAdvice() const
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
EXPORT_C void TLbsNetPosRequestPrivacyInt::SetRequestAction(TLbsRequestActionInt aAction)
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
EXPORT_C void TLbsNetPosRequestPrivacyInt::SetRequestAdvice(TLbsRequestAdviceInt aAdvice)
	{
	iAdvice = aAdvice;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosRequestQualityInt
//-----------------------------------------------------------------------------

/**
 * Constructor for the TLbsNetPosRequestQualityInt
 *
 * @see CLbsLocationSourceGpsBase
 */
EXPORT_C TLbsNetPosRequestQualityInt::TLbsNetPosRequestQualityInt()
	:
	iMinHorizontalAccuracy(0),
	iMinVerticalAccuracy(0),
	iMaxFixTime(0),
	iMaxFixAge(0)
	{
 	iClassType = ELbsNetPosRequestQualityClassInt;
	iClassSize = sizeof(TLbsNetPosRequestQualityInt);
	}

/**
The maximum time the network is prepared to wait while
lbs subsystem calculates the current location.

@see SetMaxFixTime

@return The maximum time that a data source should spend calculating
the current location.
*/
EXPORT_C TTimeIntervalMicroSeconds  TLbsNetPosRequestQualityInt::MaxFixTime() const
	{
	return iMaxFixTime;
	}

/**
The maximum age of a location fix that is acceptable to the network.

@see SetMaxFixAge

@return The maximum age of a location fix that can be returned to the
network.
*/
EXPORT_C TTimeIntervalMicroSeconds  TLbsNetPosRequestQualityInt::MaxFixAge() const
	{
	return iMaxFixAge;
	}

/**
 * The desired minimum horizontal accuracy for a location update.
 *
 * @see SetMaxFixTime
 *
 * @return The desired minimum horizontal accuracy for a location update in metres.
 */

EXPORT_C TReal32 TLbsNetPosRequestQualityInt::MinHorizontalAccuracy() const
	{
	return iMinHorizontalAccuracy;
	}

/**
 * The desired minimum vertical accuracy for a location update.
 *
 * @see SetMaxFixTime
 *
 * @return The desired minimum vertical accuracy for a location update in metres.
 */
EXPORT_C TReal32 TLbsNetPosRequestQualityInt::MinVerticalAccuracy() const
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
 * @see MaxFixTime
 *
 * @param aTime specifies the maximum time that the data source should spend
 * calculating the current location.
 */
EXPORT_C void TLbsNetPosRequestQualityInt::SetMaxFixTime(TTimeIntervalMicroSeconds aTime)
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
EXPORT_C void TLbsNetPosRequestQualityInt::SetMaxFixAge(TTimeIntervalMicroSeconds aTime)
	{
	iMaxFixAge = aTime;
	}

/**
 * The desired minimum horizontal accuracy for a location update.
 * When used in conjunction with SetMaxFixTime the data source will
 * produce an update either when the desired accuracy has be met or
 * when the time limit is reached.
 *
 * @see SetMaxFixTime
 *
 * @param aAccuracy specifies the desired minimum horizontal accuracy
 * (in metres) for a location update produced by the data source. 
 */
EXPORT_C void TLbsNetPosRequestQualityInt::SetMinHorizontalAccuracy(TReal32 aAccuracy)
	{
	iMinHorizontalAccuracy = aAccuracy;
	}

/**
 * The desired minimum vertical accuracy for a location update.
 * When used in conjunction with SetMaxFixTime the data source will
 * produce an update either when the desired accuracy has be met or
 * when the time limit is reached.
 *
 * @see SetMaxFixTime
 *
 * @param aAccuracy specifies the desired minimum vertical accuracy
 * (in metres) for a location update produced by the data source. 
 */
EXPORT_C void TLbsNetPosRequestQualityInt::SetMinVerticalAccuracy(TReal32 aAccuracy)
	{
	iMinVerticalAccuracy = aAccuracy;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosRequestOptionsInt and Family
//-----------------------------------------------------------------------------

/**
Constructor for the TLbsNetPosRequestOptionsInt

This class is used to encapsulated the common parameters for the terminal
issuing a network and a self location request.

@see TLbsNetPosRequestOptionsIntAssistance
@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TLbsNetPosRequestOptionsInt::TLbsNetPosRequestOptionsInt()
	{
	iClassType = ELbsNetPosRequestOptionsClassInt;
	iClassSize = sizeof(TLbsNetPosRequestOptionsInt);	
	}

/**
SetNewClientConnected should be set to ETrue is there is at least one
local client application that is requesting location information for
the first time.

@see CLbsNetworkProtocolBase::RequestNetworkLocation
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsInt::SetNewClientConnected
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
EXPORT_C void TLbsNetPosRequestOptionsInt::SetRequestQuality
	(
	const TLbsNetPosRequestQualityInt& aQuality
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
EXPORT_C TBool TLbsNetPosRequestOptionsInt::NewClientConnected() const
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
EXPORT_C void TLbsNetPosRequestOptionsInt::GetRequestQuality
	(
	TLbsNetPosRequestQualityInt& aQuality
	) const
	{
	aQuality = iQuality;
	}

/**
Constructor for the TLbsNetPosRequestOptionsAssistanceInt

This class is used to encapsulated the common parameters for the terminal
issuing a self location request.

@see TLbsNetPosRequestOptions
@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TLbsNetPosRequestOptionsAssistanceInt::TLbsNetPosRequestOptionsAssistanceInt()
	{
	iClassType = ELbsNetPosRequestOptionsAssistanceClassInt;
	iClassSize = sizeof(TLbsNetPosRequestOptionsAssistanceInt);		
	}

/**
Specifies the desired set of assistance data that should be obtained
from the network. A bit mask of zero indicates that no assistance
data is requried.

@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsAssistanceInt::SetDataRequestMask
	(
	TLbsAsistanceDataGroupInt aDataRequestMask
	)
	{
	iDataRequestMask = aDataRequestMask;
	}

/**
Specifies the current positioning mode for the location being requested
by the LBS subsystem

@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsAssistanceInt::SetPosMode(TPositionModuleInfoInt::TTechnologyTypeInt aPosMode)
	{
	iPosMode = aPosMode;
	}

/**
Returns the desired set of assistance data that should be obtained
from the network. A bit mask of zero indicates that no assistance
data is requried.

@see CLbsNetworkProtocolBase::RequestSelfLocation
*/

EXPORT_C TLbsAsistanceDataGroupInt TLbsNetPosRequestOptionsAssistanceInt::DataRequestMask() const
	{
	return iDataRequestMask;
	}

/**
Returns the current mode of the module requesting location.

@see CLbsNetworkProtocolBase::RequestSelfLocation
*/
EXPORT_C TPositionModuleInfoInt::TTechnologyTypeInt TLbsNetPosRequestOptionsAssistanceInt::PosMode() const
	{
	return iPosMode;
	}

/**
Constructor for the TLbsNetPosRequestOptionsTechnologyInt

This class is used to encapsulate the common parameters for the terminal
issuing a self location request.

@see TLbsNetPosRequestOptions
@see CLbsNetworkProtocolBase::RequestTransmitLocation
*/
EXPORT_C TLbsNetPosRequestOptionsTechnologyInt::TLbsNetPosRequestOptionsTechnologyInt()
	{
	iClassType = ELbsNetPosRequestOptionsTechnologyClassInt;
	iClassSize = sizeof(TLbsNetPosRequestOptionsTechnologyInt);
	}

/**
Specifies the current positioning mode for the location being requested
by the LBS subsystem

@see CLbsNetworkProtocolBase::RequestTransmitLocation
*/
EXPORT_C void TLbsNetPosRequestOptionsTechnologyInt::SetPosMode(TPositionModuleInfoInt::TTechnologyTypeInt aPosMode)
	{
	iPosMode = aPosMode;
	}

/**
Returns the current mode of the module requesting location.

@see CLbsNetworkProtocolBase::RequestTransmitLocation
*/
EXPORT_C TPositionModuleInfoInt::TTechnologyTypeInt TLbsNetPosRequestOptionsTechnologyInt::PosMode() const
	{
	return iPosMode;
	}


//-----------------------------------------------------------------------------
// TLbsNetPosMethodInt
//-----------------------------------------------------------------------------

/**
Default Constructor for the TLbsNetPosMethodInt

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
EXPORT_C TLbsNetPosMethodInt::TLbsNetPosMethodInt()
	:
	iPosMeans(TUid::Null()),
	iPosMode(TPositionModuleInfoInt::ETechnologyUnknown)
	{
 	iClassType = ELbsNetPosMethodClassInt;
	iClassSize = sizeof(TLbsNetPosMethodInt);
	}

/**
The positioning technology (means)

Currently only GPS is support. This is indicated by the value KLbsPositioningMeansGps

@return The positioning technology (means)
*/
EXPORT_C TUid TLbsNetPosMethodInt::PosMeans() const
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
EXPORT_C TPositionModuleInfoInt::TTechnologyTypeInt TLbsNetPosMethodInt::PosMode() const
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

@see TLbsNetPosMethodInt()
@see PosMode()
@see PosMeans()
@param aPosMeans The positioning technology. This must be KLbsPositioningMeansGps
@param aPosMode  The positioning operational mode
*/
EXPORT_C void TLbsNetPosMethodInt::SetPosMethod(TUid aPosMeans, TPositionModuleInfoInt::TTechnologyTypeInt aPosMode)
	{
	iPosMeans = aPosMeans;
	iPosMode = aPosMode;
	}

//-----------------------------------------------------------------------------
// TLbsNetPosRequestMethodInt
//-----------------------------------------------------------------------------

/**
Constructor for the TLbsNetPosRequestMethodInt
@see MLbsLocationSourceObserver::ProcessLocationRequest
*/
EXPORT_C TLbsNetPosRequestMethodInt::TLbsNetPosRequestMethodInt()
	:
	iNumPosMethods(0),
	iGpsTimingOfCellFramesRequested(EFalse)
	{
 	iClassType = ELbsNetPosRequestMethodClassInt;
	iClassSize = sizeof(TLbsNetPosRequestMethodInt);
	}

/**
Returns the number of positioning methods allowed by the network

@return An integer indicating the number of position methods
	that the network has indicated can be used in carrying out
	the position calculation. 
	
@see MLbsLocationSourceObserver::ProcessLocationRequest
*/
EXPORT_C TInt TLbsNetPosRequestMethodInt::NumPosMethods() const
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
EXPORT_C TInt TLbsNetPosRequestMethodInt::GetPosMethod
	(
	TInt aIndex,
	TLbsNetPosMethodInt& aPosMethod
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
EXPORT_C TInt TLbsNetPosRequestMethodInt::SetPosMethods
	(
	const TLbsNetPosMethodInt aPosMethods[],
	TInt aNumPosMethods
	)
	{
	if (aNumPosMethods < 0 || aNumPosMethods > KLbsMaxNetPosMethodsInt)
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
EXPORT_C TBool TLbsNetPosRequestMethodInt::GpsTimingOfCellFramesRequested() const
    {
    return iGpsTimingOfCellFramesRequested;
    }

/**
Sets flag indicating that the Gps Timing Of Cell Frames information has
been requeseted by the network.

@param aRequested Bool indicating if the data has been requested
*/
EXPORT_C void TLbsNetPosRequestMethodInt::SetGpsTimingOfCellFramesRequested(TBool aRequested)
    {
    iGpsTimingOfCellFramesRequested = aRequested;
    }


//------------------------------------------------------------------------------
// TLbsClassTypeBaseInt. 
//------------------------------------------------------------------------------

EXPORT_C TLbsLocClassTypeBaseInt::TLbsLocClassTypeBaseInt()
/** Default constructor for TLbsLocClassTypeBaseInt
*/
:	iClassType(0),
	iClassSize(0)
	{}

EXPORT_C TUint TLbsLocClassTypeBaseInt::ClassSize() const
/** Returns the size of the TPositionClassTypeBase-derived class
@return The size of the TPositionClassTypeBase-derived class
*/
	{
	return iClassSize;
	}


EXPORT_C TUint32 TLbsLocClassTypeBaseInt::ClassType() const
/**  Returns the type of the TPositionClassTypeBase-derived class
@return The type of the TPositionClassTypeBase-derived class
 */
	{
	return iClassType;
	}


//-----------------------------------------------------------------------------
// TLbsLocRequestQualityInt
//-----------------------------------------------------------------------------

EXPORT_C TLbsLocRequestQualityInt::TLbsLocRequestQualityInt()
	:
	iMinHorizontalAccuracy(0),
	iMinVerticalAccuracy(0),
	iMaxFixTime(0)
	{
	}

EXPORT_C TTimeIntervalMicroSeconds  TLbsLocRequestQualityInt::MaxFixTime() const
	{
	return iMaxFixTime;
	}

EXPORT_C TReal32 TLbsLocRequestQualityInt::MinHorizontalAccuracy() const
	{
	return iMinHorizontalAccuracy;
	}

EXPORT_C TReal32 TLbsLocRequestQualityInt::MinVerticalAccuracy() const
	{
	return iMinVerticalAccuracy;
	}

EXPORT_C void TLbsLocRequestQualityInt::SetMaxFixTime(TTimeIntervalMicroSeconds aTime)
	{
	iMaxFixTime = aTime;
	}

EXPORT_C void TLbsLocRequestQualityInt::SetMinHorizontalAccuracy(TReal32 aAccuracy)
	{
	iMinHorizontalAccuracy = aAccuracy;
	}

EXPORT_C void TLbsLocRequestQualityInt::SetMinVerticalAccuracy(TReal32 aAccuracy)
	{
	iMinVerticalAccuracy = aAccuracy;
	}

	
// End of File
