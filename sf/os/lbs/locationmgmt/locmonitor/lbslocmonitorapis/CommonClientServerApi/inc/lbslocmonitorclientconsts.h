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
* Description:
*
*/


#ifndef LBSLOCMONITORCLIENTCONSTS_H_
#define LBSLOCMONITORCLIENTCONSTS_H_

#include <lbs/lbslocationinfoconvertercommon.h>
#include <e32cmn.h>

/**
 * Message parameters index in a message of RMessage2. The server has to use
 * the parameter index defined below to write and read data into the IPC message.
 */

// This position in the IPC is used by the client library to receive 
// buffer size.
const TInt KParamBufferSize = 0;


// This position in the IPC is used by the client library to send location
// information.
const TInt KParamLocationInfo = 1;

// This position in the IPC is used by the client library to send client preferences.
const TInt KParamClientPrefs = 2;

// This position in the IPC is used by the server to send converted info
const TInt KParamConvertedInfo = 0;

/**
 * This structure holds conversion prefs,location info mask and 
 * the conversion module id.
 */
struct TLbsClientPrefs
    {
    TLbsConversionPrefs iConversionPrefs;
    
    TLbsConversionOutputInfoMask iRequestedInfo;
    
    TUid iConverterModuleId;
    };

#endif /*LBSLOCMONITORCLIENTCONSTS_H_*/
