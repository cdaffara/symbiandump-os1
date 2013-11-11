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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file 
 @internalTechnology
*/

#include <e32base.h>

#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

class CAsynchEvent;

class CStateMachine;
/**
 The statemachine notifier interface.
 
 If the notifier is registered with a CStateMachine instance 
 it receives state machine events
 @see CStateMachine
 @internalTechnology
**/
class MStateMachineNotify
   {
   public:
      virtual ~MStateMachineNotify() {};

      /** 
       * Called by RunL when the state machine completed its task.
       *
       * @param aStateMachine the instance that has raised the event
       * @return if the value is ETrue then aStateMachine deletes itself 
       */
      virtual TBool OnCompletion( CStateMachine* aStateMachine ) = 0;
   };

/**
 The base class for asynchronous state machine with the next state represented 
 by CAsynchEvent.
 
 The class is not thread safe
 @see CAsynchEvent
 @internalTechnology
**/
class CStateMachine : public CActive
   {
   public:
      IMPORT_C ~CStateMachine();

      TInt LastError() const;
      void SetLastError( TInt aLastError );
      void SetClientStatus( TRequestStatus* aClientStatus );
      TRequestStatus* ClientStatus() const;
      void SetActiveEvent( CAsynchEvent* aEvent );
      void SetErrorEvent( CAsynchEvent* aEvent );
      void SetSuspendRequest( TBool aSuspendRequest );
      TBool SuspendRequest() const;

      void UpdateHistory( TInt aUpdate );
      TInt History() const;

      void RegisterNotify( MStateMachineNotify* aStateMachineNotify );
      void DeRegisterNotify( MStateMachineNotify* aStateMachineNotify );
   
      IMPORT_C void Start( TRequestStatus* aClientStatus, CAsynchEvent* aErrorEvent, MStateMachineNotify* aStateMachineNotify
         /*the object is NOT taking ownership of the params*/ );
      //CActive::Cancel method should not be used
      IMPORT_C void Cancel( TInt aLastError );

      IMPORT_C HBufC8* ReAllocL( TInt aNewLength );
      HBufC8* Fragment() const;

   protected:
      IMPORT_C CStateMachine();

	   IMPORT_C virtual void DoCancel();
	   IMPORT_C virtual void RunL();
	   IMPORT_C virtual TInt RunError(TInt aError);

      IMPORT_C void OnError();
      IMPORT_C virtual void OnCompletion();

   protected:
      CAsynchEvent* iActiveEvent; //referenced not owned
      CAsynchEvent* iErrorEvent; //referenced not owned
      TInt iHistory;
      TInt iLastError;

      TRequestStatus* iClientStatus; //optional
      HBufC8* iFragment; //shared data fragment buffer owned by this class

      MStateMachineNotify* iStateMachineNotify; //one at the time so far
      TBool iSuspendRequest : 1; //if ETrue causes the state machine to stop after completion of
      //ongoing asynch event (no client request will be completed but MStateMachineNotify::OnCompletion
      //will be called if installed)
   };

inline void CStateMachine::SetActiveEvent( CAsynchEvent* aEvent )
/** 
 * Sets the active event (the next state of the instance).
 
 * An ownership is not transfered
 *
 * @param aEvent a pointer to the active event
 */
   {
   iActiveEvent = aEvent;
   }

inline void CStateMachine::SetErrorEvent( CAsynchEvent* aEvent )
/** 
 * Sets the error event (the one called if RunL leaves or completes with an error).
 
 * An ownership is not transfered
 *
 * @param aEvent a pointer to the error event
 */
   {
   iErrorEvent = aEvent;
   }

inline TBool CStateMachine::SuspendRequest() const
/** 
 * Query for suspend request
 *
 * @return iSuspendRequest
 */
   {
   return iSuspendRequest;
   }

inline void CStateMachine::SetSuspendRequest( TBool aSuspendRequest )
/** 
 * Sets the suspend request
 
 * If ETrue than it will stop the state machine to stop before 
 * processing the iActiveEvent. Notifier is notified but client request is not completed
 *
 * @return iSuspendRequest
 */
   {
   iSuspendRequest = aSuspendRequest;
   }

inline void CStateMachine::DeRegisterNotify( MStateMachineNotify* /*aStateMachineNotify*/ )
/** 
 * De-registers the notifier
 *
 * @param aStateMachineNotify not used
 */
   {
   iStateMachineNotify = NULL; //so far it's simple
   }

inline void CStateMachine::RegisterNotify( MStateMachineNotify* aStateMachineNotify )
/** 
 * Registers the notifier
 *
 * @param aStateMachineNotify a notifier to register. An ownership is not transfered
 */
   {
   iStateMachineNotify = aStateMachineNotify;
   }

inline void CStateMachine::SetClientStatus( TRequestStatus* aClientStatus )
/** 
 * Sets a request status to complete.
 
 * It does not transfer the ownership
 *
 * @param aClientStatus the client request to complete
 */
   {
   iClientStatus = aClientStatus;
   }

inline TRequestStatus* CStateMachine::ClientStatus() const
/** 
 * Gets a request status to complete.
 *
 * @return aClientStatus the client request to complete
 */
   {
   return iClientStatus;
   }

inline HBufC8* CStateMachine::Fragment() const
/** 
 * Gets a pointer to an arbitrary data fragment owned by the instance
 *
 * @return a pointer to the fragment
 */
   {
   return iFragment;
   }

inline void CStateMachine::UpdateHistory( TInt aUpdate )
/** 
 * Updates the state machine history whatever the history means in the context 
 * the instance is being used
 *
 * @param aUpdate a value to OR with the current history
 */
   {
   iHistory |= aUpdate;
   }

inline TInt CStateMachine::History() const
/** 
 * Gets the state machine history whatever the history means in the context 
 * the instance is being used
 *
 * @return the history
 */
   {
   return iHistory;
   }

inline TInt CStateMachine::LastError() const
/** 
 * Gets the last error set when an active event processing leaves or completes
   with an error
 *
 * @return the last error
 */
   {
   return iLastError;
   }

inline void CStateMachine::SetLastError( TInt aLastError )
/** 
 * Sets the last error
 *
 * @param aLastError a value of the last error
 */
   {
   iLastError = aLastError;
   }

#endif

