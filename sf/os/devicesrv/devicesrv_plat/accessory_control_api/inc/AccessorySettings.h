/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: ?Description
*
*/



#ifndef RACCESSORYSETTINGS_H
#define RACCESSORYSETTINGS_H

//  INCLUDES
#include <e32std.h>
#include <AccPolAccessoryMode.h>
#include <AccModeSetting.h>
#include <AccModeSettingArray.h>
#include "AccessorySubsessionBase.h"
#include "AccessoryServer.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Accessory Settings sub-session.
*  Accessory Settings provides methods for changing and checking
*  status of accessory related settings.
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessorySettings : public RAccessorySubsessionBase
    {

    public:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessorySettings();

    public:

        /**
        * Creates a new sub-session within an existing session.
        *
        * @since S60 3.1
        * @param    aSession    The session to which this sub-session will belong.
        * @return   KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt CreateSubSession(RAccessoryServer& aSession);

        /**
        * Closes the RAccessorySettings sub-session.
        *
        * @since S60 3.1
        * @return   KErrNone if successful, system-wide error code if failed.
        * @see CreateSubSession()
        */
        IMPORT_C TInt CloseSubSession();

        /**
        * Returns HW devices that can't be properly identified when they are
        * connected to phone without additional information.
        *
        * @since S60 3.1
        * @param    aSettings   Bit mask storing one or several HW devices. See
        *                       AccessoryTypes.h for HW device bit mask
        *                       definitions.
        * @return   KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt GetSupportedHWDeviceSettings( TUint32& aSettings );

        /**
        * Sets or removes HW devices. Used to identify HW device properly when
        * it's connected to phone.
        *
        * @since S60 3.1
        * @param    aSettings   Bit mask storing one or several HW devices.
        *                       At the moment only one setting is supported.
        *                       New setting overrides possible current setting.
        *                       See AccessoryTypes.h for HW device bit mask
        *                       definitions.
        * @param    aEnable     ETrue: sets HW device. EFalse: removes HW device.
        * @param    aForce      Reserved for future use.
        * @return   KErrNone if successful, KErrArgument if an argument was invalid.
        * @exception system-wide error code if failed.
        */
        IMPORT_C TInt SetHWDeviceSettingsL( TUint32 aSettings,
                                            TBool aEnable = ETrue,
                                            TBool aForce = EFalse );
        /**
        * Returns current HW devices.
        *
        * @since S60 3.1
        * @param    aSettings   Bit mask storing one or several HW devices.
        *                       At the moment only one setting is supported.
        *                       See AccessoryTypes.h for HW device bit mask
        *                       definitions.
        * @return   KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt GetHWDeviceSettings( TUint32& aSettings ) const;

        /**
        * Sets one setting for specified accessory mode.
        *
        * For example: When accessory mode is EAccModeWiredCarKit and settings id is 
        *              KAccServerWiredCarKitLights then setting value must be type of 
        *              enum TAccLightSettings.
        *
        * @since S60 3.1
        * @param    aAccMode    Defines an accessory mode. Parameter has to
        *                       be set before calling this method.
        * @param    aSetting    Defines setting and it's value. Fill in
        *                       identification number and TInt, TBool or TDes
        *                       type setting before calling this method. For
        *                       details on how to fill in TAccModeSetting object
        *                       see AccModeSetting.h.
        * @return   KErrNone if successful, KErrArgument if an argument was invalid.
        * @exception system-wide error code if failed.
        */
        IMPORT_C TInt SetAccessoryModeSettingL( const TAccMode aAccMode,
                                                TAccModeSetting& aSetting );
        /**
        * Sets several settings for specified accessory mode.
        *
        * @since S60 3.1
        * @param    aAccMode    Defines an accessory mode. Parameter has to
        *                       be set before calling this method.
        * @param    aArray      Defines an array of several setting and value
        *                       pairs. See AccModeSettingArray.h on how to add
        *                       TAccModeSetting objects to array. For each
        *                       TAccModeSetting object fill in identification
        *                       number and TInt, TBool or TDes type setting. For
        *                       details on how to fill in TAccModeSetting object
        *                       see AccModeSetting.h.
        * @return   KErrNone if successful, KErrArgument if an argument was invalid.
        * @exception system-wide error code if failed.
        */
        IMPORT_C TInt SetAccessoryModeSettingsL( const TAccMode aAccMode,
                                                 TAccModeSettingArray& aArray );

        /**
        * Returns one setting for specified accessory mode.
        *
        * @since S60 3.1
        * @param    aAccMode    Defines an accessory mode. Parameter has to
        *                       be set before calling this method.
        * @param    aSetting    Defines setting and it's value. Identification
        *                       of setting has to be set to aSetting before
        *                       calling this method.
        * @return   KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt GetAccessoryModeSetting(
                                        const TAccMode aAccMode,
                                        TAccModeSetting& aSetting ) const;
        
        /**
        * Returns all settings for specified accessory mode.
        *
        * @since S60 3.1
        * @param    aAccMode    Defines accessory mode. Parameter has to
        *                       be set before calling this method.
        * @param    aArray      On return contains all existing settings 
        *						for accessory mode.  
        * @return   KErrNone if successful, system-wide error code if failed.
        */
        IMPORT_C TInt GetAccessoryModeSettings(
                                        const TAccMode aAccMode,
                                        TAccModeSettingArray& aArray ) const;
    };

#endif      // RACCESSORYSETTINGS_H

// End of File
