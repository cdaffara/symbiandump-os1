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
 

#ifndef __TSIM_ADAPTATIONPLUGIN_STEP_H__
#define __TSIM_ADAPTATIONPLUGIN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>
#include "ti_adaptationplugin_stepbase.h"

/*
SIM Adaptation related opcodes, used as the different use cases for the test step.
*/
enum TTestSusSimAdaptionServerRequests
	{
	ETestGetSimOwned = 0,
	ETestGetLastSimEvent,
	ETestNotifySimCancel
	};
	
class RTestSsmSimAdaptation: public RSsmSimAdaptation
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

_LIT(KTCTestSimAdaptStep,"CTestSimAdaptationPlugin");

class CTestSimAdaptStep; 

class CSimAdaptPlugin : public CActive
	{
public:
	static CSimAdaptPlugin* NewL(CTestSimAdaptStep& aTestSimAdaptStep); 
	void CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusSimAdaptionServerRequests aRequest);
	~CSimAdaptPlugin();
	 CSimAdaptPlugin(CTestSimAdaptStep& aTestSimAdaptStep);
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	void Construct();

private:
	RTestSsmSimAdaptation iSimAdaptation;
	TRequestStatus *iSimStatus;
	TPckgBuf<TBool> iSimStatusPckg;
	TPckgBuf<TSsmSimEventType> iEventPckg;
	CTestSimAdaptStep& iTestSimAdaptStep;  
	};

class CTestSimAdaptStep : public CTestAdaptStep
	{
public:
	CTestSimAdaptStep ();
	~CTestSimAdaptStep ();

	//from CTestStep
	TVerdict doTestStepL();
private:
	void TestSimStateChange();

private:
	CSimAdaptPlugin *iSimAdapt;
	};



#endif
