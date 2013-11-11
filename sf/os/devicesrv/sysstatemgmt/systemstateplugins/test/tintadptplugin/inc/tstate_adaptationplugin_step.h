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
 

#ifndef __TSTATE_ADAPTATIONPLUGIN_STEP_H__
#define __TSTATE_ADAPTATIONPLUGIN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>
#include "ti_adaptationplugin_stepbase.h"


/**
State Adaptation related opcodes, used as the different use cases for the test step.
*/
enum TTestSusStateAdaptionServerRequests
	{
	ETestRequestCoopSysStateChange = 0,
	ETestRequestCoopSysSelfTest,
	ETestRequestCoopSysPerformRestartActions,
	ETestRequestCoopSysPerformShutdownActions,
	ETestRequestCoopSysPerformRfsActions,
	ETestRequestCoopSysCancel,
	};

class RTestSsmStateAdaptation: public RSsmStateAdaptation
	{
public:
	TInt Connect();
	TInt Connect(TInt aAsyncMessageSlotCount);
	void Close();
protected:
	TInt HeapMark();
	TInt HeapMarkEnd();
	TInt CleanupAdaptations();
	};

_LIT(KTCTestStateAdaptStep,"CTestStateAdaptationPlugin");

class CTestStateAdaptStep; 

class CStateAdaptPlugin : public CActive
	{
public:
	static CStateAdaptPlugin* NewL(CTestStateAdaptStep& aTestStateAdaptStep); 
	void CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusStateAdaptionServerRequests aRequest);
 	~CStateAdaptPlugin();
	 CStateAdaptPlugin(CTestStateAdaptStep& aTestStateAdaptStep);
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	void Construct();

private:
	RTestSsmStateAdaptation iStateAdaptation;
	TRequestStatus *iStateStatus;
	CTestStateAdaptStep& iTestStateAdaptStep;
	TPckgBuf<TSsmCoopSysEventType> iPckgEvType;
	};

class CTestStateAdaptStep : public CTestAdaptStep
	{
public:
	CTestStateAdaptStep ();
	~CTestStateAdaptStep ();

	//from CTestStep
	TVerdict doTestStepL();
	
private:
	void TestRequestSysStateChange();
	void TestRequestSysStateChangeCancel();
		
private:
	CStateAdaptPlugin *iStateAdapt;
	};



#endif
