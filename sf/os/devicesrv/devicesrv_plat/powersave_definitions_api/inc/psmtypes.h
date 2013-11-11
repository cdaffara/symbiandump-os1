/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Type definitions file for PSM Server
*
*/



#ifndef PSMTYPES_H
#define PSMTYPES_H

#include <e32base.h>

const TInt KPsmMaxBufferLength = 160;

/**
 * Power save mode type.
 */
enum TPsmsrvMode
    {
    EPsmsrvModeNormal = 0,
    EPsmsrvModePowerSave,
    EPsmsrvPartialMode
    };

/**
 * Type of the configuration parameter
 */
enum TPsmsrvConfigTypes
    {
    EConfigTypeInt,
    EConfigTypeReal,
    EConfigTypeBuf
    };

/**
 * Structure for the configuration
 */
class TPsmsrvConfigInfo
    {
    public:
    /**
     * The key for the cenrep
     */
    TInt iConfigId;

    /**
     * Type of the configuration value
     */
    TPsmsrvConfigTypes iConfigType;

    /**
     * Value of the configuration
     */
    union
        {
        TInt iIntValue;
        TReal iRealValue;
        };

    /**
     * Descriptor type value of the configuration
     */
    TBuf8<KPsmMaxBufferLength> iDesValue;

    /**
     * Indicates is value update blocked or not
     */
    TBool iBlocked;
    };

/**
 * Array containing configuration information
 */
typedef RArray<TPsmsrvConfigInfo> RConfigInfoArray;

/**
 * Array containing UIDs of the passive configurations
 */
typedef RArray<TUint32> RPassiveConfigList;

#endif //PSMTYPES_H

// End of File
