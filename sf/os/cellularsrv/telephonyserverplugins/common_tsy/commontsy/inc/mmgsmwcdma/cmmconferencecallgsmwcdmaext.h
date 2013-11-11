/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMCONFERENCECALLGSMWCDMAEXT_H
#define CMMCONFERENCECALLGSMWCDMAEXT_H

// INCLUDES
#include "cmmconferencecallextinterface.h"

// CONSTANTS
const TInt KMaxCallsInConference = 5;

//  FORWARD DECLARATIONS
class CMmConferenceCallTsy;

// CLASS DECLARATION

/**
* Provides a Gsm/Wcdma system specific extension of 
* Conference Call Tsy.
*
*/
NONSHARABLE_CLASS( CMmConferenceCallGsmWcdmaExt ) 
    : public CMmConferenceCallExtInterface
    {
    public: // Constructors and destructor

        /**
         * Default destructor
         *         
         *
         */
        ~CMmConferenceCallGsmWcdmaExt();

        /**
         * Creates new GSM specific conference call object
         *         
         *
         * @param pointer to the conference call object
         * @return reated object 
         */
        static CMmConferenceCallGsmWcdmaExt* NewL(
            CMmConferenceCallTsy* aMmConferenceCallTsy );

        /**
         * This method allows clients to create the initial conference call
         *         
         *
         * @param  CC status
         * @param call list
         * @return  result of the request
         */
        TInt CreateConferenceL( 
            RMobileConferenceCall::TMobileConferenceStatus aStatus );

        /**
         * Allows a client to switch a conference call between "Active" and 
         * "Hold" states
         *         
         *
         * @param CC status
         * @param call list
         * @return TInt: result of the request
         */
        TInt SwapL( 
			RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList* aCallList );

        /**
         * This method adds the single voice call specified by the aCallName 
         * parameter to an existing conference call
         *         
         *
         * @param name of the call to be added to CC
         * @param CC status
         * @param call list
         * @return result of the request
         */
        TInt AddCallL( 
            const TName* aCallName,
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList* aCallList );

        /**
         * Directed to extension to inform extension that a status of a single 
         * call has changed
         *         
         *
         * @param  new status of
         * a single call
         * @param TaCallId: call ID of that call
         */
        TBool CallStatusChanged( 
			RMobileCall::TMobileCallStatus aNewStatus,
			TInt aCallId );

        /**
         * Checks if is possible to create a conference call
         *         
         *
         * @param CC status
         * @param call list
         */
        void CheckConferenceCapability(
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList* aCallList );

        /**
         * This method terminates the whole conference call
         *         
         *
         * @param CC status
         * @param call list
         * @return  result of the request
         */
        TInt HangUpL( 
            RMobileConferenceCall::TMobileConferenceStatus aStatus );

        /**
         * This method returns a current snapshot of the call information 
         * associated with the call specified by the aIndex parameter
         *         
         *
         * @param index of the call
         * @param pointer where to copy the call information
         * @param call list
         * @param  CC status
         * @return result of the request
         */
        TInt GetMobileCallInfo( 
            TInt* aIndex, 
            TDes8* aCallInfo, 
            CMmCallList* aCallList,
            RMobileConferenceCall::TMobileConferenceStatus aConferenceStatus );

        /**
         * Checks if given call can be splitted from the conference call
         *         
         *
         * @param call ID of the call to be splitted from CC
         * @param CC status
         * @param call list
         * @return TInt: result of the request
         */
        TInt IsGoOneToOneSupported( 
            TInt aCallId, 
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList* aCallList );

        /**
         * Returns new status for Conference call
         *         
         *
         * @param  new CC status will be returned using this pointer.
         * @return result of the request
         */
        TInt GetNewStatus( 
            RMobileConferenceCall::TMobileConferenceStatus* aNewStatus );

        /**
         * Is used for resetting internal attributes
         *         
         *
         */
        void ResetAttributes();

        /** 
         * Informs the Gsm extension if there is conference call
         * operation initiated bypassing the ETel (ghost operetion).
         *         
         *
         * @param aDataPackage: information about performed operation
         */
        void CompleteNotifyConferenceEvent( CMmDataPackage* aDataPackage );

		/** 
		 * Informs the Gsm extension if Go-One-To-One handling
		 * has started for any call.
		 * 		
         *
		 * @param aVal True if handling has started
		 * @param aCallId Id of the call for which handling has started
		 */
		void SetGoOneToOneHandlingStarted(TBool aVal, TInt aCallId);

    private: // Constructors and destructor

        /**
         * Default Constructor
         *         
         *
         */     
        CMmConferenceCallGsmWcdmaExt();

        /**
         * Class attributes are created in ConstructL
         */        
        void ConstructL();

        /**
         * Checks if Swap is going on and when Swap is ready 
         *         
         *
         * @param CC status
         * @param call list
         * @param new status of a single call
         * @param call ID of that call
         * @return ETrue if conference call status has changed
         */
        TBool HandleSwap(
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList& aCallList );

        /**
         * Checks if AddCall is going on and when AddCall is ready 
         *         
         *
         * @param CC status
         * @param call list
         * @param new status of a single call
         * @return ETrue if conference call status has changed
         */
        TBool HandleAddCall(
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList& aCallList, 
            RMobileCall::TMobileCallStatus aNewStatus );

        /**
         * Checks if CreateConference is going on and when CreateConference is 
         * ready 
         *         
         *
         * @param CC status
         * @param call list
         * @param new status of a single call
         * @param call ID of that call
         * @return ETrue if conference call status has changed
         */
        TBool HandleCreateConference(
            CMmCallList& aCallList, 
            TInt aCallId );

        /**
         * Checks if Conference call is still active or should it go to idle 
         * state 
         *         
         *
         * @param CC status
         * @param call list
         * @param new status of a single call
         * @param call ID of that call
         * @return ETrue if conference call status has changed
         */
        TBool HandleCallRelease(
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList& aCallList, 
            RMobileCall::TMobileCallStatus aNewStatus, 
            TInt aCallId );

        /**
         * Handles situation where client has asked Hold/Resume from Call 
         * object 
         *         
         *
         * @param CC status
         * @param new status of a single call
         * @return ETrue if conference call status has changed
         */
        TBool HandleSingleCallControl(
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            RMobileCall::TMobileCallStatus aNewStatus );

        /**
         * Handles GoOneToOne
         *         
         *
         * @param CC status
         * @param call list
         * @param new status of a single call
         * @param call ID of that call
         * @return ETrue if conference call status has changed
         */
        TBool HandleGoOneToOne(
            CMmCallList& aCallList, 
            TInt aCallId );

        /**
         * Updates call capabilities of all calls after a Join operation has
         * been executed (CreateConference or AddCall).
         *         
         *
         * @param call list
         */
        void UpdateCapsAfterJoin( CMmCallList& aCallList ) const;

        /**
         * Updates call capabilities of all calls after a call participating the
         * conference call has gone to idle state.
         *         
         *
         * @param call list
         */
        void UpdateCapsAfterCallRelease( CMmCallList& aCallList ) const;

        /**
         * Updates call capabilities of all calls after a Swap operation 
         * has been executed.
         *         
         *
         * @param conf. status
         * @param call list
         */
        void UpdateCapsAfterSwap(
            RMobileConferenceCall::TMobileConferenceStatus aStatus,
            CMmCallList& aCallList ) const;

    private: 

        /**
         * Pointer to the mode independent Conference call object
         * Not own.         
         */
        CMmConferenceCallTsy* iMmConferenceCallTsy;

        /**
         * Count, how many calls are to be swapped
         */
        TInt iNumberOfCallsToBeSwapped;

        /**
         * Counter telling when Create conference is ready to be completed
         */
        TInt iCreateConferenceCounter;

        /**
         * New conference call status
         */
        RMobileConferenceCall::TMobileConferenceStatus iNewStatus;

        /**
         * Flag telling if this is not the first hold/active status indication
         * caused by client asking hold/resume for a single call.         
         */
        TBool iCallControlStarted;

        /**
         * Counter telling how many calls should still change their state before
         * Conference call changes its' state.         
         */
        TInt iCallsToBeControlled;

        /**
         * Flag telling if GoOneToOne handling is going on.
         */
        TBool iGoOneToOneHandlingStarted;

        /**
         * Go-one-to-one call id
         */
        TInt iGoOneToOneCallId;

        /**
         * Operation initiated bypassing the ETel (ghost operetion).
         */
        TInt iCurrentGhostOperation;

    };

#endif // CMMCONFERENCECALLGSMWCDMAEXT_H

