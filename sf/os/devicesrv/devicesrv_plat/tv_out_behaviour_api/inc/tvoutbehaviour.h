/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TV Out Behaviour API declaration.
*
*/

#ifndef __TVOUTBEHAVIOUR_H__
#define __TVOUTBEHAVIOUR_H__

//- Include Files  ----------------------------------------------------------

#include <e32base.h>

//- Namespace ---------------------------------------------------------------

//- Constants ---------------------------------------------------------------

//- Forward Declarations ----------------------------------------------------

//- Class Definitions -------------------------------------------------------

/**
*  CTVOutBehaviour
*
*  Contains core data structures and member functions in the TV-out behaviour API
*/
class CTvOutBehaviour : public CBase
    {
    public:

        /**
        *  TTvOutScreenDevice
        *
        *  Enumerations for screen devices that can be used for TV-out.
        *  Note! The enumeration here is different than screen numbering.
        */
        enum TTvOutScreenDevice
            {
            EScreenNone      = 0x00000000, // Initialisation value, don't use
            EScreen0         = 0x00000001, // Primary display is used for TV-out
            EScreen1         = 0x00000002, // Secondary display is used for TV-out
            ETvScreen        = 0x00000004, // TV-out specific screen device with S60 layout for TV
            ETvOverlayScreen = 0x00000008  // TV-out specific screen device only for overlays (CPostingBehaviour)
            };

        /**
        *  TTvOutSettings
        *
        *  Speficies the settings that client can give for the TV-out behaviour API.
        */
        class TTvOutSettings
            {
            public:
                TTvOutSettings() {};
            public:
                ///Screen Device used for TV-out
                TTvOutScreenDevice iScreenDevice;
            };

        /**
        *  Constructor.
        *
        *  @return  KErrNone if success
        */
        IMPORT_C static CTvOutBehaviour* NewL();

        ///Destructor
        virtual ~CTvOutBehaviour() {};

        /**
        *  This function can be used to set TV-out settings.
        *  The function can be called at any time.
        *  @see DeActivateSettings
        *  @see SettingsListener
        *
        *  @param   aParams  New settings for TV-out
        *  @return  KErrNone if success
        */
        virtual TInt SetTvOutSettings(const TTvOutSettings& aParams) = 0;

        /**
        *  Disables settings of set by SetTvOutSettings.
        *  Normally will be done when application goes background.
        *  For activating settings again, SetTvOutSettings to be called.
        *  @see SetTvOutSettings
        *
        *  @return  KErrNone if success
        */
        virtual TInt DeActivateSettings() = 0;

        /**
        *  This function can be used to get supported TV-out screen devices.
        *
        *  @return  Combination of TTvOutScreenDevice (bit mask)
        */
        virtual TUint GetTvOutScreenDevices() = 0;

        /**
        *  This function can be used to get display number of the TV-out.
        *
        *  @return  TV-out display number
        */
        virtual TInt GetTvOutDisplayNumber() = 0;

        /**
        *  This function can be used to get current settings in use
        *  @see SettingsListener
        *
        *  @param   aParams  Data class where current settings are saved
        *  @return  KErrNone if success
        */
        virtual TInt GetTvOutSettings(TTvOutSettings& aParams) = 0;

        /**
        *  This method registers client to listening if Tv-out settings will
        *  change (SetTvOutSettings), or new settings are came in use.
        *  When status is completed without errors, GetTvOutSettings to be called.
        *  @see GetTvOutSettings
        *
        *  @param   aStatus  Will be completed when SetTvOutSettings is called
        *  @return  KErrNone if success
        */
        virtual TInt SettingsListener(TRequestStatus& aStatus) = 0;

        /**
        *  Cancels SettingsListener.
        *  @see SettingsListener
        *
        *  @return  KErrNone if success
        */
        virtual TInt CancelSettingsListener() = 0;

        /**
        *  This function can be used to get default settings for TV-out.
        *
        *  @param   aParams  Data class where current settings are saved
        *  @return  KErrNone if success
        */
        virtual TInt GetTvOutDefaultSettings(TTvOutSettings& aParams) = 0;

    protected:
        ///Constructor
        CTvOutBehaviour() {};
    };

#endif // __TVOUTBEHAVIOUR_H__

