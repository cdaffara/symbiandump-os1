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

#ifndef __SUPLPROXYPOSSESSION_H__
#define __SUPLPROXYPOSSESSION_H__

#include <e32base.h>
#include "suplproxyserver.h"
#include "lbssuplproxyservermsgenums.h"
#include "suplproxypossubsesregistry.h"

/** 
CSuplProxyPosSession
*/
class CSuplProxyPosSession : public CSession2
	{
	public:
	    static CSuplProxyPosSession* NewL(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId);
		virtual ~CSuplProxyPosSession();
		
		// CSession2
		void CreateL();
		void ServiceL(const RMessage2& aMessage);
		void Disconnect(const RMessage2& aMessage);

	protected:
		CSuplProxyServer& Server();

	private:
        // Constructor
        CSuplProxyPosSession(MSuplProxyPosProtocol& aProtocol, TInt aRefLocSourceId);
        void ConstructL();
	    void OpenSubSessionL(const RMessage2& aMessage);
	    void CloseSubSessionL(const RMessage2& aMessage);	    
	    TInt CreateSubSessionL(const RMessage2& aMessage, TInt aRequestId);
	    void ForwardToSubSessionL(const RMessage2& aMessage);
	private:
		 MSuplProxyPosProtocol& iPosProtocol;     
		 CPosSubsessionRegistry* iSubSessionRegistry;
		 TInt iRefLocSourceId;
	};

#endif // __SUPLPROXYPOSSESSION_H__
