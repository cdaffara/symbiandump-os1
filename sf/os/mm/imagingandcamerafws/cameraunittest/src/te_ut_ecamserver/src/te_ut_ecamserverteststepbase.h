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

#if (!defined __TE_UT_ECAM_SERVER_STEP_BASE__)
#define __TE_UT_ECAM_SERVER_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <ecam.h>
#include <f32file.h>
#include "w32std.h"

_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");

const TInt KTSU_ECM_OMX_Interval = 0x1000000;

// pre-declare
class CTe_ut_ecam_serverSuiteStepBase;

class CAsyncTestActiveScheduler : public CActiveScheduler
	{
public:
	CAsyncTestActiveScheduler(CTe_ut_ecam_serverSuiteStepBase& aTestStep);
protected:
	// from CActiveScheduler
	void Error(TInt aError) const;
private:
	CTe_ut_ecam_serverSuiteStepBase& iTestStep;
	};

class CTe_ut_ecam_serverSuiteStepBase : public CTestStep
	{
	friend class CAsyncTestActiveScheduler;
	
public:
	virtual ~CTe_ut_ecam_serverSuiteStepBase();
	CTe_ut_ecam_serverSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	virtual void PostKickOffTestL(TInt aReason, TVerdict aResult);
	virtual void ChangeState();
	void CreateWindowL();
	void CreateDependencyFileL();
	void RemoveDependencyFile();
    
private:
	
protected:
	RFs		iFs;
	RWsSession iWsSession;
    RWindowGroup iWindowGroup;  // Window group of the AO windows. 
    TInt iWindowHandle; // Window handle(s) for the AO windows. This handle
    	               //  is incremental and reused by various WServ artifacts. 
    CWsScreenDevice *iWsSd;        // Screen Device for this WServ session. 
    CWindowGc *iGc;     // Graphics Context associated with the window. 
    
	RWindow	*iWindow;
	
	HBufC8*	iReadData;
	HBufC8*	iWriteData;
	CActiveScheduler* iScheduler;
	};

class CAsyncTestShutdown : public CTimer
	{
	friend class CAsyncTestStep;
	
public:	
	static CAsyncTestShutdown* NewL(CTe_ut_ecam_serverSuiteStepBase* aAsyncTestStep);
	
	CAsyncTestShutdown(CTe_ut_ecam_serverSuiteStepBase* aAsyncTestStep);
	void ConstructL();
	void Start(TTimeIntervalMicroSeconds32 aInterval, TInt aReason, TVerdict aResult);

private:
	void RunL();
	
private:
	CTe_ut_ecam_serverSuiteStepBase* iAsyncTestStep;
	TTimeIntervalMicroSeconds32 iInterval;
	TInt iReason;
	TVerdict iResult;
	};

class CRetrieveTimer : public CTimer
	{
public:	
	static CRetrieveTimer* NewL(CTe_ut_ecam_serverSuiteStepBase* aTestStep);
	
	CRetrieveTimer(CTe_ut_ecam_serverSuiteStepBase* aTestStep);
	void ConstructL();
	void Start(TTimeIntervalMicroSeconds32 aInterval);

private:
	void RunL();
	
private:
	CTe_ut_ecam_serverSuiteStepBase* iTestStep;
	TTimeIntervalMicroSeconds32 iInterval;
	};

#endif // TE_UT_ECAMSERVERTESTSTEPBASE_H
