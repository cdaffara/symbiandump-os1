/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CACCSRVSUBSETTINGS_H
#define CACCSRVSUBSETTINGS_H

// INCLUDES
#include <e32base.h>
#include "AsyProxyAPI.h"
#include "AccSrvSubBase.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccSrvMainSession;

// CLASS DECLARATION
/**
*  Sub session class for Accessory Settings.
*
*  @lib AccServer
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvSubSettings ) : public CAccSrvSubBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvSubSettings* NewL( CASYProxyAPI* aSession,
                                         CAccSrvConnectionController* aConnectionController,
                                         CAccSrvServerModel* aServerModel );

        /**
        * Destructor.
        */
        ~CAccSrvSubSettings();

    public: // New functions

    public: // Functions from base classes
        
        /**
        * From CAccSrvSubBase
        * Handles received messages from client.
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void DispatchMessageL( const RMessage2& aMessage );

    protected: // New functions

        /**
        * C++ default constructor.
        */
        CAccSrvSubSettings( CASYProxyAPI* aSession,
                            CAccSrvConnectionController* aConnectionController,
                            CAccSrvServerModel* aServerModel );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();


    protected: // Functions from base classes

    private: // New methods

        /**
        * Returns supported HW devices. HW device is an accessory that can't
        * be detected properly without UI setting.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetSupportedHWDevicesL( const RMessage2& aMessage );

        /**
        * Sets HW device. Used to detect accessories that require UI setting.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void SetHWDeviceSettingsL( const RMessage2& aMessage );

        /**
        * Returns HW devices.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetHWDeviceSettingsL( const RMessage2& aMessage );

        /**
        * Sets accessory mode related setting.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void SetAccessoryModeSettingL( const RMessage2& aMessage );

        /**
        * Returns accessory mode related setting.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetAccessoryModeSettingL( const RMessage2& aMessage );

        /**
        * Sets several accessory mode related settings.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void SetAccessoryModeSettingsL( const RMessage2& aMessage );

        /**
        * Returns several accessory mode related settings.
        *
        * @since S60 3.1
        * @param aMessage Client request.
        * @return void
        */
        void GetAccessoryModeSettingsL( const RMessage2& aMessage );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Bit mask for device type
        TUint32 iSettings;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif // CACCSRVSUBSETTINGS_H

// End of File

