/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Types used in PhoneClient.
*
*/


#ifndef PHCLTTYPES_H
#define PHCLTTYPES_H



// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <etelmm.h>

#include <data_caging_path_literals.hrh>

// CONSTANTS

// Lengths of various strings.
const TInt KPhCltTelephoneNumberLength = 100;  // Telephony number length.
const TInt KPhCltNameBufferLength = 50;       // The name length.
const TInt KPhCltSubAddressLength = 21;       // The subaddress length, see
                                              // ITU-T I.330 and 3GPP TS 11.14.
const TInt KPhCltBearerLength = 14;           // The bearer length.
const TInt KPhCltUUILength = 129;             // Max user to user info length

/**
* Call type.
*/
enum TPhCltCallType
    {
    EPhCltVoice       = 0, // Voice call.
    EPhCltVideo       = 1, // Video call.
    EPhCltForcedVideo = 2,  // Forced video call.
    EPhCltCallVoIP    = 3 // Internet call
    };


// Type for telephone number.
typedef TBuf< KPhCltTelephoneNumberLength > TPhCltTelephoneNumber;

// Type for name.
typedef TBuf< KPhCltNameBufferLength > TPhCltNameBuffer;


// Type for result of call.
typedef TInt TPhCltPhoneResults;

// Type for calling party subaddress.
typedef TBuf< KPhCltSubAddressLength > TPhCltSubAddress;

// Type for bearer capability.
typedef TBuf8< KPhCltBearerLength > TPhCltBearerCapability;

#endif      // PHCLTTYPES_H

// End of File
