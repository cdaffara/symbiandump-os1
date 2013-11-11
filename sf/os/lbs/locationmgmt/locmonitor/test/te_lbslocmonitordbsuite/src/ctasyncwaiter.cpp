/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "ctasyncwaiter.h"


/**
  Function : NewL
  Description : Creates an object of CT_LbsAsyncWaiter
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C CT_AsyncWaiter* CT_AsyncWaiter::NewL()
	{
	CT_AsyncWaiter* self = new(ELeave) CT_AsyncWaiter();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


void CT_AsyncWaiter::ConstructL()
	{
	CTimer::ConstructL();
	}
	
/**
  Function : CT_AsyncWaiter
  Description : Constructor
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_AsyncWaiter::CT_AsyncWaiter() : CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}	


/**
  Function : CT_AsyncWaiter
  Description : Destructor
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_AsyncWaiter::~CT_AsyncWaiter()
	{
	Cancel();
	}

	
/**
  Function : StartAndWait
  Description : Starts the Active scheduler, stops when RunL is hit
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C void CT_AsyncWaiter::StartAndWait()
	{
	iError = iStatus.Int();
	SetActive();
	CActiveScheduler::Start();
	}


/**
  Function : StartAndWaitTimer
  Description : Starts the Active scheduler, steops when timer expires
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C void CT_AsyncWaiter::StartAndWaitTimer(const TTimeIntervalMicroSeconds32& aTime)
	{
	iError = iStatus.Int();
	After(aTime);
	SetActive();
	CActiveScheduler::Start();
	}
	
/**
  Function : Result
  Description : Gives the error
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
EXPORT_C TInt CT_AsyncWaiter::Result() const
	{
	return iError;
	}
	
	
/**
  Function : RunL
  Description : 
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_AsyncWaiter::RunL()
	{
	iError = iStatus.Int();
	CActiveScheduler::Stop();
	}

	
/**
  Function : DoCancel
  Description : 
  
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_AsyncWaiter::DoCancel()
	{
	iError = KErrCancel;
	CActiveScheduler::Stop();
	}

