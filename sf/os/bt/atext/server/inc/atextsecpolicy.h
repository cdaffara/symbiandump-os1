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
* Description:  ATEXT server security policy definition
*
*/


#ifndef SECURITYPOLICY_H
#define SECURITYPOLICY_H

// Total number of ranges
// Definition of the ranges of IPC numbers
const TInt KSrvRanges[] =
    {
    0,            // Range 1: all client APIs
    EInvalidIpc   // Range 2: Invalid IPC opcode
    };

const TUint KSrvRangeCount = sizeof(KSrvRanges) / sizeof(TInt);

// Policy to implement for each of the above ranges
const TUint8 KSrvElementsIndex[KSrvRangeCount] =
    {
    0,                              //policy 0 applies to 0th range
    CPolicyServer::ENotSupported    //applies to 7th range(out of range IPC)
    };

// Specific capability checks
const CPolicyServer::TPolicyElement KSrvElements[] =
    {
        {_INIT_SECURITY_POLICY_C2(ECapabilityLocalServices, ECapabilityNetworkControl),
         CPolicyServer::EFailClient
        }
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KSrvPolicy =
        {
        0,                  // only process with SID KATExtUserProcessSid can connect to server.
        KSrvRangeCount,     // Range count
        KSrvRanges,         // ranges array
        KSrvElementsIndex,  // elements' index
        KSrvElements        // array of elements
        };

#endif // SECURITYPOLICY_H
