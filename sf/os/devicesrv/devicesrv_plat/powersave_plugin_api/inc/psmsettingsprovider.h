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
* Description:  PSM Settings Provider API for plug-ins
*
*/



#ifndef PSMSETTINGSPROVIDER_H
#define PSMSETTINGSPROVIDER_H

//  INCLUDES
#include <e32base.h>
#include <psmtypes.h>

// CLASS DECLARATION

/**
 * PSM Settings Provider class for PSM plug-ins settings
 *
 */
class MPsmSettingsProvider
    {
public:

    /**
     * Current mode where to switch to.
     *
     * @return Mode where we are currently switching to.
     */
    virtual TPsmsrvMode Mode() = 0;

    /**
     * Changes the values. Backups current settings and returns changed settings. 
     * Plug-ins should give same settings set no matter what the mode is. PSM server takes care 
     * of setting correct values back to parameter aPsmConfigArray.
     *
     * @param[in, out] aPsmConfigArray Contains current settings, on exit, contains updated settings
     * @param[in] aRepository Specific reposiroty UID where these settings are in. Identifies the
     *                        configuration file located in PSM servers private folder
     * @leave KErrNotFound if no settings are found for this plugin
     */
    virtual void BackupAndGetSettingsL( RConfigInfoArray& aPsmConfigArray, 
                                        const TUint32 aRepository ) = 0;

    /**
     * Gets configuration set for current mode without doing any backups. Only reads 
     * configurations for currently active power save mode. If current mode is not 
     * a power save mode, returns settings from backup storage.
     *
     * Gets prefilled config array as a parameter, and only those settings that are in array
     * are got, for example if there is three different settings for this repository and one 
     * wants to get only information which config Id is 2, only one config info with config Id
     * 2 should exists in aParamConfigArray
     *
     * @param[in, out] aPsmConfigArray Prefilled config info array. Each prefilled config info 
     *                                 must contain at least config Id
     * @param[in] aRepository Specific reposiroty UID where these settings are in
     * @return A system wide error code
     */
    virtual void GetSettingsL( RConfigInfoArray& aPsmConfigArray, 
                               const TUint32 aRepository ) = 0;

    /**
     * Backups settings that exists in aPsmConfigArray. Note that any existing config information
     * with same config Id in aRepository is overriden with new values from aPsmConfigArray.
     *
     * @param[in] aPsmConfigArray Contains settings to store into backup
     * @param[in] aRepository Specific reposiroty UID where these settings are in
     * @leave KErrWrite if device is not in any power save mode,
     *        a system wide error code
     */
    virtual void BackupSettingsL( const RConfigInfoArray& aPsmConfigArray, 
                                  const TUint32 aRepository ) = 0;

    };

#endif //PSMSETTINGSPROVIDER_H

// End of File


