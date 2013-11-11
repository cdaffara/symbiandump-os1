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
 

#ifndef __TCLE_STEP_RCLESESSION_H__
#define __TCLE_STEP_RCLECESSION_H__

#include <test/testexecutestepbase.h>

#include "clesrv.h"
#include "clecli.h"

#include "tcle_cleclient.h"

_LIT(KTCleRCleSessionStep,"CleRCleSessionStep");

class CCleRCleSessionTest : public CTestStep
	{
public:
	CCleRCleSessionTest();
	~CCleRCleSessionTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	CSsmCommandList* CreateListWithWaitForApparcInitL(TInt aCount);
	void ExecuteCommandListL();
	void ExecuteCommandListWithDelaysL();
	void ExecuteEmptyListL();
	void ExecuteListAndCancelL();
	void ExecuteListTocompletionThenCancelL();
	void ExecuteTwoListsL();
	void TryToBreakSessionRegistrationL();
	void ExecuteCmdListPluralityL();
	void OpenHandleExecuteAndCloseL();
	void VariousHandleBadnessL();
	void MultipleCallsToExecuteL();
	
	
private:
	RCleSessionTest iCleCli;
	RCleSessionTest iCleCli2;
	};


#endif
