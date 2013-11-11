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
// The declaration of the CTSY integration TEF test suite server.
// 
//

/**
 @internalTechnology
*/

#ifndef __CCTSYINTEGRATIONTESTSUITESERVER_H__
#define __CCTSYINTEGRATIONTESTSUITESERVER_H__

#include <test/testexecuteserverbase.h>
#include "rpsownnumbers.h"
#include "townnumbersmanager.h"


	
class CEtelSessionMgr;
class CRPSMaster;
class CCTSYIntegrationTestSuiteServer : public CTestServer
	{
public:
	static CCTSYIntegrationTestSuiteServer* NewL();
	~CCTSYIntegrationTestSuiteServer();
	
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

private:
	CTestStep* DoCreateTestStep(const TDesC& aStepName);
	virtual void ConstructL(const TDesC& aName);
	void ReadConfigL();
	void CreateRemotePartySimulatorL();
	
private:
	TBool iKeepSessOpen;
	TName iTsyName;
	TName iPhoneName;
	TBool iUseRps;
	CEtelSessionMgr* iSessManager;
	CRPSMaster* iRPS;
	TBool iRPSPresent;
	TOwnNumbersManager iOwnNumbersManager; //singleton shared across each test step.
	}; // class CCTSYIntegrationTestSuiteServer

#endif // __CCTSYINTEGRATIONTESTSUITESERVER_H__
