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

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include "debug.h"
#include <btsdp.h>
#include "sdpnetdb.h"
#include "sdp.h"
#include "sdppdu.h"
#include "sdpkey.h"
#include "sdpstackutil.h"
#include "sdpclient.h"

#include "BTSec.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDP);
#endif

//Diagnostic string for security check failures, in builds without platsec
//diagnostics this will be NULL.
const char* const KBT_SDPNETDB_NAME_DIAG = __PLATSEC_DIAGNOSTIC_STRING("Bluetooth SDP Net Database");


CSdpNetDbProvider::CSdpNetDbProvider(CSdpProtocol& aProtocol)
: iProtocol(aProtocol)
	{
	LOG_FUNC
	}

void CSdpNetDbProvider::ConstructL()
	{
	LOG_FUNC
	}

CSdpNetDbProvider* CSdpNetDbProvider::NewL(CSdpProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	CSdpNetDbProvider* self = new(ELeave) CSdpNetDbProvider(aProtocol);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpNetDbProvider::~CSdpNetDbProvider()
	{
	LOG_FUNC
	if (iClient)
		iClient->RemoveNetDbProvider(*this);
	delete iResultBuf;
	}

void CSdpNetDbProvider::Query(TDes8& aBuffer)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iQueryBuffer, Panic(ESdpTwoQuerys));
	iQueryBuffer = &aBuffer;
	if (iQueryBuffer->Length() < TInt(sizeof(TUint)))
		{// Not big enough to hold the query type!!
		Error(KErrSdpBadRequestBufferLength);
		return;
		}
	TUint type = *reinterpret_cast<const TUint*>(aBuffer.Ptr());

	switch(type & 0x7f)
		{
	case KSDPConnectQuery:
		ConnectQuery();
		break;
	case KSDPServiceQuery:
		ServiceQuery();
		break;
	case KSDPAttributeQuery:
		AttributeQuery();
		break;
	case KSDPCancelQuery:
		CancelCurrentOperation();
		break;
	case KSDPEncodedQuery:
		EncodedQuery();
		break;
	case KSDPRetrieveResultQuery:
		RetrieveResult();
		break;
	default:
		Error(KErrNotSupported);
		return;
		};
	}

void CSdpNetDbProvider::Add(TDes8& /*aBuffer*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iQueryBuffer, Panic(ESdpTwoQuerys));
	iNotify->QueryComplete(KErrNotSupported);
	}

void CSdpNetDbProvider::Remove(TDes8& /*aBuffer*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iQueryBuffer, Panic(ESdpTwoQuerys));
	iNotify->QueryComplete(KErrNotSupported);
	}

void CSdpNetDbProvider::CancelCurrentOperation()
	{
	LOG_FUNC
	iQueryBuffer = 0;
	}

void CSdpNetDbProvider::QueryComplete(const TDesC8& aData)
	{
	LOG_FUNC
	if (!iQueryBuffer)
		return;

	TInt ret = KErrNone;
	TUint type = *reinterpret_cast<const TUint*>(iQueryBuffer->Ptr());
	if (type & 0x80)
		{// Buffer result and Just send size back
		delete iResultBuf;
		iResultBuf = 0;
		TRAP(ret, iResultBuf = aData.AllocL());
		if (ret == KErrNone)
			{
			iQueryBuffer->Copy(TPckgC<TInt>(aData.Length()));
			}
		}
	else
		{// Copy the whole result back
		ret = ReturnResult(aData);
		}
	iNotify->QueryComplete(ret);
	iQueryBuffer = 0;
	}

TInt CSdpNetDbProvider::ReturnResult(const TDesC8& aData)
	{
	LOG_FUNC
	if(!iQueryBuffer)
		{
		return KErrGeneral;
		}

	if (iQueryBuffer->MaxLength() < aData.Length())
		{
		return KErrArgument;
		}
	iQueryBuffer->Copy(aData);
	return KErrNone;
	}

void CSdpNetDbProvider::ClientUp()
	{
	LOG_FUNC
	if(!iQueryBuffer)
		return;

	iQueryBuffer->Zero();
	QueryComplete(KNullDesC8);
	}

void CSdpNetDbProvider::ClientDown()
	{
	LOG_FUNC
	if (iClient) 
		iClient->RemoveNetDbProvider(*this);
	iClient = 0;
	Error(KErrDisconnected);
	}

void CSdpNetDbProvider::Error(TInt aErrorCode)
	{
	LOG_FUNC
	LOG1(_L("SDP: Error encountered in SDP NetDb: %d\n"), aErrorCode);
	if(!iQueryBuffer)
		return;

	iNotify->QueryComplete(aErrorCode);
	iQueryBuffer = 0;
	}
	
TInt CSdpNetDbProvider::SecurityCheck(MProvdSecurityChecker *aSecurityChecker)
	{
	LOG_FUNC	
	__ASSERT_ALWAYS(aSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
	
	iSecurityChecker = aSecurityChecker;
	return iSecurityChecker->CheckPolicy(KLOCAL_SERVICES, KBT_SDPNETDB_NAME_DIAG);			
	}

void CSdpNetDbProvider::ConnectQuery()
	{
	LOG_FUNC
	if(iClient)
		{
		LOG(_L("SDP: ConnectQuery request from net db whilst already connected: REJECTED!\n"));
		Error(KErrInUse);
		return;
		}
	if (iQueryBuffer->Length() != sizeof(TSDPConnectQuery))
		{
		LOG(_L("SDP: ConnectQuery request from net db with bad query length: REJECTED!\n"));
		Error(KErrSdpBadRequestBufferLength);
		return;
		}
	LOG(_L("SDP: ConnectQuery request from net db\n"));
	TSDPConnectQuery& queryKey = *(TSDPConnectQuery*)iQueryBuffer->Ptr();
	iRemoteDev = queryKey.iAddr;
	iProtocol.GetClient(*this);
	}

void CSdpNetDbProvider::ServiceQuery()
	{
	LOG_FUNC
	if (!iClient)
		{
		Error(KErrSdpClientNotConnected);
		return;
		}
	if (iQueryBuffer->Length() != sizeof(TSDPServiceSearchKey))
		{
		Error(KErrSdpBadRequestBufferLength);
		return;
		}
	TSDPServiceSearchKey& queryKey = *(TSDPServiceSearchKey*)iQueryBuffer->Ptr();
	// Ought to do this check, but can't because of esock defect EDNJDIN-4HPKJZ.
//	if (iQueryBuffer->MaxLength() < (TInt)(queryKey.iMaxCount*sizeof(TUint)) + 21)
//		{
//		Error(KErrSdpBadResultBufferLength);
//		return;
//		}

	if (queryKey.iStateLength > KSdpContinuationStateMaxLength)
		{
		Error(KErrSdpBadContinuationState);
		return;
		}

	TPtrC8 contState(queryKey.iContinuationState, queryKey.iStateLength);

	iClient->ServiceSearchRequest(*this, 
		                          queryKey.iMaxCount, 
								  queryKey.iUUID, 
								  contState);
	}

void CSdpNetDbProvider::AttributeQuery()
	{
	LOG_FUNC
	if (!iClient)
		{
		Error(KErrSdpClientNotConnected);
		return;
		}
	if (iQueryBuffer->Length() != sizeof(TSDPAttributeKey))
		{
		Error(KErrSdpBadRequestBufferLength);
		return;
		}
	TSDPAttributeKey& queryKey = *(TSDPAttributeKey*)iQueryBuffer->Ptr();

	// Ought to do this check, but can't because of esock defect EDNJDIN-4HPKJZ.
//	if (iQueryBuffer->MaxLength() < queryKey.iMaxLength + 19)
//		{
//		Error(KErrSdpBadResultBufferLength);
//		return;
//		}

	if (queryKey.iStateLength > KSdpContinuationStateMaxLength)
		{
		Error(KErrSdpBadContinuationState);
		return;
		}

	TPtrC8 contState(queryKey.iContinuationState, queryKey.iStateLength);
	iClient->ServiceAttributeRequest(*this, 
		                             queryKey.iServiceRecordHandle,
		                             queryKey.iMaxLength,
									 queryKey.iRange,
									 queryKey.iAttribute,
									 contState);
	}

void CSdpNetDbProvider::EncodedQuery()
	{
	LOG_FUNC
	if (!iClient)
		{
		Error(KErrSdpClientNotConnected);
		return;
		}
	if (iQueryBuffer->Length() <= STATIC_CAST(TInt,sizeof(TSDPEncodedKey/*Buf??*/)))
		{
		Error(KErrSdpBadRequestBufferLength);
		return;
		}
	TSDPEncodedKey& queryKey = *(TSDPEncodedKey*)iQueryBuffer->Ptr();
	const TUint8* pData = iQueryBuffer->Ptr();
	pData += sizeof(TSDPEncodedKey/*Buf??*/);
	TPtrC8 dataDesc(pData, iQueryBuffer->Length()-sizeof(TSDPEncodedKey));
	//TPtrC8 dataDesc(iQueryBuffer->Mid(sizeof(TSDPEncodedKey)));

	iClient->EncodedRequest(*this, 
		                    queryKey.iPduId, 
							dataDesc);
	}


#if 0
void CSdpNetDbProvider::EncodedQuery()
	{
	LOG_FUNC
	if (!iClient)
		{
		Error(KErrSdpClientNotConnected);
		return;
		}
	if (iQueryBuffer->Length() < sizeof(TSDPEncodedKey/*Buf??*/))
		{
		Error(KErrSdpBadRequestBufferLength);
		return;
		}


	TSDPEncodedKey& queryKey = *(TSDPEncodedKey*)iQueryBuffer->Ptr();
	TPtrC8 dataDesc(iQueryBuffer->Mid(sizeof(TSDPEncodedKey)));

	iClient->EncodedRequest(*this, 
							queryKey.iPduId, 
							dataDesc);
	}
#endif



void CSdpNetDbProvider::RetrieveResult()
	{
	LOG_FUNC
	TInt ret;
	if (!iResultBuf)
		{// No result to send
		ret = KErrArgument;
		}
	else
		{// Copy over the result
		ret = ReturnResult(*iResultBuf);
		delete iResultBuf;
		iResultBuf = 0;
		}
	iNotify->QueryComplete(ret);
	iQueryBuffer = 0;
	}

