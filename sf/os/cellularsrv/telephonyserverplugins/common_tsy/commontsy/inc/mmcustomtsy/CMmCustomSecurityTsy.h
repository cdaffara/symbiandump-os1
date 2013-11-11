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



#ifndef CCUSTOMMMSECURITYTSY_H
#define CCUSTOMMMSECURITYTSY_H

//  INCLUDES
#include "CMmCustomTsy.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/serviceapi/cmmcustomsecuritytsyconstants.h>

// FORWARD DECLARATIONS
class CMmMessageManagerBase;
class CMmDataPackage;


// CLASS DECLARATION

/**
*  CMmCustomSecurityTsy
*/
NONSHARABLE_CLASS( CMmCustomSecurityTsy ) : public CBase
    {

    public: 

    // TSecurityRequestType enumerates indexes to Security's request handle
    // table. Request handles are stored there while waiting for 
    // completion of the request.
    enum TSecurityRequestType
        {
        ESecurityTsyReqHandleUnknown,
        ESecurityRequestTypeIsBlocked,
        ESecurityRequestTypeCheckSecurityCode,
        ESecurityRequestTypeDisablePhoneLock,

        // Declare constant for those requests that need
        // own request handle record in iTsyReqHandleStore above the
        // following
        ESecurityRequestTypeMaxNumOfRequests,

        // Let the next literal be the last
        ESecurityRequestTypeUndefined = 0xff
        };

    public:  // Constructors and destructor
        
        /**
         * C++ default constructor.
         */
        CMmCustomSecurityTsy();

        /**
         * Two-phased constructor.
         */
		static CMmCustomSecurityTsy* NewL( CMmCustomTsy* aMmCustomTsy, 
		    CMmPhoneTsy* aPhoneTsy );

        /**
         * Delivers password to extension for lock setting change
         *          
         * @param aCodes
         * @return KErrNone
         */
        virtual TInt DeliverCodeL( RMobilePhone::TCodeAndUnblockCode aCodes );
        
        /**
         * Destructor.
         */
        ~CMmCustomSecurityTsy();

    public:

        /**
         * Initialisation method
         *          
         */
        void Init();

        /**
         * Tells whether the object supports given IPC.
         *          
         * @param aIpc Request's IPC value
         * @return TBool ETrue if object supports given IPC
         */
        TBool SupportingIPC( const TInt aIpc );

		/**
         * Handles extended client requests. Uses TRAP to ensure functioning  
         * on memory allocation failure.
         *          
         * @param aTsyReqHandle Request handle
         * @param aIpc Request's IPC value
         * @param aPackage Reference to a data package
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number.
         *          
         * @param aIpc An interprocess communication command identifier
         * @return The ReqMode corresponding to given IPC
         */
        CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *          
         * @param aIpc An interprocess communication command identifier
         * @return The number of slots reserved for a given IPC
         */
        TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request of which IPC number and request handle are given 
         * in parameters.
         * 
         * @param aIpc An interprocess communication command identifier
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification.
         *          
         * @param aIpc An interprocess communication command identifier
         * @return  KErrNotSupported
         */
        TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification.
         *          
         * @param aIpc An interprocess communication command identifier
         * @return KErrNotSupported
         */
        TInt DeregisterNotification( const TInt aIpc );
       
    public:  // New functions

		/**
         * Completes the IsBlocked response message.
         *          
         * @param aError Error value
         * @param aIsBlocked
         */
        void CompleteIsBlocked( TInt aError, TBool aIsBlocked );

		/**
         * Completes the CheckSecurityCode response message.
         *          
         * @param aError Error value
         */
        void CompleteCheckSecurityCode( TInt aError );

		/**
         * Completes the CheckSecurityCodeCancel response message.
         *          
         */
        void CompleteCheckSecurityCodeCancel();

		/**
         * Completes the DisablePhoneLock request.
         *          
         * @param aError Error value
         */
        void CompleteDisablePhoneLock( TInt aError );

        /**
         * Change the information about the currently active PIN.
         *          
         * @param aDataPackage Data package
         */
        void SetActivePin( CMmDataPackage* aDataPackage );


#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due the timer expiration
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
        void ConstructL( CMmCustomTsy* aMmCustomTsy, CMmPhoneTsy* aMmPhoneTsy );

        /**
         * Is Blocked
         *          
         * @param aTsyReqHandle Request handle
         * @param aCode Security code type
         * @param aIsBlocked
         * @return Error value
         */
        TInt IsBlocked( const TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSecurityCodeType* aCode, 
            TBool* aIsBlocked );

        /**
         * Check Security Code
         * 
         * @param aTsyReqHandle Request handle
         * @param aCode Security code type
         * @return Error value
         */
        TInt CheckSecurityCodeL( const TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSecurityCodeType* aCode );

        /**
         * Cancel the CheckSecurityCode request
         *          
         * @return KErrNone
         */
        TInt CheckSecurityCodeCancelL();

        /**
         * Disable Phone Lock
         *          
         * @param aTsyReqHandle Request handle
         * @param aPassword Password for verify
         * @return Error value
         */
        TInt DisablePhoneLockL( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TMobilePassword* aPassword );

        /**
         * Gets the currently active PIN (PIN/UPIN) code
         *          
         * @param aTsyReqHandle Request handle
         * @param aActivePin
         * @return KErrNone
         */
        TInt GetActivePin( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TMobilePhoneSecurityCode* aActivePin );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         * @param aReqHandleType
         * @param aTsyReqHandle 
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif // REQHANDLE_TIMER

    private:    // Data

        /**
         * Pointer to CMmCustomTsy class
         * Not Own.                
         */
        CMmCustomTsy* iMmCustomTsy;
        
        /**
         * Pointer to request handle store    
         * Own.         
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;
        
        /**
         *  SIM CustomAPI request handle table         
         */
        TTsyReqHandle iSecReqHandles[ESecurityRequestTypeMaxNumOfRequests];

        /**
         * SIM CustomAPI request handle table         
         */
        TUint8 iTransId;

        /**
         * Saves temporarily last TSY request type        
         */
        TSecurityRequestType iReqHandleType;

        /**
         *  Code: PIN, PIN2 or Security      
         */
        TUint8 iCodeID;

        /**
         * Pointer to client side data    
         * Not Own.            
         */
        TBool* iIsBlocked;

        /**
         * Variable for storing temporarily Security code       
         */
        RMobilePhone::TMobilePassword iSecCode;

        /**
         *  Pointer to CMmPhoneTsy class   
         *  Not Own.             
         */        
        CMmPhoneTsy* iMmPhoneTsy;

        /**
         * Holding the information of the currently active PIN       
         */
        RMobilePhone::TMobilePhoneSecurityCode iActivePin;

    };

#endif      // CMMCUSTOMSECURITYTSY_H
            
// End of File
