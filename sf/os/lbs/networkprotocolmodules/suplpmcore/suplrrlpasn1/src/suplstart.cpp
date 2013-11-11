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
#include "suplstart.h"
#include "supldevloggermacros.h" 
#include "suplrrlpasn1common.h"
#include <math.h>

/**
Default minimum horizontal accuracy in meters
*/
const TInt KLbsSuplMinHorAccDefault = 20;


/**
Static factory constructor
*/
EXPORT_C CSuplStart* CSuplStart::NewL()
	{
	SUPLLOG(ELogP1, "CSuplStart::NewL() Begin\n");
	CSuplStart* self = CSuplStart::NewLC();
	SUPLLOG(ELogP1, "CSuplStart::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

/**
Static factory constructor
*/
EXPORT_C CSuplStart* CSuplStart::NewLC()
	{
	SUPLLOG(ELogP1, "CSuplStart::NewL() Begin\n");
	CSuplStart* self = new (ELeave) CSuplStart();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplStart::NewLC() End\n");
	return self;
	}

/** 
Constructor 
*/
CSuplStart::CSuplStart()
 : CSuplMessageBase::CSuplMessageBase(ESuplStart, ETrue)
	{
	SUPLLOG(ELogP1, "CSuplMessageBase::CSuplMessageBase() Begin\n");
	SUPLLOG(ELogP1, "CSuplMessageBase::CSuplMessageBase() End\n");
	}
	

/** 
Second stage constructor 
*/
void CSuplStart::ConstructL()
	{
	// call the base class ConstructL() to create data structures
	CSuplMessageBase::ConstructL();

	// local reference to context object
	OSCTXT* pctxt = iControl->getCtxtPtr();

	iData->message.t =  T_UlpMessage_msSUPLSTART;
	iData->message.u.msSUPLSTART = (ASN1T_SUPLSTART*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SUPLSTART));
   	LeaveIfAllocErrorL();
	}
	

/**
Destructor
*/
CSuplStart::~CSuplStart()
	{
	SUPLLOG(ELogP1, "CSuplStart::~CSuplStart() Begin\n");
	SUPLLOG(ELogP1, "CSuplStart::~CSuplStart() End\n");
	}


/** 
SetCapabilities()

Populates the SET Capabilities parameter of the outgoing SUPL START message.

SET Capabilities
	- Pos Technology (m) Bitmask indicating technologies the SET supports
	- Pref Method (m)    which GPS mode the SET prefers (TA, TB, noPref)
	- Pos Protocol (m)   which underlying positioning protocols are supported

@param  aCapabilities, current capabilities reported by LBS subsystem
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplStart::SetCapabilities(TLbsNetPosCapabilities& aCapabilities)
	{
	SUPLLOG(ELogP1, "CSuplStart::SetCapabilities() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLSTART != NULL, User::Invariant());
	
	TInt err = PopulateSetCapabilities(aCapabilities, iData->message.u.msSUPLSTART->sETCapabilities);

	SUPLLOG(ELogP1, "CSuplStart::SetCapabilities() End\n");
	return err;
	}
	

/**	
SetLocationId()

Set the location ID parameter 

Location ID - Describes the globally unique cell identification of the most current serving cell.
	Cell Info (m) The following cell IDs are supported:
		- GSM Cell Info
		- WCDMA Cell Info
		- CDMA Cell Info
	Status (m) Describes whether or not the cell info is:
		- Not Current, last known cell info
		- Current, the present cell info
		- Unknown (ie not known whether the cell id is current or not current)

Note: The Status parameter does NOT apply to WCDMA optional parameters 
(Frequency Info, Primary Scrambling Code and Measured Results List). Frequency
Info, Primary Scrambling Code and Measured Results List, if present, are always
considered to be correct for the current cell.

@param  aLocationId, describes the current location
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplStart::SetLocationId(const CSuplLocationId& aLocationId)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLSTART != NULL, User::Invariant());
	SUPLLOG(ELogP1, "CSuplStart::SetLocationId() Begin\n");
	
	TInt err = PopulateLocationId(aLocationId, iData->message.u.msSUPLSTART->locationId);

	SUPLLOG(ELogP1, "CSuplStart::SetLocationId() End\n");
	return err;
	}


/**	
SetQoP()

Set the QoP parameter 

QoP - the desired quality of position
	horacc    (m) (0..127)
	veracc    (o) (0..127)
	maxLocAge (o) (0..65535)
	delay     (o) (0..7)

@param  aQuality, the required quality of position
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplStart::SetQoP(TLbsNetPosRequestQuality& aQuality)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLSTART != NULL, User::Invariant());
	SUPLLOG(ELogP1, "CSuplStart::SetQoP() Begin\n");

	// pointer to message body
	ASN1T_SUPLSTART* msgBody = iData->message.u.msSUPLSTART;
	
	// local reference to context object
	iControl->getCtxtPtr();
	
	// indicate that the optional QoP component is present
   	msgBody->m.qoPPresent = 1;
	
	// Horizontal accuracy
	if (aQuality.MinHorizontalAccuracy() != 0)
		{
		msgBody->qoP.horacc = Uncertainty(aQuality.MinHorizontalAccuracy());
		}
	else
		{
		msgBody->qoP.horacc = Uncertainty(KLbsSuplMinHorAccDefault);
		}
	
	// vertical accuracy
	if (aQuality.MinVerticalAccuracy() != 0)
		{
		msgBody->qoP.veracc = UncertaintyAltitude(aQuality.MinVerticalAccuracy());
		msgBody->qoP.m.veraccPresent = 1;
		}
		
	// max location age / max fix age
	TTimeIntervalMicroSeconds maxAge = aQuality.MaxFixAge();
	if (maxAge.Int64() != 0)
		{
		TInt64 maxAgeSeconds = maxAge.Int64() / KLbsMilliSeconds;
		if (maxAgeSeconds > 65535)
			{
			// limit to 16 bits (65535 = 2^16 - 1)
			maxAgeSeconds = 65535;
			}
		msgBody->qoP.maxLocAge = maxAgeSeconds;
		msgBody->qoP.m.maxLocAgePresent = 1;
		}
	
	// delay / max fix time
	TTimeIntervalMicroSeconds maxFixTime = aQuality.MaxFixTime();
	if (maxFixTime.Int64() != 0)
		{
		msgBody->qoP.m.delayPresent = 1;
		TInt seconds = maxFixTime.Int64() / KLbsMilliSeconds;
		if (seconds <= 1)
			{
			msgBody->qoP.delay = 0;
			}
		else if (seconds <= 2)
			{
			msgBody->qoP.delay = 1;
			}
		else if (seconds <= 4)
			{
			msgBody->qoP.delay = 2;
			}
		else if (seconds <= 8)
			{
			msgBody->qoP.delay = 3;
			}
		else if (seconds <= 16)
			{
			msgBody->qoP.delay = 4;
			}
		else if (seconds <= 32)
			{
			msgBody->qoP.delay = 5;
			}
		else if (seconds <= 64)
			{
			msgBody->qoP.delay = 6;
			}
		else
			{
			// maximum value
			msgBody->qoP.delay = 7;
			}
		}

	SUPLLOG(ELogP1, "CSuplStart::SetQoP() End\n");
	return KErrNone;
	}
	
