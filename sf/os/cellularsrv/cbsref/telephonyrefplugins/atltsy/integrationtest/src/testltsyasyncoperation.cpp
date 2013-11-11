// TestTelephonyAsyncOperation.cpp
//
// Copyright (c) 2004-2007 Symbian Ltd.  All rights reserved.
//
// The active object derived class for asynchronous requests
//

// Epoc includes
#include <testproperty.h>
#include <e32property.h>
#include <sacls.h>

// User includes
#include "TestLtsyAsyncOperation.h"

CTelephonyAsyncOperation::CTelephonyAsyncOperation(CTestStep& aTestStep)
/*
Constructor

Adds the active object to active scheduler.

@param aInt Operation id
*/
:	CActive(EPriorityStandard)
,	iTestStep(aTestStep)
	{
	CActiveScheduler::Add(this);
	}

CTelephonyAsyncOperation::~CTelephonyAsyncOperation()
/*
Destructor
*/
	{
	}

void CTelephonyAsyncOperation::DoCancel()
/*
For canceling the request
*/
	{
	}

void CTelephonyAsyncOperation::RunL()
/*
Stop the active scheduler
*/
	{
	CActiveScheduler::Stop();
	}

TInt CTelephonyAsyncOperation::SetOperation()
/*
Set the active object request and start the active scheduler
*/
	{
	SetActive();
	CActiveScheduler::Start();
	return iStatus.Int();
	}
