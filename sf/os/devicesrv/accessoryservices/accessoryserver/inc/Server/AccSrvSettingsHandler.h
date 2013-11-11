/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control for accessory related settings.
*
*/


#ifndef CACCSRVSETTINGSHANDLER_H
#define CACCSRVSETTINGSHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <AccModeSettingArray.h>
#include "AccSrvServerModel.h"
#include "AccPolAccessoryPolicy.h"

// CONSTANTS
_LIT_SECURITY_POLICY_C1(KNoCapability, ECapability_None);
_LIT_SECURITY_POLICY_C1(KWriteDeviceDataPolicy, ECapabilityWriteDeviceData);
    
// MACROS

// DATA TYPES
enum TAccSettingsLights
    {
    EAccSettingsLightsNotSet,
    EAccSettingsLightsOff,
    EAccSettingsLightsOn
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvConnectionController;
class CHWRMLight;

// CLASS DECLARATION

/**
*  Controller for accessory settings functionality.
*
*  @lib AccServer
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSettingsHandler ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvSettingsHandler* NewL( CAccSrvConnectionController* aConCtrl,
                                             CAccSrvServerModel& aModel,
                                             CAccPolAccessoryPolicy* aPolicy );

        /**
        * Destructor.
        */
        virtual ~CAccSrvSettingsHandler();

    public: // New functions

        /**
        * Should be called to indicate settings handler that connection status
        * has changed.
        *
        * @since S60 3.1
        * @param aArray list of ids representing current connection status
        * @return void
        */
        void ConnectionStatusChangedL( TAccPolGenericIDArray& aArray );

        /**
        * Returns all supported HW devices that can't be properly identified
        * when they are connected to phone without additional information.
        *
        * @since S60 3.1
        * @param void
        * @return supported HW devices in a bit mask
        */
        TUint32 GetSupportedHWDevicesL() const;

        /**
        * Sets or removes HW devices. Used to identify HW device properly when
        * it's connected to phone.
        *
        * @since S60 3.1
        * @param    aSettings   Bit mask storing one or several HW devices.
        *             See AccessoryTypes.h for HW device bit mask
        *             definitions.
        * @param    aEnable   ETrue: sets HW device. EFalse: removes HW device.
        * @param    aForce    Reserved for future use.
        * @return     void
        * @exception  KErrArgument if an argument was invalid or system-wide
        *       error code if failed.
        */
        void SetHWDeviceSettingsL( TUint32 aSettings,
                                   TBool aEnable,
                                   TBool aForce ) const;

        /**
        * Returns current HW devices.
        *
        * @since S60 3.1
        * @param void
        * @return HW devices in a bit mask.
        */
        TUint32 GetHWDeviceSettings() const;

        /**
        * Sets one setting for specified accessory mode.
        *
        * @since S60 3.1
        * @param aAccMode   Defines an accessory mode.
        * @param aSetting   Defines setting and it's value.
        * @return void
        * @exception system-wide error code if failed.
        */
        void SetAccessoryModeSettingL( const TAccMode aAccMode,
                                       const TAccModeSetting& aSetting) const;

        /**
        * Returns one setting for specified accessory mode.
        *
        * @since S60 3.1
        * @param  aAccMode  Defines an accessory mode.
        * @param  aSetting  Defines setting and it's value.
        * @return void
        * @exception system-wide error code if failed.
        */
        void GetAccessoryModeSettingL( const TAccMode aAccMode,
                                       TAccModeSetting& aSetting ) const;

        /**
        * Sets several settings for specified accessory mode.
        *
        * @since S60 3.1
        * @param aAccMode   Defines an accessory mode.
        * @param aArray   Defines an array of several setting and value
        *         pairs.
        * @return void
        * @exception system-wide error code if failed.
        */
        void SetAccessoryModeSettingsL( const TAccMode aAccMode,
                                        const TAccModeSettingArray& aArray ) const;

        /**
        * Returns all settings for specified accessory mode.
        *
        * @since S60 3.1
        * @param aAccMode   Defines accessory mode.
        * @param aArray   Defines an array of several setting and value
        *         pairs.
        *
        * @return void
        * @exception system-wide error code if failed.
        */
        void GetAccessoryModeSettingsL( const TAccMode aAccMode,
                                        TAccModeSettingArray& aArray ) const;

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccSrvSettingsHandler( CAccSrvConnectionController* aConCtrl,
                                CAccSrvServerModel& aModel,
                                CAccPolAccessoryPolicy* aPolicy );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Returns new settings value. Leaves, if new settings contradict with
        * policy.
        */
        TUint32 ResolveNewSettingsL( TUint32 aSettings,
                                     TBool aEnable,
                                     TBool aForce ) const;

        /**
        * Checks whether vibra can be allowed or not. As a result aVibra will
        * be modified: aVibra = ETrue   -> vibra is allowed,
        *              aVibra = EFalse  -> vibra isn't allowed
        */
        void GetVibraAllowed( TAccPolGenericID& aId,
                               TBool& aVibra ) const;

        /**
        * Checks whether lights are forced on or not. As a result aLights will
        * be modified: aLights = ETrue   -> lights should be put on,
        *              aLights = EFalse  -> lights shouldn't be put on
        */
        void GetLightsAllowedL( TAccPolGenericID& aId,
                                TBool& aLights ) const;

        /**
        * Creates CR key dynamically.
        */
        TUint32 CreateCRKeyL( const TAccMode aAccMode, const TInt aId ) const;

        /**
        * Converts setting from client format to CR format.
        */
        HBufC* CodeSettingL( const TAccModeSetting& aSetting ) const;

        /**
        * Converts setting from CR format to format readable by client.
        */
        void DecodeSettingL( const TDesC& aIn, TAccModeSetting& aOut ) const;

        /**
        * Returns ETrue, if setting is known.
        */
        TBool StaticKey( const TInt id ) const;

        /**
        * Returns all static keys for accessory mode.
        */
        void GetStaticKeysL( const TAccMode aAccMode,
                   RArray< TUint32 >& aArray ) const;

    public:     // Data

    protected:  // Data

    private:    // Data
        // Controls connection handling
        CAccSrvConnectionController* iConCtrl;

        // Model that stores server status when server is running
        CAccSrvServerModel& iModel;

        // Lights API services
        CHWRMLight* iLightsApi;

        // Tells wheter lights set on or off by accessory server.
        TAccSettingsLights iLightsOn;

        //Handle to the Accessory Policy
        CAccPolAccessoryPolicy*         iPolicy;//Not owned


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // CACCSRVSETTINGSHANDLER_H

// End of File
