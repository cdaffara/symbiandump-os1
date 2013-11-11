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

#ifndef __SUPLPROXYSERVER_H__
#define __SUPLPROXYSERVER_H__

#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include "lbssuplproxyservermsgenums.h"

class MSuplProxyPrivacyProtocol;
class MSuplProxyPosProtocol;
class CSuplProxyShutdown;


/** 
CSuplProxyServer
*/
class CSuplProxyServer : public CPolicyServer
	{
	public:
		static CSuplProxyServer* NewL(MSuplProxyPrivacyProtocol& aPrivacyProtocol, MSuplProxyPosProtocol& aPosProtocol, 
										TInt aRefLocSourceId);
		~CSuplProxyServer();
		
		virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

		void SetMaxNetworkSessions(TInt aMaxNetworkSessions);
		
		void IncSessionCount(TLbsProxyApiType aApiType);
		void DecSessionCount(TLbsProxyApiType aApiType);
		TBool MaxSessionsReached(TLbsProxyApiType aApiType) const;

	protected:
		// CPolicyServer methods:
		virtual CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2 &aMsg, TInt &aAction, TSecurityInfo &aMissing);
		
	private:
		// Constructors
		void ConstructL();
		CSuplProxyServer(MSuplProxyPrivacyProtocol& aPrivacyProtocol, MSuplProxyPosProtocol& aPosProtocol, 
						TInt aRefLocSourceId);
		
		/** Maintains a count of the number of active local sessions */
		TInt iLocalSessionCount;

		/** Maintains a count of the number of active network sessions */
		TInt iNetworkSessionCount;

        /** Maintains a count of the number of active positioning sessions */
        TInt iPosSessionCount;
        
		/** The maximum number of network sessions allowed */
		TInt iMaxNetworkSessions;
		
		/** Interface to send privacy requests to */
		MSuplProxyPrivacyProtocol& iPrivacyProtocol;

        /** Interface to send positioning requests to */
        MSuplProxyPosProtocol& iPosProtocol;

		/** The UID of the reference location source plugin */
		TInt iRefLocSourceId;

	};

#endif // __SUPLPROXYSERVER_H__
