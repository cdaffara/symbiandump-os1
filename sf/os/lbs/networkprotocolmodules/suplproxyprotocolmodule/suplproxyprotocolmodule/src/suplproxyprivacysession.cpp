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
// suplproxyprivacysession.cpp
// 
//

#include "lbsdevloggermacros.h"
#include "suplproxyprivacysession.h"
#include "suplproxyprivacyrequestinfo.h"
#include "suplproxyprotocol.h"
#include "EPos_PrivacyServerPanic.h"
#include <lbs/lbsprivacyrequestcommon.h>


/**
Constructor.

@param aProtocol Interface to send supl proxy requests to.
*/
CSuplProxyPrivacySession::CSuplProxyPrivacySession(MSuplProxyPrivacyProtocol& aProtocol, 
									 TLbsProxyApiType aApiType) :
	iPrivacyProtocol(aProtocol),
	iApiType(aApiType)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CSuplProxyPrivacySession() Begin And End \n");
	}


/**
Destructor.
*/
CSuplProxyPrivacySession::~CSuplProxyPrivacySession()
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::~CSuplProxyPrivacySession() Begin and End\n");
	}


/**
Called when the session is created.
*/
void CSuplProxyPrivacySession::CreateL()
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CreateL() Begin\n");

	Server().IncSessionCount(iApiType);

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CreateL() End\n");
	}


/**
Called when a message is received for this session.

@param aMessage The message from the client.
*/
void CSuplProxyPrivacySession::ServiceL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::ServiceL() Begin\n");

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

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::ServiceL() End\n");
	}


/**
Called when the session disconnects.

@param aMessage The message from the client.
*/
void CSuplProxyPrivacySession::Disconnect(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::Disconnect() Begin\n");

	TInt reason(aMessage.Int1());
	iPrivacyProtocol.PrivacySessionDisconnected(this,reason);
	Server().DecSessionCount(iApiType);
	CSession2::Disconnect(aMessage);

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::Disconnect() End\n");
	}


/**
Get a reference to the server for this session.

@return A reference to the CSuplProxyServer.
*/
CSuplProxyServer& CSuplProxyPrivacySession::Server()
	{
	return *static_cast<CSuplProxyServer*>(const_cast<CServer2*>(CSession2::Server()));
	}


/**
Cancel a verification request.

@param aMessage The message from the client.
*/
void CSuplProxyPrivacySession::CancelVerifyLocationRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CancelVerifyLocationRequest() Begin\n");

	CCancelSuplProxyPrivacyRequestInfo* cancelInfo = NULL;
	TRAPD(err, cancelInfo = CCancelSuplProxyPrivacyRequestInfo::NewL());
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
	iPrivacyProtocol.CancelVerifyLocationRequest(cancelInfo);

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CancelVerifyLocationRequest() End\n");
	}


/**
Submit a notification request.

@param aMessage The message from the client.
*/
void CSuplProxyPrivacySession::NotifyLocationRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::NotifyLocationRequest() Begin\n");

	TLbsNetSessionId sessionId;
	iPrivacyProtocol.GetNextSessionId(sessionId);

	TPckg<TInt> pkgReqId(sessionId.SessionNum());
	TInt error = aMessage.Write(1,pkgReqId);
	if(error != KErrNone)
		{
		aMessage.Complete(error);
		return;
		}

	// NOTE: requestInfo does not complete the message in this case
	CSuplProxyPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CSuplProxyPrivacyRequestInfo::NewL(aMessage));
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

	iPrivacyProtocol.NotifyLocationRequest(requestInfo);
	aMessage.Complete(KErrNone);

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::NotifyLocationRequest() End\n");
	}


/**
Submit a notification timeout request.

@param aMessage The message from the client.
*/
void CSuplProxyPrivacySession::NotifyVerificationTimeout(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::NotifyVerificationTimeout() Begin\n");

	// NOTE: requestInfo does not complete the message in this case
	CSuplProxyPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CSuplProxyPrivacyRequestInfo::NewL(aMessage));
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
	
	// temporary. Since NRH state machine can't deal with receiving a second privacy request while waiting for a location request, complete the first session so we start a new one.
	iPrivacyProtocol.EndPrivacySession(aMessage.Int1(), KErrNone);	 
	
	iPrivacyProtocol.NotifyVerificationTimeout(requestInfo);
	aMessage.Complete(KErrNone);

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::NotifyVerificationTimeout() End\n");
	}


/**
Submit a verify location request.

@param aMessage The message from the client.
*/
void CSuplProxyPrivacySession::VerifyLocationRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::VerifyLocationRequest() Begin\n");

	TLbsNetSessionId sessionId;
	iPrivacyProtocol.GetNextSessionId(sessionId);

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

	CSuplProxyPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CSuplProxyPrivacyRequestInfo::NewL(aMessage));
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
	
	iPrivacyProtocol.VerifyLocationRequest(requestInfo);
	// The message should be completed later in this case

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::VerifyLocationRequest() End\n");
	}

void CSuplProxyPrivacySession::NewPrivacyRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::NewPrivacyRequest() Begin\n");

	TLbsNetSessionId sessionId;
	iPrivacyProtocol.GetNextSessionId(sessionId);

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

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::NewPrivacyRequest() End\n");
	}

void CSuplProxyPrivacySession::RepeatPrivacyRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::RepeatPrivacyRequest() Begin\n");

	PrivacyRequest(aMessage, aMessage.Int0());

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::RepeatPrivacyRequest() End\n");

	}

void CSuplProxyPrivacySession::PrivacyRequest(const RMessage2& aMessage, TInt aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::PrivacyRequest() Begin\n");

	CSuplProxyPrivacyRequestInfo* requestInfo = NULL;
	TRAPD(err, requestInfo = CSuplProxyPrivacyRequestInfo::NewL(aMessage));
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
	
	iPrivacyProtocol.PrivacyLocationRequest(requestInfo);
	// The message should be completed later in this case

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::PrivacyRequest() End\n");

	}

void CSuplProxyPrivacySession::CompleteRequest(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CompleteRequest() Begin\n");

	TUint32 requestId(aMessage.Int0());
	TInt reason(aMessage.Int1());

	iPrivacyProtocol.CompletePrivacyRequest(requestId, reason);

	aMessage.Complete(KErrNone);

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::CompleteRequest() End\n");
	}

/**
Validates the data values for a privacy request.

@param aRequestInfo The request data to validate
*/
TBool CSuplProxyPrivacySession::IsRequestInfoValid(const CSuplProxyRequestInfoBase& aRequestInfo)
	{
	LBSLOG(ELogP1, "CSuplProxyPrivacySession::IsRequestInfoValid() Begin\n");
	
	TBool valid = (aRequestInfo.GetRequestId() >= 1);
	
	switch (aRequestInfo.Type())
		{
		case CSuplProxyRequestInfoBase::ESuplProxyPrivacyRequestTypeRequest:
			{
			break;
			}
		case CSuplProxyRequestInfoBase::ESuplProxyPrivacyRequestTypeCancel:
			{
			const CCancelSuplProxyPrivacyRequestInfo& cancelInfo = 
				static_cast<const CCancelSuplProxyPrivacyRequestInfo&>(aRequestInfo);

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

	LBSLOG(ELogP1, "CSuplProxyPrivacySession::IsRequestInfoValid() End\n");
	
	return valid;
	}

