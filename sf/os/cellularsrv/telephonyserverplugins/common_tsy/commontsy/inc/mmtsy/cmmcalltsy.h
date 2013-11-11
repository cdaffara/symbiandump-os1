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



#ifndef CMMCALLTSY_H
#define CMMCALLTSY_H

//INCLUDES
#include <et_phone.h>
#include <etelmm.h>
#include "MmTsy_timeoutdefs.h"

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmLineTsy;
class CMmCallExtInterface;
class CAcquireOwnerList;
class CHeartbeatRunner;
class CMmTsyReqHandleStore;
class CMmMessageManagerBase;
class CMmDataPackage;
class MTelephonyAudioControl;

// CLASS DECLARATION

/**
 *  CMmCallTsy contains extended call functionality that is not mode-dependent
 */
NONSHARABLE_CLASS( CMmCallTsy ) : public CCallBase
    {
    public: // Enumerations
    
        /**Enumeration for TSY request handle types*/
        enum TCallRequestTypes
            {
            EMultimodeCallReqHandleUnknown, //0
            EMultimodeCallNotifyStatusChange, 
            EMultimodeCallNotifyDurationChange,
            EMultimodeCallCapsChangeNotification,
            EMultimodeCallDial, 
            EMultimodeCallAnswer, //5
            EMultimodeCallHangUp,
            EMultimodeMobileCallHold,
            EMultimodeMobileCallResume,
            EMultimodeMobileCallSwap,
            EMultimodeCallNotifyMobileCallStatusChange, //10
            EMultimodeCallNotifyCallEvent,
            EMultimodeCallNotifyRemotePartyInfoChange,
            EMultimodeMobileCallDialEmergencyCall,
            EMultimodeMobileCallDeflectCall,
            EMultimodeCallNotifyMobileCallCapsChange, //15
            EMultimodeCallTransfer, 
            EMultimodeCallGoOneToOne,
            EMultimodeCallNotifyDataCallCapsChange,
            EMultimodeCallSetDynamicHscsdParams,  
            EMultimodeCallNotifyHscsdInfoChange, //20
            EMultimodeCallNotifyPrivacyConfirmation, //21
            EMultimodeCallDialISV,
            EMultimodeCallAnswerISV,
            EMultimodeCallActiveUUS,
            EMultimodeCallReceiveUUI,//25
            EMultimodeCallDialNoFdnCheck, //26
            // NOTE!: Declare constant for those requests that need
            // own request handle record in iTsyReqHandleStore above the
            // following!
            EMultimodeCallMaxNumOfRequests       
            };

        /**Enumeration for Dial Cancel*/
        enum TDialCancelStatus
            {
            EDialCancelNotCalled,
            EDialCancelCallCalled,
            EDialCancelCustomCalled
            };

    public:  // Constructors and destructor
        
        /**
         * Destructor.
         */
        virtual ~CMmCallTsy();

    public: // New functions

        /**
         * Returns pointer to the Phone object
         *          
         *
         * @return Pointer to the Phone object
         */
        CMmPhoneTsy* Phone();

        /**
         * Returns Line object from which this call was opened.
         *          
         *
         * @return Pointer to the Line object
         */
        CMmLineTsy* Line();

        /**
         * Returns currently active call extension
         *          
         *
         * @return CMmCallExtInterface* Pointer to the call extension
         */
        CMmCallExtInterface* ActiveCallExtension();

        /**
         * Complete dial request
         *          
         *
         * @param aResult Result of the request
         */
        virtual void CompleteDial( TInt aResult );

        /**
         * Complete hang up
         *          
         *
         * @param aResult Result of the request
         */
        virtual void CompleteHangUp( TInt aResult );

        /**
         * Complete answering to the incoming call
         *          
         *
         * @param aResult Result of the request
         */
        virtual void CompleteAnswerIncomingCall( TInt aResult );

        /**
         * Complete status change notification
         *          
         *
         * @param aResult Result of the request
         * @param aDataPackage: new call status value
         */
        virtual void CompleteNotifyStatusChange( TInt aResult, 
            CMmDataPackage* aDataPackage ) = 0;

        /**
         * Complete mobile call information change notification
         *          
         *
         * @param aDataPackage: new call information
         */
        void CompleteNotifyMobileCallInfoChange( 
            CMmDataPackage* aDataPackage );

        /**
         * This method completes NotifyRemotePartyInfoChange notification
         *          
         *
         */
        void CompleteNotifyRemotePartyInfoChange();

        /**
         * This method completes call event initiated by remote party
         *          
         *
         * @param aDataPackage: remote event
         */
        void CompleteNotifyRemoteCallEvent( 
            const CMmDataPackage* aPackage );

        /**
         * Complete a NotifyMobileCallCapsChange method
         *          
         *
         * @param aErrorCode Error code
         */
        void CompleteNotifyMobileCallCapsChange( TInt aErrorCode );

        /**
         * Complete call duration change notification
         *          
         *
         */
        void CompleteNotifyCallDurationChange();

        /**
         * This method completes privacy confirmation notification
         *          
         *
         * @param aStatus privacy value
         */
        void CompleteNotifyPrivacyConfirmation( 
            RMobilePhone::TMobilePhonePrivacy aStatus );

        /**
         * Completes call transfering
         *          
         *
         * @param aErrorCode Error code
         */
        void CompleteTransfer( TInt aErrorCode );

        /**
         * Fills current parameters of mobile call info 
         *          
         *
         * @param aInfo Pointer to mobile call information
         * @return TInt Return value to ETel server
         */
        virtual TInt FillMobileCallInfo( TDes8* aInfo );
        
        /**
         * Fills current parameters of mobile call info 
         *          
         *
         * @param aInfo Pointer to mobile call information
         * @return TInt Return value to ETel server
         */
        TInt FillMobileCallInfoDefaults( RMobileCall::TMobileCallInfoV1* aInfo );


        /**
         * Get call name
         *          
         *
         * return Call name
         */
        const TDesC& CallName() const;

        /**
         * Get call ID
         *          
         *
         * @return Call Id 
         */
        TInt CallId() const;
        
        /**
         * Set call ID
         *          
         *
         * @param aCallId Call ID
         * @return Success/failure value.
         */
        TInt SetCallId( TInt aCallId );

        /**
         * Set call Status
         *          
         *
         * @param aCallStatus: Core call status
         * @param aMobileCallStatus: Call status
         */
        void SetCallStatus(
            RCall::TStatus aCallStatus,
            RMobileCall::TMobileCallStatus aMobileCallStatus );

        /**
         * Returns call ID of the previous active call
         *          
         *
         * @return Call Id of the previous active call
         */
        TInt PreviousCallId() const;

        /**
         * Resets call ID of the previous active call
         *          
         *
         */
        void ResetPreviousCallId();

        /**
         * Get call mode
         *          
         *
         * return  Call mode
         */
        RMobilePhone::TMobileService CallMode() const;

        /**
         * Returns call status. For use inside the TSY
         *          
         *
         * return Call core status
         */        
        RCall::TStatus Status() const;

        /**
         * Returns mobile call status. For use inside the TSY
         *          
         *
         * @return Current mobile call status
         */
        RMobileCall::TMobileCallStatus MobileCallStatus() const;

        /** 
         * Returns call direction 
         *          
         *
         * return Call direction (MO/MT call)
         */ 
        RMobileCall::TMobileCallDirection CallDirection() const; 

        /**
         * Sets call direction
         *          
         *
         * @param aCallDirection Call direction (MO/MT call)
         * @return Success/failure value
         */
        TInt SetCallDirection( 
            RMobileCall::TMobileCallDirection aCallDirection );

        /**
         * Returns call capabilities
         *          
         *
         * @return Call capabilities
         */
        TUint32 CallCaps() const;

        /**
         * Sets call capabilities
         *          
         *
         * @param Call capabilities
         */
        void SetCallCaps( TUint32 aCallCaps );

        /**
         * Returns value or internal attribute iGhostCall
         *          
         *
         * @return Value of iGhostCall
         */
        TBool GetGhostCall() const;

        /**
         * Returns ETrue if this service is pending on this call object
         *
         *          
         * @param aReqType Type of request
         * @return requested or not from this call object.
         */
        TBool ServiceRequested( CMmCallTsy::TCallRequestTypes aReqType );

        /**
         * Sets a flag that indicates if this call is not created through ETel
         *          
         *
         * @param aValue Ghost call or not
         */
        void SetGhostCall( TBool aValue );

        /**
         * Handles call status change indications of ghost calls
         *          
         *
         * @param aResult Result value of the status change
         * @param aDataPackage Package containing new call status
         */
        void HandleGhostCallStatusChange (
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
         * Sets the value of the attribute iDiagnosticOctet
         *          
         *
         * @param aDiagnosticOctect SS diagnostic octet value
         */
        void SetDiagnostics ( TUint8 aDiagnosticOctet );

        /**
         * Returns value or internal attribute iDiagnosticOctect
         *          
         *         
         * @return Value of iDiagnosticOctect
         */
        TUint8 GetDiagnostics () const;

        /**
         * Return ETrue if this type of request is requested by any of TSY's 
         * client
         *          
         *
         * @param aReqType Type of request
         * @return Locally requested or not.
         */
        TBool IsServiceLocallyRequested( CMmCallTsy::TCallRequestTypes aReqType );

        /**
         * Set flag that indicates if client has opened existing call object 
         * for incoming call or ghost call that is created using AT-commands.
         *          
         *
         * @param aIsUnownedGhostCall Is this unowned ghost call object
         */
        void SetUnownedCallObjectFlag( TBool aIsUnownedCallObject );

        /**
         * Returns iUnownedCallObject boolean that indicates if client has  
         * opened existing call object for incoming call or ghost call that 
         * is created using AT-commands.
         *          
         *
         * @return Value of iUnownedGhostCall boolean
         */
        TBool IsUnownedCallObject() const;

        /**
         * Returns ETrue if this call was originated through ETel API
         *          
         *
         * @return Value of iEtelOriginated boolean
         */
        TBool ETelOriginated() const;

        /** 
         * Returns ETrue if this call is part of a MO conference
         * (does not apply to an externally controlled conference call)
         *          
         *
         * @return Value of iIsPartOfConference boolean
         */
        TBool IsPartOfConference() const;

        /** 
         * Sets the flag indicating that this call is part of a MO conference
         * (does not apply to an externally controlled conference call)
         *          
         *
         * @param Is call part of conference
         */
        void SetPartOfConference( TBool aPartOfConference );

        /**
         * Completes DialNoFdnCheck request
         *          
         *
         * @param aResult Result of the request
         */
        virtual void CompleteDialNoFdn( TInt aResult );

        /** 
         * Sets Dialflag value indicating if call is in dialling state
         *          
         *
         * @param TBool is dial ongoing
         */
		virtual void SetDialFlag( TBool aDialFlag );
		
		/** 
         * Returns Dialflag value indicating if call is in dialling state
         *          
         *
         * @return is dial ongoing
         */
        virtual	TBool GetDialFlag();
		
#ifdef REQHANDLE_TIMER
        /**
         * Calls the appropriate complete method due timer expiration
         *          
         *
         * @param aReqHandleType TSY request handle type.
         * @param aError Error value for completion
         */
        virtual void Complete( TInt aReqHandleType, TInt aError );
#endif
       
        /**
         * UUI message received from the remote user, complete req
         *          
         * @param aDataPackage Includes received UUI information
         * @param aResult Result code
         */
        void CompleteReceiveUUI( CMmDataPackage* aDataPackage, TInt aResult );    

    public: // Functions from base classes

        /**
         * Opens a call object using given name
         *          
         * @param aName Object name that should be opened
         * @return Opened object 
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& /*aName*/ );
 
        /**
         * Opens a call object and gives it a name 
         *          
         *
         * @param aName On return contains the name of opened object
         * @return Opened object 
         */
        CTelObject* OpenNewObjectL( TDes& /*aNewName*/ );
        
        /**
         * TRAP's all CMmCallTsy related MM API requests in case that
         * they fail. This method functions only as a centralized TRAP for the
         * DoExtFuncL method that does the actual mapping of IPC number to
         * TSY method call.
         *          
         *
         * @param aTsyReqHandle TSY request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone/KErrNotSupported
         */
        TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number
         *          
         *
         * @param aIpc IPC number of the request
         * @return Request mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc ) = 0;

        /**
         * Returns number of slots to be used for a given IPC
         *          
         *
         * @param aIpc IPC number of the request
         * @return Number of slots for this request
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc ) = 0;

        /**
         * Cancels request that's IPC number and request handle are given as
         * parameters
         *          
         *
         * @param aIpc: IPC number of the request
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle ) = 0;

        /**
         * Register given notification
         *          
         *
         * @param aIpc IPC number of the request
         * @return Result of the request
         */
        virtual TInt RegisterNotification( const TInt aIpc ) = 0;

        /**
         * Deregister given notification
         *          
         *
         * @param aIpc IPC number of the request
         * @return Result of the request
         */
        virtual TInt DeregisterNotification( const TInt aIpc ) = 0;

        /**
         * Transfers call ownership.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        virtual TInt TransferOwnership(
            const TTsyReqHandle /*aTsyReqHandle*/ );

        /**
         * Acquires ownership.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        virtual TInt AcquireOwnership(
            const TTsyReqHandle /*aTsyReqHandle*/ );
        
        /**
         * Cancels method for AcquireOwnership.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        virtual TInt AcquireOwnershipCancel(
            const TTsyReqHandle /*aTsyReqHandle*/ );
        
        /**
         * Relinquishes ownership.
         *          
         *
         * @return KErrNone
         */
        virtual TInt RelinquishOwnership();

        /**
         * Get call ownership status.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aOwnershipStatus Ownership status
         * @return KErrNone
         */
        virtual TInt GetOwnershipStatus(
            const TTsyReqHandle /*aTsyReqHandle*/,
            RCall::TOwnershipStatus* /*aOwnershipStatus*/ );

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
        TInt NotifyHookChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns the call status to the client
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aStatus Current call status
         * @return Return value to the ETel Server
         */        
        TInt GetStatus( const TTsyReqHandle aTsyReqHandle,
            RCall::TStatus *aStatus );

        /**
         * Notify when the status of the call has changed
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aStatus Contains Core call status on return
         * @return Return value to the ETel Server
         */
        TInt NotifyStatusChange( const TTsyReqHandle aTsyReqHandle,
            RCall::TStatus* aStatus );

        /**
         * Cancels status change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns the call capabilities to the client
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps Contains call capabilities on return
         * @return Return value to the ETel Server
         */        
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle, 
            RCall::TCaps* aCaps );

        /**
         * Request for core caps change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps Contains core call caps on return
         * @return Return value to the ETel Server
         */        
        TInt NotifyCapsChange( const TTsyReqHandle aTsyReqHandle, 
            RCall::TCaps* aCaps );

        /**
         * Cancels core caps change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */        
        TInt NotifyCapsChangeCancel(
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns call duration information to the client
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aTime Current duration
         * @return Return value to the ETel Server
         */
        TInt GetCallDuration( const TTsyReqHandle aTsyReqHandle, 
            TTimeIntervalSeconds* aTime );

        /**
         * Notifies about changes in call duration
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aTime On return contains current call duration
         * @return Return value to the ETel Server
         */
        TInt NotifyDurationChange( const TTsyReqHandle aTsyReqHandle, 
            TTimeIntervalSeconds* aTime );

        /**
         * Cancels duration change notifications
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyDurationChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns call parameters to the client
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aParams Contains call parameters on return 
         * @return Return value to the ETel Server
         */
        TInt GetCallParams( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aParams );

        /**
         * Returns call information (core) to the client
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCallInfo Contains core call information on return
         * @return Return value to the ETel Server
         */        
        TInt GetInfo( const TTsyReqHandle aTsyReqHandle, 
            RCall::TCallInfo* aCallInfo );

        /**
         * Dials a call.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @param aCallParams Call parameters
         * @param aTelNumber Phone number to be called
         * @return KErrNone or KErrNotSupported
         */
        virtual TInt Dial( const TTsyReqHandle aTsyReqHandle, 
            const TDesC8* aCallParams, TDesC* aTelNumber ) = 0;

        /**
         * Cancels dialling.
         *          
         *
         * @param aTsyReqHandle Tsy request handle
         * @return KErrNone, KErrNotSupported, KErrInUse or KErrGeneral
         */
        virtual TInt DialCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Answers to an incoming call
         *          
         *
         * @param aTsyReqHandle Request handle from the ETel server
         * @param aCallParams Call parameters
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt AnswerIncomingCall( const TTsyReqHandle aTsyReqHandle,
            const TDesC8* aCallParams ) = 0;

        /**
         * Cancels answering to an incoming call
         *          
         *
         * @param aTsyReqHandle Request handle from the ETel server
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt AnswerIncomingCallCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Hangs up the call
         *          
         *
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt HangUp( const TTsyReqHandle aTsyReqHandle ) = 0;

        /**
         * Cancels the hangup
         *          
         *
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        TInt HangUpCancel( const TTsyReqHandle aTsyReqHandle );
        
        /** 
         * Returns ETrue if this call is part of a MO conference and MT released
         * (does not apply to an externally controlled conference call)
         *          
         *
         * @return Value of iIsRemoteReleasedCall boolean
         */
        TBool IsRemoteReleasedCall() const;

#ifdef TF_LOGGING_ENABLED       
        /**
         * Overloads original ReqCompleted for logging purposes
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aError error value
         */
        virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
#endif

        /**
         * Returns fax settings to the client
         *          
         *
         * @param aTsyReqHandle TSY req handle
         * @param TFaxSessionSettings Fax settings
         * @return KErrNotSupported
         */
        virtual TInt GetFaxSettings(
            const TTsyReqHandle /*aTsyReqHandle*/,
            RCall::TFaxSessionSettings* /*aSettings*/ );

        /**
         * Sets new fax settings
         *          
         *
         * @param aTsyReqHandle TSY req handle
         * @param TFaxSessionSettings Fax settings
         * @return KErrNotSupported
         */
        virtual TInt SetFaxSettings(
            const TTsyReqHandle /*aTsyReqHandle*/,
            const RCall::TFaxSessionSettings* /*aSettings*/ );
            
        /**
         * Sets iExtensionId
         * @param aExtensionId 
         */ 
        virtual void SetExtensionId( TInt aExtensionId );
		
        /**
         * Gets iExtensionId
         *          
         *         
         * @return ExtensionId
         */ 
        virtual TInt GetExtensionId();

        /**
         * Sets iDialTypeId
         *
         * @param aDialType
         *
         */ 
        virtual void SetDialTypeId( TUint8 aDialType );
        
        /**
         * Gets iDialTypeId
         *          
         *        
         * @return iDialTypeId
         */ 
        virtual TUint8 GetDialTypeId();	
        
    protected:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmCallTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         *          
         *
         * @param RMobilePhone::TMobileService aMode Call mode
         */
        virtual void ConstructL( RMobilePhone::TMobileService aMode );

    protected:  // New functions

        /**
         * Initialises extension modules
         *          
         *
         * @param RMobilePhone::TMobileService Call mode
         */
        virtual void InitExtensionModulesL( RMobilePhone::TMobileService aMode );

        /**
         * Initialises miscellaneous internal attributes
         *          
         *
         */
        virtual void InitInternalAttributes();

        /**
         * DoExtFuncL is called by the server when it has a "extended", 
         * i.e. non-core ETel request for the TSY. To process a request handle,
         * request type and request data are passed to the TSY.
         *           
         *
         * @param aTsyReqHandle Request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, const TInt aIpc, 
            const TDataPackage& aPackage ) = 0;

        /**
         * Returns pointer to iTsyReqHandleStore
         *          
         *
         * @return CMmTsyReqHandleStore* Pointer to the TsyReqHandleStore
         */
        CMmTsyReqHandleStore* GetCallReqHandleStore();

        /**
         * Completes caps change notification
         *          
         *
         */
        void CompleteNotifyCapsChange();

        /**
         * Complete call event notification
         *          
         *
         * @param event that has occurred
         */
        void CompleteNotifyCallEvent( RMobileCall::TMobileCallEvent aEvent );

        /**
         * Returns the call control and call event capabilities
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCaps Contains mobile call capabilities on return
         * @return Return value to the ETel Server
         */
        TInt GetMobileCallCaps( const TTsyReqHandle aTsyReqHandle,
            TDes8* aCaps );
        
        /**
         * Notifies change of mobile call capabilities 
         *          
         *
         * @param aCaps Contains mobile call capabilities on return
         * @return Return value to the ETel Server
         */
        TInt NotifyMobileCallCapsChange( TDes8* aCaps );

        /**
         * Cancels NotifyMobileCallCapsChange method
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyMobileCallCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Transfer a call to a remote party with out answering the call
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return KErrNone
         */
        TInt TransferL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Set call name
         *          
         *
         * @param aCallName New call name
         */
        void SetCallName( const TName& aCallName );

        /**
         * Requests notifications of privacy status changes
         *           
         *
         * @param aPrivacyStatus* Current privacy status
         * @return Return value to ETel server
         */
        TInt NotifyPrivacyConfirmation ( 
            RMobilePhone::TMobilePhonePrivacy* aPrivacyStatus );

        /**
         * Cancels privacy status change notifications 
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to ETel server
         */
        TInt NotifyPrivacyConfirmationCancel (  
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Returns the current status of the call through the aStatus argument
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aStatus Contains mobile call status on return
         * @return Return value to the ETel Server
         */        
        TInt GetMobileCallStatus( const TTsyReqHandle aTsyReqHandle, 
            RMobileCall::TMobileCallStatus* aStatus );

        /**
         * Allows a client to be notified when the mobile call changes state.
         *          
         *
         * @param aStatus Contains mobile call status on return
         * @return Return value to the ETel Server
         */        
        TInt NotifyMobileCallStatusChange( 
            RMobileCall::TMobileCallStatus* aStatus );

        /**
         * Cancels an outstanding asynchronous NotifyMobileCallStatusChange 
         * request.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */        
        TInt NotifyMobileCallStatusChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Allows clients to be notified when various call events occur.
         *          
         *
         * @param aEvent Contains call event on return
         * @return Return value to the ETel Server
         */        
        TInt NotifyCallEvent( RMobileCall::TMobileCallEvent* aEvent );

        /**
         * This method cancels an outstanding asynchronous NotifyCallEvent 
         * request.
         *
         *          
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */        
        TInt NotifyCallEventCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * The method returns current values of the call information
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aInfo Contains mobile call info on return
         * @return Return value to the ETel Server
         */
        TInt GetMobileCallInfo( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aInfo );

        /**
         * Allows the client to be notified of any change in the remote party 
         * information.
         *          
         *
         * @param aRemotePartyInfo Contains remote party information on return
         * @return Return value to the ETel Server
         */
        TInt NotifyRemotePartyInfoChange( TDes8* aRemotePartyInfo );

        /**
         * This method cancels an outstanding asynchronous 
         * NotifyRemotePartyInfoChange request.
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @return Return value to the ETel Server
         */
        TInt NotifyRemotePartyInfoChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Resets the status of the call
         *          
         *
         */
        virtual void ClearCallStatus();

        /**
         * Set previous active call ID
         *          
         *
         * @param TInt Call id of the previous active call 
         */
        void SetPreviousCallId( TInt aCurrentId );

        /**
         * Returns call duration in seconds
         *          
         *
         * @return Call duration in seconds
         */
        TTimeIntervalSeconds GetCallDurationInSeconds();

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         *
         * @param aReqHandleType TSY request handle type.
         * @param aTsyReqHandle TSY request handle to be stored.
         */
        virtual void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle ) = 0;
#endif

        /**
         * Specifies which User-To-User Signalling service(s) 
         * the phone should request to activate.
         *          
         * @param aUUSRequest Pointer to client side data 
         * @return Return value to the ETel Server
         */
        TInt ActivateUUS( const TTsyReqHandle aTsyReqHandle,
            RMobileCall::TMobileCallUUSRequestV1Pckg* aUUSRequest );
               
        /**
         * Enables the client to receive the next incoming 
         * UUI message from the remote user.
         *          
         * @param aTsyReqHandle TSY request handle,
         * @param aUUI Pointer to client side data 
         * @return Return value to the ETel Server
         */
        TInt ReceiveUUI( const TTsyReqHandle aTsyReqHandle,
            RMobileCall::TMobileCallUUI* aUUI );
        
        /**
         * Cancels an outstanding EMobileCallReceiveUUI request 
         *          
         * @param aTsyReqHandle TSY request handle, complete with KErrCancel
         * @return KErrNone
         */
        TInt ReceiveUUICancel( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Updates life time call duration with remaining call duration 
         *          
         */
        void UpdateLifeTimer();                                         

    private:
        /**
         * Checks wether or not a ETel request can be performed while 
         * offline mode is enabled
         *          
         *         
         * @param aIpc Ipc number of the request
         * @return Is request possible in offline mode
         */
        TBool IsRequestPossibleInOffline( TInt aIpc ) const;
    
    protected:  // Data

        /**
         * A pointer to the multimode line object
         * Own.         
         */         
        CMmLineTsy* iMmLine;

        /** 
         * A pointer to external call interface object
         * Own.         
         */          
        CMmCallExtInterface* iMmCallExtInterface;

        /** 
         * A pointer to multimode phone object
         * Own.         
         */ 
        CMmPhoneTsy* iMmPhone;
 
        /** 
         * Pointer to request handle store
         * Own.         
         */ 
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /** 
         * A table for call request handles   
         */ 
        TTsyReqHandle iCallReqHandles[EMultimodeCallMaxNumOfRequests];
    
        /** 
         * Call mode    
         */ 
        RMobilePhone::TMobileService iCallMode;
 
        /** 
         * Pointer to the message manager
         * Own.         
         */ 
        CMmMessageManagerBase* iMessageManager;
        
        /** 
         * Call status        
         */ 
        RCall::TStatus iCallStatus;

        /** 
         * Call ID, used for communication between TSY and call server       
         */         
        TInt iCallId;

        /** 
         * Last active Call ID, the call id is reset when call goes
         * to idle state               
         */
        TInt iPreviousCallId;

        /** 
         * Dynamic Call caps               
         */
        RCall::TCaps iCallCaps;

        /** 
         * Call name               
         */
        TName iCallName;

        /** 
         * Mobile call parameters              
         */
        RMobileCall::TMobileCallParamsV7 iCallParams;

        /** 
         * Mobile call parameters package              
         */
        RMobileCall::TMobileCallParamsV1Pckg iCallParamsPckg;

        /** 
         * Contains the reason for the call termination              
         */
        TInt iLastExitCode;

        /** 
         * Duration change notifier              
         */
        CHeartbeatRunner* iCallTimer;

        /** 
         * Current Mobile Call status              
         */
        RMobileCall::TMobileCallStatus iMobileCallStatus;

        /** 
         * Call direction              
         */
        RMobileCall::TMobileCallDirection iCallDirection;

        /** 
         * Status of dialCancel              
         */
        TDialCancelStatus iDialCancelFlag;

        /** 
         * Status of AnswerIncomingCallCancel              
         */
        TBool iAnswerCancelFlag;

        /** 
         * Request handle type              
         */
        TCallRequestTypes iReqHandleType;

        /** 
         * diagnostic Octets              
         */
        TUint8 iDiagnosticOctet;

        /** 
         * Boolean that indicates is this unowned call object             
         */
        TBool iUnownedCallObject;

        /** 
         * Boolean that indicates is this ghost call 
         * (created using AT-commands)             
         */
        TBool iGhostCall;

        /** 
         * Previous call capabilities. Used to determine if notification 
         * about capabilities change should be sent            
         */
        RCall::TCaps iPreviousCaps;

        /** 
         * Previous call control capabilities.             
         */
        TUint32 iPreviousCallControlCaps;

        /** 
         * Pointer to Notify Mobile Call capabilities (pointing to ETel
         * server's address space) 
         * Own.                  
         */
        TDes8* iRetNotifyMobileCallCaps;

        /** 
         * Pointer to the core call capabilities (pointing to ETel server's
         * address space)
         * Own.                  
         */
        RCall::TCaps* iRetCaps;
        
        /** 
         * Pointer to the core call status (pointing to ETel server's address 
         * space)
         */
        RCall::TStatus* iRetStatus;

        /** 
         * Pointer to the mobile call status (pointing to ETel server's address
         * space)
         * Own.                  
         */
        RMobileCall::TMobileCallStatus* iRetMobileCallStatus;

        /** 
         * Pointer to the mobile remote party info (pointing to ETel server's
         * address space)
         * Own.                  
         */
        TDes8* iRetRemotePartyInfo;

        /** 
         * Pointer to the call duration (pointing to ETel server's address
         * space)
         * Own.                  
         */
        TTimeIntervalSeconds* iRetTimeIntervalSeconds;

        /** 
         * Pointer to the mobile call event (pointing to ETel server's address
         * space)
         * Own.                  
         */
        RMobileCall::TMobileCallEvent* iRetCallEvent;

        /** 
         * Pointer to privacy confirmation status (pointing to ETel server's
         * address space)
         * Own.                  
         */
        RMobilePhone::TMobilePhonePrivacy* iRetPrivacyStatus;

        /** 
         * Call ownership list
         * Own.                  
         */
        CAcquireOwnerList* iList;

        /** 
         * Is this an emergency call                  
         */
        TBool iEmergencyCall;

        /** 
         * Is this call originated through ETel API                  
         */
        TBool iEtelOriginated;

        /** 
         * Boolean that indicates if this call is part of a MO conference call
         * (false if part of an externally controlled conference call)                         
         */
        TBool iIsPartOfConference;
 
        /** 
         * Specify the ExtensionId                         
         */      
        TInt iExtensionId;

        /** 
         * Boolean indicating if dial is ongoing                         
         */ 
        TBool iDialFlag;

        /** 
         * Boolean that indicates if this call is MT released
         * and part of internally controlled conference call                                 
         */
        TBool iIsRemoteReleasedCall;
        
        /** 
         * Boolean indicating if the hangUp is ongoing                         
         */
        TBool iHangUpFlag;

        /** 
         * Pointer to the UUI message (pointing to ETel server's address
         * space)
         * Not own.                  
         */
        RMobileCall::TMobileCallUUI* iRetUUI; 
        
        /** 
         * Buffer the incoming UUI information. This is only used with the
         * UUS1 case, to be able to get the first complete data from LTSY.
         */
        RMobileCall::TMobileCallUUI iReceivedUUI; 
        
        /** 
         * Buffer the ActivateUUS request, 
         * send the infromation to LTSY during the dial (UUS1). 
         */
        RMobileCall::TMobileCallUUSRequestV1 iUUSRequest; 
       	
       	/** 
         * Specify the dial type                         
         */      
        TUint8 iDialTypeId;
        
        /**
         * Pointer to the telephony audio control
         */ 
        MTelephonyAudioControl* iTelephonyAudioControl;
  		
        /** 
         * Boolean that indicates if TelephonyAudioControl is set up or not.
         * True if call routing control has been set up for the call.
         * False when call is not set up (idle).
         */
        TBool iTelephonyAudioControlSetup;
  
    };

#endif      // CMMCALLTSY_H   
            
// End of File

