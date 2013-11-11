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
* Description:  This file contains domain central repository key definitions
*                owned by HWRM.
*
*/


#ifndef HWRMDOMAINCRKEYS_H
#define HWRMDOMAINCRKEYS_H

//  INCLUDES
//#include <HWRMSDKCRKeys.h>
#include <e32cmn.h>

// CONSTANTS

/**
* HW Settings API
*
* Contains Hardware related settings that need to be available for adaptation also.
*/
const TUid KCRUidHWSettings = {0x1020506A};

/**
* Indicates whether T-coil hearing aid mode is enabled or not.
* 
* Possible integer values:
*
* 0 - Hearing aid mode disabled
* 1 - Hearing aid mode enabled
*
* Default value: 0
*/
const TUint32 KHWSettingsHearingAidMode         = 0x00000001;

#endif  // HWRMDOMAINCRKEYS_H
            
// End of File
