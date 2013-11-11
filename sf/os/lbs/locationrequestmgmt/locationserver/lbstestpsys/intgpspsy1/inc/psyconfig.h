/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef PSYCONFIG_H
#define PSYCONFIG_H

#include <lbs.h>

struct TPsyLRResponseConfig
    {
    TInt iNumOfResponse; //0 indicate for ever with same response
    TInt iErr; //Error code
    TInt iResponseTime; //In seconds, 0 means imediately
    TReal iLat;
    TReal iLon;
    TReal iAlt;
    TInt iNmeaDataSize; //If this is not zero, then returned 
    //HGenericInfo contains some NMEA data
    };
    
struct TPsyModuleStatusConfig
    {
    TInt iDeviceStatus;
    TInt iDataQuality;
    TInt iResponseTime; //Delay when change the module status. In seconds.
    };

struct TPsyConfig
    {
    enum
        {
        EConfigLRResponse,
        EConfigModuleStatus
        };
        
    TInt iType;
    union
        {
        TPsyLRResponseConfig iLRConfig;
        TPsyModuleStatusConfig iStatusConfig;
        } iData;
    };

#endif

// End of file
