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
* Description:  PSM client settings API implementation class.
*
*/


#ifndef PSMSETTINGSIMPL_H
#define PSMSETTINGSIMPL_H

#include <e32base.h>
#include <psmsettings.h>
#include "psmsession.h"

/**
 *  PSM settings API class.
 *
 *  Implementents power save mode client settings API.
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CPsmSettingsImpl ) : public CPsmSettings
    {

    friend class CPsmSettings;

    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CPsmSettingsImpl();

        /**
         * C++ constructor.
         */
        CPsmSettingsImpl( RPsmServer& aPsmServer );

    public: // API methods

        /**
         * Changes the values. Backups current settings and returns changeable settings
         *
         * @since S60 5.0
         * @param[in, out] aPsmConfigArray Contains current settings, on exit, contains updated settings
         * @param[in] aRepository Specific reposiroty UID where these settings are in
         * @return A system wide error code.
         */
        TInt BackupAndGetSettings( RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository );

        /**
         * Gets configuration set for current mode without doing any backups or changing any values.
         * Only reads configurations for currently active power save mode. If current mode is not
         * a power save mode, returns empty array.
         *
         * @param[in, out] aPsmConfig Pre-filled configuration info. Must contain at least config identification
         * @param[in] aRepository Specific reposiroty UID where these settings are in
         * @return A system wide error code
         */
        TInt GetSettings( RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository );

        /**
         * Backups settings that exists in aPsmConfigArray. Note that any existing config information
         * with same config Id in aRepository is overriden with new values from aPsmConfigArray.
         *
         *
         * @param[in, out] aPsmConfigArray Contains current settings, on exit, contains updated settings
         * @param[in] aRepository Specific reposiroty UID where these settings are in
         * @return A system wide error code
         */
        TInt BackupSettings( RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository );

        /**
         * Gives currently active power save mode. If mode change is ongoing, gives the PSM mode for which 
         * is changing.
         *
         * @return TPsmsrvMode Currently active mode
         */
        TInt GetCurrentMode( TInt& aMode );

    private:

        /**
         * C++ constructor.
         */
        CPsmSettingsImpl();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

        /**
         * Handles settings related requests
         */
        TInt HandleSettingsRequest( RConfigInfoArray& aPsmConfigArray, 
                                    const TUint32 aRepository,
                                    const TPsmServerRequest aRequest );

        /**
         * Updates config array
         */
        void UpdateConfigArray( RConfigInfoArray& aTargetArray, 
                                RConfigInfoArray& aSourceArray );

    private: // data

        /**
         * Client-server session to PSM server.
         */
        RPsmServer iPsmServer;

        /**
         * Indicates is iPsmServer owned
         */
        TBool iOwnSession;

    };

#endif // PSMSETTINGSIMPL_H 
