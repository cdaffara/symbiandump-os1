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
// Internal implementation of the SUPL ASN1 Decoder
// 
//

/**
 @file
 @internalTechnology
 
*/

#include "ULP.h"
#include "suplasn1decoderimpl.h"
#include "suplinit.h"
#include "suplresponse.h"
#include "suplpos.h"
#include "suplend.h"
#include "supldevloggermacros.h" 

/**
Static factory constructor
*/
EXPORT_C CSuplAsn1DecoderImpl* CSuplAsn1DecoderImpl::NewL()
	{
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::NewL() Begin\n");
	CSuplAsn1DecoderImpl* self = new (ELeave) CSuplAsn1DecoderImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Constructor
*/
CSuplAsn1DecoderImpl::CSuplAsn1DecoderImpl()
	{
	}
	
/**
Second stage constructor
*/
void CSuplAsn1DecoderImpl::ConstructL()
	{
	}

/**
Destructor
*/
CSuplAsn1DecoderImpl::~CSuplAsn1DecoderImpl()
	{
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::~CSuplAsn1DecoderImpl() Begin\n");
	delete iDecodeBuffer;
	delete iData;
	delete iControl;
	// release the STDLIB resources associated with this thread
	CloseSTDLIB();
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::~CSuplAsn1DecoderImpl() End\n");
	}

/**
DecodeL()

Decodes a received message, passing back the decoded content in the form of a
CSuplMessageBase derived object. The calling code receives ownership of this
object and is responsible for its destruction.

@param  aBuf   buffer from which the ASN1 message is to be decoded
@param  aError on return, indicates error. KErrNone otherwise.
@return pointer to the decoded SUPL message.
*/
EXPORT_C CSuplMessageBase* CSuplAsn1DecoderImpl::DecodeL(const TPtrC8* aBuf, TInt& aError)
	{
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::DecodeL() Begin\n");
	aError = KErrNone;
	
	// Log the encoded ASN1	
	SUPLLOG(ELogP8, "<- SUPL MESSAGE RECEIVED (HEX)\n");
	SUPLLOGHEX(ELogP8, aBuf->Ptr(), aBuf->Length());
		
	// buffer pointer, fixed max length
	OSOCTET* msgBuf = (OSOCTET*)aBuf->Ptr();
	TInt len = aBuf->Length();

	// Create the message decode buffer
	ASN1Context* context = new (ELeave) ASN1Context;
	CleanupDeletePushL(context);
	iDecodeBuffer = new (ELeave) ASN1PERDecodeBuffer (msgBuf, len, FALSE, context);
	// construction of iDecodeBuffer successful, pop context off the cleanup stack
	CleanupStack::Pop(context);

	// Create data and control objects to manage the decode
	iData = new (ELeave) ASN1T_ULP_PDU();
	
	// The 'new' above does not initialise member valuables.
 	// Need to do it manually. (include INC136464)
 	iData->message.t = 0;
 	iData->message.u.msSUPLINIT = NULL;
 	iData->message.u.msSUPLSTART = NULL;
 	iData->message.u.msSUPLRESPONSE = NULL;
 	iData->message.u.msSUPLPOSINIT = NULL;
 	iData->message.u.msSUPLPOS = NULL;
 	iData->message.u.msSUPLEND = NULL;
 	iData->message.u.msSUPLAUTHREQ = NULL;
 	iData->message.u.msSUPLAUTHRESP = NULL;
	
	iControl = new (ELeave) ASN1C_ULP_PDU (*iDecodeBuffer, *iData);
	
	// Decode the contents of the message buffer
	iControl->Decode();
	
	// check for errors
	TInt stat = iControl->getStatus();
	if (stat == 0)
		{
		stat = AdditionalMessageValidation();
		}
	if (stat != 0)
		{
		aError = ProcessAsn1Error(stat);
		// delete all objects before return null
        delete iDecodeBuffer;
        iDecodeBuffer = NULL;
        delete iData;
        iData = NULL;
        delete iControl;
        iControl = NULL;
		
		return NULL;
		}
	
	// build the appropriate LBS SUPL Message Type, and return.
	TInt messageType = iData->message.t;
	CSuplMessageBase* aMessage;
	switch (messageType)
		{
		case T_UlpMessage_msSUPLINIT:
			{
			aMessage = CSuplInit::NewL();
			CleanupStack::PushL(aMessage);
			break;
			}
		case T_UlpMessage_msSUPLRESPONSE:
			{
			aMessage = CSuplResponse::NewL();
			CleanupStack::PushL(aMessage);
			break;
			}
		case T_UlpMessage_msSUPLPOS:
			{
			aMessage = CSuplPos::NewL(EFalse);
			CleanupStack::PushL(aMessage);
			break;
			}
		case T_UlpMessage_msSUPLEND:
			{
			aMessage = CSuplEnd::NewL(EFalse);
			CleanupStack::PushL(aMessage);
			break;
			}
		default:
			{
			// unexpected message type
			aError = KErrNotSupported;
			// delete all objects before return null
            delete iDecodeBuffer;
            iDecodeBuffer = NULL;
            delete iData;
            iData = NULL;
            delete iControl;
            iControl = NULL;
			
			return NULL;
			}
		}
	
	// Pass ownership of decoded data to the message object
	aMessage->SetDecodedData(iData, iControl);
	iControl = NULL;
	iData = NULL;
	
	// delete the decode buffer object
	delete iDecodeBuffer;
	iDecodeBuffer = NULL;

	// Decode the content of encapsulated positioning method
	if (messageType == T_UlpMessage_msSUPLPOS)
		{
		CSuplPos* posMessage = (CSuplPos*)aMessage;
		posMessage->DecodePosPayloadL(aError);
		}
		
	// store un-decoded SUPL INIT message for ver hash calculation	
	if (messageType == T_UlpMessage_msSUPLINIT)
		{
		CSuplInit* initMessage = (CSuplInit*)aMessage;
		initMessage->SetReceivedMessageL(*aBuf);
		}
	
	// LOG the received message
	SUPLLOG(ELogP9, "<- SUPL MESSAGE RECEIVED AND DECODED\n");
	aMessage->LogMessageContent();
	
	// pop the constructed message off the stack and return to caller.
	CleanupStack::Pop(aMessage);
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::DecodeL() End\n");
	return aMessage;
	}
	
/**
Translates error codes returned by the ASN1 runtime library to distinguish
from Symbian global error codes.

Errors are simply translated to positive error codes. They maintain their
meaning as described in rtxErrCodes.h and asn1ErrCodes.h, with the exception
of RTERR_NOMEM is translated to global error code KErrNoMemory.

@see rtxErrCodes.h
@see asn1ErrCodes.h
*/
TInt CSuplAsn1DecoderImpl::ProcessAsn1Error(TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::ProcessAsn1Error() Begin\n");
	if (aError == RTERR_NOMEM)
		{
		SUPLLOG(ELogP1, "CSuplAsn1DecoderImpl::ProcessAsn1Error() End (Out Of Memory)\n");
		return KErrNoMemory;
		}
	else
		{
		SUPLLOG2(ELogP1, "CSuplAsn1DecoderImpl::ProcessAsn1Error() End (ASN1 Runtime Error %d)\n", aError);
		return aError * -1;
		}
	}

/**
Check for additional errors in messages that
the control object cannot report for
various reasons.
This method returns error codes defined by the asn1
compiler supplier.
*/
TInt CSuplAsn1DecoderImpl::AdditionalMessageValidation()
{
	TInt stat = 0;
	
	// Set Session Id is mandatory in every
	// received message. Its absence is not flagged
	// by the controller because it is an optional
	// parameter in some outgoing messages.
	if (!iData->sessionID.m.slpSessionIDPresent)
		{
		stat = RTERR_SETMISRQ;
		}
	
	return stat;
}

