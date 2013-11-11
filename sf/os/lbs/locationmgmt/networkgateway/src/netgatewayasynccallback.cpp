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
// The asynchronous callbacks used by the Network Gateway
// 
//

#include <lbs/lbsnetprotocolbase.h>

#include "netgatewayasynccallback.h"
#include "lbsdevloggermacros.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "lbsnetextintversions.h"
#endif


/**
@file
@internalComponent
@released
*/

CRejectPrivacyRequestCallback* CRejectPrivacyRequestCallback::NewL()
	{
	return new(ELeave) CRejectPrivacyRequestCallback;
	}

CRejectPrivacyRequestCallback::CRejectPrivacyRequestCallback() : CAsyncCallBack(EPriorityHigh)
	{
	}

CRejectPrivacyRequestCallback::~CRejectPrivacyRequestCallback()
	{	
	Cancel();
	}

void CRejectPrivacyRequestCallback::CallBack(CLbsNetworkProtocolBase* aProtocol, 
		const TLbsNetSessionId& aSessionId)
	{
	Cancel();
	iProtocol = aProtocol;
	iSessionId = aSessionId;
	CAsyncCallBack::CallBack();
	}

void CRejectPrivacyRequestCallback::RunL()
	{
	__ASSERT_DEBUG(iProtocol, User::Invariant());
	if(!iProtocol)
		{
		return;
		}
	
	CLbsNetworkProtocolBase2* networkProtocolExt = reinterpret_cast<CLbsNetworkProtocolBase2*>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
					(iProtocol->ExtendedInterface(CLbsNetworkProtocolBase::EExtInterface2,0,0));
#else
					(iProtocol->ExtendedInterface(ELbsNetExtInterface2,0,0));
#endif

				
	if (networkProtocolExt != NULL)
		{
		networkProtocolExt->RespondPrivacyRequest(iSessionId, CLbsNetworkProtocolBase::EPrivacyResponseRejected, KErrAccessDenied);
		}
	else 
		{
		iProtocol->RespondPrivacyRequest(iSessionId, CLbsNetworkProtocolBase::EPrivacyResponseRejected);
		}
				
	LBSLOG(ELogP9, "->A CLbsNetworkProtocolBase::RespondPrivacyRequest() ProtocolModule\n");
	LBSLOG(ELogP9, "  > TLbsNetMtLrReponseMsg msg.SessionId()  = \n");
	LBSLOG_SESSIONID(iSessionId);
	LBSLOG2(ELogP9, "  > TLbsNetMtLrReponseMsg msg.Response()  = %d", CLbsNetworkProtocolBase::EPrivacyResponseRejected);
	}
