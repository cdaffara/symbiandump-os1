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

#include "RRLP-Messages.h"
#include "ULP.h"

#include "suplpos.h"
#include "suplpospayload.h"
#include "rrlpmeasureposrequest.h"
#include "rrlpassistancedata.h"
#include "rrlpprotocolerror.h"
#include "suplasn1error.h"
#include "supldevloggermacros.h" 

#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>

/** 
Static factory constructor 
*/
EXPORT_C CSuplPos* CSuplPos::NewL(TBool aIsOutgoingMessage)
	{
	SUPLLOG(ELogP1, "CSuplPos::NewL() Begin\n");
	CSuplPos* self = CSuplPos::NewLC(aIsOutgoingMessage);
	SUPLLOG(ELogP1, "CSuplPos::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSuplPos* CSuplPos::NewLC(TBool aIsOutgoingMessage)
	{
	SUPLLOG(ELogP1, "CSuplPos::NewLC() Begin\n");
	CSuplPos* self = new (ELeave) CSuplPos(aIsOutgoingMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplPos::NewLC() End\n");
	return self;
	}


/** 
Constructor 
*/
CSuplPos::CSuplPos(TBool aIsOutgoingMessage)
 : CSuplMessageBase::CSuplMessageBase(ESuplPos, aIsOutgoingMessage)
	{
	}


/** 
Second stage constructor 
*/
void CSuplPos::ConstructL()
	{
	// call the base class ConstructL() to create common data structures
	CSuplMessageBase::ConstructL();

	// message specific structures for outgoing messages.
	if (iIsOutgoingMessage)
		{
		// local reference to context object
		OSCTXT* pctxt = iControl->getCtxtPtr();

		// create the SUPL POS specific data structures
		iData->message.t =  T_UlpMessage_msSUPLPOS;
		iData->message.u.msSUPLPOS = (ASN1T_SUPLPOS*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SUPLPOS));
   		LeaveIfAllocErrorL();
		
		iData->message.u.msSUPLPOS->m.velocityPresent = 0;
		
		iData->message.u.msSUPLPOS->posPayLoad.t = T_PosPayLoad_rrlpPayload;
		iData->message.u.msSUPLPOS->posPayLoad.u.rrlpPayload = (ASN1T_PosPayLoad_rrlpPayload*)rtxMemAllocZ(pctxt, sizeof(ASN1T_PosPayLoad_rrlpPayload));
   		LeaveIfAllocErrorL();
		}
	}


/** 
Destructor 
*/
CSuplPos::~CSuplPos()
	{
	SUPLLOG(ELogP1, "CSuplPos::~CSuplPos() Begin\n");
	delete iPosPayload;
	delete iDecodeBuffer;
	delete iPayloadData;
	delete iPayloadPdu;
	SUPLLOG(ELogP1, "CSuplPos::~CSuplPos() End\n");
	}

	
/** 
SetVelocity()

Set the velocity parameter. The parameter TPositionInfoBase must contain
course information (eg be TPositionCourseInfo or derived) for the velocity
parameter to be set.

@param  aPosInfo: contains current velocity estimate
@return KErrNone normally
		KErrNotFound if no TCourse information present
		KErrNoMemory if failed to allocate memory to store the outgoing data
*/
EXPORT_C TInt CSuplPos::SetVelocity(const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplPos::SetVelocity() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLPOS != NULL, User::Invariant());

		// handle velocity (TCourse) information
	if ((aPosInfo.PositionClassType() & EPositionCourseInfoClass) != 0)
		{
		TPositionCourseInfo courseInfo = reinterpret_cast <const TPositionCourseInfo&> (aPosInfo);
		TCourse course;
		courseInfo.GetCourse(course);
		
		// populate the velocity parameter
		iData->message.u.msSUPLPOS->m.velocityPresent = 1;
		TInt velErr = PopulateVelocity(course, iData->message.u.msSUPLPOS->velocity);
		if (velErr != KErrNone)
			{
			SUPLLOG(ELogP1, "CSuplPos::SetVelocity() End (error populating velocity)\n");
			return velErr;
			}
		}
	else
		{
		// passed parameter does not contain course information.
		SUPLLOG(ELogP1, "CSuplPos::SetVelocity() End (Course Information Not Present)\n");
		return KErrNotFound;
		}
		
	SUPLLOG(ELogP1, "CSuplPos::SetVelocity() End\n");
	return KErrNone;
	}


/**
VelocityPresent()

@return ETrue if the optional velocity parameter is present
*/
EXPORT_C TBool CSuplPos::VelocityPresent()
	{
	SUPLLOG(ELogP1, "CSuplPos::VelocityPresent() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLPOS != NULL, User::Invariant());
	
	if (iData->message.u.msSUPLPOS->m.velocityPresent != 0)
		{
		SUPLLOG(ELogP1, "CSuplPos::VelocityPresent(ETrue) End\n");
		return ETrue;
		}

	SUPLLOG(ELogP1, "CSuplPos::VelocityPresent(EFalse) End\n");
	return EFalse;
	}

	
/** 
Velocity()

Populates aPosInfo according to the velocity parameter 
The parameter TPositionInfoBase must be able to store course information 
(eg be TPositionCourseInfo or derived) for the velocity parameter to be set.

Note that the LBS course API currently does not support vertical velocity, 
so vertical speed information is ignored.

@param  aPosInfo: on return, populated with velocity information.
@return KErrNotFound if the velocity is not available in the received message
		KErrNotSupported if the TPositionInfoBase can not hold course information.
		KErrArgument if the received data is not in the expected format.
		KErrNone otherwise
*/
EXPORT_C TInt CSuplPos::GetVelocity(TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplPos::Velocity() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLPOS != NULL, User::Invariant());

	// check the velocity was present in the received message
	if (iData->message.u.msSUPLPOS->m.velocityPresent == 0)
		{
		return KErrNotFound;
		}
	
	// check the TPositionInfoBase can receive course information.
	if ((aPosInfo.PositionClassType() & EPositionCourseInfoClass) == 0)
		{
		__ASSERT_DEBUG(0, User::Invariant());
		return KErrNotSupported;
		}
		
	// source velocity reference
	ASN1T_Velocity& velocity = iData->message.u.msSUPLPOS->velocity;

	// target course reference
	TCourse course;
	
	// set the course information according to the received velocity type
	switch (velocity.t)
		{
		case T_Velocity_horvel:
			{
			TUint bearing = 0;
			bearing = velocity.u.horvel->bearing.data[0] << 1;
			bearing |= velocity.u.horvel->bearing.data[1] >> 7;
			
			TUint speed = 0;
			speed = velocity.u.horvel->horspeed.data[0] << 8;
			speed |= velocity.u.horvel->horspeed.data[1];
			
			course.SetHeading((TReal32)bearing);
			course.SetSpeed(ConvertHorSpeedToMetersPerSecond(speed));
			break;
			}
		case T_Velocity_horandvervel:
			{
			TUint bearing = 0;
			bearing = velocity.u.horandvervel->bearing.data[0] << 1;
			bearing |= velocity.u.horandvervel->bearing.data[1] >> 7;
			
			TUint speed = 0;
			speed = velocity.u.horandvervel->horspeed.data[0] << 8;
			speed |= velocity.u.horandvervel->horspeed.data[1];
			
			course.SetHeading((TReal32)bearing);
			course.SetSpeed(ConvertHorSpeedToMetersPerSecond(speed));
			break;
			}
		case T_Velocity_horveluncert:
			{
			TUint bearing = 0;
			bearing = velocity.u.horveluncert->bearing.data[0] << 1;
			bearing |= velocity.u.horveluncert->bearing.data[1] >> 7;
			
			TUint speed = 0;
			speed = velocity.u.horveluncert->horspeed.data[0] << 8;
			speed |= velocity.u.horveluncert->horspeed.data[1];
			
			TUint speedAccuracy = velocity.u.horveluncert->uncertspeed.data[0];
			
			course.SetHeading((TReal32)bearing);
			course.SetSpeed(ConvertHorSpeedToMetersPerSecond(speed));
			course.SetSpeedAccuracy(ConvertHorSpeedAccuracyToMetersPerSecond(speedAccuracy));
			break;
			}
		case T_Velocity_horandveruncert:
			{
			TUint bearing = 0;
			bearing = velocity.u.horandveruncert->bearing.data[0] << 1;
			bearing |= velocity.u.horandveruncert->bearing.data[1] >> 7;
			
			TUint speed = 0;
			speed = velocity.u.horandveruncert->horspeed.data[0] << 8;
			speed |= velocity.u.horandveruncert->horspeed.data[1];
			
			TUint speedAccuracy = velocity.u.horandveruncert->horuncertspeed.data[0];
			
			course.SetHeading((TReal32)bearing);
			course.SetSpeed(ConvertHorSpeedToMetersPerSecond(speed));
			course.SetSpeedAccuracy(ConvertHorSpeedAccuracyToMetersPerSecond(speedAccuracy));
			break;
			}
		default:
			{
			__ASSERT_DEBUG(0, User::Invariant());
			return KErrArgument;
			}
		}

	// set the course information in the return parameter.
	TPositionCourseInfo posInfo = reinterpret_cast <const TPositionCourseInfo&> (aPosInfo);
	posInfo.SetCourse(course);
	
	SUPLLOG(ELogP1, "CSuplPos::Velocity() End\n");
	return KErrNone;
	}
	

/** 
SetPosPayload()

Assigns the payload RRLP message for outgoing messages. 
CSuplPos instance takes immediate ownership of the CSuplPosPayload object.

@param
*/
EXPORT_C void CSuplPos::SetPosPayload(CSuplPosPayload* aPayload)
	{
	SUPLLOG(ELogP1, "CSuplPos::SetPosPayload() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iPosPayload == NULL, User::Invariant());
	iPosPayload = aPayload;	
	SUPLLOG(ELogP1, "CSuplPos::SetPosPayload() End\n");
	}
	
	
/**
DecodePosPayloadL()

Decodes the content of received positioning payload. Must be called before content
of the payload may be accessed

@param aError, on return indicates if an error was encountered, 
               eg KErrNotSupported for unsupported payload types.
*/
void CSuplPos::DecodePosPayloadL(TInt& aError)
	{
	SUPLLOG(ELogP1, "CSuplPos::DecodePosPayloadL() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iPosPayload == NULL, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLPOS != NULL, User::Invariant());
	
	// check that the payload is RRLP
	if (iData->message.u.msSUPLPOS->posPayLoad.t != T_PosPayLoad_rrlpPayload)
		{
		aError = ESuplAsn1ErrUnsupportedPosProtocol;
		return;
		}
	
	// extract the RRLP payload and decode and compare...
	TInt payloadLength  = iData->message.u.msSUPLPOS->posPayLoad.u.rrlpPayload->numocts;
	OSOCTET* payloadptr = &iData->message.u.msSUPLPOS->posPayLoad.u.rrlpPayload->data[0];

	// Create the payload decode buffer
	ASN1Context* payloadContext = new (ELeave) ASN1Context;
	CleanupDeletePushL(payloadContext);
	iDecodeBuffer = new (ELeave) ASN1PERDecodeBuffer (payloadptr, payloadLength, FALSE, payloadContext);
	// construction of iDecodeBuffer successful, pop payloadContext off the cleanup stack
	CleanupStack::Pop(payloadContext);

	// Create data and control objects to manage the decode
	iPayloadData  = new (ELeave) ASN1T_PDU();
	iPayloadPdu   = new (ELeave) ASN1C_PDU (*iDecodeBuffer, *iPayloadData);
	
	TInt stat = iPayloadPdu->Decode();

	// return now if error encountered while decoding
	if (stat != KErrNone)
		{
		aError = ProcessAsn1Error(stat);
		return;
		}

	// build payload content wrapper.
	switch (iPayloadData->component.t)
		{
		case T_RRLP_Component_msrPositionReq:
			{
			iPosPayload = CRrlpMeasurePositionRequest::NewL();
			break;
			}
			
		case T_RRLP_Component_assistanceData:
			{
			iPosPayload = CRrlpAssistanceData::NewL();
			break;
			}

		case T_RRLP_Component_protocolError:
			{
			iPosPayload = CRrlpProtocolError::NewL(EFalse);
			break;
			}

		case T_RRLP_Component_msrPositionRsp:
 		case T_RRLP_Component_assistanceDataAck:
		default:
			{
			// unsupported message type
			aError = ESuplAsn1ErrUnsupportedPosMessageType;
			return;
			}
		}
	
	// pass ownership of the decoded message content.
	CRrlpMessageBase* rrlpMessage = (CRrlpMessageBase*)iPosPayload;
	rrlpMessage->SetDecodedData(iPayloadData, iPayloadPdu);
	iPayloadData = NULL;
	iPayloadPdu  = NULL;

	// no longer need the decode buffer object.
	delete iDecodeBuffer;
	iDecodeBuffer = NULL;
	SUPLLOG(ELogP1, "CSuplPos::DecodePosPayloadL() End\n");
	}
	

/** 
PosPayload()

Returns the decoded positioning payload for received SUPL POS messages.
Note that ownership of the position payload is returned - the calling entity 
becomes responsible for the destruction of the positioning payload.

@return pointer to the positioning payload.
*/
EXPORT_C CSuplPosPayload* CSuplPos::PosPayload()
	{
	SUPLLOG(ELogP1, "CSuplPos::PosPayload() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iPosPayload != NULL, User::Invariant());
	__ASSERT_DEBUG(iData->message.u.msSUPLPOS != NULL, User::Invariant());
	
	CSuplPosPayload* posPayload = iPosPayload;
	iPosPayload = NULL;

	SUPLLOG(ELogP1, "CSuplPos::PosPayload() End\n");

	return posPayload;
	}

	
/** 
EncodePosPayloadL()

Encodes the positioning payload.

@return error indication, KErrNone otherwise
*/
TInt CSuplPos::EncodePosPayloadL()
	{
	SUPLLOG(ELogP1, "CSuplPos::EncodePosPayloadL() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iPosPayload!=NULL, User::Invariant());

	// encode the positioning payload. 
	// Payload is encoded directly to the supl payload container.
	TPtr8 payloadPtr(iData->message.u.msSUPLPOS->posPayLoad.u.rrlpPayload->data, 8192);
	TInt err = iPosPayload->EncodeToL(payloadPtr);
	if (err!=KErrNone)
		{
		SUPLLOG(ELogP1, "CSuplPos::EncodePosPayloadL() Error\n");
		return ProcessAsn1Error(err);
		}
	
	// set the SUPL payload length to the encoded payload.
	iData->message.u.msSUPLPOS->posPayLoad.u.rrlpPayload->numocts = payloadPtr.Length();
	
	SUPLLOG(ELogP1, "CSuplPos::EncodePosPayloadL() End\n");
	return KErrNone;
	}

/** 
ConvertHorSpeedToMetersPerSecond()

Converts received speed value N to meters per second, according to
3GPP GAD:

for N = 0 					N <= h < N+0.5
for 0 < N < 2^16-1			N-0.5 <= h < N+0.5
for N=2^16-1				N-0.5 <= h

where h = speed in km/hour

@param  aSpeed - the received, encoded speed N.
@return the uncertainty speed in meters per second.
*/
TReal32 CSuplPos::ConvertHorSpeedToMetersPerSecond(TUint aSpeed)
	{
	SUPLLOG(ELogP1, "CSuplPos::ConvertHorSpeedToMetersPerSecond() Begin\n");
	TReal32 kph;
	if (aSpeed == 0)
		{
		kph = 0;
		}
	else if (aSpeed < 65536)
		{
		kph = aSpeed - 0.5;
		}
	else
		{
		kph = 65535 - 0.5;
		}
	
	TReal32 metersPerSec = kph / 3.6;
	SUPLLOG(ELogP1, "CSuplPos::ConvertHorSpeedToMetersPerSecond() End\n");
	return metersPerSec;
	}

/** 
ConvertHorSpeedAccuracyToMetersPerSecond()

Converts received speed uncertainty value N to meters per second, according to
3GPP GAD:

Uncertainty is encoded in increments of 1 kilometer per hour using an 8bit
encoded number N. N is therefore the uncertainty speed in kmph, except for 
N=255 which indicates that the uncertainty is not specified

@param  aSpeedAccuracy - the received, encoded uncertainty speed N.
@return the uncertainty speed in meters per second.
*/
TReal32 CSuplPos::ConvertHorSpeedAccuracyToMetersPerSecond(TUint aSpeedAccuracy)
	{
	SUPLLOG(ELogP1, "CSuplPos::ConvertHorSpeedAccuracyToMetersPerSecond() Begin\n");
	TReal32 metersPerSec = aSpeedAccuracy / 3.6;
	SUPLLOG(ELogP1, "CSuplPos::ConvertHorSpeedAccuracyToMetersPerSecond() End\n");
	return metersPerSec;
	}

/** 
Prints the content payload data structure to the logger 
*/
void CSuplPos::LogPayload()
	{
	if (iPosPayload)
		{
		iPosPayload->LogMessageContent();
		}
	}

