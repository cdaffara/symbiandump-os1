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
 

#ifndef __TRTC_ADAPTATIONPLUGIN_STEP_H__
#define __TRTC_ADAPTATIONPLUGIN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>
#include "ti_adaptationplugin_stepbase.h"


_LIT(KTCTestRtcAdaptStep,"CTestRtcAdaptationPlugin");

/*
RTC Adaptation related opcodes, used as the different use cases for the test step.
*/
enum TTestSusRtcAdaptionServerRequests
	{
	ETestValidateRtc = 0,
	ETestSetWakeupAlarm,
	ETestUnsetWakeupAlarm,
	ETestRtcCancel
	};

	
class RTestSsmRtcAdaptation: public RSsmRtcAdaptation
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

class CTestRtcAdaptStep;

class CRtcAdaptPlugin : public CActive
	{
public:
	static CRtcAdaptPlugin* NewL(CTestRtcAdaptStep& iTestRtcAdaptStep); 
	void CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusRtcAdaptionServerRequests aRequest);
	~CRtcAdaptPlugin();
	 CRtcAdaptPlugin(CTestRtcAdaptStep& iTestRtcAdaptStep);
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	void Construct();

private:
	RTestSsmRtcAdaptation iRtcAdaptation;
	TRequestStatus *iRtcStatus;
	CTestRtcAdaptStep& iTestRtcAdaptStep;
	TPckgBuf<TBool> iPckgValidity;
	TPckgBuf<TTime> iPckgAlarmTime;
		
	};

class CTestRtcAdaptStep : public CTestAdaptStep
	{
public:
	CTestRtcAdaptStep ();
	~CTestRtcAdaptStep ();

	//from CTestStep
	TVerdict doTestStepL();
		
private:
	void TestRtcState();

private:	
	CRtcAdaptPlugin *iRtcAdapt;
	};

#endif
