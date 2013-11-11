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

#ifndef __SUPLPROXYPRIVACYSESSION_H__
#define __SUPLPROXYPRIVACYSESSION_H__

#include <e32base.h>
#include "suplproxyserver.h"
#include "lbssuplproxyservermsgenums.h"


class CSuplProxyRequestInfoBase;

/** 
CSuplProxyPrivacySession
*/
class CSuplProxyPrivacySession : public CSession2
	{
	public:
		// Constructor
		CSuplProxyPrivacySession(MSuplProxyPrivacyProtocol& aProtocol, 
				TLbsProxyApiType aApiType);
		virtual ~CSuplProxyPrivacySession();
		
		// CSession2
		void CreateL();
		void ServiceL(const RMessage2& aMessage);
		void Disconnect(const RMessage2& aMessage);

	protected:
		CSuplProxyServer& Server();

	private:
		void CancelVerifyLocationRequest(const RMessage2& aMessage);
		void NotifyLocationRequest(const RMessage2& aMessage);
		void NotifyVerificationTimeout(const RMessage2& aMessage);
		void VerifyLocationRequest(const RMessage2& aMessage);
		void NewPrivacyRequest(const RMessage2& aMessage);
		void RepeatPrivacyRequest(const RMessage2& aMessage);
		void PrivacyRequest(const RMessage2& aMessage, TInt aRequestId);
		void CompleteRequest(const RMessage2& aMessage);
		
		TBool IsRequestInfoValid(const CSuplProxyRequestInfoBase& aRequestInfo);

	private:
		
		MSuplProxyPrivacyProtocol& iPrivacyProtocol;
		TLbsProxyApiType iApiType;

	};

#endif // __SUPLPROXYPRIVACYSESSION_H__
