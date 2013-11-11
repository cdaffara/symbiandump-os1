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


#ifndef __TCLAYER_STEP_STARTERSESSION_H__
#define __TCLAYER_STEP_STARTERSESSION_H__

#include <test/testexecutestepbase.h>

_LIT(KTCCLayerTestStarterSession, "CCLayerTestStarterSession");

class CCLayerTestStarterSession : public CTestStep
	{
public:
	CCLayerTestStarterSession();
	~CCLayerTestStarterSession();

	//from CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	void doTestConnectAndClose();
	void doTestSetState();
	void doTestRequestReset();
	void doTestRequestShutdown();
	void doTestRfForEmergencyCall();
	void doTestEndSplashScreen();

private:
	};

#endif	// __TCLAYER_STEP_STARTERSESSION_H__
