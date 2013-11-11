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

/**
 @file
 @internalComponent
 @released
*/
#ifndef __NETGATEWAYASYNCCALLBACK_H__
#define __NETGATEWAYASYNCCALLBACK_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>

class CLbsNetworkProtocolBase;

/**
The async callback class used to reject the PrivacyRequest from a PM

@see CNetworkGateway::ProcessPrivacyRequest
@see CAsyncCallBack
*/
class CRejectPrivacyRequestCallback : public CAsyncCallBack
	{
public:
	static CRejectPrivacyRequestCallback* NewL();
	virtual ~CRejectPrivacyRequestCallback();
	
	void CallBack(CLbsNetworkProtocolBase* aProtocol, const TLbsNetSessionId& aSessionId);
	
protected:
	virtual void RunL();
	
private:
	CRejectPrivacyRequestCallback();
	
private:
	CLbsNetworkProtocolBase* iProtocol;
	TLbsNetSessionId iSessionId;
	};

#endif //__NETGATEWAYASYNCCALLBACK_H__
