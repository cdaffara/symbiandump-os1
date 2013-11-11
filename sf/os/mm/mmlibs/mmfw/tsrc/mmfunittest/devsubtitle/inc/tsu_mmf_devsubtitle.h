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

#ifndef TSU_MMF_DEVSUBTITLE_H
#define TSU_MMF_DEVSUBTITLE_H

#include <testframework.h>
#include <w32std.h>

#include "mmfdevsubtitle.h"
#include "srtreader.h"

/*
Common utils for both sync and asyc versions of test steps
*/
class CDevSubtitleTestStepCommonUtils : public CBase
	{
public:
   	void InitWservL();
	void DeInitWserv(); 
    void PrepGc();
	void RetireGc();
	void InitCrpL();
	void DestroyCrp();
	TVerdict StartCrpDrawL(TInt aWindowId);
	
	void DoTestStepPreambleL();
    void DoTestStepPostambleL();

public:
	CMMFDevSubtitle* iDevSubtitle;
	RFs iFs;
	CSrtReader* iSubtitleSource;

    // Window server related objects
    RWsSession iWs;
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWindowGroup iRootWindow;
    CWindowGc *iGc;
	};

/* 
Base test class for all DevSutitle unit tests
*/
class RDevSubtitleTestStep : public RTestStep
	{
protected:
	RDevSubtitleTestStep(const TDesC& aStepName);
	CMMFDevSubtitle* DevSubtitle();

	// from RTestStep
    TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
    
protected:
	CActiveScheduler* iActiveScheduler;
	TBool iActiveSchedulerStarted;
	CDevSubtitleTestStepCommonUtils* iCommonUtils;
	};

/* 
Base test class for all DevSutitle unit tests
*/
class RDevSubtitleTestStepAsync : public RAsyncTestStep
	{
public:
	void StopTest();
	void StopTest(TInt aError);
	void Test(TBool aResult);
	CDevSubtitleTestStepCommonUtils* CommonUtils(){	return iCommonUtils; }
	
protected:   
	RDevSubtitleTestStepAsync(const TDesC& aStepName);
	CMMFDevSubtitle* DevSubtitle();
	
	// From RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
    
protected:
	CDevSubtitleTestStepCommonUtils* iCommonUtils;
	};


/**
Base class for ASync test steps
*/
class CDevSubtitleTestStepAoBase : public CTimer
	{
public:
	~CDevSubtitleTestStepAoBase();
	
	// From CActive
	void DoCancel();
	
protected:
	CDevSubtitleTestStepAoBase(CMMFDevSubtitle* aDevSubtitle, RDevSubtitleTestStepAsync& aParent);

protected:
	enum TTestStepState
		{
		ETestStep1,
		ETestStep2,
		ETestStep3,
		ETestStep4,
		ETestStep5,
		ETestStep6,
		ETestStep7,
		ETestStep8,
		ETestStep9,
		ETestStepEnd
		};	
		
	CMMFDevSubtitle* iDevSubtitle;
	TTestStepState iState;
	TVerdict iTestResult;
	RDevSubtitleTestStepAsync& iParent; 
	};


#endif // TSU_MMF_DEVSUBTITLE_H
