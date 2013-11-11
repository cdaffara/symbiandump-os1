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
#ifndef LBS_SUPL_POS_H
#define LBS_SUPL_POS_H

#include "suplmessagebase.h"
#include "suplpospayload.h"
#include <lbspositioninfo.h>

// forward declarations
class ASN1PERDecodeBuffer;
class ASN1T_PDU;
class ASN1C_PDU;


/**
Encapsulation for outgoing/received SUPL POS messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplPos) : public CSuplMessageBase
	{
public:
	IMPORT_C static CSuplPos* NewL(TBool aIsOutgoingMessage);
	IMPORT_C static CSuplPos* NewLC(TBool aIsOutgoingMessage);
		
public:
	virtual ~CSuplPos();
	
public:
	
	/** Set the velocity parameter (for outgoing messages) */
	IMPORT_C TInt SetVelocity(const TPositionInfoBase& aPosInfo);

	/** Assigns the payload RRLP message (for outgoing messages) */
	IMPORT_C void SetPosPayload(CSuplPosPayload* aPayload);

	/** Returns ETrue if the position parameter is present (for received messages) */
	IMPORT_C TBool VelocityPresent();

	/** Populates aPosInfo according to the velocity parameter (for received messages) */
	IMPORT_C TInt GetVelocity(TPositionInfoBase& aPosInfo);
	
	/** Returns the positioning payload (for received messages).
	    The calling method becomes responsible for destruction of the pos payload. */
	IMPORT_C CSuplPosPayload* PosPayload();

public:

	/** Encodes the SUPL POS message to the passed buffer 
		Called by CSuplMessageBase::EncodeToL() and not intended for DLL export */
	TInt EncodePosPayloadL();
	
	/** Decodes the content of the positioning payload (for received messages)
		Called by CSuplAsn1DecoderImpl::DecodeL() and not intended for DLL export */
	void DecodePosPayloadL(TInt& aError);
	
	void LogPayload();

private:
	/** Constructor */
	CSuplPos(TBool aIsOutgoingMessage);

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CSuplPos(const CSuplPos&);

	/** Prohibit assigment operator */
	CSuplPos& operator= (const CSuplPos&);
	
private:

	// converts speed from encoded value N to meters per second.
	TReal32 ConvertHorSpeedToMetersPerSecond(TUint aSpeed);
	
	// converts speed accuracy from encoded value N to meters per second.
	TReal32 ConvertHorSpeedAccuracyToMetersPerSecond(TUint aSpeedAccuracy);

private:

	/** The positioning payload */	
	CSuplPosPayload* iPosPayload;
	
	/** Objects used in decoding of the positioning payload */
	ASN1PERDecodeBuffer* iDecodeBuffer;
	ASN1T_PDU* iPayloadData;
	ASN1C_PDU* iPayloadPdu;
	};

#endif // LBS_SUPL_POS_H
