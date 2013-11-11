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
#include "suplpospayload.h"
#include "supldevloggermacros.h" 
#include "rrlputils.h"
#include "rrlpmeasureposrequest.h"
#include "rrlpassistancedata.h"
#include "rrlpprotocolerror.h"
#include "suplasn1error.h"

/**
Default Constructor
*/
CSuplPosPayload::CSuplPosPayload(TSuplPosPayloadType aType, TBool aIsOutgoingMsg)
 : iMessageType(aType), iIsOutgoingMessage(aIsOutgoingMsg)
	{
	SUPLLOG(ELogP1, "CSuplPosPayload::CSuplPosPayload() Begin\n");
	SUPLLOG(ELogP1, "CSuplPosPayload::CSuplPosPayload() End\n");
	}

/**
Default Constructor
*/
CSuplPosPayload::~CSuplPosPayload()
	{
	SUPLLOG(ELogP1, "CSuplPosPayload::~CSuplPosPayload() Begin\n");
	SUPLLOG(ELogP1, "CSuplPosPayload::~CSuplPosPayload() End\n");
	}
	
/** 
EncodeToL()

Encode a populated outgoing message 

@param aBuf the destination buffer to encode to
@return error indication, KErrNone otherwise
*/
TInt CSuplPosPayload::EncodeToL(TPtr8& /*aBuf*/)
	{
	SUPLLOG(ELogP1, "CSuplPosPayload::~CSuplPosPayload() Begin\n");
	SUPLLOG(ELogP1, "CSuplPosPayload::~CSuplPosPayload() End\n");
	return KErrNotSupported;
	}

/**
Prints the content payload data structure to the logger
Default implementation logs nothing and should be overridden
*/
void CSuplPosPayload::LogMessageContent()
	{
	SUPLLOG(ELogP1, "CSuplPosPayload::LogMessageContent() Begin\n");
	SUPLLOG(ELogP1, "CSuplPosPayload::LogMessageContent() End\n");
	}

/** 
MessageType()

@return the positioning payload message type
*/
EXPORT_C CSuplPosPayload::TSuplPosPayloadType CSuplPosPayload::MessageType()
	{
	SUPLLOG(ELogP1, "CSuplPosPayload::MessageType() Begin\n");
	SUPLLOG2(ELogP1, "CSuplPosPayload::MessageType() End (iMessageType = %d)\n", iMessageType);
	return iMessageType;
	}
	
/**
 * DecodePosPayload
 * 
 * decodes a pospayload and returns a CSuplPosPayload* (eg CRrlpMeasurePositionRequest*) containing the decoded rrlp payload 
 * 
 * @param aPosPayload	an asn1-encoded rrlp pos payload. Caller takes ownership.
 * @param aError		any error encountered 
 */
EXPORT_C CSuplPosPayload* CSuplPosPayload::DecodePosPayloadL(const HBufC8* aPosPayload, TInt& aError)
	{
	SUPLLOG(ELogP1, "CSuplPosPayload::DecodePosPayloadL() Begin\n");	
	
	CSuplPosPayload *decodedRrlpMsg = NULL;
	
	const ASN1T_PosPayLoad_rrlpPayload* posPayLoad = reinterpret_cast<const ASN1T_PosPayLoad_rrlpPayload*>(aPosPayload);
	
	// extract the RRLP payload and decode and compare...
	TInt payloadLength  = posPayLoad->numocts;
	const OSOCTET* payloadptr = &posPayLoad->data[0];

	// Create the payload decode buffer
    // CleanupDeletePushL used for ASN1x_X classes rather than CleanupStack::PushL() to ensure 
	// class destructors are called on PopAndDestroy. This is necessary to ensure ASN1Context 
	// reference counter is decremented correctly and memory released.
	ASN1Context* payloadContext = new (ELeave) ASN1Context;
	CleanupDeletePushL(payloadContext);
	ASN1PERDecodeBuffer* decodeBuffer = new (ELeave) ASN1PERDecodeBuffer (payloadptr, payloadLength, FALSE, payloadContext);
	// construction of iDecodeBuffer successful, pop payloadContext off the cleanup stack
	CleanupStack::Pop(payloadContext);	// now owned by decodeBuffer
	CleanupDeletePushL(decodeBuffer);	
	
	// Create data and control objects to manage the decode		
	ASN1T_PDU* payloadData  = new (ELeave) ASN1T_PDU();
	CleanupDeletePushL(payloadData);	
	ASN1C_PDU* payloadPdu   = new (ELeave) ASN1C_PDU(*decodeBuffer, *payloadData);
	CleanupDeletePushL(payloadPdu);
	
	TInt stat = payloadPdu->Decode();

	// return now if error encountered while decoding
	if (stat != KErrNone)
		{
		aError = RrlpUtils::ProcessAsn1Error(stat);
		CleanupStack::PopAndDestroy(payloadPdu);
		CleanupStack::PopAndDestroy(payloadData);
		CleanupStack::PopAndDestroy(decodeBuffer);
		return NULL;	
		}

	// build payload content wrapper.
	switch (payloadData->component.t)
		{
		case T_RRLP_Component_msrPositionReq:
			{
			decodedRrlpMsg = CRrlpMeasurePositionRequest::NewL();	
			break;
			}
			
		case T_RRLP_Component_assistanceData:
			{
			decodedRrlpMsg = CRrlpAssistanceData::NewL();
			break;
			}

		case T_RRLP_Component_protocolError:
			{
			decodedRrlpMsg = CRrlpProtocolError::NewL(EFalse);
			break;
			}

		case T_RRLP_Component_msrPositionRsp:
		case T_RRLP_Component_assistanceDataAck:
		default:
			{
			// unsupported message type
			aError = ESuplAsn1ErrUnsupportedPosMessageType;
			CleanupStack::PopAndDestroy(payloadPdu);
			CleanupStack::PopAndDestroy(payloadData);
			CleanupStack::PopAndDestroy(decodeBuffer);
			return NULL;	
			}
		}
	
	CleanupStack::Pop(payloadPdu);
	CleanupStack::Pop(payloadData);
	CleanupStack::PopAndDestroy(decodeBuffer);
	
	// pass ownership of the decoded message content.
	static_cast<CRrlpMessageBase*>(decodedRrlpMsg)->SetDecodedData(payloadData, payloadPdu);
			
	SUPLLOG(ELogP1, "CSuplPosPayload::DecodePosPayloadL() End\n");
	
	return decodedRrlpMsg;
		
	}
