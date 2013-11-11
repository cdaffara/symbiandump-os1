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
 

#ifndef __TMISC_ADAPTATIONPLUGIN_STEP_H__
#define __TMISC_ADAPTATIONPLUGIN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>
#include "ti_adaptationplugin_stepbase.h"

/*
Misc Adaptation opcodes, used as the different use cases for the test step.
@released
*/
enum TTestSusMiscAdaptionServerRequests
	{
	ETestSecurityStateChange = 0,
	ETestGetGlobalStartupMode,
	ETestPrepareSimLanguages,
	ETestGetSimLanguages,
	ETestGetHiddenReset,
	ETestMiscCancel
	};
	
class RTestSsmMiscAdaptation: public RSsmMiscAdaptation
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

_LIT(KTCTestMiscAdaptStep,"CTestMiscAdaptationPlugin");

class CTestMiscAdaptStep;

class CMiscAdaptPlugin : public CActive
	{
public:
	static CMiscAdaptPlugin* NewL(CTestMiscAdaptStep& aTestMiscAdaptStep); 
	void CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusMiscAdaptionServerRequests aRequest);
	~CMiscAdaptPlugin();
	 CMiscAdaptPlugin(CTestMiscAdaptStep& aTestMiscAdaptStep);
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	void Construct();

private:
	RTestSsmMiscAdaptation iMiscAdaptation;
	TRequestStatus *iMiscStatus;
	CTestMiscAdaptStep& iTestMiscAdaptStep;  
	
	TInt iState;
	TSsmLanguageListPriority iPriority;
	TBool iGetSimLanguages;
	
	TPckgBuf<TInt> iPckgResponse;
	TPckgBuf<TInt> iPckgMode;
	TPckgBuf<TInt> iPckgSize;
	TPckgBuf<TInt> iPckgList;
	TPckgBuf<TBool> iPckgHiddenReset;
	};

class CTestMiscAdaptStep : public CTestAdaptStep
	{
public:
	CTestMiscAdaptStep ();
	~CTestMiscAdaptStep ();

	//from CTestStep
	TVerdict doTestStepL();
private:
	void TestMIscStateChange();
	
private:
	CMiscAdaptPlugin *iMiscAdapt;
	};



#endif
