// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CLWSPPduHandlerTraces.h"
#endif

#include "CLWSPPduHandler.h"
#include "wapmsgerr.h"

void CCLWSPPduHandler::UnpackWSPPduL(HBufC8* aWSPPdu, TWSPPduType& aType, HBufC8*& aWSPHeader, HBufC8*& aBody, TUint8& aId, TWSPStatus& aStatus)
/** 
Unpack the received WSP PDU from remote peer to several data structure expected by the client.
@internalComponent
@released 
@since v8.0 
@param aWSPPdu the received WSP PDU from the remote peer. 
@param aType the type of the received WSP PDU.
@param aWSPHeader the WSP header of the received WSP PDU.
@param aBody the data body of the received WSP PDU.
@param aId the transaction ID or Push ID of the received WSP PDU.
@param aStatus the WSP status of the received WSP PDU.
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TPtr8 des=aWSPPdu->Des();
	if(des.Length() < KPDUTransactionIDAndPDUTypeLength)
	{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLWSPPDUHANDLER_UNPACKWSPPDUL_1, "CCLWSPPduHandler::UnpackWSPPduL() Corrupted InComing Wsp PDU");
        User::Leave(KErrCorrupt);
	}
	aId=des[KPDUHeaderTransactionIDOffSet];
	aType=TWSPPduType(des[KPDUHeaderWSPPDUTypeOffSet]);
	switch (aType)
		{
	case EReply:
		UnpackReplyPduL(aWSPPdu, aWSPHeader, aBody, aStatus);
		break;
	case EPush:
		UnpackPushPduL(aWSPPdu, aWSPHeader, aBody);
		break;
	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLWSPPDUHANDLER_UNPACKWSPPDUL_2, "CCLWSPPduHandler::UnpackWSPPduL() Unknown InComing Wsp PDU Type");
		User::Leave(KErrCorrupt);
		}
	}

void CCLWSPPduHandler::PackWSPPduL(HBufC8*& aWSPPdu, TWSPPduType aType, const TDesC& aURI, const TDesC8& aWSPHeader, const TDesC8& aBody, const TUint8 aId)
/** 
Pack the information to be sent into WSP PDU.
@internalComponent
@released 
@since v8.0 
@param aWSPPdu the WSP PDU that is to sent 
@param aType the WSP PDU type(the method is to be invoked) 
@param aURI  the URI where the WSP PDU goes
@param aWSPHeader the WSP header
@param aBody the data body
@param aId the transaction ID
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	switch (aType)
		{
	case EGet:
	case EGetOptions:
	case EGetHead:
	case EGetDelete:
	case EGetTrace:
		PackGetPduL(aWSPPdu, aType, aURI, aWSPHeader, aId);
		break;
	
	case EPost:
	case EPostPut:
		PackPostPduL(aWSPPdu, aType, aURI, aWSPHeader, aBody, aId);
		break;

	default:
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLWSPPDUHANDLER_PACKWSPPDUL_1, "CCLWSPPduHandler::PackWSPPduL() Unknown Method Invoke Wsp PDU Type");
	    User::Leave(Wap::EWspClParameterError);
		}
	}

void CCLWSPPduHandler::UnpackPushPduL(HBufC8* aWSPPdu, HBufC8*& aWSPHeader, HBufC8*& aBody)
/**
Unpack the WSP PUSH PDU.
@internalComponent
@released 
@since v8.0 
@param aWSPPdu the received WSP PDU from the remote peer. 
@param aWSPHeader the WSP header of the received WSP PDU.
@param aBody the data body of the received WSP PDU.
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TPtrC8 pduBuffer(*aWSPPdu);
	TUint32 pduLength=pduBuffer.Length();
	TUint32 headerLength(0);
	TInt uintvarLength(0);

	// if uintvarLength less than KErrNone, then it is a error code.
	if ((uintvarLength=UintVar(headerLength, pduBuffer, KPDUTransactionIDAndPDUTypeLength))<KErrNone)
		{
		User::Leave(uintvarLength);
		}
 	TInt32 bodyLength= pduLength-KPDUTransactionIDAndPDUTypeLength-uintvarLength-headerLength;
	if (bodyLength<0)
		{
		User::Leave(KErrCorrupt);
		}
	// Extract the WSP Header.
	aWSPHeader=HBufC8::NewL(headerLength);
	TPtr8 headerDes=aWSPHeader->Des();
	headerDes.Copy(pduBuffer.Mid(KPDUTransactionIDAndPDUTypeLength+uintvarLength, headerLength));
	// Extract the WSP data body.
	aBody=HBufC8::NewL(bodyLength);
	TPtr8 bodyDes=aBody->Des();
	bodyDes.Copy(pduBuffer.Mid(KPDUTransactionIDAndPDUTypeLength+uintvarLength+headerLength));
	}

void CCLWSPPduHandler::UnpackReplyPduL(HBufC8* aWSPPdu, HBufC8*& aWSPHeader, HBufC8*& aBody, TWSPStatus& aStatus)
/**
Unpack the WSP Reply PDU.
@internalComponent
@released 
@since v8.0 
@param aWSPPdu the received WSP PDU from the remote peer. 
@param aWSPHeader the WSP header of the received WSP PDU.
@param aBody the data body of the received WSP PDU.
@param aStatus the WSP status of the received WSP PDU.  
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	TPtrC8 pduBuffer(*aWSPPdu);
	TUint32 pduLength=pduBuffer.Length();

	//Extract the WSP status
	aStatus=pduBuffer[KPDUTransactionIDAndPDUTypeLength];

	TUint32 headerLength(0);
	TInt uintvarLength(0);
	// if uintvarLength less than KErrNone, then it is a error code.
	if ((uintvarLength=UintVar(headerLength, pduBuffer, KPDUTransactionIDAndPDUTypeLength+KWSPStatusLength))<KErrNone)
		{
		User::Leave(uintvarLength);
		}
		
	TInt32 bodyLength= pduLength-KPDUTransactionIDAndPDUTypeLength-KWSPStatusLength-uintvarLength-headerLength;
	if (bodyLength<0)
		{
		User::Leave(KErrCorrupt);
		}
		
	// Extract the WSP Header.
	aWSPHeader=HBufC8::NewL(headerLength);
	TPtr8 headerDes=aWSPHeader->Des();
	headerDes.Copy(pduBuffer.Mid(KPDUTransactionIDAndPDUTypeLength+KWSPStatusLength+uintvarLength, headerLength));

	// Extract the WSP data body.
	aBody=HBufC8::NewL(bodyLength);
	TPtr8 bodyDes=aBody->Des();
	bodyDes.Copy(pduBuffer.Mid(KPDUTransactionIDAndPDUTypeLength+KWSPStatusLength+uintvarLength+headerLength));
	}

void CCLWSPPduHandler::PackPostPduL(HBufC8*& aWSPPdu, TWSPPduType aType, const TDesC& aURI, const TDesC8& aWSPHeader, const TDesC8& aBody, const TUint8 aId)
/** 
Pack the WSP Post PDU.
@internalComponent
@released 
@since v8.0 
@param aWSPPdu the WSP PDU that is to sent 
@param aType the WSP PDU type(the method is to be invoked) 
@param aURI the URI where the WSP PDU goes
@param aWSPHeader the WSP header
@param aBody the data body
@param aId the transaction ID
@returns TInt KErrNone on successful completion, or one of the system error codes on failure.
*/
	{
	//Convert the URI length to Varible length unsigned integer
	TUint32 uriLength=aURI.Length();
	HBufC8* uriLengthDes=UintVarL(uriLength);

	//Convert the URI length to Varible length unsigned integer
	TUint32 headerLength=aWSPHeader.Length();
	HBufC8* headerLengthDes=UintVarL(headerLength);

	// Calculate the pdu length
	TUint32 pduLength = KPDUTransactionIDAndPDUTypeLength+
						uriLengthDes->Length()+
						headerLengthDes->Length()+
						uriLength+
						headerLength+
						aBody.Length();

	//Build the PDU.
	aWSPPdu = HBufC8::NewL(pduLength);
	TPtr8 pduPtr(aWSPPdu->Des());
	pduPtr.Append(aId);
	pduPtr.Append(aType);
	pduPtr.Append(*uriLengthDes);
	pduPtr.Append(*headerLengthDes);
	pduPtr.Append(aURI);
	pduPtr.Append(aWSPHeader);
	pduPtr.Append(aBody);

	// delete the temporary buffers
	delete uriLengthDes;
	delete headerLengthDes;
	}
void CCLWSPPduHandler::PackGetPduL(HBufC8*& aWSPPdu, TWSPPduType aType, const TDesC& aURI, const TDesC8& aWSPHeader, const TUint8 aId)
/** 
Pack the WSP Get PDU.
@internalComponent
@released 
@since v8.0 
@param aWSPPdu the WSP PDU that is to sent 
@param aType the WSP PDU type(the method is to be invoked) 
@param aURI the URI where the WSP PDU goes
@param aWSPHeader the WSP header
@param aId the transaction ID
@returns KErrNone on successful completion, or one of the system error codes on failure.
*/
	{

	//Convert the URI length to Varible length unsigned integer
	TUint32 uriLength=aURI.Length();
	HBufC8* uriLengthDes=UintVarL(uriLength);

	// Calculate the pdu length
	TUint32 pduLength =	KPDUTransactionIDAndPDUTypeLength+
						uriLengthDes->Length()+
						uriLength+
						aWSPHeader.Length();

	// Build the PDU.
	aWSPPdu = HBufC8::NewL(pduLength);
	TPtr8 pduPtr(aWSPPdu->Des());
	pduPtr.Append(aId);
	pduPtr.Append(aType);
	pduPtr.Append(*uriLengthDes);
	pduPtr.Append(aURI);
	pduPtr.Append(aWSPHeader);
	
	//Delete the temporary buffer
	delete uriLengthDes;
	}
TInt CCLWSPPduHandler::UintVar(TUint32& aVal, TPtrC8& aBuffer, TUint aOffset)
/** 
Decode the variable length Unsigned integer to TUint32
@internalComponent
@released 
@since v8.0 
@param aVal the value of the variable length Unsigned integer
@param aBuffer the descriptor that contains the variable length Unsigned integer
@param aOffset the offset of the variable length Unsigned integer
@returns one of the system error codes on failure, or the size of variable length Unsigned integer.
*/
	{
	// have we run out of buffer?
	if (aOffset >= (TUint)aBuffer.Length())
		{
		return KErrCorrupt;
		}
		
	// maximum length for a uintvar is 5
	TInt lenLeft = Min(aBuffer.Mid(aOffset).Length(), KMaxUintVarLength);

	// get the first octet
	TUint8 byte = aBuffer[aOffset++];
	TInt numBytes = 1;
	--lenLeft;

	// Check if any of the top 3 bits, ignoring the very top 'continue' bit, are set.  
	// Later if we see that this is a 5 byte number - we'll know it is corrupt.  
	// Encoding uses 7 bits/number 7x5=35 and we only support a maxiumum number 
	// of 32 bits.
	TBool topThreeBitsSet = byte & KTop3BitSet; 

	// copy over data from the byte into our return value (the top bit is a carry bit)
	aVal = byte & KWapQuote;

	// while the 'continue' bit is set and we have more data
	while ((byte & KCarryBitMask) && (lenLeft > 0))
		{
		// shift our last value up
		aVal <<= KUIntVarOctetShift;
		// get the next byte
		byte = aBuffer[aOffset++];
		// copy it over to the lowest byte
		aVal |= byte & KWapQuote;
		--lenLeft;
		++numBytes;
		} 

	// last octet has continue bit set ... NOT allowed Or
	// this was encoded wrong - can't have a number bigger than 32 bits
	if ((byte & KCarryBitMask) || (numBytes == 5 && topThreeBitsSet))
		{
		return KErrCorrupt;
		}
	// number of bytes read
	return numBytes;

	}

HBufC8* CCLWSPPduHandler::UintVarL(const TUint32 aInt)
/** 
Encode the TUint32 to the variable length Unsigned integer
@internalComponent
@released 
@since v8.0 
@param aInt the length of the data
@returns the descriptor that contains the variable length Unsigned integer
*/
	{
	TUint8 size = 0; // maximum value is 5 with a 32bit integer
	TUint32 value=aInt;
	do {
		++size;
		value >>=KUIntVarOctetShift; ; // shift by 7 bits.
		} while (value>0);

	HBufC8* output = HBufC8::NewL(size);
	TPtr8 outPtr(output->Des());

	TInt ii = size; 
	while (--ii > 0)
		{
		outPtr.Append( (TUint8)(aInt>>(KUIntVarOctetShift*(ii))  & KWapQuote) | KCarryBitMask); 
		} 

	// Finally the first 7 bits, last octet, do not set first bit.
	outPtr.Append( (TUint8)(aInt & KWapQuote) ); // Add even if 0 value.

	return output;
	}
