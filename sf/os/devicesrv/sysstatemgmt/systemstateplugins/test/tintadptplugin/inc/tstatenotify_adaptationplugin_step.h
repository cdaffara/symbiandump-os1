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
 

#ifndef __TSTATENOTIFY_ADAPTATIONPLUGIN_STEP_H__
#define __TSTATENOTIFY_ADAPTATIONPLUGIN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>
#include "ti_adaptationplugin_stepbase.h"

/**
StateNotify Adaptation related opcodes, used as the different use cases for the test step.
*/
enum TTestSusStateNotifyAdaptionServerRequests
	{
	ETestNotifyCoopSysEvent = 0,
	ETestNotifyCoopSysCancel
	};

_LIT(KTCTestStateNotifyAdaptStep,"CTestStateNotifyAdaptStep");
const TInt KTestStateNotifyStepDelay = 10000000;	//delay required for completing the outstanding request

class CStateNotifyAdaptPlugin : public CActive
	{
public:
	static CStateNotifyAdaptPlugin* NewL(); 
	void CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusStateNotifyAdaptionServerRequests aRequest);
 	~CStateNotifyAdaptPlugin();
	 CStateNotifyAdaptPlugin();
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	void Construct();

private:
	RSsmStateAdaptation iStateNotifyAdaptation;
	TRequestStatus *iStateNotifyStatus;
	TPckgBuf<TSsmCoopSysEventType> iEventTypePckg;
	};

class CTestStateNotifyAdaptStep : public CTestAdaptStep
	{
public:
	CTestStateNotifyAdaptStep ();
	~CTestStateNotifyAdaptStep ();

	//from CTestStep
	TVerdict doTestStepL();
private:
	void TestRequestSysStateNotifyChange();
		
private:
	CStateNotifyAdaptPlugin *iStateNotifyAdapt;
	};



#endif
