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



#ifndef CMMPHONEEXTINTERFACE_H
#define CMMPHONEEXTINTERFACE_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>
#include <mmretrieve.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class TDataPackage;
class CMmDataPackage;

// CLASS DECLARATION

/**
 *  CMmPhoneExtInterface is used to provide an extension interface for system 
 *  specific phone object extensions. 
 */
class CMmPhoneExtInterface
        : public CBase 
    {
    public:

        /**
         * Get static extension mode
         *          
         *
         * @param aMode: pointer to mode
         * @return return value decided by the extension
         */        
        virtual TInt GetStaticExtensionMode( 
            RMobilePhone::TMobilePhoneNetworkMode* aMode ) = 0;

        /**
         * Default implementation of trapped Initialisation method
         *          
         *
         * @return error value
         */
        virtual TInt InitSimL( )= 0;

        /**
         * Default implementation of GetFdnStatusL-method
         *          
         *
         * @return error value
         */
        virtual TInt GetFdnStatusL()= 0;

        /**
         * Default implementation of SetFdnSetting-method
         *          
         *
         * @param TMobilePhoneFdnSetting* aFdnSetting
         * @return error value
         */
        virtual TInt SetFdnSetting(
            RMobilePhone::TMobilePhoneFdnSetting* aFdnSetting)= 0;           

        //NET related methods

        /**
         * Default implementation of GetNITZInfo-method.
         *
         * @param aNwtTimeInfo
         * @return error value
         */ 
        virtual TInt GetNITZInfo( 
            RMobilePhone::TMobilePhoneNITZ* aNwtTimeInfo )= 0;

        /**
         * Update Nitz information.
         *          
         *
         * @param aNITZInfo NITZ info
         */
        virtual void ReceivedNITZInfo(
            RMobilePhone::TMobilePhoneNITZ& aNITZInfo )= 0;

        /**
         * Implementation of GetCurrentNetwork version 1 information
         *          
         *
         * @param aNetInfo network information version 1
         * @param aArea Location area
         * @return error value
         */
        virtual TInt GetCurrentNetworkV1( 
            RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo,
            RMobilePhone::TMobilePhoneLocationAreaV1* aArea )= 0;
         
        /**
         * Implementation of GetCurrentNetwork version 2 information
         *          
         * @param aNetInfo network information version 2
         * @param aArea Location area
         * @return error value
         */
        virtual TInt GetCurrentNetworkV2( 
            RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo,
            RMobilePhone::TMobilePhoneLocationAreaV1* aArea )= 0;     

        /**
         * Implementation of GetCurrentNetwork version 5 information
         *          
         *
         * @param aNetInfo network information version 5
         * @param aArea Location area
         * @return error value
         */
        virtual TInt GetCurrentNetworkV5( 
            RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo,
            RMobilePhone::TMobilePhoneLocationAreaV1* aArea )= 0;
            
        /**
         * Default implementation of GetCurrentNetwork (version 8)
         *          
         *
         * @param aNetInfo network information version 8
         * @param aArea Location area
         * @return error value
         */
        virtual TInt GetCurrentNetwork( 
            RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo,
            RMobilePhone::TMobilePhoneLocationAreaV1* aArea )= 0;
        
        /**
         * Version 1 implementation of GetCurrentNetworkNoLocation
         *
         * @param aNetInfo network information version 1
         * @return error value
         */
        virtual TInt GetCurrentNetworkNoLocationV1( 
            RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo )= 0;
            
        /**
         * Version 2 information implementation of GetCurrentNetworkNoLocation
         *          
         * @param aNetInfo network information version 2
         * @return error value
         */
        virtual TInt GetCurrentNetworkNoLocationV2( 
            RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo )= 0;

        /**
         * Version 5 information implementation of GetCurrentNetworkNoLocation
         *          
         *
         * @param aNetInfo network information version 5
         * @return error value
         */
        virtual TInt GetCurrentNetworkNoLocationV5( 
            RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo )= 0;
            
        /**
         * Default implementation of GetCurrentNetworkNoLocation (version 8)
         *          
         *
         * @param aNetInfo network information version 8
         * @return error value
         */
        virtual TInt GetCurrentNetworkNoLocation( 
            RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo )= 0;            

        /**
         * Default implementation of SetCurrentNetwork
         *          
         * @param aNetInfo network information version 8
         * @param aArea Location area
         * @return error value
         */
        virtual TInt SetCurrentNetwork( 
            const RMobilePhone::TMobilePhoneNetworkInfoV8& aNetInfo,
            const RMobilePhone::TMobilePhoneLocationAreaV1& area )= 0;
            
        /**
         * Get the Cell Information.
         *
         * @return error value
         */ 
        virtual TInt GetCellInfoL()= 0; 

        /**
          * Request notifications of cell information changes.
          *
          * @return error value
          */ 
        virtual TInt NotifyCellInfoChangeL()= 0;
 
        /**
         * Version 1 implementation of get home network information.
         *          
         * @param aNetInfo network information version 1
         * @return error value
         */ 
        virtual TInt GetHomeNetworkV1( 
            RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo )= 0;

        /**
         * Version 2 implementation of get home network information.
         *          
         * @param aNetInfo network information version 2
         * @return error value
         */ 
        virtual TInt GetHomeNetworkV2( 
            RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo )= 0;

        /**
         * Version 5 implementation of get home network information.
         *          
         *
         * @param aNetInfo network information version 5
         * @return error value
         */ 
        virtual TInt GetHomeNetworkV5( 
            RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo )= 0;

		/**
         * Default implementation of get home network information (version 8)
         *          
         *
         * @param aNetInfo network information version 8
         * @return error value
         */ 
        virtual TInt GetHomeNetwork( 
            RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo )= 0;
		
        /**
         * Default implementation of GetNetworkCaps-method.
         *          
         *
         * @param aCaps 
         * @return error value
         */
        virtual TInt GetNetworkCaps( TUint32* aCaps )= 0;

        /**
         * Default implementation of GetSecurityCaps.
         *          
         *
         * @param aCaps 
         * @return error value
         */
        virtual TInt GetSecurityCaps( TUint32* aCaps )= 0;

        /**
         * Default implementation of ChangeSecurityCodeL.
         *          
         *
         * @param aPackage
         * @return error value
         */
        virtual TInt ChangeSecurityCodeL( const TDataPackage& aPackage )= 0;

        /**
         * Sets new setting for a lock
         *          
         *
         * @param aDataPackage 
         * @return error value
         */
        virtual TInt SetLockSettingL( CMmDataPackage* aPackage  )= 0;

        /**
         * Default implementation of VerifySecurityCodeL.
         *          
         *
         * @param aPackage
         * @return error value
         */
        virtual TInt VerifySecurityCodeL( 
            const TDataPackage& aPackage )= 0;

        /**
         * Default implementation of AbortSecurityCodeL.
         *          
         *
         * @param aPackage
         * @return error value
         */
        virtual TInt AbortSecurityCodeL( 
            const TDataPackage& aPackage )= 0;

        /**
         * Default implementation of UpdateSecurityCaps.
         *          
         *
         * @param aEvent 
         * @return error value
         */
        virtual TInt UpdateSecurityCaps(
            RMobilePhone::TMobilePhoneSecurityEvent aEvent )= 0;

        /**
         * Default implementation of DeliverCode.
         *          
         *
         * @param aCodes
         * @return error value
         */
        virtual TInt DeliverCode( 
            RMobilePhone::TCodeAndUnblockCode aCodes )= 0;

        /**
         * Checks the state of the currently active PIN code.
         *          
         *
         * @param None
         */
        virtual TInt SimActivePinState()= 0;

        /**
         * Default implementation of get service provider name
         *          
         *
         * @param None
         * @return error value
         */
        virtual TInt GetServiceProviderNameL()= 0;

        /**
         * Default implementation of GetDetectedNetworksPhase1L
         *          
         *
         * @param None
         * @return status KErrNotSupported
         */
        virtual  TInt GetDetectedNetworksPhase1L()= 0;

        /**
         * Default implementation of SelectNetworkL
         *          
         *
         * @param aDataPackage
         * @return status KErrNotSupported
         */
        virtual  TInt SelectNetworkL( 
            const TDataPackage& aDataPackage )= 0;

        /**
         * Default implementation of SelectNetworkCancelL
         *          
         *
         * @return status KErrNotSupported
         */
        virtual TInt SelectNetworkCancelL()= 0;

        /**
         * Default implementation of GetDetectedNetworksCancelL
         *          
         *
         * @return status KErrNotSupported
         */
        virtual  TInt GetDetectedNetworksCancelL()= 0;

        /**
         * Completes a network registration status changed in this extension
         *          
         *
         * @param  aRegistrationStatus new registration status
         */
        virtual  void CompleteNotifyNetworkRegistrationStatusChange( 
            RMobilePhone::TMobilePhoneRegistrationStatus aRegistrationStatus 
                )= 0;
             
        /**
         * Gets the ICC access capabilites
         *          
         *
         * @param aCaps Pointer to return the capabilites value
         * @return status KErrNotSupported
         */
        virtual TInt GetIccAccessCaps ( TUint32* aCaps )= 0;
         
        /**
         * Sets the network selection setting
         *          
         *
         * @param aDataPackage
         * @return status KErrNotSupported
         */
        virtual TInt SetNetworkSelectionSettingL(
            const TDataPackage& aDataPackage )= 0;

        /**
         * Gets the network registration status
         *          
         *
         * @param aTransactionId transaction Id
         * @return status KErrNotSupported
         */
        virtual TInt GetNetworkRegistrationStatusL()= 0;
        
        /**
         * Copletes GetHomeNetwork request
         *          
         *
         * @param aDataPackage package
         */
        virtual void CompleteGetHomeNetwork( 
            CMmDataPackage* aDataPackage )= 0;
        
        /**
         * Gets network security level
         *          
         *
         * @param aNetworkSecurity
         * @return Error value
         */
        virtual TInt GetNetworkSecurityLevel ( 
            RMobilePhone::TMobilePhoneNetworkSecurity& aNetworkSecurity
                ) const = 0;
        
        /**
         * Sets network security level
         *          
         *
         * @param aNetworkSecurity
         * @return Error value
         */
        virtual TInt SetNetworkSecurityLevel ( 
            RMobilePhone::TMobilePhoneNetworkSecurity& aNetworkSecurity )= 0;
        
        /**
         * Gets ciphering indicator status
         *          
         *
         * @param  aDataPackage
         * @return Error value
         */
        virtual TInt GetCipheringIndicatorStatusL(
            const TDataPackage* aDataPackage )= 0;

        /**
         * Sets a network mode into iCurrentNwInfo.iMode variable
         * 		
         *
         * @param aMode Network Mode
         */
         virtual void SetNetworkModeInExtensionPhone( 
            RMobilePhone::TMobilePhoneNetworkMode aMode ) = 0;

        /**
         * Makes the SIM status request to SIM server
         *          
         *
         * @return request error status
         */
        virtual TInt NotifySimStatusReadyL() = 0;

        /**
         * Makes the MTC status request to SIM server
         *          
         *
         * @return request error status
         */
        virtual TInt MtcStateQueryL()= 0;

        /**
         * Gets the status and setting of a given lock
         *          
         *
         * @param aPackage
         * @return request error status
         */
        virtual TInt GetLockInfoL( const TDataPackage& aPackage ) = 0;

        /**
         * Gets the status and setting of a given lock
         *          
         *
         * @param aStatus
         * @param aSetting
         * @param aLock
         * @return request error status
         */
        virtual TInt UpdateLockInfo( 
            RMobilePhone::TMobilePhoneLockStatus aStatus,
            RMobilePhone::TMobilePhoneLockSetting aSetting,
            RMobilePhone::TMobilePhoneLock aLock ) = 0;

        /**
         * Gets the type of the ICC
         *          
         *
         * @param None
         * @return request error status
         */
        virtual TInt GetICCTypeL() = 0; 
         
        /**
         * Completes a GetICCType request
         *          
         *
         * @param aDataPackage
         * @return request error status
         */
        virtual void CompleteGetICCType ( CMmDataPackage* aDataPackage ) = 0;

        /**
         * Returns the current ICCType
         *          
         * @param None
         * @return error value
         */
        virtual TInt CurrentICCType() = 0;
        
};

#endif // CMMPHONEEXTINTERFACE_H

//	End of file
