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
// privacyserver.cpp
// 
//

#include "lbsdevloggermacros.h"
#include "privacysession.h"
#include "privacyrequestinfo.h"
#include "privacyprotocol.h"
#include "EPos_PrivacyServerPanic.h"
#include <lbs/lbsprivacyrequestcommon.h>


/**
Constructor.

@param aProtocol Interface to send privacy requests to.
*/
CPrivacySession::CPrivacySession(MPrivacyProtocol& aProtocol, 
									 TLbsProxyApiType aApiType) : 
	iProtocol(aProtocol),
	iApiType(aApiType)
	{
	LBSLOG(ELogP1, "CPrivacySession::CPrivacySession()\n");
	}


/**
Destructor.
*/
CPrivacySession::~CPrivacySession()
	{
	LBSLOG(ELogP1, "CPrivacySession::~CPrivacySession()\n");
	}


/**
Called when the session is created.
*/
void CPrivacySession::CreateL()
	{
	LBSLOG(ELogP1, "CPrivacySession::CreateL() Begin\n");
	Server().IncSessionCount(iApiType);
	LBSLOG(ELogP1, "CPrivacySession::CreateL() End\n");
	}


/**
Called when a message is received for this session.

@param aMessage The message from the client.
*/
void CPrivacySession::ServiceL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacySession::ServiceL() Begin\n");
	switch (aMessage.Function())
		{
		case ELbsPrivacyServerNotifyLocation:
			NotifyLocationRequest(aMessage);
			break;
			
		case ELbsPrivacyServerVerifyLocation:
			VerifyLocationRequest(aMessage);
			break;
			
		case ELbsPrivacyServerNotifyVerificationTimeout:
			NotifyVerificationTimeout(aMessage);
			break;
			
		case ELbsPrivacyServerCancelVerifyLocationRequest:
			CancelVerifyLocationRequest(aMessage);
			break;

		case ELbsPrivacyServerNewPrivacyRequestLocal:
		case ELbsPrivacyServerNewPrivacyRequestNetwork:
			NewPrivacyRequest(aMessage);
			break;

		case ELbsPrivacyServerRepeatPrivacyRequestLocal:
		case ELbsPrivacyServerRepeatPrivacyRequestNetwork:
			RepeatPrivacyRequest(aMessage);
			break;

		case ELbsPrivacyServerCompleteRequest:
			CompleteRequest(aMessage);
			break;
		
		default:
			User::Leave(KErrNotSupported);
		}
	LBSLOG(ELogP1, "CPrivacySession::ServiceL() End\n");
	}


/**
Called when the session disconnects.

@param aMessage The message from the client.
*/
void CPrivacySession::Disconnect(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacySession::Disconnect() Begin\n");
	iProtocol.SessionDisconnected(this);
	Server().DecSessionCount(iApiType);
	CSession2::Disconnect(aMessage);
	LBSLOG(ELogP1, "CPrivacySession::Disconnect() End\n");
	}


/**
Get a reference to the server for this session.

@return A reference to the CPrivacyServer.
*/
CPrivacyServer& CPrivacySession::Server()
	{
	return *static_cast<CPrivacyServer*>(const_cast<CServer2*>(CSession2::Server()));
	}


/**
Cancel a verification request.

@param aMessage The message from the client.
*/
void CPrivacySession::CancelVerifyLocationRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacySession::CancelVerifyLocationRequest() Begin\n");

	CCancelPrivacyRequestInfo* cancelInfo = NULL;
	TRAPD(err, cancelInfo = CCancelPrivacyRequestInfo::NewL());
	if (err != KErrNone)
		{
		// Error creating request info object, complete with the error and return
		aMessage.Complete(err);
		return;
		}
	
	cancelInfo->SetRequestId(aMessage.Int0());
	cancelInfo->SetCancelReason(aMessage.Int1());

	if (!IsRequestInfoValid(*cancelInfo))
		{
		aMessage.Panic(KPosPrivacyPanicCategory,
					   EPosPrivSrvPanicRequestOverflow);
		delete cancelInfo;
		return;
		}
	
	aMessage.Complete(KErrNone);
	iProtocol.CancelVerifyLocationRequest(cancelInfo);

	LBSLOG(ELogP1, "CPrivacySession::CancelVerifyLocationRequest() End\n");
	}


/**
Submit a notification request.

@param aMessage The message from the client.
*/
void CPrivacySession::NotifyLocationRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacySession::NotifyLocationRequest() Begin\n");

	TLbsNetSessionId sessionId;
	iProtocol.GetNextSessionId(sessionId);

	TPckg<TInt> pkgReqId(sessionId.SessionNum());
	TInt error = aMessage.Write(1,pkgReqId);
	if(error != KErrNone)
		{
		aMessage.Complete(error);
		return;
		}

	// NOTE: requestInfo does not complete the message in this case
	CPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CPrivacyRequestInfo::NewL(aMessage));
	if (err != KErrNone)
		{
		// Error creating request info object, complete with the error and return
		aMessage.Complete(err);
		return;
		}

	TLbsExternalRequestInfo2 extRequestInfo;
    TPckg<TLbsExternalRequestInfo2> extRequestInfoPkg(extRequestInfo);
    aMessage.Read(0,extRequestInfoPkg);
    requestInfo->SetRequestInfo(extRequestInfo);

	requestInfo->SetRequestId(sessionId.SessionNum());
	
	TLbsNetPosRequestPrivacy netPosRequestPrivacy;
	TPckg<TLbsNetPosRequestPrivacy>	netPosRequestPrivacyPkg(netPosRequestPrivacy);
	aMessage.Read(2,netPosRequestPrivacyPkg);
	requestInfo->SetRequestPrivacy(netPosRequestPrivacy);
    
    requestInfo->SetIsResponseRequired(EFalse);
    
	if (!IsRequestInfoValid(*requestInfo))
		{
		aMessage.Panic(KPosPrivacyPanicCategory,
					   EPosPrivSrvPanicInvalidArguments);
		delete requestInfo;
		return;
		}

	iProtocol.NotifyLocationRequest(requestInfo);
	aMessage.Complete(KErrNone);

	LBSLOG(ELogP1, "CPrivacySession::NotifyLocationRequest() End\n");
	}


/**
Submit a notification timeout request.

@param aMessage The message from the client.
*/
void CPrivacySession::NotifyVerificationTimeout(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacySession::NotifyVerificationTimeout() Begin\n");

	// NOTE: requestInfo does not complete the message in this case
	CPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CPrivacyRequestInfo::NewL(aMessage));
	if (err != KErrNone)
		{
		// Error creating request info object, complete with the error and return
		aMessage.Complete(err);
		return;
		}
    
    TLbsExternalRequestInfo2 extRequestInfo;
    TPckg<TLbsExternalRequestInfo2> extRequestInfoPkg(extRequestInfo);
    aMessage.Read(0,extRequestInfoPkg);
    requestInfo->SetRequestInfo(extRequestInfo);
    
	requestInfo->SetRequestId(aMessage.Int1());

	TLbsNetPosRequestPrivacy netPosRequestPrivacy;
	TPckg<TLbsNetPosRequestPrivacy>	netPosRequestPrivacyPkg(netPosRequestPrivacy);
	aMessage.Read(2,netPosRequestPrivacyPkg);
	requestInfo->SetRequestPrivacy(netPosRequestPrivacy);
    
	requestInfo->SetIsResponseRequired(EFalse);
	
	if (!IsRequestInfoValid(*requestInfo))
		{
		aMessage.Panic(KPosPrivacyPanicCategory,
					   EPosPrivSrvPanicInvalidArguments);
		delete requestInfo;
		return;
		}
	
	iProtocol.NotifyVerificationTimeout(requestInfo);
	aMessage.Complete(KErrNone);

	LBSLOG(ELogP1, "CPrivacySession::NotifyVerificationTimeout() End\n");
	}


/**
Submit a verify location request.

@param aMessage The message from the client.
*/
void CPrivacySession::VerifyLocationRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CPrivacySession::VerifyLocationRequest() Begin\n");

	TLbsNetSessionId sessionId;
	iProtocol.GetNextSessionId(sessionId);

	TPckg<TInt> pkgReqId(sessionId.SessionNum());
	TInt error = aMessage.Write(0,pkgReqId);
	if(error != KErrNone)
		{
		aMessage.Complete(error);
		return;
		}
	
	RSemaphore semaphore;
	error = semaphore.Open(aMessage, 3, EOwnerThread);
	if(error != KErrNone)
		{
		aMessage.Complete(error);
		return;
		}
	semaphore.Signal();
	semaphore.Close();

	CPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CPrivacyRequestInfo::NewL(aMessage));
	if (err != KErrNone)
		{
		// Error creating request info object, complete with the error and return
		aMessage.Complete(err);
		return;
		}

	requestInfo->SetRequestId(sessionId.SessionNum());

    TLbsExternalRequestInfo2 extRequestInfo;
    TPckg<TLbsExternalRequestInfo2> extRequestInfoPkg(extRequestInfo);
    aMessage.Read(1,extRequestInfoPkg);
    requestInfo->SetRequestInfo(extRequestInfo);

	TLbsNetPosRequestPrivacy netPosRequestPrivacy;
	TPckg<TLbsNetPosRequestPrivacy>	netPosRequestPrivacyPkg(netPosRequestPrivacy);
	aMessage.Read(2,netPosRequestPrivacyPkg);
	requestInfo->SetRequestPrivacy(netPosRequestPrivacy);
	
	requestInfo->SetIsResponseRequired(ETrue);

	if (!IsRequestInfoValid(*requestInfo))
		{
		aMessage.Panic(KPosPrivacyPanicCategory,
					   EPosPrivSrvPanicInvalidArguments);
		delete requestInfo;
		return;
		}
	
	iProtocol.VerifyLocationRequest(requestInfo);
	// The message should be completed later in this case

	LBSLOG(ELogP1, "CPrivacySession::VerifyLocationRequest() End\n");
	}

void CPrivacySession::NewPrivacyRequest(const RMessage2& aMessage)
	{
	TLbsNetSessionId sessionId;
	iProtocol.GetNextSessionId(sessionId);

	TPckg<TUint32> pkgReqId(sessionId.SessionNum());
	TInt error = aMessage.Write(0,pkgReqId);
	if(error != KErrNone)
		{
		aMessage.Complete(error);
		return;
		}
	
	RSemaphore semaphore;
	error = semaphore.Open(aMessage, 3, EOwnerThread);
	if(error != KErrNone)
		{
		aMessage.Complete(error);
		return;
		}
	semaphore.Signal();
	semaphore.Close();

	PrivacyRequest(aMessage, sessionId.SessionNum());
	}

void CPrivacySession::RepeatPrivacyRequest(const RMessage2& aMessage)
	{
	PrivacyRequest(aMessage, aMessage.Int0());
	}

void CPrivacySession::PrivacyRequest(const RMessage2& aMessage, TInt aRequestId)
	{
	CPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CPrivacyRequestInfo::NewL(aMessage));
	if (err != KErrNone)
		{
		// Error creating request info object, complete with the error and return
		aMessage.Complete(err);
		return;
		}

	requestInfo->SetRequestId(aRequestId);

    TLbsExternalRequestInfo2 extRequestInfo;
    TPckg<TLbsExternalRequestInfo2> extRequestInfoPkg(extRequestInfo);
    aMessage.Read(1,extRequestInfoPkg);
    requestInfo->SetRequestInfo(extRequestInfo);

	TLbsNetPosRequestPrivacy netPosRequestPrivacy;
	TPckg<TLbsNetPosRequestPrivacy> netPosRequestPrivacyPkg(netPosRequestPrivacy);
	aMessage.Read(2,netPosRequestPrivacyPkg);
	
    requestInfo->SetRequestPrivacy(netPosRequestPrivacy);
	
	requestInfo->SetIsResponseRequired(ETrue);
	requestInfo->SetIsSessionCompleteAutomatic(EFalse);
	requestInfo->SetConvertResponseCode(EFalse);

	if (!IsRequestInfoValid(*requestInfo))
		{
		aMessage.Complete(netPosRequestPrivacy.RequestAction());
		delete requestInfo;
		return;
		}
	
	iProtocol.PrivacyLocationRequest(requestInfo);
	// The message should be completed later in this case

	}

void CPrivacySession::CompleteRequest(const RMessage2& aMessage)
	{

	TUint32 requestId(aMessage.Int0());
	TInt reason(aMessage.Int1());

	iProtocol.CompletePrivacyRequest(requestId, reason);

	aMessage.Complete(KErrNone);
	}

/**
Validates the data values for a privacy request.

@param aRequestInfo The request data to validate
*/
TBool CPrivacySession::IsRequestInfoValid(const CRequestInfoBase& aRequestInfo)
	{
	LBSLOG(ELogP1, "CPrivacySession::IsRequestInfoValid() Begin\n");
	
	TBool valid = (aRequestInfo.GetRequestId() >= 1);
	
	switch (aRequestInfo.Type())
		{
		case CRequestInfoBase::EPrivacyRequestTypeRequest:
			{
			break;
			}
		case CRequestInfoBase::EPrivacyRequestTypeCancel:
			{
			const CCancelPrivacyRequestInfo& cancelInfo = 
				static_cast<const CCancelPrivacyRequestInfo&>(aRequestInfo);

			valid &= (cancelInfo.CancelReason() == KErrCancel
					  || cancelInfo.CancelReason() == KErrTimedOut);
			break;
			}
		default:
			{
			valid = EFalse;
			break;
			}
		}
		
	return valid;
	
	LBSLOG(ELogP1, "CPrivacySession::IsRequestInfoValid() End\n");
	}

