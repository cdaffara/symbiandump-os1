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



#ifndef CMMLINETSY_H
#define CMMLINETSY_H

//  INCLUDES
#include <et_tsy.h>
#include <et_phone.h>
#include <etelmm.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmCallTsy;
class CMmTsyReqHandleStore;
class CMmMessageManagerBase;
class CMmDataPackage;
class MLtsyFactoryBase;

// CLASS DECLARATION

/**
 * CMmLineTsy implements mode independent Line based functionality (defined by
 * Symbian
 */
NONSHARABLE_CLASS( CMmLineTsy ) : public CLineBase
    {
    public: // Enumerations

        /**Enumeration for request handle types*/
        enum TLineRequestTypes
            { 
            EMultimodeLineReqHandleUnknown, //0
            EMultimodeLineNotifyIncomingCall, 
            EMultimodeLineNotifyStatusChange,
            EMultimodeLineNotifyCallAdded,
            EMultimodeLineCapsChangeNotification,
            EMultimodeLineMobileLineStatusChange, //5

            // NOTE!: Declare constant for those requests that need
            // own request handle record in iTsyReqHandleStore above the
            // following!
            EMultimodeLineMaxNumOfRequests
            };

    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CMmLineTsy();

        // New functions
        
        /**
         * This method is used for notifying a client about an incoming call.
         *          
         *
         * @param aDataPackage Package containing information about the 
         * incoming call
         */
        virtual void CompleteNotifyIncomingCall( 
            CMmDataPackage* aDataPackage ) = 0;

        /**
         * This method is used for notifying a line of a call entering 
         * Dialling state
         *          
         *
         * @param aDataPackage Package containing information about the call
         */
        virtual void CompleteNotifyDiallingStatus( 
            CMmDataPackage* aDataPackage ) = 0;

        /**
         * This method is used for notifying a client that a 
         * call has been added
         *          
         *
         * to the line.
         * @param aName Name of the call added to this line
         */
        void CompleteNotifyCallAdded( const TDesC& aName );

        /**
         * This method is used to notify a client about a line status change
         *          
         *
         */
        virtual void CompleteNotifyStatusChange();

        /**
         * Returns current line mode
         *          
         *
         * @return Line mode
         */
        RMobilePhone::TMobileService LineMode() const;

        /**
         * Returns current line name
         *          
         *
         * @return Line name
         */
        TName LineName() const;

        /**
         * Returns current line information
         *          
         *
         * @return Line information
         */
        RPhone::TLineInfo LineInfo() const;

        /**
         * Decrements counter containing information about the number of calls 
         * opened from this line
         *          
         *
         */
        void DecrementNumberOfCalls();

        /**
         * Notifies Line object about a new MO call that has been initiated
         * bypassing TSY
         *          
         *
         * @param aDataPackage Information about the call
         */
        void CompleteNotifyAddBypassingCall( CMmDataPackage* aDataPackage ); 

        // Functions from base classes

        /**
         * Dispatches Multimode API functions calls
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aIpc IPC number of the request
         * @param aPackage Packaged request parameters
         * @return Result of the request
         */
        TInt ExtFunc( 
                const TTsyReqHandle aTsyReqHandle, 
                const TInt aIpc, 
                const TDataPackage& aPackage );

        /**
         * This function opens a requested line.
         *          
         *
         * @param aName Name of the call to be opened.
         * @return Call object that was opened.
         */
        CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * This function creates a new name for a call and opens a new call.
         *          
         * 
         * @param aName Name of the new call.
         * @return Call object that was opened.
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName ) = 0;

        /**
         * Returns request mode for a given IPC.
         *          
         *
         * @param aIpc: IPC number
         * @return Request mode for the given IPC
         */
        CTelObject::TReqMode ReqModeL( const TInt aIpc );
        
        /**
         * Cancels request that's IPC number and request handle are given as
         * parameters
         *          
         *
         * @param aIpc IPC number of the cancelled request
         * @param aTsyReqHandle TSY request handle of the cancelled request
         * @return Result of the request
         */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Register a notification by IPC number.
         *          
         *
         * @param aIpc IPC number of the request.
         * @return Result of the request
         */
        TInt RegisterNotification( const TInt aIpc );
        
        /**
         * Deregisters a given notification
         *          
         *
         * @param aIpc IPC number of the request
         * @return Result of the request
         */
        TInt DeregisterNotification( const TInt aIpc );
        
        /**
         * Returns number of slots to be used for a given IPC
         *          
         *
         * @param aIpc IPC number of the request
         * @return Number of slots for the given IPC
         */
        TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Returns the number of calls opened from this line.
         *          
         *
         * @param aTsyReqHandle: TSY request handle
         * @param aCount: Number of calls opened from this line
         * @return Return value to the ETel Server
         */
        TInt EnumerateCall( const TTsyReqHandle aTsyReqHandle, 
            TInt* aCount );
        
        /**
         * This function retrieves the current line information.
         *          
         *
         * @param aTsyReqHandle TSY Request handle
         * @param aLineInfo Current line information
         * @return Return value to the ETel Server 
         */
        TInt GetInfo( const TTsyReqHandle aTsyReqHandle, 
            RLine::TLineInfo* aLineInfo );

        /**
         * This function retrieves the current line capabilities.
         *           
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps Current line capabilities
         * @return Return value to the ETel Server
         */
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle, 
            RLine::TCaps* aCaps );

        /**
         * This function returns core line status information.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aStatus Current line status
         * @return Return value to the ETel Server
         */
        TInt GetStatus( const TTsyReqHandle aTsyReqHandle, 
            RCall::TStatus* aStatus );

        /**
         * This function returns information about a call.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCallInfo Call ínformation
         * @return Return value to the ETel Server
         */
        TInt GetCallInfo( 
                const TTsyReqHandle aTsyReqHandle, 
                TCallInfoIndex* aCallInfo );

        /**
         * Used for subscribing a notification in a line status change.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aLineStatus New status of this line
         * @return Return value to the ETel Server
         */
        TInt NotifyStatusChange( const TTsyReqHandle aTsyReqHandle,
            RCall::TStatus* aLineStatus );
        
        /**
         * Cancels line status notifications.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method retrieves the current hook status.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aHookStatus Hook status
         * @return Return value to the ETel Server
         */        
        TInt GetHookStatus( const TTsyReqHandle aTsyReqHandle,
            RCall::THookStatus* /*aHookStatus*/ );
        
        /**
         * Notify when hook status has changed
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aHookStatus Hook status
         * @return Return value to the ETel Server
         */
        TInt NotifyHookChange( const TTsyReqHandle aTsyReqHandle,
            RCall::THookStatus* /*aHookStatus*/ );
        
        /**
         * Cancels hook status change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyHookChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This method provides notification of a change in the line 
         * capabilities.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps New capabilities of this line
         * @return Return value to the ETel Server
         */
        TInt NotifyCapsChange( const TTsyReqHandle aTsyReqHandle, 
            RLine::TCaps* aCaps );
        
        /**
         * This method cancels line capabilities change notification 
         * request.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * This method is used for notifyinf a client when an incoming call is 
         * detected.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aName Name of the incoming call
         * @return Return value to the ETel Server
         */
        virtual TInt NotifyIncomingCall( const TTsyReqHandle aTsyReqHandle,
            TName* aName );
        
        /**
         * This method cancels an outstanding incoming call notification.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyIncomingCallCancel( 
            const TTsyReqHandle aTsyReqHandle );
        
        /**
         * This method provides a notification that a new call has been added to
         * the line.
         *
         * @param aTsyReqHandle TSY request handle
         * @param aName The name of the added call
         * @return Return value to the ETel Server
         */
        TInt NotifyCallAdded( const TTsyReqHandle aTsyReqHandle, 
            TName* aName );
        
        /**
         * This method cancels an outstanding new call added notification 
         * request.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyCallAddedCancel( const TTsyReqHandle aTsyReqHandle );

#ifdef TF_LOGGING_ENABLED     
   
        /**
         * Overloads original ReqCompleted for logging purposes
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aError Result
         */
        virtual void ReqCompleted(const TTsyReqHandle aTsyReqHandle,
            const TInt aError);
#endif

    protected:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmLineTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        // New functions

        /**
         * Returns the current status of the line
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aStatus Mobile line status
         * @return Return value to the ETel Server
         */
        TInt GetMobileLineStatus( const TTsyReqHandle aTsyReqHandle,
            RMobileCall::TMobileCallStatus* aStatus );
        
        /**
         * Allows a client to be notified when the mobile line changes state.
         *          
         *
         * @param aStatus New mobile status of a line
         * @return Return value to the ETel Server
         */
        TInt NotifyMobileLineStatusChange( 
            RMobileCall::TMobileCallStatus* aStatus );
        
        /**
         * Cancels an outstanding asynchronous NotifyMobileLineStatusChange 
         * request.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyMobileLineStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Handles extended client requests. 
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aIpc IPC number of the request
         * @param aPackage Packaged request parameters
         * @return Result of the request
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Creates a call object for the call that has been initiated bypassing 
         * TSY
         *          
         *
         * @param aCallId: Call id of the new call
         * @param aCallMode: Call mode
         * @param aCallStatus: Call status
         * @return Pointer to created call object
         */
        virtual CMmCallTsy* CreateGhostCallObject(
            TInt aCallId,
            RMobilePhone::TMobileService aCallMode,
            RMobileCall::TMobileCallStatus aCallStatus ) = 0;

        /**
         * Initialises miscellaneous internal attributes
         *          
         *
         */
        virtual void InitInternalAttributesL();

        /**
         * Returns the preallocated Call object for incoming call or creates a 
         * new one
         *          
         *
         * @return Pointer to the call object for incoming call
         */
        CMmCallTsy* CallObjectForIncomingCall();

        /**
         * Creates and stores a Call object for incoming call
         *          
         *
         * @return TInt Success/failure value
         */
        virtual TInt CreateCallObjectForIncomingCall() = 0;
        
        /**
         * Creates a new name for a call.
         *          
         *
         * @param aNewName On return contains the new call name
         * @return Success/failure value
         */
        TInt CreateNewCallName( TDes& aNewName );
        
        /**
         * Returns the sequence number of the call created from this line.
         *          
         *
         * @return Sequence number of calls after incrementing.
         */
        TUint IncrementCallSequenceNumber();
        
        /**
         * This method is used for notifying a client about the capabilities 
         * change.
         *          
         *
         */
        void CompleteNotifyCapsChange();

    protected:  // Data

        /**
         * A pointer to the multimode phone object.        
         */                 
        CMmPhoneTsy* iMmPhone;

        /**
         * A table for line request handles
         */         
        TTsyReqHandle iLineReqHandles[EMultimodeLineMaxNumOfRequests];

        /**
         * Pointer to request handle store
         * Own.
         */                  
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * Line request handle type
         */         
        TLineRequestTypes iReqHandleType;

        /**
         * Pointer to the call object reserved for incoming call
         */                  
        CMmCallTsy* iCallForIncomingCall;
        
        /**
         * Current core line status
         */         
        RCall::TStatus iLineStatus;
        
        /**
         * Current Mobile line status
         */         
        RMobileCall::TMobileCallStatus iMobileLineStatus;

        /**
         * Current line capabilities
         */         
        RLine::TCaps iLineCaps;
        
        /**
         * Current number of calls opened from this line
         */         
        TInt iNumCalls;
        
        /**
         * Name of this line
         */         
        TName iLineName;
        
        /**
         * Sequence number given for a new call
         */         
        TUint iCallSequenceNumber;
        
        /**
         * The name of the last call opened from this line
         */         
        TName iNameOfLastCallAdded;

        /**
         * The name of the incoming call to be opened from this line
         */         
        TName iNameOfCallForAnswering;

        /**
         * Line status pointer (pointing to ETel server's address space)
         */                  
        RCall::TStatus* iRetLineStatus;
        
        /**
         * Line capabilities pointer (pointing to ETel server's address space)
         */                  
        RLine::TCaps* iRetCaps;
        
        /**
         * Incoming call name pointer 
         * (pointing to ETel server's address space)
         */                   
        TName* iRetIncomingCallName;
        
        /**
         * Pointer to the name of the last call added (pointing to ETel  
         * server's address space)
         */                  
        TName* iRetCallAdded;    
        
        /**
         * Pointer to the mobile status (pointing to ETel server's address 
         * space)
         */                  
        RMobileCall::TMobileCallStatus* iRetMobileLineStatus;
           
        /**
         * Pointer to the message manager
         */                  
        CMmMessageManagerBase* iMessageManager;

	    /**
	     * Line Mode
	     */         		
	    RMobilePhone::TMobileService iLineMode;
	       
    };

#endif      // CMMLINETSY_H   
            
// End of File




        
        
        

        

        




