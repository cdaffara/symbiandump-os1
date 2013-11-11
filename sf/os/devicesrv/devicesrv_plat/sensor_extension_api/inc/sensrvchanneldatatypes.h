/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel data types definitions
*
*/



#ifndef SENSRVCHANNELDATATYPES_H
#define SENSRVCHANNELDATATYPES_H

//  INCLUDES
#include <e32base.h>
#include <sensrvtypes.h> 
#include <sensrvsensorchannels.h>

/**
 * Channel data types moved to sensor specific headers
 */


/**
 * These are the values that can be published, during authentication swipe
 */
enum TFpAuthenticationStatus {
	EAuthenticationFailed = 0,
	EAuthenticationSuccess,
};

/**
 * PnS key Category for FPS
 */
const TUid KUidFpsCategory = {0x1020507E };

/**
 * PnS key for Authentication Status
 */
const TUint32 KFpsAuthenticationKey = 0x00000001;

// Cenrep Constants
// Cenrep key
const TUid KCRUidFPSStatus = { 0x2002875E };

//FPS enabled/disabled
const TUint32 KFPSOnOffStatus =  0x1 ;
// Finger registered/not registered
const TUint32 KFPSRegisteredFingers =  0x2 ;
#endif //SENSRVCHANNELDATATYPES_H

// End of File


