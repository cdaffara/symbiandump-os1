/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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





#ifndef HWRMFMTXAUDIOPOLICYDOMAINPSKEYS_H
#define HWRMFMTXAUDIOPOLICYDOMAINPSKEYS_H

// INCLUDES
#include <e32std.h>
#include <hwrm/hwrmfmtxdomainpskeys.h>

// =============================================================================
// HWRM FM Tx Audio Policy API
// ============================================================================= 

/**
* This is the "active audio" boolean flag, published by Audio Policy
* A value of ETrue indicates that application audio is being generated, 
* and is currently routed to FM Tx.
*
* @publishedPartner
* @released
*/
const TUint32 KHWRMFmTxAudioRoutedFlag = 0x00000022;

/**
* Possible FMTx available states
*
* @publishedPartner
* @released
*/
enum THWRMFmTxAvailableFlag
    {
    EHWRMFmTxAvailableOff        = 0, // FM transmitter off
    EHWRMFmTxAvailableOn         = 1, // FM transmitter on
    EHWRMFmTxAvailableStandby    = 2  // FM transmitter off, but indicates transmitter has been 
                                      // switched off due audio inactivity within configured 
                                      // period (KFmTxCenResKeyPowerSaveTimerLength). 
                                      // So, audio policy can switch FM transmitter back on
                                      // by toggling KHWRMFmTxAudioRoutedFlag ETrue.
    };

/**
* This is the "FM Tx available" flag, published by HWRM Server
* Available states are enumerated above. 
*
* @publishedPartner
* @released
*/
const TUint32 KHWRMFmTxAvailableFlag = 0x00000023;

#endif      // HWRMFMTXAUDIOPOLICYDOMAINPSKEYS_H

// End of File

