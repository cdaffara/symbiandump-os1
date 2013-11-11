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

#if (!defined __TESTEINTSOCKSERVER_H__)
#define __TESTEINTSOCKSERVER_H__

#include <es_sock.h>
#include <test/testexecuteserverbase.h>

_LIT(KServerName,"TE_EIntSock");

class CTestEIntSockServer : public CTestServer
	{
	public:
		static CTestServer* NewL();
		CTestEIntSockServer();
		~CTestEIntSockServer();
		
		CTestStep* CreateTestStep(const TDesC& aStepName);
		
	private:
	};

#endif /* __TESTSUITESOCKET_H__ */

