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
// Offers a base class for an asynchronous state machine
// 
//

/**
 @file 
*/

#include "StateMachine.h"
#include "AsynchEvent.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemStateMachStM, "ElemStateMachStM");
#endif

EXPORT_C CStateMachine::CStateMachine() :
   CActive( EPriorityStandard )
   {
	CActiveScheduler::Add(this);
   }

EXPORT_C CStateMachine::~CStateMachine()
   {
   delete iFragment;
   }

EXPORT_C HBufC8* CStateMachine::ReAllocL( TInt aNewLength )
/** 
 * Re-allocates the instance fragment
 *
 * @param aNewLength a length to put into the buffer
 * @return a pointer to the allocated fragment
 */
   {
   if ( !iFragment )
      {
      iFragment = HBufC8::NewL( aNewLength );
      }
   else
      {
      TInt n = iFragment->Length();
      if ( (iFragment->Des().MaxLength() - n) < aNewLength )
         {
         iFragment = iFragment->ReAllocL( aNewLength + n );
         }
      }
   return iFragment;
   }

EXPORT_C void CStateMachine::Start( TRequestStatus* aClientStatus, CAsynchEvent* aErrorEvent, MStateMachineNotify* aStateMachineNotify )
/** 
 * Starts the state machine
 *
 * @param aClientStatus the client request to complete on completion
 * @param aErrorEvent an error event to execute on error
 * @param aStateMachineNotify a notifier to receive state achine events
 */
   {
   //history is set beforehand
   iLastError = KErrNone;
   iClientStatus = aClientStatus;
   if (iClientStatus != NULL)
      *iClientStatus = KRequestPending;
   iErrorEvent = aErrorEvent;
   iSuspendRequest = EFalse;
   iStateMachineNotify = aStateMachineNotify;
   //jump to RunL
   TRequestStatus* p=&iStatus;
   if ( !IsActive() )
   	{
	   SetActive();
   	}
   User::RequestComplete( p, KErrNone );
   }

EXPORT_C void CStateMachine::Cancel( TInt aLastError )
/** 
 * Cancels the state machine with an error
 *
 * @param aLastError a desired error code. 
 * if the last eror is KErrNone the state machine
 * will be cancelled and client request will be completed with KErrCancel.
 * In any other case the active event will become the error event and the 
 * processing continues.
 */
   {
   iSuspendRequest = EFalse;
   if ( aLastError == KErrNone )
      {
      iErrorEvent = 0;
      }
   //cancel the current activity
   CActive::Cancel();
   iLastError = KErrCancel;
   iActiveEvent = iErrorEvent;
   if ( iActiveEvent )
      {
      //jump to RunL //!!This makes the cancel asynchronous
      TRequestStatus* p=&iStatus;
      SetActive();
      User::RequestComplete( p, KErrNone );
      }
   }

EXPORT_C void CStateMachine::DoCancel()
/** 
 * completes the cancel request
 *
 */
   {
   if ( !iErrorEvent ) //otherwise the "cancel" completes after the error event has been sent
      {
      OnCompletion();
      }
   }

EXPORT_C void CStateMachine::OnError()
/** 
 * Called from ::RunL when an error occures
 *
 */
   {
   if ( iLastError != KErrNone )
      {
      if ( iActiveEvent != iErrorEvent )
         {
         iActiveEvent = 0;
         }
      }
   else
      {
      iLastError = iStatus.Int();
      iActiveEvent = iErrorEvent;
      }
   }

EXPORT_C void CStateMachine::RunL()
/** 
 * Called on completion of one active event
 *
 */
   {
   if ( iStatus.Int() != KErrNone )
      {//error
      OnError();
      }
   if ( iActiveEvent )
      {//process iActiveEvent if not iSuspendRequest
      if ( iSuspendRequest )
         {
         if ( iStateMachineNotify )
            {//return value ignored
            iStateMachineNotify->OnCompletion( this );
            }
         }
      else
         {
         SetActive();
         iActiveEvent = iActiveEvent->ProcessL( iStatus );
         }
      }
   else
      {//complete the state machine task
      OnCompletion();
      }
   }

EXPORT_C TInt CStateMachine::RunError(TInt aError)
/** 
 * RunL left. Complete status with the aError and let RunL function to handle it
 *
 */
   {
   //if RunL (ProcessL) leaves the current request must not be completed
   __ASSERT_DEBUG(IsActive(), User::Panic(KSpecAssert_ElemStateMachStM, 1));
   iActiveEvent = 0;
   TRequestStatus* p=&iStatus;
   User::RequestComplete( p, aError );
	return KErrNone; //no error propagation to central error handler
   }

EXPORT_C void CStateMachine::OnCompletion()
/** 
 * Called by RunL when the state machine has completed its task.
 *
 */
   {
   //!!!notifier cannot delete this
   TBool bDelete = iStateMachineNotify ? iStateMachineNotify->OnCompletion( this ) : EFalse;
   if ( iClientStatus )
      {
      User::RequestComplete( iClientStatus, iLastError );
      }
   if ( bDelete )
      {
      delete this;
      }
   }

