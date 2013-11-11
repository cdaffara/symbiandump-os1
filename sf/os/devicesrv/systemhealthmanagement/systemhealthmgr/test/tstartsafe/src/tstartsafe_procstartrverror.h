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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/
 
#ifndef __TSTARTSAFE_PROCSTARTRVERROR_H__
#define __TSTARTSAFE_PROCSTARTRVERROR_H__


#include <test/testexecutestepbase.h>
#include <startup.hrh>

#include "tstartsafe_asyncnotifier.h"

//APPFWK-STARTSAFE-0017

_LIT(KCTestCaseProcStartRvError, "ProcStartRvError");


class CAppFwkStartSafeTestStepProcStartRvError : public CTestStep, public MSsTestAsyncNotifier
	{
	virtual void SsTestNotify( TInt aNotify );
	
public:
	CAppFwkStartSafeTestStepProcStartRvError();
	~CAppFwkStartSafeTestStepProcStartRvError();

	
public:
	// From CTestStep
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
		
private:
	void DoTestBadRendezvousL();
	void TestProcPanic();
	void DoTestOOML(const TDesC& aTestApp, TStartupType aStartType);
	void DoTestProcPanicL(const TDesC& aTestApp, const TDesC& aPanicType, TStartupType aStartType);
	
private:
	CActiveScheduler* iActiveScheduler;
	RProcess iProc;
	TInt iTried;
	
	};

#endif // __TSTARTSAFE_PROCSTARTRVERROR_H__
