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



#ifndef CMMSIMLOCKTSY_H
#define CMMSIMLOCKTSY_H

//  INCLUDES
#include "CMmCustomTsy.h"
#include "cmmtsyreqhandlestore.h"
#include "CMmSubTsyBase.h"

// CLASS DECLARATION

/**
*  CMmSimLockTsy contains mode-independent SimLock functionality.
*  Extension request handles and parameters are stored as attributes.
*
*/
NONSHARABLE_CLASS( CMmSimLockTsy ) : public CMmSubTsyBase
    {
    public:

    // TSimLockRequestType enumerates indexes to SimLock's request handle
    // table. Request handles are stored there while waiting for 
    // completion of the request.
    enum TSimLockRequestType
        {
        ESimLockRequestTypeActivateSimLock,
        ESimLockRequestTypeDeActivateSimLock,
 
        // Declare constant for those requests that need
        // own request handle record in iTsyReqHandleStore above the
        // following
        ESimLockRequestTypeMaxNumOfRequests,

        // Let the next literal be the last
        ESimLockRequestTypeUndefined = 0xFF
        };

    public:  // Constructors and destructor
        
        /**
         * C++ default constructor.
         */
        CMmSimLockTsy();

        /**
         * Two-phased constructor.
         */
        static CMmSimLockTsy* NewL( CMmCustomTsy* mmCustomTsy );
        
        /**
         * Destructor.
         */
        ~CMmSimLockTsy();

    public: // Functions from base classes

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
         * @param aIpc request's IPC value
         * @return ETrue if object supports given IPC
         */
        TBool SupportingIPC( const TInt aIpc );

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning  
         * on memory allocation failure.
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aPackage Reference to a data package
         * @return KErrNone if request accepted, else several possibilities
         *         depending on IPC
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number.
         *          
         *
         * @param aIpc an interprocess communication command identifier
         * @return The ReqMode corresponding to given IPC
         */
        CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *          
         *
         * @param aIpc An interprocess communication command identifier
         * @return The number of slots reserved for a given IPC
         */
        TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request of which IPC number and request handle are given 
         * in parameters.
         *          
         *
         * @param aIpc An interprocess communication command identifier 
         * @param aTsyReqHandle Request handle
         * @return TInt KErrNone, KErrGeneral if unknown IPC
         */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification.
         *          
         *
         * @param aIpc An interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification.
         *          
         *
         * @param aIpc An interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt DeregisterNotification( const TInt aIpc );
        
    public: // New functions

        /**
         * Completes ActivateSimLock request.
         *          
         *
         * @param aError Error value
         */
        void CompleteActivateSimLock( TInt aError );

        /**
         * Completes DeActivateSimLock request.
         *          
         *
         * @param aError Error value
         */
        void CompleteDeActivateSimLock( TInt aError );

#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due the timer expiration
         *          
         *
         * @param aReqHandleType
         * @param aError
         */
        void Complete( TInt aReqHandleType, TInt aError );
#endif //REQHANDLE_TIMER

    private:

        /**
         * By default EPOC constructor is private.
         */
        void ConstructL( CMmCustomTsy* aMmCustomTsy );

        /**
         * Activate SimLock
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aPassword Pointer to simlock password
         * @param aLockNumber Pointer to simlock number to be activated
         * @return Error value
         */
        TInt ActivateSimLockL( const TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSimLockPassword* aPassword, 
            RMmCustomAPI::TLockNumber* aLockNumber );

        /**
         * DeActivate SimLock
         *          
         *
         * @param aTsyReqHandle Request handle
         * @param aPassword Pointer to simlock password
         * @param aLockNumber Pointer to simlock number to be deactivated
         * @return Error value
         */
        TInt DeActivateSimLockL( const TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSimLockPassword* aPassword, 
            RMmCustomAPI::TLockNumber* aLockNumber );

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
#endif //REQHANDLE_TIMER

    private: // Data
    
        /**
         * Pointer to CustomTsy
         * Not Own.         
         */
        CMmCustomTsy* iMmCustomTsy;

        /**
         * Pointer to request handle store
         * Own.         
         */        
        CMmTsyReqHandleStore* iTsyReqHandleStore;
        
        /**
         * SimLock request handles        
         */   
        TTsyReqHandle iSimLockReqHandles[ESimLockRequestTypeMaxNumOfRequests];
        
        /**
         * SimLock activating data
         * Not Own.        
         */ 
        RMmCustomAPI::TLockNumber* iActivateSimLockNumber;
        
        /**
         * Activation SimLock password
         * Not Own.        
         */ 
        RMmCustomAPI::TSimLockPassword* iActivateSimLockPassword;
        
        /**
         * SimLock deactivating data
         * Not Own.        
         */ 
        RMmCustomAPI::TLockNumber* iDeActivateSimLockNumber;
        
        /**
         * Deactivation SimLock password
         * Not Own.        
         */ 
        RMmCustomAPI::TSimLockPassword* iDeActivateSimLockPassword;

    };

#endif      // CMMSIMLOCKTSY_H  
            
//  End of File
