// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TCLAYER_STEP_SSMSIMSTATUSOBSERVER_H__
#define __TCLAYER_STEP_SSMSIMSTATUSOBSERVER_H__

#include <test/testexecutestepbase.h>

_LIT(KTCCLayerTestSsmSimStatusObserver, "CCLayerTestSsmSimStatusObserver");

class CCLayerTestSsmSimStatusObserver : public CTestStep
	{
public:
	CCLayerTestSsmSimStatusObserver();
	~CCLayerTestSsmSimStatusObserver();

	//from CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	//Helper method
	void CallBackRunL();	
private:
	void doTestInitalizationAndDestructionL();
	void doTestSimChangedAndOwnedL();
	void doTestSimEventsL();
private:
    CAsyncCallBack* iAsyncStopScheduler;
    TRequestStatus iRequestStatus;
    CActiveScheduler* iActiveScheduler;
	};

#endif	// __TCLAYER_STEP_SSMSIMSTATUSOBSERVER_H__
