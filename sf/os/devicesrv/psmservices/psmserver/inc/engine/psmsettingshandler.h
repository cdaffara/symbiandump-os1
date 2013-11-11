/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM settings handler class.
*
*/


#ifndef PSMSETTINGSHANDLER_H
#define PSMSETTINGSHANDLER_H

#include <e32base.h>
#include <psmsettingsprovider.h>

// FORWARD DECLARATIONS
class CPsmStorage;
class CPsmConfigStorage;
class CPsmCenRepStorage;
class CPsmBackupStorage;

/**
 *  PSM Settings handler class
 *
 *  Handles settings related events during power save mode change
 *
 *  @since S60 5.0
 */
class CPsmSettingsHandler : public CBase,
                            public MPsmSettingsProvider
    {
    public:  // Constructors and destructor

        /**
         * Creates a new PSM settings handler.
         *
         * @since S60 5.0
         * @param aMode Mode where to swith to
         * @return A pointer to the created object.
         */
        static CPsmSettingsHandler* NewL( TPsmsrvMode aMode );

        /**
         * Creates a new PSM settings handler. Newly created instance is left in the cleanup stack.
         *
         * @since S60 5.0
         * @param aMode Mode where to swith to
         * @return A pointer to the created object.
         */
        static CPsmSettingsHandler* NewLC( TPsmsrvMode aMode );

        /**
         * Destructor.
         */
        virtual ~CPsmSettingsHandler();

        /**
         * From MPsmSettingsProvider. Changes the settings
         */
        void BackupAndGetSettingsL( RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository );

        /**
         * From MPsmSettingsProvider. Gets configuration set for current PSM mode
         */
        void GetSettingsL( RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository );

        /**
         * From MPsmSettingsProvider. Creates backup for configuration set.
         */
        void BackupSettingsL( const RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository );

        /**
         * Changes CenRep keys
         */
        void ChangeCenRepSettingsL();

        /**
         * Sets new mode
         */
        void SetMode( TPsmsrvMode aMode ); 

        /**
         * Returns mode where to switch to
         */
        TPsmsrvMode Mode() { return iMode; };

        /**
         * Returns current mode before mode change
         */
        TPsmsrvMode PreviousMode() const { return iCurrentMode; };

    private:

        /**
         * C++ constructor.
         * @param aMode Mode where to swith to
         */
        CPsmSettingsHandler( TPsmsrvMode aMode );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Changes CenRep keys
         */
        void ChangeCenRepSettingsL( RConfigInfoArray& aPsmConfigArray, TUint32 aRepository );

        /**
         * Compares current settings to new settings, creates backup and returns 
         * new settings
         * 
         * @param aConfigStorage Storage where to get new config values
         * 
         */
        void ChangeSettingsL( RConfigInfoArray& aPsmConfigArray, 
                              TUint32 aRepository,
                              CPsmStorage& aConfigStorage );

        /**
         * Compares two configuration informations
         */ 
        TBool Compare( const TPsmsrvConfigInfo& aSource, const TPsmsrvConfigInfo& aTarget ) const;

    private:    // Data

        /**
         * Mode where to switch to
         */
        TPsmsrvMode iMode;

        /**
         * Mode before PSM change completed
         */
        TPsmsrvMode iCurrentMode;

        /**
         * Handle to file server session
         */
        RFs iFileSession;

        /**
         * Storage file handlers
         */
        CPsmConfigStorage* iConfigStorage;
        CPsmBackupStorage* iBackupStorage;
        CPsmCenRepStorage* iCenRepStorage;

    };

#endif // PSMSETTINGSHANDLER_H 
