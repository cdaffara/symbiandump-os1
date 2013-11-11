// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestCaseRootServer which is the base class for all 
// the ROOTSERVER suite test cases

#if (!defined __ROOTSERVERTESTSTEP_H__)
#define __ROOTSERVERTESTSTEP_H__

// EPOC includes
#include <e32base.h>

// RootServer includes
#include <es_mbman.h>

#include "RootServerTest.h"
#include "TestAsynchHandler.h"
#include <comms-infras/commsdebugutility.h>
#include "rsshared.h"
#include "cfshared.h"
#include "c32cmiutils.h"
#include "TE_RootServerServer.h"

_LIT8(KModule1, "module1");
_LIT8(KModule2, "module2");
_LIT8(KSubModule1, "Test Protocol1");
_LIT8(KSubModule2, "Test Protocol2");

class CTE_RootServerServer;
class CTestAsynchHandler;

class CTestStepRootServer : public CTestStep
{
public:
	CTestStepRootServer(CTE_RootServerServer *aSuite);
	~CTestStepRootServer();

	// config file access - RootServer specific


	// override pre and post ambles
	TVerdict doTestStepPreambleL( void );
	TVerdict doTestStepPostambleL( void );

	// global methods
	void GetTestStepServerName(const TDesC &aNamePostFix, TDes &aServerName);


	// pointer to suite which owns this test 
	CTE_RootServerServer * iRootServerSuite;

	// expected test server lifetime in microsecs
	static const TInt isTestServerLifeTime;

	// time to wait for all test servers to die
	static const TInt isTestServerWaitTime;

	// time to delay testserver statrup rendezvous when required
	static const TInt isRendezvousDelayTime;

	// Narrow-char overloads; would be better in base class
	enum { EMaxLogLength = 256 };
	void Log( TRefByValue<const TDesC8> aFormat, ... );
	void Log( TInt aSeverity, TRefByValue<const TDesC8> aFormat, ... );
	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC8> aFmt,...);

	// And forwards to the base class, since the narrow overloads mask the usual ones
	void Log( TRefByValue<const TDesC16> aFormat, ... );
	void Log( TInt aSeverity, TRefByValue<const TDesC16> aFormat, ... );
	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...);

protected:
	void DefaultStartParams(RootServer::TRSStartModuleParams& aStartParams, TTestModuleIniData& iIniData);
	void LoadNormalModuleL(CTestAsynchHandler* aHandler, const TDesC8& aModule, TThreadPriority aPriority = EPriorityNormal);
	TInt TryHierachicalBind(CTestAsynchHandler* aHandler, 
							const TDesC8& aTopModule, const TDesC8& aTopSubModule, 
							const TDesC8& aBottomModule, const TDesC8& aBottomSubModule);

private:

};

#endif /* __ROOTSERVERTESTSTEP_H__ */
