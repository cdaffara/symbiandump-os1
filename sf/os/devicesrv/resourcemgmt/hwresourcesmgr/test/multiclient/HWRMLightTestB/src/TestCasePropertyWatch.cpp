// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "TestCasePropertyWatch.h"

/* This is active object class that track changes to 
 * Publish and Subscribe KMyHWRMTestPpoperty 
 */

CTestCasePropertyWatch* CTestCasePropertyWatch::NewL()
	{
	CTestCasePropertyWatch* me=new(ELeave) CTestCasePropertyWatch;
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

CTestCasePropertyWatch::CTestCasePropertyWatch()
	:CActive(EPriority)
	{}

void CTestCasePropertyWatch::ConstructL()
	{
	//User::LeaveIfError(iProperty.Attach(KMyHWRMTestPpoperty,EHWRMNextTestToRun));
	User::LeaveIfError(iProperty.Attach(KMyHWRMTestPpoperty,EHWRMStepResult));
	
	//iTimer.After(iStatus, 61000000); // 61s timer
	
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	RunL();
	}

CTestCasePropertyWatch::~CTestCasePropertyWatch()
	{
	Cancel();
	iProperty.Close();
	//iTimer.Close();
	}

void CTestCasePropertyWatch::DoCancel()
	{
	iProperty.Cancel();
	}

void CTestCasePropertyWatch::RunL()
	{
	// resubscribe before processing new value to prevent missing updates
	iProperty.Subscribe(iStatus);
	
	SetActive();
	
	//iTimer.Cancel();
	
	// property updated, get new value
	TInt tstep;
	if (iProperty.Get(tstep)==KErrNotFound)
		{
		// property deleted, do necessary actions here...
		//NameDeleted();
		}
	else
		{
		// use new value ...
		//NameChanged(n);
		}
	}
