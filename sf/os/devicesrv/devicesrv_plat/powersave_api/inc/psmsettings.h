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
* Description:  PSM Client Settings API class.
*
*/


#ifndef PSMSETTINGS_H
#define PSMSETTINGS_H

#include <e32base.h>
#include <psmtypes.h>

/**
 *  PSM Client Settings API class
 * 
 *  Allows clients to make power save mode settings changes 
 *  during power save mode change.
 *
 *  @lib psmclient.lib
 */
class CPsmSettings : public CBase
    {
   public:  // Constructors and destructor

        /**
         * Creates a new PSM client.
         */
        IMPORT_C static CPsmSettings* NewL();
        IMPORT_C static CPsmSettings* NewLC();

    public: // API methods

        /**
         * Changes the values. Backups current settings and returns new settings for current PSM mode
         *
         * Only those settings that exists in aPsmConfigArray are updated, there are no new settings
         * added to array on completion.
         *
         * @param[in, out] aPsmConfigArray Contains current settings, on exit, contains updated settings
         * @param[in] aRepository Specific repository UID where these settings are in
         * @return A system wide error code
         */
        virtual TInt BackupAndGetSettings( RConfigInfoArray& aPsmConfigArray, 
                                           const TUint32 aRepository ) = 0;

        /**
         * Gets configuration set for current mode without doing any backups. Only reads 
         * configurations for currently active power save mode. If current mode is not 
         * a power save mode, returns empty array.
         *
         * Gets prefilled config array as a parameter, and only those settings that are in array
         * are got, for example if there is three different settings for this repository and one 
         * wants to get only information which config Id is 2, only one config info with config Id
         * 2 should exists in aParamConfigArray
         *
         * @param[in, out] aPsmConfigArray Prefilled config info array. Each prefilled config info 
         *                                 must contain at least config Id
         * @param[in] aRepository Specific repository UID where these settings are in
         * @return A system wide error code
         */
        virtual TInt GetSettings( RConfigInfoArray& aPsmConfigArray, 
                                  const TUint32 aRepository ) = 0;

        /**
         * Backups settings that exists in aPsmConfigArray. Note that any existing config information
         * with same config Id in aRepository is overriden with new values from aPsmConfigArray.
         *
         *
         * @param[in] aPsmConfigArray Contains settings to store into backup
         * @param[in] aRepository Specific repository UID where these settings are in
         * @return KErrWrite if device is not in any power save mode,
         *         a system wide error code
         */
        virtual TInt BackupSettings( RConfigInfoArray& aPsmConfigArray, 
                                     const TUint32 aRepository ) = 0;

        /**
         * Gives currently active power save mode. If mode change is ongoing, gives the PSM mode for which 
         * is changing.
         *
         * @param[in, out] aMode Contains currently active power save mode on completion
         * @return A system wide error code
         */
        virtual TInt GetCurrentMode( TInt& aMode ) = 0;

    };

#endif // PSMSETTINGS_H 
