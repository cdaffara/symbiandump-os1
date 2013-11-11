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



#ifndef CMMWIMTSY_H
#define CMMWIMTSY_H

//  INCLUDES
#include "CMmCustomTsy.h"
#include "CMmSubTsyBase.h"
#include "cmmtsyreqhandlestore.h"

// CLASS DECLARATION

/**
 *  CMmWimTsy contains mode-independent WIM functionality. 
 *  Extension request handles and parameters are stored as attributes.
 */
NONSHARABLE_CLASS( CMmWimTsy ) : public CMmSubTsyBase
    {
    public:

    // TWIMRequestType enumerates indexes to WIM's request handle
    // table. Request handles are stored there while waiting for 
    // completion of the request.
    enum TWIMRequestType
        {
        EWIMTsyReqHandleUnknown,
        EWIMRequestTypeSendAPDUReq,
        EWIMRequestTypeGetATR,
        EWIMRequestTypeSimWarmReset,        
        EWIMRequestTypeGetSimCardReaderStatus,
        EWIMRequestTypeNotifySimCardStatus,
        EWIMRequestTypeSendAPDUReqV2,
        EWIMRequestTypePowerSimOn,
        EWIMRequestTypePowerSimOff,
        // Declare constant for those requests that need
        // own request handle record in iTsyReqHandleStore above the
        // following
        EWIMRequestTypeMaxNumOfRequests,

        // Let the next literal be the last
        EWIMRequestTypeUndefined = 0xff
        };

    public:

        /**
         * Two-phased constructor.
         *          
         *         
         * @param mmCustoTsy A pointer to a MM Custom Tsy
         * @return A pointer to a MM WIM TSY object
         */
        static CMmWimTsy* NewL( CMmCustomTsy* mmCustomTsy );

        /**
         * Destructor
         */
        ~CMmWimTsy();

        /**
         * Initialisation method
         *          
         *
         */
        void Init();

        /**
         * Tells whether the object supports given IPC.
         *         
         *
         * @param anIpc Request's IPC value
         * @return TBool ETrue if object support given IPC
         */
        TBool SupportingIPC( const TInt anIpc );

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning  
         * on memory allocation failure.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param anIpc Request's IPC value
         * @param aPackage Reference to a data package
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt anIpc, 
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number.
         *          
         *
         * @param anIpc An interprocess communication command identifier
         * @return The ReqMode corresponding to given IPC
         */
        CTelObject::TReqMode ReqModeL( const TInt anIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *          
         *
         * @param anIpc An interprocess communication command identifier
         * @return The number of slots reserved for a given IPC
         */
        TInt NumberOfSlotsL( const TInt anIpc );

        /**
         * Cancels request of which IPC number and request handle are given in 
         * parameters.
         *          
         *
         * @param anIpc An interprocess communication command identifier
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt CancelService( const TInt anIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification.
         *
         *          
         * @param anIpc an interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt RegisterNotification( const TInt anIpc );

        /**
         * Deregister given notification.
         *
         *          
         * @param anIpc An interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt DeregisterNotification( const TInt anIpc );

        /**
         * Complete APDU response message from adaptation
         *          
         *          
         * @param aDataPackage Packaged response data
         * @param aError Error code
         */
        void CompleteSendAPDUReq( CMmDataPackage * aDataPackage, TInt aError );
            
        /**
         * Complete GetATR response message
         *          
         *          
         * @param aData Response data
         * @param aError Error code
         */
        void CompleteGetATR( TPtrC8& aData, TInt aError );
        
        /**
         * Complete SimWarmReset response message
         *          
         *
         * @param aError Error code
         */
        void CompleteSimWarmReset( TInt aError );
        
        /**
         * Complete GetSimCardReaderStatus response message
         *          
         *
         * @param aData Response data
         * @param aError Error code
         */
        void CompleteGetSimCardReaderStatus( TPtrC8& aData, TInt aError );
        
        /**
         * Complete NotifySimCardStatus notification
         *          
         *
         * @param aCardStatus Response data
         */
        void CompleteNotifySimCardStatus( 
            RMmCustomAPI::TSIMCardStatus aCardStatus );
        
        /**
         * Complete SendAPDUReqV2 response message
         *          
         *
         * @param aData Response data
         * @param aError Error code
         */
        void CompleteSendAPDUReqV2( TPtrC8& aData, TInt aError );
        
        /**
         * Complete PowerSimOn response message
         *          
         *
         * @param aError Error code
         */
        void CompletePowerSimOn( TInt aError );
        
        /**
         * Complete PowerSimOff response message
         *          
         *
         * @param aError Error code
         */
        void CompletePowerSimOff( TInt aError );
        
#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due the timer expiration
         *          
         *
         * @param aReqHandleType
         * @param aError
         */
        void Complete( TInt aReqHandleType, TInt aError );
#endif // REQHANDLE_TIMER

    private:

        /**
         * Send APDU request message to adaptation
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aInfo Pointer to the detail's of the request
         * @param aData Request's data buffer pointer
         * @return KErrNone
         */
        TInt SendAPDUReqL( const TTsyReqHandle aTsyReqHandle, 
            TDes8 * aInfo, 
            TDes8 * aData );
                            
        /**
         * Cancel SendAPDUReq request
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return error code         
         */
        TInt SendAPDUReqCancel( TTsyReqHandle aTsyReqHandle );

        /**
         * By default Symbian 2nd phase constructor is private.
         *          
         *
         * @param aMmCustomTsy a pointer to MM Custom Tsy object
         * @return error code            
         */
        void ConstructL( CMmCustomTsy * aMmCustomTsy );

        /**
         * C++ default constructor
         */
        CMmWimTsy();

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         *
         * @param aReqHandleType 
         * @param aTsyReqHandle
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif // REQHANDLE_TIMER

        /**
         * Send GetATR request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aATR Pointer to the detail's of the request
         * @return Error value
         */
        TInt GetATRL( const TTsyReqHandle aTsyReqHandle, TDes8 * aATR );
        
        /**
         * Cancel GetATR request
         *          
         *
         * @return TInt KErrNone
         */
        TInt GetATRCancel();
           
        /**
         * Send SimWarmReset request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt SimWarmResetL( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Cancel SimWarmReset request
         *          
         *
         * @return  KErrNone
         */
        TInt SimWarmResetCancel();
        
        /**
         * Send GetSimCardReaderStatus request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aSimCardReaderStatus Pointer to the detail's of the request
         * @return Error value
         */
        TInt GetSimCardReaderStatusL( const TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSimCardReaderStatus * aSimCardReaderStatus );
            
        /**
         * Cancel GetSimCardReaderStatus request
         *          
         *
         * @return KErrNone
         */
        TInt GetSimCardReaderStatusCancel();
        
        /**
         * Send NotifySimCardStatus request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aCardStatus Pointer to the detail's of the request
         * @return Error value
         */
        TInt NotifySimCardStatus( const TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSIMCardStatus * aCardStatus );
        
        /**
         * Cancel NotifySimCardStatus request
         *          
         *
         * @param ?arg1 ?description
         * @return KErrNone
         */
        TInt NotifySimCardStatusCancel();
        
        /**
         * Send APDUReqV2 request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aApduParameters Pointer to class on user side
         * @return Error value
         */
        TInt SendAPDUReqV2L( TTsyReqHandle aTsyReqHandle,
            RMmCustomAPI::TApduParameters * aApduParameters );
        
        /**
         * Cancel SendAPDUReqV2 request
         *          
         * @sins S60 v.2.6
         * return KErrNone
         */
        TInt SendAPDUReqV2Cancel();
        
        /**
         * Send PowerSimOn request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt PowerSimOnL( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Cancel PowerSimOn request
         *          
         *
         * return KErrNone
         */
        TInt PowerSimOnCancel();
             
        /**
         * Send PowerSimOff request message
         *          
         *
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt PowerSimOffL( const TTsyReqHandle aTsyReqHandle );
        
        /**
         * Cancel PowerSimOff request
         *          
         *         
         * @return KErrNone
         */
        TInt PowerSimOffCancel();
        
    private:
        
        
        /**
         * Pointer to CMmCustomTsy object
         * Not Own.        
         */
        CMmCustomTsy* iMmCustomTsy;   

        /**
         * Pointer to request handle store
         * Own.      
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * WIM Custom API request handle table
         * Own.      
         */
        TTsyReqHandle iWIMReqHandles[EWIMRequestTypeMaxNumOfRequests];

        /**
         * Return pointers to client side
         * Own.      
         */
        TDes8* iRetApduInfo;
        
        /**
         * Return pointers to client side
         * Own.      
         */
        TDes8* iRetApduData;
        
        /**
         * Return pointers to client side
         * Own.      
         */
        TDes8* iAnswerToReset;
        
        /**
         * Pointer to the buffer to hold ApduParameters
         * Own.      
         */
        RMmCustomAPI::TApduParameters* iApduParameters;
               
        /**
         * Pointer to the buffer to hold SIM card reader status
         * Own.      
         */
        RMmCustomAPI::TSimCardReaderStatus* iSimCardReaderStatus;
        
        /**
         *  Pointer to the buffer to hold SIM card status
         * Own.      
         */
        RMmCustomAPI::TSIMCardStatus* iNotifiedSimCardStatus;        
        };

#endif     // CMMWIMTSY_H
