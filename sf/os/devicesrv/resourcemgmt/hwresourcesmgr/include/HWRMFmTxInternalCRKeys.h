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





#ifndef HWRMFMTXINTERNALCRKEYS_H
#define HWRMFMTXINTERNALCRKEYS_H

#include <e32std.h>
#include <hwrm/hwrmfmtxdomaincrkeys.h>

// FM Tx API

/**
* FM Tx range - minimum frequency (kHz)
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyMinFrequency = 0x00000002; 

/**
* FM Tx range - maximum frequency (kHz)
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyMaxFrequency = 0x00000003; 

/**
* FM Tx frequency range - step size (kHz)
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyStepSize = 0x00000004; 

/**
* RDS PS string
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyRdsPs = 0x00000010; 

/**
* RDS unsupported character replacement
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyRdsReplacementChar = 0x00000011; 

/**
* SID of process controlling KHWRMFmTxAudioRoutedFlag flag
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyAudioPolicySid = 0x00000020; 

/**
* Timeout in seconds used to disable FM transmitter if there is no audio routed.
*
* @internalComponent
* @released
*/
const TUint32 KFmTxCenResKeyPowerSaveTimerLength = 0x00000021;

#endif  // HWRMFMTXINTERNALCRKEYS_H
            
// End of File
