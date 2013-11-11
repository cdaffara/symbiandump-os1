// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ULP.h"
#include "ASN1TTime.h"
#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include "suplend.h"
#include "supldevloggermacros.h" 

/**
Static factor constructor
*/
EXPORT_C CSuplEnd* CSuplEnd::NewL(TBool aIsOutgoingMessage)
	{
	SUPLLOG(ELogP1, "CSuplEnd::NewL() Begin\n");
	CSuplEnd* self = CSuplEnd::NewLC(aIsOutgoingMessage);
	SUPLLOG(ELogP1, "CSuplEnd::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplEnd* CSuplEnd::NewLC(TBool aIsOutgoingMessage)
	{
	SUPLLOG(ELogP1, "CSuplEnd::NewLC() Begin\n");
	CSuplEnd* self = new (ELeave) CSuplEnd(aIsOutgoingMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplEnd::NewLC() End\n");
	return self;
	}

/** 
Constructor 
*/
CSuplEnd::CSuplEnd(TBool aIsOutgoingMessage)
 : CSuplMessageBase::CSuplMessageBase(ESuplEnd, aIsOutgoingMessage)
	{
	}


/** 
Second stage constructor 
*/
void CSuplEnd::ConstructL()
	{
	// call the base class ConstructL() to create common data structures
	CSuplMessageBase::ConstructL();

	// message specific structures for outgoing messages.
	if (iIsOutgoingMessage)
		{
		// local reference to context object
		OSCTXT* pctxt = iControl->getCtxtPtr();

		// create the SUPL POS specific data structures
		iData->message.t =  T_UlpMessage_msSUPLEND;
		iData->message.u.msSUPLEND = (ASN1T_SUPLEND*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SUPLEND));
   		LeaveIfAllocErrorL();
		}
	}
		
/**
Destructor
*/
CSuplEnd::~CSuplEnd()
	{
	SUPLLOG(ELogP1, "CSuplEnd::~CSuplEnd() Begin\n");
	SUPLLOG(ELogP1, "CSuplEnd::~CSuplEnd() End\n");
	}
	

/**
SetPosition()

Set the Position parameter

@param  aPosInfo, position info as provided by LBS subsystem
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplEnd::SetPosition(const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplEnd::SetPosition() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLEND != NULL, User::Invariant());
	
	// flag that the optional position parameter is present
	iData->message.u.msSUPLEND->m.positionPresent = 1;

	SUPLLOG(ELogP1, "CSuplEnd::SetPosition() End (Calling PopulatePosition())\n");
	return PopulatePosition(aPosInfo, iData->message.u.msSUPLEND->position);
	}

	
/** 
SetStatusCode()

Set the SUPL Status Code 

@param  aStatusCode, the status code for session termination
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplEnd::SetStatusCode(TSuplStatusCode& aStatusCode)
	{
	SUPLLOG(ELogP1, "CSuplEnd::SetStatusCode() Begin\n");
	SUPLLOG2(ELogP1, "    - TSuplStatusCode aStatusCode  = %d\n", aStatusCode);
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLEND != NULL, User::Invariant());

	// flag that the optional status code parameter is present and set param
	iData->message.u.msSUPLEND->m.statusCodePresent = 1;
	iData->message.u.msSUPLEND->statusCode = aStatusCode;
	
	SUPLLOG(ELogP1, "CSuplEnd::SetStatusCode() End\n");
	return KErrNone;
	}

	

/** 
SetVer()

Sets the Ver parameter, a hash of the received SUPL INIT message of the
outgoing message

@param aVer, pointer to buffer containing the hash of the SUPL INIT message
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplEnd::SetVer(const TDesC8& aVer)
	{
	SUPLLOG(ELogP1, "CSuplEnd::SetVer() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLEND != NULL, User::Invariant());

	ASN1T_SUPLEND& suplEnd = *iData->message.u.msSUPLEND;
	suplEnd.m.verPresent = 1;
	suplEnd.ver.numbits = 64;
	TPtr8 target(suplEnd.ver.data, 8);
	target.Copy(aVer);

	SUPLLOG(ELogP1, "CSuplEnd::SetVer() End\n");
	return KErrNone;
	}

/** 
PositionPresent()

@return ETrue if the position parameter is present 
*/
EXPORT_C TBool CSuplEnd::PositionPresent()
	{
	SUPLLOG(ELogP1, "CSuplEnd::PositionPresent() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLEND != NULL, User::Invariant());

	if (iData->message.u.msSUPLEND->m.positionPresent != 0)
		{
		SUPLLOG(ELogP1, "CSuplEnd::PositionPresent(ETrue) End\n");
		return ETrue;
		}
	SUPLLOG(ELogP1, "CSuplEnd::PositionPresent(EFalse) End\n");
	return EFalse;
	}

	
/** 
Postion()

Retrieve the received position information.

@param  aPosition, on return populated with the received position information
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplEnd::GetPosition(TPosition& aPosition)
	{
	SUPLLOG(ELogP1, "CSuplEnd::Position() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLEND != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLEND->m.positionPresent != 0)
		{
		ASN1T_Position& receivedPos = iData->message.u.msSUPLEND->position;

		TTime time(0);
		TInt err = DecodeTime((TUint8*)receivedPos.timestamp, time);

		aPosition.SetTime(time);
		
		TReal64 latitude  = receivedPos.positionEstimate.latitude;
		latitude *= 90;
		latitude /= 8388608; // 2^23
		if (receivedPos.positionEstimate.latitudeSign == PositionEstimate_latitudeSign::south)
			{
			latitude *= -1;
			}
		TReal64 longitude = receivedPos.positionEstimate.longitude;
		longitude *= 360;
		longitude /= 16777216;
		aPosition.SetCoordinate(latitude, longitude);
		
		// horizontal uncertainty
		// from 3gpp ts 23.032:   uncert = C( (1+x)^k - 1 ), C = 10, x = 0.1
		if (receivedPos.positionEstimate.m.uncertaintyPresent != 0)
			{
			TUint k = receivedPos.positionEstimate.uncertainty.uncertaintySemiMajor;
			TReal uncert;
			Math::Pow(uncert, 1.1, k);
			uncert -= 1;
			uncert *= 10;
			aPosition.SetHorizontalAccuracy(uncert);
			}
	
		// vertical position
		if (receivedPos.positionEstimate.m.altitudeInfoPresent != 0)
			{
			// actual altitude in meters
			TReal32 altitude = receivedPos.positionEstimate.altitudeInfo.altitude;
			// direction
			if (receivedPos.positionEstimate.altitudeInfo.altitudeDirection == AltitudeInfo_altitudeDirection::depth)
				{
				altitude *= -1;
				}
			aPosition.SetCoordinate(latitude, longitude, altitude);
			
			// altitude uncertainty 
			// from 3gpp ts 23.032:   altUncert = C( (1+x)^k - 1 ), C = 45, x = 0.025
			TUint kAlt = receivedPos.positionEstimate.altitudeInfo.altUncertainty;
			TReal altUncert;
			Math::Pow(altUncert, 1.025, kAlt);
			altUncert -= 1;
			altUncert *= 45;
			aPosition.SetVerticalAccuracy(altUncert);
			}
		}
	else
		{
		return KErrNotFound;
		}
	
	SUPLLOG(ELogP1, "CSuplEnd::Position() End\n");
	return KErrNone;
	}

	
/** 
StatusCodePresent()

@return ETrue if the status code parameter is present 
*/
EXPORT_C TBool CSuplEnd::StatusCodePresent()
	{
	SUPLLOG(ELogP1, "CSuplEnd::StatusCodePresent() Begin\n");
	if (iData->message.u.msSUPLEND->m.statusCodePresent != 0)
		{
		SUPLLOG(ELogP1, "CSuplEnd::StatusCodePresent(ETrue) End\n");
		return ETrue;
		}

	SUPLLOG(ELogP1, "CSuplEnd::StatusCodePresent(EFalse) End\n");
	return EFalse;
	}


/** 
StatusCode()

@return the SUPL status code
*/
EXPORT_C TSuplStatusCode CSuplEnd::StatusCode()
	{
	SUPLLOG(ELogP1, "CSuplEnd::StatusCode() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLEND != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLEND->m.statusCodePresent != 0)
		{
		SUPLLOG2(ELogP1, "CSuplEnd::StatusCode() End (Status Code == %d\n", iData->message.u.msSUPLEND->statusCode);
		return (TSuplStatusCode)iData->message.u.msSUPLEND->statusCode;
		}
	
	SUPLLOG(ELogP1, "CSuplEnd::StatusCode() End (Status Code not present)\n");
	return ESuplStatusUnspecified;
	}



TInt CSuplEnd::DecodeTime(const TUint8* aTimeString, TTime& aTimeOut)
	{
	SUPLLOG(ELogP1, "CSuplEnd::DecodeTime() Begin\n");
	
	TInt err = KErrNone;
	
	ASN1TUTCTime t;
	err = t.parseString((const char*)aTimeString);
	if (err != KErrNone)
		{
		return err;
		}

	TInt month = t.getMonth();

	TMonth monthEnum;
	switch (month)
		{
		case 1:  monthEnum = EJanuary; break;
		case 2:  monthEnum = EFebruary; break;
		case 3:  monthEnum = EMarch; break;
		case 4:  monthEnum = EApril; break;
		case 5:  monthEnum = EMay; break;
		case 6:  monthEnum = EJune; break;
		case 7:  monthEnum = EJuly; break;
		case 8:  monthEnum = EAugust; break;
		case 9:  monthEnum = ESeptember; break;
		case 10: monthEnum = EOctober; break;
		case 11: monthEnum = ENovember; break;
		case 12: monthEnum = EDecember; break;
		default:
			__ASSERT_DEBUG(0, User::Invariant());
			return KErrArgument;
		}
	
	//Get the day value, subtract 1 from it to convert to internal day values (0-...)
	TInt day = t.getDay() - 1;
	
	TDateTime dateTime(t.getYear(), monthEnum, day, t.getHour(), t.getMinute(), t.getSecond(), 0);
	aTimeOut = dateTime;
	
	SUPLLOG(ELogP1, "CSuplEnd::DecodeTime() End\n");
	return KErrNone;
	}


