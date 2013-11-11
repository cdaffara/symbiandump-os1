// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CDummyAnswer.h"
#include "CMainMenu.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object that owns this object
@return             Instance of CDummyAnswer class
*/
CDummyAnswer* CDummyAnswer::NewL(MExecAsync* aController)
	{
	CDummyAnswer* self = new(ELeave) CDummyAnswer(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object that owns this object.
*/
CDummyAnswer::CDummyAnswer(MExecAsync* aController)
	: CActive(EPriorityStandard),
	  iController(aController)
	{
	iConsole = iController->GConsole();
	CActiveScheduler::Add(this);
	}

/**
Notifies the menu object that it has completed its request.
*/
void CDummyAnswer::ExampleComplete()
	{
	iController->ExecComplete(KNotType);
	}

/**
Second phase constructor.
*/
void CDummyAnswer::ConstructL()
	{
	// Creates a thread-relative timer
	User::LeaveIfError(iTimer.CreateLocal());
	iFirstTime = ETrue;
	}

/**
Sets iFirstTime to ETrue.
*/
void CDummyAnswer::ResetFirstTime()
	{
	iFirstTime = ETrue;
	}

/**
Destructor.
Cancels outstanding requests and closes the iTimer object.
*/
CDummyAnswer::~CDummyAnswer()
	{
	Cancel();
	iTimer.Close();
	}

/**
Starts the timer for the duration specified in aDelay.
*/
void CDummyAnswer::StartCount(TTimeIntervalMicroSeconds32 aDelay)
	{
	_LIT(KDummyAnswerPanic, "CDummyAnswer");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iTimer.After(iStatus, aDelay);
	SetActive();
	}

/**
Checks the status of the active object and displays the current call duration
if there is no error.
*/
void CDummyAnswer::RunL()
	{
	if (iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print console output message if there is no error
		iConsole->ClearScreen();
		iConsole->Printf(KMenuMsg);
		iConsole->Printf(KHangupMsg);
		iConsole->Printf(_L("Call Duration %d seconds\n"), iCount);
		iCount++;
		StartCount(1000000);
		}
	if (iFirstTime)
		{
		iFirstTime = EFalse;
		ExampleComplete();
		}
	}

/**
Stops counting and resets the call duration.
*/
void CDummyAnswer::DoCancel()
	{
	ResetFirstTime();
	iCount = 0;
	iTimer.Cancel();
	}
