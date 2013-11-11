/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMCONFERENCECALLTSY_H
#define CMMCONFERENCECALLTSY_H

//  INCLUDES
#include <etelmm.h>
#include <et_phone.h>
#include "MmTsy_timeoutdefs.h"
#include "cmmcalltsy.h"

// FORWARD DECLARATIONS
class CMmConferenceCallExtInterface;
class CMmTsyReqHandleStore;
class CMmPhoneTsy;

// CONSTANTS
_LIT( KDelimiter, ";" );

// CLASS DECLARATION

/**
 * The CMmConferenceCallTsy object is an aggregation of all the
 * CMmCallTsy objects that are part of the conference call.
 */
NONSHARABLE_CLASS( CMmConferenceCallTsy ) : public CSubSessionExtBase
    {
    
    public: // Enumerations

        /** Request types used for Conference Calls. */
        enum TConferenceCallRequestTypes
            {
            EMultimodeConferenceCallReqHandleUnknown, //0
            EMultimodeConferenceCallCreateConference,
            EMultimodeConferenceCallAddCall,
            EMultimodeConferenceCallSwap,
            EMultimodeConferenceCallHangUp,
            EMultimodeConferenceCallStatusChangeNotification, //5
            EMultimodeConferenceCallCapsChangeNotification,
            EMultimodeConferenceCallConferenceEventNotification, //7

            // ATTENTION:: Declare constant for those requests that need
            // own request handle record in iTsyReqHandleStore above the
            // following!
            EMultimodeConferenceCallMaxNumOfRequests
            };
    
    public: // Constructors and destructor

        /**
         * Two-phased constructor. 
         * @param CMmPhoneTsy* aMmPhone: pointer to the Phone object
         * @return created object
         */
        static CMmConferenceCallTsy* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor
         */
        ~CMmConferenceCallTsy();

        // New functions
    
        /**
         * Pure virtual method inherited from MTelObjectTSY
         *          
         *
         * @param const TDesC& aName: name of the object that should be opened
         * @return opened object
         */
        CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Pure virtual method inherited from MTelObjectTSY
         *          
         *
         * @param const TDesC& aName: name of the object that was opened
         * @return created object
         */
        CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
         * Dispatches multimode API functions calls
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param const TInt aIpc: request IPC number
         * @param const TDataPackage& aPackage: packaged request parameters
         * @return result of the request
         */
        TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number
         *          
         *
         * @param const TInt aIpc: request IPC number
         * @return request mode
         */
        CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC
         *          
         *
         * @param const TInt aIpc: request IPC number
         * @return result of the request
         */
        TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given in
         * parameters
         *          
         *
         * @param const TInt aIpc: request IPC number
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Initialisation method that is called by ETel Server
         *          
         *
         */
        void Init();

        /**
         * Returns conference call status
         *          
         *
         * @return CC status
         */
        RMobileConferenceCall::TMobileConferenceStatus Status() const;

        /**
         * This function returns conference call capabilities
         *          
         *
         * @return CC capabilities
         */
        TUint32 Caps() const;

        /**
         * Register given notification
         *          
         *
         * @param const TInt aIpc: request IPC number
         * @return result of the request
         */
        TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *          
         *
         * @param const TInt aIpc: request IPC number
         * @return result of the request
         */
        TInt DeregisterNotification( const TInt aIpc );

        /**
         * Complete caps change notification
         *          
         *
         * @param New conference call capabilities
         */
        void CompleteNotifyCapsChange( TUint32 aCaps );

        /**
         * Complete creation of conference call
         *          
         *
         * @param TInt aResult: error code
         */
        void CompleteCreateConference( TInt aResult );

        /**
         * Complete Adding a call to Conference call
         *          
         *
         * @param TInt aResult: error code
         */
        void CompleteAddCall( TInt aResult );

        /**
         * Complete swap request
         *          
         *
         * @param TInt aResult: error code
         */
        void CompleteSwap( TInt aResult );

        /**
         * Complete hangup request
         *          
         *
         * @param TInt aResult: error code
         */
        void CompleteHangUp( TInt aResult );

        /**
         * Complete conference status change notification
         *          
         *
         */
        void CompleteNotifyConferenceStatusChange();

        /**
         * Complete conference event notification
         *          
         *
         * @param TName aName: name of the call to which the event occured
         * @param RMobileConferenceCall::TMobileConferenceEvent aEvent: event
         */
        void CompleteNotifyConferenceEvent( const TName& aName,
            RMobileConferenceCall::TMobileConferenceEvent aEvent );

        /**
         * Checks if given call can be splitted from the conference call
         *          
         *
         * @param TInt aCallId: call ID
         * @return TBool: ETrue if can be splitted, else EFalse
         */
        TBool IsGoOneToOneSupported( TInt aCallId );

        /**
         * Returns the active Conference call extension
         *          
         *
         * @return active CC extension
         */
        CMmConferenceCallExtInterface* GetActiveConferenceCallExtension();

        /**
         * Sets the active call extension
         *          
         *
         * @param CMmConferenceCallExtInterface*: new active CC extension
         * @return KErrNone or error value
         */
        TInt SetActiveConferenceCallExtension(
                CMmConferenceCallExtInterface* aMmConferenceCallExt );

        /**
         * Checks if is possible to create a conference call
         *          
         *
         */
        void CheckConferenceCapability();

        /**
         * Returns current count of calls in conference call
         *          
         *
         * @return Number of calls in conference call
         */
        TInt NumberOfCallsInConference() const;

        /**
         * Can be used for checking if certain request is pending in TSY
         *          
         *
         * @param CMmConferenceCallTsy::TConferenceCallRequestTypes
         *           aReqType: request type
         * @return request active or not
         */
        TBool ServiceRequested(
            CMmConferenceCallTsy::TConferenceCallRequestTypes aReqType );

        /**
         * Inform Conference Call object that a status of a single 
         * call has changed
         *          
         *
         * @param RMobileCall::TMobileCallStatus aNewStatus: new status of a single call
         * @param TInt aCallId: call ID of that call
         */
        void CallStatusChanged( RMobileCall::TMobileCallStatus aNewStatus,
			     TInt aCallId );
		
		    /**
         * Returns the Phone object
         *          
         *
         * @return Pointer to the Phone object
         */
        CMmPhoneTsy* Phone();

#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due timer expiration
         *          
         *
         * @param TInt aReqHandleType: req handle type.
         * @param TInt aError: error value
         */
        void Complete( TInt aReqHandleType, TInt aError );
#endif

#ifdef USING_CTSY_DISPATCHER
        
        /**
         * Get call object for connected call used in conference creation phase. 
         */         
        CMmCallTsy* ConnectedCall() { return iConnectedCall; }
        
        /**
         * Get call object for held call used in conference creation phase. 
         */         
        CMmCallTsy* HeldCall() { return iHoldCall; }
        
#endif // USING_CTSY_DISPATCHER       
        
    private:
    
        /**
         * C++ default constructor.
         */
        CMmConferenceCallTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning on
         * memory allocation failure
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param const TInt aIpc: request IPC number
         * @param const TDataPackage& aPackage: packaged request parameters
         * @return result of the request
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc, const TDataPackage& aPackage);

        /**
         * Returns the call capabilities to the client
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param TUint32* aCaps: pointer where to copy the capabilities
         * @return result of the request
         */
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle, TUint32* aCaps );

        /**
         * Request for Conference call caps change notifications
         *          
         *
         * @param TUint32* aCaps: pointer where to copy the capabilities when changed
         * @return result of the request
         */
        TInt NotifyCapsChange( TUint32* aCaps );

        /**
         * Cancel Conference call caps change notifications
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        TInt NotifyCapsChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method allows clients to create the initial conference call
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        TInt CreateConferenceL( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method adds the single voice call specified by the aCallName
         * parameter to an existing conference call
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param const TName* aCallName: name of the call to be added
         * @return result of the request
         */
        TInt AddCallL( const TTsyReqHandle aTsyReqHandle,
            const TName* aCallName );

        /**
         * Allows a client to switch a conference call between "Active" and
         * "Hold" states
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        TInt SwapL( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method terminates the whole conference call
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return TInt: result of the request
         */
        TInt HangUpL( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method returns the number of calls that are currently part of
         * the conference call
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param TInt* aCount: number of calls in conference
         * @return result of the request
         */
        TInt EnumerateCalls( const TTsyReqHandle aTsyReqHandle,
            TInt* aCount );

        /**
         * This method returns a current snapshot of the call information
         * associated with the call specified by the aIndex parameter
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param TInt* aIndex: index of the call
         * @param TDes8* aCallInfo: pointer where to copy the call information
         * @return result of the request
         */
        TInt GetMobileCallInfo( const TTsyReqHandle aTsyReqHandle,
            TInt* aIndex, TDes8* aCallInfo );

        /**
         * This method allows clients to retrieve the current status of the
         * conference call
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @param RMobileConferenceCall::TMobileConferenceStatus*
         *       aStatus: pointer where to copy the CC status
         * @return result of the request
         */
        TInt GetConferenceStatus( const TTsyReqHandle aTsyReqHandle,
            RMobileConferenceCall::TMobileConferenceStatus* aStatus );

        /**
         * This method allows clients to be notified of a change in the status
         * of a conference call
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceStatus*
         *       aStatus: pointer where to copy the CC status when changed
         * @return TInt: result of the request
         */
        TInt NotifyConferenceStatusChange(
            RMobileConferenceCall::TMobileConferenceStatus* aStatus );

        /**
         * Cancels an outstanding asynchronous NotifyConferenceStatusChange 
         * request
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        TInt NotifyConferenceStatusChangeCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method allows a client to be notified when a conference call
         * event occurs
         *          
         *
         * @param RMobileConferenceCall::TMobileConferenceEvent*
         *       aEvent: pointer where to copy the CC event when occured
         * @param TName* aCallName: name of the call to which the eevnt 
         * occurred
         * @return result of the request
         */
        TInt NotifyConferenceEvent(
            RMobileConferenceCall::TMobileConferenceEvent* aEvent,
            TName* aCallName );

        /**
         * Cancels an outstanding asynchronous NotifyConferenceEvent request
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle: TSY request handle
         * @return result of the request
         */
        TInt NotifyConferenceEventCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Resets internal attributes of Conference call
         *
         */
        void ResetAttributes();

        /**
         * Remove calls from Conference call that are not participating it
         */
		void RemoveNonParticipatingCall();


#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         *
         * @param const TInt aIndex: index where to store the req handle.
         * @param const TTsyReqHandle aTsyReqHandle: req handle to be stored.
         */
        void SetTypeOfResponse( const TInt aIndex,
            const TTsyReqHandle aTsyReqHandle );
#endif

#ifdef TF_LOGGING_ENABLED
        /**
         * Overloads original ReqCompleted for logging purposes
         *          
         *
         * @param const TTsyReqHandle aTsyReqHandle
         * @param const Tint aError
         */
        void ReqCompleted( const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
#endif

    private: // member data

        /**
         * Pointer to multimode phone object
         */         
        CMmPhoneTsy* iMmPhone;

        /**
         * Pointer to external conference call interface object
         * Own.
         */                  
        CMmConferenceCallExtInterface* iMmConferenceCallExtInterface;

        /**
         * Number of calls in conference call
         */         
        TInt iNumOfCallsInConferenceCall;

        /**
         * Conference call status
         */         
        RMobileConferenceCall::TMobileConferenceStatus iStatus;

        /**
         * Conference call capabilities
         */         
        TUint32 iConferenceCaps;

        /**
         * Flag telling if confenrence call caps have changed
         */         
        TBool iCapsChanged;

        /**
         * Last used req handle type
         */         
        TConferenceCallRequestTypes iReqHandleType;

        /**
         * Table for conference call request handles
         */         
        TTsyReqHandle iConferenceCallReqHandles[
            EMultimodeConferenceCallMaxNumOfRequests];

        /**
         * Storate for Conference call's TSY req handles
         * Own.         
         */         
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * Pointer to the conference call capabilities
         */         
        TUint32* iRetCaps;

        /**
         * Pointer to the conference call status
         */         
        RMobileConferenceCall::TMobileConferenceStatus* iRetStatus;

        /**
         * Pointer to the name of the call that has had a conference event
         */         
        TName* iRetConferenceEventCallName;

        /**
         * Pointer to the conference call event
         */         
        RMobileConferenceCall::TMobileConferenceEvent* iRetConferenceEvent;

        /**
         * Name of the call to be added to Conference call
         */         
        TName iAddCallName;
        
        /**
         * used in creation phase to store  call objected
         */         
        CMmCallTsy* iConnectedCall;
        
        /**
         * used in creation phase to store  call objected
         */         
        CMmCallTsy* iHoldCall;

    };

#endif // CMMCONFERENCECALLTSY_H

// End of File
