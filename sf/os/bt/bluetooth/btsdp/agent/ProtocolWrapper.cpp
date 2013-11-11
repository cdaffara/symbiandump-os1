// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <utf.h>
#include <es_sock.h>
#include <bt_sock.h>
#include <btsdp.h>
#include "ipcinternals.h"
#include "sdpkey.h"
#include "sdputil.h"
#include "SDPDatabase.h"
#include "DataEncoder.h"
#include "SDPAttribute.h"
#include "EncoderVisitor.h"
#include "ExtractorVisitor.h"
#include "agtypes.h"
#include "agutil.h"
#include "ProtocolWrapper.h"



/************************************************************************/
//
//   SDP agent client API 
//	(only exists as an exported API for porting purposes)
//
/************************************************************************/


RSdpAgent::RSdpAgent()
:iRequestBuf(0), iResultSize(0)
/**
	Constructer:
	Set the HBufC pointer iRequestBuf to NULL (This is an 'R' class).
	Set the TPckg iResultSize to package nothing. 
**/
	{
	}

TInt RSdpAgent::Open(RSdpSession& aSession)
/**
	Open a NetDatabase session on ESock using SDP over Bluetooth.
**/
	{
	return iNetDb.Open(aSession,KBTAddrFamily,KSDP);
	}

void RSdpAgent::Close()
/**
	Free iRequestBuf.
	Close RNetDatabase.
**/
	{
	delete iRequestBuf;
	iRequestBuf = 0;
	iNetDb.Close();
	}

TBool RSdpAgent::IsOpen()
	{
	return iNetDb.SubSessionHandle()==0?EFalse:ETrue;
	}
	
void RSdpAgent::Connect(TBTDevAddr aRemoteAddr, TRequestStatus& aStatus)
/**
	Ask to connect to remote device.
**/
	{
	TSDPConnectQuery connQ;

	connQ.iQueryType = KSDPConnectQuery;
	connQ.iAddr = aRemoteAddr;

	iResultSize.Set(0,0,0);
	iConnectBuf = connQ;
	iNetDb.Query(iConnectBuf, iResultSize, aStatus);
	}

void RSdpAgent::SdpSearchRequestL(TInt& aResultSize, 
								  CSdpAttrValueDES& aUUIDList,
								  TUint16 aMaxRecCount, 
								  const TDesC8& aContState,
								  TRequestStatus& aStatus)
/**
	Ask for a service search using a list attribute value (CSdpAttrValueDES).
	THIS FUNCTION SHOULD NEVER BE EXPORTED AS IS...a general
	list attribute value is open to misuse.
	The reply to this function is to send the SIZE of the response back
	in the aResultSize parameter. Once this is done the user of this class
	can set up a buffer big enough to collect the response from server side.
**/
	{
	TSDPEncodedKeyBuf key;
	if(aContState.Length() > KSdpContinuationStateMaxLength)
		{
		User::Leave(KErrArgument);
		}
	key().iQueryType = KSDPEncodedQuery | KSDPBufferResult; 
	key().iPduId = EServiceSearchRequest;

	TUint size = TElementEncoder::EncodedSize(ETypeDES, aUUIDList.DataSize());
	delete iRequestBuf;
	iRequestBuf = 0;
	iRequestBuf = HBufC8::NewL(sizeof(key)+size+2+1+KSdpContinuationStateMaxLength);
	TPtr8 ptr = iRequestBuf->Des();

	// Build the request PDU
	ptr.Copy(key);
	TElementEncoder ee(ptr);
	CAttrEncoderVisitor::EncodeAttributeL(ee, aUUIDList);
	TBuf8<2> sbuf;
	sbuf.SetMax();
	BigEndian::Put16(&sbuf[0], aMaxRecCount);	// Maximum Service record count
	ptr.Append(sbuf);
	ptr.Append((TUint8)aContState.Length());
	ptr.Append(aContState);

	iResultSize.Set(TPckg<TInt>(aResultSize));
		iResultSize.SetMax();
	iNetDb.Query(*iRequestBuf, iResultSize, aStatus);
	}


void RSdpAgent::SdpSearchRequestL(TInt& aResultSize, 
								  CSdpSearchPattern& aUUIDFilter,
								  TUint16 aMaxRecCount, 
								  const TDesC8& aContState,
								  TRequestStatus& aStatus)
/**
	Ask for a service search using a UUID search pattern (CSdpSearchPattern).
	THIS FUNCTION SHOULD MAY BE EXPORTED (UNLIKE THE OTHER "SdpSearchRequestL")
	AT SOME FUTURE DATE.
	The reply to this function is to send the SIZE of the response back
	in the aResultSize parameter. Once this is done the user of this class
	can set up a buffer big enough to collect the response from server side.
**/
	{
	CSdpAttrValueDES* avEquiv = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(avEquiv);
	MSdpElementBuilder* bldr = avEquiv->StartListL();
	for (TInt i = 0; i < aUUIDFilter.Count(); ++i)
		{
		TUUID uuid = aUUIDFilter.At(i);
		bldr = bldr->BuildUUIDL(uuid);
		}
	bldr->EndListL();
	CSdpAttrValueDES* avEquivT = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(avEquivT);
	avEquivT->StartListL()
		->BuildUUIDL(0x1100)
		->BuildUUIDL(0x1101)
		->BuildUUIDL(0x1102)
	->EndListL();

	SdpSearchRequestL(aResultSize, *avEquiv, aMaxRecCount, aContState, aStatus);
	CleanupStack::PopAndDestroy(); //avEquivT
	CleanupStack::PopAndDestroy(); //avEquiv
	}


void RSdpAgent::SdpAttributeRequestL(TInt& aResultSize, 
									 TSdpServRecordHandle aHandle, 
								     TUint16 aMaxAttrByteCount,
									 CSdpAttrIdMatchList& aMatchList,
									 const TDesC8& aContState,
									 TRequestStatus& aStatus)
/**
	Ask for an attribute request using an ID match list (CSdpAttrIdMatchList).
	The reply to this function is to send the SIZE of the response back
	in the aResultSize parameter. Once this is done the user of this class
	can set up a buffer big enough to collect the response from server side.
**/
	{
	if(aContState.Length() > KSdpContinuationStateMaxLength)
		{
		User::Leave(KErrArgument);
		}

	__ASSERT_ALWAYS(aMaxAttrByteCount >= 7, AgPanic(ESdpAgentBadResultLength));
	
	TSDPEncodedKeyBuf key;
	key().iQueryType = KSDPEncodedQuery | KSDPBufferResult;
	key().iPduId = EServiceAttributeRequest;

	TUint size = TElementEncoder::EncodedSize(ETypeDES, aMatchList.Count() * 5);
	delete iRequestBuf;
	iRequestBuf = 0;
	iRequestBuf = HBufC8::NewL(size + 4 + 2 + KSdpContinuationStateMaxLength + 1);
	TPtr8 ptr = iRequestBuf->Des();

	TElementEncoder ee(ptr);
	
	// Build the request PDU
	ptr.Copy(key);
	TBuf8<6> sbuf(6);
	BigEndian::Put32(&sbuf[0], aHandle);	// Record handle
	BigEndian::Put16(&sbuf[4], TUint16(aMaxAttrByteCount));	// Max byte count
	ptr.Append(sbuf);
	aMatchList.EncodeL(ee);	// AttrID list
	ptr.Append((TUint8)aContState.Length());
	ptr.Append(aContState);

	iResultSize.Set(TPckg<TInt>(aResultSize));
	iResultSize.SetMax();
	iNetDb.Query(*iRequestBuf, iResultSize, aStatus);
	}

TInt RSdpAgent::RetrieveResult(TDes8& aResult)
/**
	On completion of a service search or attribute request RSdpAgent
	is sent the size of the response by the server. To obtain the actual
	result (buffered in the server) a user needs to call this function.
	NB. This is a synchronous request.
**/
	{
	TPckgBuf<TUint> key(KSDPRetrieveResultQuery);
	return iNetDb.Query(key, aResult);
	}

void RSdpAgent::Cancel()
/**
	Pass "Cancel" call up to RNetDatabase.
**/
	{
	iNetDb.Cancel();
	}



