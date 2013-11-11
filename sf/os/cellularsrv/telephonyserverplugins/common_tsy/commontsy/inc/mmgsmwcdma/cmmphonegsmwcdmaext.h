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



#ifndef CMMPHONEGSMWCDMAEXT_H
#define CMMPHONEGSMWCDMAEXT_H

//  INCLUDES
#include "cmmphoneextinterface.h"
#include "CMmPrivateUtility.h"
#include "mmgsmwcdmaprivateutils.h"

//  FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmMessageManagerBase;
class CMmDataPackage;

// CLASS DECLARATION

/**
*  CMmPhoneGsmWcdmaExt contains mode-dependent phone functionality
*  for GSM phone.
*  Extension request handles are stored in MM Phone object.
*/
NONSHARABLE_CLASS( CMmPhoneGsmWcdmaExt ) : public CMmPhoneExtInterface
    {

public:
     /*
     * Destructor
     */
    ~CMmPhoneGsmWcdmaExt();

    /**
     * Creates a new CMmPhoneGsmWcdmaExt object instance.
     *    
     *     
     * @param aMmPhoneTsy Phone TSY
     * @param aMessageManager Message manager object
     * @return gsm extension phone object
     */
    static CMmPhoneGsmWcdmaExt* NewL( 
        CMmPhoneTsy* aMmPhoneTsy, 
        CMmMessageManagerBase* aMessageManager );

    /**
     * Initializes the CMmPhoneGsmWcdmaExt object instance
     *   
     *      
     * @return error value
     */
    TInt InitSimL( void );

    /**
     * Default implementation of GetNITZInfo-method.
     *     
     *    
     * @param aNwtTimeInfo Pointer to time and date information
     * @return error value
     */ 
    TInt GetNITZInfo( 
        RMobilePhone::TMobilePhoneNITZ* aNwtTimeInfo );

    /**
     * Default implementation of GetCellInfo-method.
     *     
     *    
     * @return error value
     */ 
    virtual TInt GetCellInfoL(); 

    /**
     * Default implementation of NotifyCellInfoChange-method.
     *     
     *    
     * @return error value
     */ 
    virtual TInt NotifyCellInfoChangeL();
    
    /**
     * Get Network capabilities.
     *     
     *    
     * @param aCaps Pointer to client side NW caps
     * @return KErrNone
     */
    TInt GetNetworkCaps( TUint32* aCaps );

    /**
     * Get current Network, information version 1 implementation.
     *     
     *    
     * @param aNetInfo network information version 1
     * @param aArea location area
     * @return KErrNone
     */
    TInt GetCurrentNetworkV1( 
        RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea );
        
    /**
     * Get current Network, information version 2 implementation.
     *     
     * @param aNetInfo network information version 2
     * @param aArea location area
     * @return KErrNone
     */
    TInt GetCurrentNetworkV2( 
        RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea );
        
    /**
     * Get current Network, information version 5 implementation.
     *     
     *    
     * @param aNetInfo network information version 5
     * @param aArea location area
     * @return KErrNone
     */
    TInt GetCurrentNetworkV5( 
        RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea );
   
     /**
     * Get current Network, information version 8 implementation.
     *     
     *    
     * @param aNetInfo network information version 8
     * @param aArea location area
     * @return KErrNone
     */
    TInt GetCurrentNetwork( 
        RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea );
        
    /**
     * Get current Network (V1), but not the location area.
     *     
     *    
     * @param aNetInfo network information version 1
     * @return KErrNone
     */
    TInt GetCurrentNetworkNoLocationV1( 
        RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo ); 

    /**
     * Get current Network (V2), but not the location area.
     *       
     * @param aNetInfo network information version 2
     * @return KErrNone
     */
    TInt GetCurrentNetworkNoLocationV2( 
        RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo );
        
    /**
     * Get current Network (V5), but not the location area.
     *     
     *    
     * @param aNetInfo network information version 5
     * @return KErrNone
     */
    TInt GetCurrentNetworkNoLocationV5( 
        RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo );   
        
    /**
     * Get current Network (V8), but not the location area.
     *     
     *    
     * @param aNetInfo network information version 8
     * @return KErrNone
     */
    TInt GetCurrentNetworkNoLocation( 
        RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo );
        
    /**
     * Set current Network.
     *     
     *    
     * @param aNetInfo network information version 8
     * @param aArea location area
     * @return KErrNone
     */
    TInt SetCurrentNetwork( 
        const RMobilePhone::TMobilePhoneNetworkInfoV8& aNetInfo,    
        const RMobilePhone::TMobilePhoneLocationAreaV1& area );
        
    /**
     * Get home Network, version 1 implementation.
     *     
     *    
     * @param aNetInfo network information version 1
     * @return KErrNone or KErrNotFound
     */
    TInt GetHomeNetworkV1(
        RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo );

    /**
     * Get home Network, version 2 implementation.
     *     
     *    
     * @param aNetInfo network information version 2
     * @return KErrNone or KErrNotFound
     */
    TInt GetHomeNetworkV2(
        RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo );
        
    /**
     * Get home Network.
     *     
     *    
     * @param aNetInfo network information version 5
     * @return KErrNone or KErrNotFound
     */
    TInt GetHomeNetworkV5(
        RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo ); 

	/**
     * Get home Network.
     *     
     *    
     * @param aNetInfo network information version 8
     * @return KErrNone or KErrNotFound
     */
    TInt GetHomeNetwork(
        RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo ); 
        
    /**
     * Update Nitz information.
     *     
     *    
     * @param aNITZInfo NITZ info
     */
    void ReceivedNITZInfo( 
        RMobilePhone::TMobilePhoneNITZ& aNITZInfo );

    /**
     * Get fixed dialling number service status
     *     
     *    
     * @param aStatus service status
     * @return error value
     */
    TInt GetFdnStatusL( void );

    /**
     * Set fixed dialling number service setting
     *     
     *    
     * @param aFdnSetting service setting
     * @return error value
     */
    TInt SetFdnSetting(
        RMobilePhone::TMobilePhoneFdnSetting* aFdnSetting );

    /**
     * Receive sim pn list information 
     *     
     *    
     * @param aStatus
     * @param aSSTFileData, pointer to sst file data
     * @return error value
     */
    TInt ReceivedSimPnListStatusResp( TUint8 aStatus, 
        SSSTFileData* aSSTFileData );

    /**
     * Receive sim table read information 
     * 
     *        
     * @param aStatus
     * @param AdnExtensionActive, boolean to tell if adn is active
     * @return error value
     */
    TInt ReceivedSimReadTableResp( TUint8 aStatus,
        TBool aAdnExtensionActive );

    /**
     * Get service provider name
     *   
     *
     * @return error value
     */
    TInt GetServiceProviderNameL( void );

    /**
     * Checks the state of the currently active PIN code.
     * 
     *        
     * @return KErrNone
     */
    TInt SimActivePinState( void );

    /**
     * Get ICC Type
     * 
     *        
     * @return status
     */
    TInt GetICCTypeL( void );

    /**
     * Set Complete Get ICC Type 
     * 
     *        
     * @param aDataPackage
     */
    void CompleteGetICCType( CMmDataPackage* aDataPackage );

    /*
     * Notify sim status ready
     * 
     *          
     * @return request error status
     */
    TInt NotifySimStatusReadyL( void );

    /*
     * Makes the MTC status request to adaptation
     * 
     *     
     * @return request error status
     */
    TInt MtcStateQueryL( void );

    /**
     * Updates security capabilities.
     * 
     *          
     * @param aEvent
     * @Return KErrNone
     */
    TInt UpdateSecurityCaps( 
        RMobilePhone::TMobilePhoneSecurityEvent aEvent );

    /**
     * Updates lock info
     * 
     *         
     * @param  aStatus
     * @param  aSetting
     * @param  aLock
     * @Return KErrNone
     */
    TInt UpdateLockInfo( 
        RMobilePhone::TMobilePhoneLockStatus aStatus,
        RMobilePhone::TMobilePhoneLockSetting aSetting,
        RMobilePhone::TMobilePhoneLock aLock );

    /**
     * Delivers security capabilities.
     * 
     *          
     * @param aCaps
     * @Return KErrNone
     */
    TInt GetSecurityCaps( TUint32* aCaps );

    /**
     * Changes the state of one of the security codes
     * 
     *          
     * @param aPackage
     * @Return KErrNone or error value
     */
    TInt ChangeSecurityCodeL( const TDataPackage& aPackage );

    /**
     * Sets new setting for a lock
     * 
     *          
     * @param aPackage 
     * @return KErrNone
     */
    TInt SetLockSettingL( CMmDataPackage* aPackage );

    /**
     * Handles Security Code verify
     * 
     *          
     * @param aPackage
     * @Return KErrNone or error value
     */
    TInt VerifySecurityCodeL( const TDataPackage& aPackage );

    /**
     * Handles SecurityCode abort
     *     
     *     
     * @param   aPackage
     * @param Return KErrNone or error value
     */
    TInt AbortSecurityCodeL( 
        const TDataPackage& aPackage );
    /**
     * Delivers password to extension for lock setting change
     *     
     *        
     * @param aCodes
     * @Return KErrNone
     */
    TInt DeliverCode( 
        RMobilePhone::TCodeAndUnblockCode aCodes );
    
    /**
     * Return extension mode
     * 
     *           
     * @param aMode, extension mode
     * @Return error value
     */
    TInt GetStaticExtensionMode( 
        RMobilePhone::TMobilePhoneNetworkMode* aMode );

    /**
     * Sets a network mode
     *     
     *        
     * @param aMode, Network Mode
     */
    void SetNetworkModeInExtensionPhone( 
        RMobilePhone::TMobilePhoneNetworkMode aMode );

    /**
     * Gets the list of detected networks
     * 
     *            
     * @return KErrNone or error code
     */
    TInt GetDetectedNetworksPhase1L( );

    /**
     * Selects a network
     * 
     *   
     * @param aDataPackage  
     * @return status: KErrNone or error code
     */
    TInt SelectNetworkL( const TDataPackage& aDataPackage );

    /**
     * Cancels an ongoing network selection
     * 
     *           
     * @return status: KErrNone or error code
     */
    TInt SelectNetworkCancelL( void );

    /**
     * Cancels an ongoing network search
     * 
     *           
     * @return status: KErrNone or error code
     */
    TInt GetDetectedNetworksCancelL( void );

    /**
     * Completes a network registration status changed in this extension
     * 
     *           
     * @param  aRegistrationStatus: new registration status
     */
    void CompleteNotifyNetworkRegistrationStatusChange
        ( RMobilePhone::TMobilePhoneRegistrationStatus aRegistrationStatus );

    /**
     * Gets the ICC access capabilites
     * 
     *             
     * @param aCaps, Pointer to return the capabilites value
     * @return KErrNone
     */
	TInt GetIccAccessCaps ( TUint32* aCaps );

    /**
     * Sets the network selection setting
     * 
     *         
     * @param setting
     * @return status: KErrNone or error code
     */
    TInt SetNetworkSelectionSettingL( const TDataPackage& setting );

    /**
     * Gets the network registration status
     * 
     *        
     * @return status: KErrNone or error code
     */
    TInt GetNetworkRegistrationStatusL( void );

    /**
     * Gets the network registration status
     * 
     *        
     * @param aDataPackage
     */    
    void CompleteGetHomeNetwork( CMmDataPackage* aDataPackage );
    
    /**
     * Gets cached network's security level (encription level)
     * 
     *     
     * @param  aNetworkSecurity
     * @return status: KErrNone or error code
     */
    TInt GetNetworkSecurityLevel ( 
            RMobilePhone::TMobilePhoneNetworkSecurity& aNetworkSecurity ) const;
            
    /**
     * Sets cached network's security level (encription level)
     * 
     *        
     * @param  aNetworkSecurity
     * @return status: KErrNone or error code
     */
    TInt SetNetworkSecurityLevel ( 
            RMobilePhone::TMobilePhoneNetworkSecurity& aNetworkSecurity );
            
    /**
     * Gets the status of the ciphering indicator
     *     
     *     
     * @param const TDataPackage* aDataPackage
     * @return status: KErrNone or error code
     */
    TInt GetCipheringIndicatorStatusL( const TDataPackage* aDataPackage );

    /*
     * Returns the current ICCType
     * 
     *       
     * @return status: KErrNone or error code        
     */
    TInt CurrentICCType( void );

    /*
     * Gets the status and setting of a given lock
     * 
     *           
     * @param aPackage
     * @return request error status
     */
    TInt GetLockInfoL( const TDataPackage& aPackage );
    
private: // Constructors

    /*
     * Constructor
     */
    CMmPhoneGsmWcdmaExt( void );

    /**
     * Class attributes are created in ConstructL
     */   
    void ConstructL( void );
    
public: 

    /**  TICCType enumerates the SIM card types */
    enum TICCType
        {
        EICCTypeSim2GGsm,
        EICCTypeSim3G,
        EICCTypeSimUnknown
        };

    /**   TRequestedConditions */
    struct TRequestedConditions
        {
        TUint8 iTransactionId;
        CArrayFixFlat<TUint16>* iConditionArray;
        };
        
    /** TSetSSCallServiceStatusData  */
    struct TSetSSCallServiceStatusData
        {
        TUint8 iTransactionId;
        TUint8 iOperation;
        TUint16 iMmiSsCode;
        TUint8 iTypeOfNumber;
        TInt iNoReplyTime;
        TDesC16* iData;
        TUint8 iMmiBasicServiceCode;
        };

private: // Data

    /**
     * Pointer to the mode independent Phone object
     * Not own.      
     */
    CMmPhoneTsy* iMmPhoneTsy;

    /**
     * Keeps track of pending service type requests for GetServiceTable
     */
    TUint8 iServiceTypesPending;

    /**
     * Indicates whether the whole service table is requested or just
     * one specific type info     
     */
    TBool iCompleteSTRequested;

    /**
     * pointer to the message manager
     * Not own.      
     */
    CMmMessageManagerBase* iMessageManager;

    /**
     * Keeps track of current SIM card type
     */
    TICCType iCurrentICCType;

    /**
     * Home network
     * Not own.      
     */
    RMobilePhone::TMobilePhoneNetworkInfoV8* iGsmHomeNetwork;  

    /**
     * Current network
     * Not own.      
     */
    RMobilePhone::TMobilePhoneNetworkInfoV8* iCurrentNwInfo; 

    /**
     * Current location id
     * Not own.      
     */
    RMobilePhone::TMobilePhoneLocationAreaV1* iCurrentLocationArea;

    /**
     * Fdn status is fetched
     */    
    TBool iFdnStatusFetched;
    
    /**
     * Adn is enabled
     */ 
    TBool iAdnEnabled;
    
    /**
     *  Adn status is fetched
     */    
    TBool iAdnStatusFetched;

    /**
     *  Empty buffer
     */
    TPtrC16 iEmptyBuffer;

    /**
     *  Nitz info
     */
    RMobilePhone::TMobilePhoneNITZ iNitzInfo;

    /**
     * All Call Services SupportedA?
     */    
    TBool iCallServicesSupportedAll;

    /**
     * call forwarding conditionally or unconditionally active
     */    
    RMobilePhone::TMobilePhoneCFActive iCallForwardingActiveType;
    
    /**
     * Call forwarding active service type
     */        
    RMobilePhone::TMobileService iCallForwardingActiveService;

    /**
     * Stored security capabilities
     */    
    TUint32 iSecurityCaps;

    /**
     * Variable for storing temporarily either PIN1 or Security code
     */    
    RMobilePhone::TMobilePassword iSecCode;

    /**
     * Status of security lock
     */    
    RMobilePhone::TMobilePhoneLockStatus iPhoneDeviceStatus;
    
    /**
     * Setting of security lock
     */    
    RMobilePhone::TMobilePhoneLockSetting iPhoneDeviceSetting;

    /**
     * Status of PIN1 lock
     */    
    RMobilePhone::TMobilePhoneLockStatus iICCStatus;
    
    /**
     * Setting of PIN1 lock
     */    
    RMobilePhone::TMobilePhoneLockSetting iICCSetting;

    /**
     * Status of security lock during the next start up
     */    
    RMobilePhone::TMobilePhoneLockStatus iPhoneToICCStatus;
    
    /**
     * Setting of security lock during the next start up
     */        
    RMobilePhone::TMobilePhoneLockSetting iPhoneToICCSetting;

    /**
     * Flag for home network info availability
     */
    TBool iIsHomeNetworkInfoAvailable;

    /**
     * Status of the network security (ciphering)
     */
    RMobilePhone::TMobilePhoneNetworkSecurity iNetworkSecurity;

    };

#endif // CMMPHONEGSMWCDMAEXT_H

// End of File
