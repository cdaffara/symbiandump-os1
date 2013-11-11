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



#ifndef CMMPACKETTSY_H
#define CMMPACKETTSY_H

// INCLUDES
#include "CMmCustomTsy.h"
#include "cmmtsyreqhandlestore.h"
#include "CMmSubTsyBase.h"
#include <ctsy/rmmcustomapi.h>
#include "cmmpacketservicegsmwcdmaext.h"

// FORWARD DECLARATIONS
class CMmCustomTsy;
class CMmPacketContextTsy;
class CMmTsyReqHandleStore;

// CLASS DECLARATION

/**
*  CMmPacketTsy contains GPRS related functionality
*  It does not contain an extension. Instead it calls functions from 
*  CMmPacketContextTsy
*
*/
NONSHARABLE_CLASS( CMmPacketTsy ) : public CMmSubTsyBase
    {
    public: // Enumerations

        // TPacketRequestType enumerates indexes to Packet's request handle
        // table. Request handles are stored there while waiting for 
        // completion of the request.
        enum TPacketRequestType
            {
            EMultimodePacketContextReqHandleUnknown,
            EMultimodePacketContextSetAuthenticationInfo,
            EMultimodePacketContextNotifyEGprsInfoChange,

            // ATTENTION: Declare constant for those requests that need
            // own request handle record in iTsyReqHandleStore above the
            // following!
            EMultimodePacketMaxNumOfRequests
            };

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CMmPacketTsy* NewL( CMmCustomTsy* aMmCustomTsy, 
            CMmPhoneTsy* aMmPhoneTsy );

        /**
         * Destructor
         */
        virtual ~CMmPacketTsy();

    public: // New functions

        /**
         * Initialisation method
         *          
         *
         */
        virtual void Init();

        /**
         * Register given notification.
         *          
         *
         * @param aIpc IPC of the request
         * @return Error value
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification.
         *          
         *
         * @param aIpc IPC of the request
         * @return Error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Tells whether the object supports given IPC.
         *          
         *
         * @param aIpc IPC of the request
         * @return true or false
         */
        virtual TBool SupportingIPC( const TInt aIpc );

        /**
         * Handles extended client requests.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Contains parameters for request
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number.
         *          
         *
         * @param aIpc IPC number of request
         * @return Request mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *          
         *
         * @param aIpc IPC number of request
         * @return Number of slots to be used
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request of which IPC number and request handle are given in
         * parameters.
         *          
         *
         * @param aIpc IPC number of request
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Completes notification to client.
         *          
         *
         * @param aGprsSupporsInCell
         */
        virtual void CompleteNotifyEGprsInfoChange( 
            const TBool aGprsSupporsInCell );
        /** 
         * Request Set Always On
         *
         * @param aTsyReqHandle TSY ReqHandle
         * @param aMode pointer to requested mode
         * @return error value
         */ 
        TInt SetAlwaysOnL( TTsyReqHandle aTsyReqHandle, 
        	RMmCustomAPI::TSetAlwaysOnMode* aMode );
 
        /** 
         * Completes SetAlwaysOn request
         * 
         * @param aError error value from LTSY 
         */
        void CompleteSetAlwaysOn( TTsyReqHandle aReqHandle, TInt aError );
        
        /** 
         * Cancels Set Always On
         *
         * @param aTsyReqHandle TSY reghandle
         */
        void CancelSetAlwaysOn( TTsyReqHandle aReqHandle );
        
#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due the timer expiration
         *          
         *
         * @param aReqHandleType Request handle type
         * @param aError
         */
        virtual void Complete( TInt aReqHandleType, TInt aError );
#endif // REQHANDLE_TIMER

        /**
         * Resets an entry in the req handle store 
         * 
         * 
         * @param aTsyReqHandle req handle 
         * @param aIpc IPC value
         * return EFalse if req handle not found
         */      
        virtual TBool ResetReqHandle(const TTsyReqHandle aTsyReqHandle, const TInt aIpc);
        
    private:

        /**
         * C++ default constructor.
         */
        CMmPacketTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Overload the original ReqCompleted for logging purposes.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aError
         */
        virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle, 
            const TInt aError );

        /**
         * Request Notification when EGprs information changes.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aGprsInformation GPRS Information
         * @return Error value
         */
        TInt NotifyEGprsInfoChange( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aGprsInformation );

        /**
         * Get EGprs information.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aGprsInformation GPRS information
         * @return Error value
         */
        TInt GetEGprsInfo( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aGprsInformation );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         *
         * @param aReqHandleType Request handle type
         * @param aTsyReqHandle Request handle
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif // REQHANDLE_TIMER

    private: // Data

        /**
         * Pointer to CMmCustomTsy
         * Not own.         
         */
        CMmCustomTsy* iMmCustomTsy;

        /**
         * Pointer to the Phone TSY
         * Not own.         
         */
        CMmPhoneTsy* iMmPhone;

        /**
         * Pointer to the Req handle store
         * Own.         
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        // Table for packet context request handles
        TTsyReqHandle iPacketReqHandles[EMultimodePacketMaxNumOfRequests];

        /**
         * Gprs connection information
         * Not Own.         
         */
        RMmCustomAPI::TGprsInformationPckg* iGprsInformationPckg;
        
        /**
         * Allways on mode parameter
         * Not Own
         */
        RMmCustomAPI::TSetAlwaysOnMode iAllwaysOnMode;
        
    };

#endif // CMMPACKETTSY_H

//  End of File
