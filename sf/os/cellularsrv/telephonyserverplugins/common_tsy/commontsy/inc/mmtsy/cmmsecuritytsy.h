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



#ifndef CMMSECURITYTSY_H
#define CMMSECURITYTSY_H

//  INCLUDES
#include <e32base.h>
#include <et_tsy.h>
#include <etelmm.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmDataPackage;

// CLASS DECLARATION

/**
 *  CMmPhoneTsy contains mode-independent phone functionality.
 *  Extension request handles and parameters are stored as attributes.
 */
NONSHARABLE_CLASS( CMmSecurityTsy ) : public CBase
    {
    private:
        
        /**
         * Struct to hold GetSecurityCodeInfo requests data.
         */
        struct TGetSecurityCodeInfoRequest
            {
            /** ETel request handle */
            TTsyReqHandle iReqHandle;
            /** Security code type */
            RMobilePhone::TMobilePhoneSecurityCode iSecurityCode;
            /** Pointer to client-side data */
            TDes8* iSecurityCodeInfo;
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * return CMmPhoneTsy*: created phone object 
         */
        static CMmSecurityTsy* NewL( CMmPhoneTsy* aPhoneTsy );

        /**
         * Destructor.
         */
        virtual ~CMmSecurityTsy();

        // New functions

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning on 
         * memory allocation failure
         *          
         *
         * @param aTsyReqHandle TSY request handle
         * @param aIpc request IPC number
         * @param aPackage packaged request parameters
         * @return result of the request
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Cancels request that's IPC number and request handle are given in
         * parameters
         *          
         *
         * @param aIpc request IPC number
         * @param aTsyReqHandle TSY request handle
         * @return result of the request
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Completes security capabilities change notification
         *          
         *
         * @param aDataPackage
         * @return KErrNone
         */
        virtual TInt CompleteNotifySecurityCapsChange( TUint32 aCaps );

        /**
         * Notifies client about security event
         *          
         *
         * @param aEvent security event
         * @param aErrorCode error/success code
         */
        virtual void CompleteNotifySecurityEventL( 
            RMobilePhone::TMobilePhoneSecurityEvent aEvent, 
            TInt aErrorCode );

        /**
         * Completes lock state setting
         *          
         *
         * @param aErrorCode: error/success code
         * @param  aStatus current lock status
         * @param aSetting current lock setting
         *          
         */
        virtual void CompleteSetLockSetting( TInt aErrorCode, 
            RMobilePhone::TMobilePhoneLockStatus aStatus,
            RMobilePhone::TMobilePhoneLockSetting aSetting );

        /**
         * Completes security code change
         *
         * @param aErrorCode
         * @return KErrNone
         */
        virtual TInt CompleteChangeSecurityCode( TInt aErrorCode );

        /**
         * Completes security code verification
         *          
         *
         * @param aErrorCode
         */
        virtual void CompleteVerifySecurityCodeL( TInt aErrorCode );

        /**
         * Completes security code verification aborting
         *          
         *
         * @param aErrorCode
         */
        virtual void CompleteAbortSecurityCode( TInt aErrorCode );

        /**
         * Completes lock information notification
         *          
         *
         * @param aDataPackage
         * @return KErrNone
         */
        virtual void CompleteNotifyLockInfoChange(
            CMmDataPackage* aDataPackage, TInt aErrorCode );

        /**
         * Sets iPin1DisableSupported flag to EFalse, no support for PIN 
         * disable
         *          
         *         
         */
        virtual void SetPin1DisableNotSupported();

        /**
         * Complete get lock info request
         *          
         *
         * @param aDataPackage
         */
        virtual void CompleteGetLockInfo( CMmDataPackage* aDataPackage,
            TInt aErrorCode );

        /**
         * Complete GetSecurityCodeInfo request
         *          
         * @param aDataPackage
         * @param aErrorCode
         */
        virtual void CompleteGetSecurityCodeInfo( 
            CMmDataPackage* aDataPackage, TInt aErrorCode );

        /**
         * Complete NotifySecurityCodeInfoChange notification
         *          
         * @param aDataPackage
         * @param aErrorCode
         */
        virtual void CompleteNotifySecurityCodeInfoChange( 
            CMmDataPackage* aDataPackage, TInt aErrorCode );

    private:
    
        /**
         * C++ default constructor.
         */
        CMmSecurityTsy(void);

        /**
         * Class attributes are created in ConstructL
         */
        void ConstructL( void );

        /**
         * Delivers current security capabilies
         *          
         *
         * @param aTsyReqHandle
         * @param aCaps
         * @return KErrNone
         */
        virtual TInt GetSecurityCaps( const TTsyReqHandle aTsyReqHandle,
            TUint32* aCaps );
        
        /**
         * Sets TSY to notify if security capabilities change
         *          
         *
         * @param aCaps
         * @return KErrNone
         */
        virtual TInt NotifySecurityCapsChange( TUint32* aCaps );

        /**
         * Cancels notification request about security capabilities change
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNone
         */
        virtual TInt NotifySecurityCapsChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Delivers current lock information
         *          
         *
         * @param aTsyReqHandle
         * @param aPackage Parameters where lock information is stored
         * @return KErrNone or KErrArgument
         */
        virtual TInt GetLockInfoL( const TTsyReqHandle aTsyReqHandle,
                const TDataPackage& aPackage );

        /**
         * Sets TSY notify if lock information changes
         *          
         *
         * @param aLock
         * @param aLockInfo
         * @return KErrNone
         */
        virtual TInt NotifyLockInfoChange(
            RMobilePhone::TMobilePhoneLock* aLock,
            TDes8* aLockInfo );

        /**
         * Cancel notification request about lock information change
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNone
         */
        virtual TInt NotifyLockInfoChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Checks if setting a new setting for a lock is possible
         *          
         * 
         * @param aTsyReqHandle
         * @param aPackage
         * @return KErrNone
         */
        virtual TInt SetLockSettingL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
         * Cancel the request about lock setting, can be cancelled if no ISI
         * sending done yet.
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNone
         */
        virtual TInt SetLockSettingCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Cancel the request about lock setting, can be cancelled if no ISI
         * sending done yet.
         *          
         *
         * @param aTsyReqHandle
         * @return KErrNone
         */
        virtual TInt GetLockInfoCancel ( const  TTsyReqHandle aTsyReqHandle );

        /**
         * Sets new setting for a lock
         *          
         *
         * @param aPackage
         * @return KErrNone
         */
        virtual TInt LockSettingL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
         * Replaces old security code with new one
         *          
         *
         * @param aType
         * @param aChange
         * @return Error value
         */
        virtual TInt ChangeSecurityCodeL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
         * Sets TSY notify if security event appears
         *          
         *
         * @param aEven
         * @return KErrNone
         */
        virtual TInt NotifySecurityEventL( 
            RMobilePhone::TMobilePhoneSecurityEvent* aEvent );

        /**
         * Cancel notification request about lock information change
         *          
         *
         * @const aTsyReqHandle
         * @return KErrNone
         */
        virtual TInt NotifySecurityEventCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Verifies current security code
         *          
         *
         * @param aTsyReqHandle 
         * @param aType
         * @param aCodes
         * @return KErrNone
         */
        virtual TInt VerifySecurityCodeL( const TTsyReqHandle aTsyReqHandle, 
            const TDataPackage& aPackage );
        
        /**
         * Aborts current security code verification query
         *          
         *
         * @param aTsyReqHandle
         * @param aPackage
         * @return KErrNone
         */
        virtual TInt AbortSecurityCodeL( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
         * This method retrieves the current number of remaining entry 
         * attemps of security code.
         *          
         * @param aTsyReqHandle
         * @param aSecurityCode
         * @param aSecurityCodeInfo
         * @return Error value
         */
        virtual TInt GetSecurityCodeInfoL( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode, 
            TDes8* aSecurityCodeInfo );

        /**
         * Cancel GetSecurityCodeInfo request
         *          
         * @param aTsyReqHandle
         * @return Error value
         */
        virtual TInt GetSecurityCodeInfoCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Notifies security code info changes
         *          
         * @param aSecurityCode
         * @param aSecurityCodeInfo
         * @return Error value
         */
        virtual TInt NotifySecurityCodeInfoChange( 
            RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode, 
            TDes8* aSecurityCodeInfo );

        /**
         * Cancel notification about security code info changes
         *          
         * @param aTsyReqHandle
         * @return Error value
         */
        virtual TInt NotifySecurityCodeInfoChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * It resets the (GetSecurityCodeInfo) request handle.
         *          
         * @param aSecurityCode Security Code.
         */
        virtual void ResetGetSecurityCodeInfoTsyReqHandle( 
            const RMobilePhone::TMobilePhoneSecurityCode aSecurityCode );

        /**
         * Checks that security codes are well formed.
         * 
         *@param aCodeType The security code to check, e.g. PIN, PUK, etc.
         *@param aCodes The code that needs to be checked.		 		          
         */
        TBool AreCodesWellFormed(
            const RMobilePhone::TMobilePhoneSecurityCode& aCodeType,
            const RMobilePhone::TCodeAndUnblockCode& aCodes ) const;
            
    private:    // Data

        /**
         * Pointer to phone tsy
         */         
        CMmPhoneTsy* iMmPhoneTsy;

        /**
         * get lock info
         */         
        TDes8* iRetGetLockInfo;

        /**
         * notify phone lock change
         */         
        RMobilePhone::TMobilePhoneLock* iRetNotifyPhoneLockChange;

        /**
         * notify phone lock info change
         */         
        TDes8* iRetNotifyLockInfoChange;

        /**
         * Set Lock Setting phone lock
         */         
        RMobilePhone::TMobilePhoneLock iLockSettingPhoneLock;
        
        /**
         * Set Lock Setting
         */         
        RMobilePhone::TMobilePhoneLockSetting iLockSetting; 

        /**
         * notify security event
         */         
        RMobilePhone::TMobilePhoneSecurityEvent* iRetNotifySecurityEvent;

        /**
         * notify security capabilities change
         */         
        TUint32* iRetNotifySecurityCapsChange;

        /**
         * Lock type
         */         
        RMobilePhone::TMobilePhoneSecurityCode iLockType;

        /**
         * Is PIN1 disable supported
         */         
        TBool iPin1DisableSupported;

        /**
         * Is Puk code verify
         */         
        TBool iPukCodeVerify;

        /**
         * Is Phone password verify
         */         
        TBool iPhonePasswordVerify;

        /**
         * Is security codes checked for client after boot
         */         
        TBool iSecurityCheckedForBoot;

        /**
         * member enum for storing the last requested pin
         */         
        enum TLastPinRequested
        	{
        	EPinUnknown,
        	EPin1Requested,
        	EPin2Requested
        	};
        
        /** 
         * State while changing active code from PIN to UPIN
         */         
        TLastPinRequested iLastPinRequested;
        
        /**
         * ETrue if requested through TSY and not yet completed to client.
         */         
        TBool iIsSecurityCodeRequestCachedInBoot;

        /** These states are used when changing active code 
            from PIN to UPIN */        
        enum TActiveCodeToUpinState
	        {
	        EActiveCodeToUpinIdle,
	        EActiveCodeToUpinAskUpin,        
	        EActiveCodeToUpinAskPin,             
            };

        /** 
         * State while changing active code from PIN to UPIN
         */         
        TActiveCodeToUpinState iActiveCodeToUpinState;

        /**
         * Security code info. Pointer to client side data.
         * Not Own.
         */
        TDes8* iRetSecurityCodeInfo;

        /**
         * Security code. Pointer to client side data.
         * Not Own.
         */
        RMobilePhone::TMobilePhoneSecurityCode* iRetNotifySecurityCode;

        /**
         * Security code info. Pointer to client side data.
         * Not Own.
         */
        TDes8* iRetNotifySecurityCodeInfo;

        /**
	     * Array for buffering Get Security Code Info requests
	     */		     
	    RPointerArray< TGetSecurityCodeInfoRequest > iGetSecurityCodeInfoRequests;

    };
#endif // CMMSECURITYTSY_H

// End of File
