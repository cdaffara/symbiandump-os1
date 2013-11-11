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

#ifndef __PRIVACYSESSION_H__
#define __PRIVACYSESSION_H__

#include <e32base.h>
#include "privacyserver.h"
#include "lbsprivacyservermsgenums.h"


class CRequestInfoBase;

/** 
CPrivacySession
*/
class CPrivacySession : public CSession2
	{
	public:
		// Constructor
		CPrivacySession(MPrivacyProtocol& aProtocol, 
				TLbsProxyApiType aApiType);
		virtual ~CPrivacySession();
		
		// CSession2
		void CreateL();
		void ServiceL(const RMessage2& aMessage);
		void Disconnect(const RMessage2& aMessage);

	protected:
		CPrivacyServer& Server();

	private:
		void CancelVerifyLocationRequest(const RMessage2& aMessage);
		void NotifyLocationRequest(const RMessage2& aMessage);
		void NotifyVerificationTimeout(const RMessage2& aMessage);
		void VerifyLocationRequest(const RMessage2& aMessage);
		void NewPrivacyRequest(const RMessage2& aMessage);
		void RepeatPrivacyRequest(const RMessage2& aMessage);
		void PrivacyRequest(const RMessage2& aMessage, TInt aRequestId);
		void CompleteRequest(const RMessage2& aMessage);
		
		TBool IsRequestInfoValid(const CRequestInfoBase& aRequestInfo);

	private:
		
		MPrivacyProtocol& iProtocol;
		TLbsProxyApiType iApiType;

	};

#endif // __PRIVACYSESSION_H__