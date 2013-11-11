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

#ifndef __PRIVACYSERVER_H__
#define __PRIVACYSERVER_H__

#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include "lbsprivacyservermsgenums.h"

class MPrivacyProtocol;
class CPrivacyShutdown;


/** 
CPrivacyServer
*/
class CPrivacyServer : public CPolicyServer
	{
	public:
		static CPrivacyServer* NewL(MPrivacyProtocol& aProtocol);
		~CPrivacyServer();
		
		// CPolicyServer methods:
		
		virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

		void SetShutdownDelay(TInt32 aShutdownDelay);
		void SetMaxNetworkSessions(TInt aMaxNetworkSessions);
		
		void IncSessionCount(TLbsProxyApiType aApiType);
		void DecSessionCount(TLbsProxyApiType aApiType);
		TBool MaxSessionsReached(TLbsProxyApiType aApiType) const;

	protected:
		
		virtual CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2 &aMsg, TInt &aAction, TSecurityInfo &aMissing);
		
	private:
		// Constructors
		void ConstructL();
		CPrivacyServer(MPrivacyProtocol& aProtocol);
		
		/** Maintains a count of the number of active local sessions */
		TInt iLocalSessionCount;

		/** Maintains a count of the number of active network sessions */
		TInt iNetworkSessionCount;

		/** The maximum number of network sessions allowed */
		TInt iMaxNetworkSessions;
		
		/** Interface to send privacy requests to */
		MPrivacyProtocol& iProtocol;

		/** Timer used to shutdown LBS after the last session has finished */
		CPrivacyShutdown* iShutdown;

		/** Time to wait after the last client disconnects before shutting down LBS */
		TInt32 iShutdownDelay;

	};

#endif // __PRIVACYSERVER_H__
