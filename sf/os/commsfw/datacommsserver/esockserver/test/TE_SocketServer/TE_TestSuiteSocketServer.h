// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TESTSUITESOCKET_H__)
#define __TESTSUITESOCKET_H__

#include <es_sock.h>
#include <test/testexecuteserverbase.h>

_LIT(KServerName,"TE_SocketServer");

class CTestServerSocketServer : public CTestServer
	{
	public:
		static CTestServerSocketServer* NewL();
		CTestServerSocketServer();
		~CTestServerSocketServer();
		
		CTestStep* CreateTestStep(const TDesC& aStepName);
		
	private:
	};

#endif /* __TESTSUITESOCKET_H__ */

