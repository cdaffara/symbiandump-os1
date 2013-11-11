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
 

#ifndef __TCMN_STEP_STATEAWARESESSION_H__
#define __TCMN_STEP_STATEAWARESESSION_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmstateawaresession.h>

_LIT(KTCmnStateAwareSessionStep,"CmnStateAwareSessionStep");

class CCmnStateAwareSessionTest : public CTestStep, public MStateChangeNotificationSubscriber
	{
public:
	CCmnStateAwareSessionTest();
	~CCmnStateAwareSessionTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	//from MStateChangeNotificationSubscriber
	void StateChanged(TSsmState aSsmState);
	
private:
	void doTestForMemoryLeaksL();
	void doTestRConnectL();
	void StartThread(TInt aOption);
	void doTestCSsmStateAwareSessionL();
	
	RArray<TSsmState> iStateChanges;
	
	//void doTestfunctionality();
	};

#endif
