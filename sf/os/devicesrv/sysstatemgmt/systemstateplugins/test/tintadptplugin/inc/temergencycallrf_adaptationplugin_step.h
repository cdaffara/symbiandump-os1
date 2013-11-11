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
 
#ifndef __TEMERGENCYCALL_ADAPTATIONPLUGIN_STEP_H__
#define __TEMERGENCYCALL_ADAPTATIONPLUGIN_STEP_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>
#include <ssm/ssmadaptationcli.h>
#include "ti_adaptationplugin_stepbase.h"


_LIT(KTCTestEmergencyAdaptStep,"CTestEmergencyCallRfAdaptationPlugin");

/*
Emergency call Rf Adaptation opcodes,used as the different use cases for the test step
*/
enum TTestSusEmergencyCallAdaptionServerRequests
	{
	ETestActivateRfForEmergencyCall=0,
	ETestDeactivateRfForEmergencyCall,
	ETestEmergencyCallRfActAndDeActCall,
	ETestEmergencyCallRfCancel
	};
	
class RTestSsmEmergencyCallRfAdaptation: public RSsmEmergencyCallRfAdaptation
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

class CTestEmergencyAdaptStep;

class CEmergencyAdaptPlugin : public CActive
	{
public:
	static CEmergencyAdaptPlugin* NewL(CTestEmergencyAdaptStep& aTestEmergencyAdaptStep); 
	void CallCommandsOfAdaptation(TRequestStatus& aTrs,TTestSusEmergencyCallAdaptionServerRequests aRequest);
	~CEmergencyAdaptPlugin();
	 CEmergencyAdaptPlugin(CTestEmergencyAdaptStep& aTestEmergencyAdaptStep);
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
private:
	void Construct();

private:
	RTestSsmEmergencyCallRfAdaptation iEmergencyAdaptation;
	TRequestStatus *iEmergencyStatus;
	CTestEmergencyAdaptStep& iTestEmergencyAdaptStep;
	};


class CTestEmergencyAdaptStep : public CTestAdaptStep
	{
public:
	CTestEmergencyAdaptStep ();
	~CTestEmergencyAdaptStep ();

	//from CTestStep
	TVerdict doTestStepL();
	
private:
	void TestEmergencyAdaptState();
	void TestEmergencyAdaptStateCancel();
	
private:		
	CEmergencyAdaptPlugin *iEmergencyAdapt;
	};

#endif
