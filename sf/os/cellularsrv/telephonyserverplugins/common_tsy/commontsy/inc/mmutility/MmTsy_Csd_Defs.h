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



#ifndef CSD_DEFS_H
#define CSD_DEFS_H

//  INCLUDES
#include <etelmm.h>

// CSD call related definitions

// DATA TYPES
/** Data call mode */
enum TDataCallMode
    {
    EDataCallModeUnknown,
    EDataCallModeNormal,
    EDataCallModeHscsd
    };
        
// CONSTANTS
// MultimodeTsy HSCSD support 
const TUint8 KMultimodeTsyDataHscsdSupport = ETrue;

// MultimodeTsy Data User Init Upgrade
const TUint8 KMultimodeTsyDataUserInitUpgrade = EFalse;

// Multislot class, values 1 to 29 (dec)
const TUint8 KMultimodeTsyDataMClassDefValue = 0x6;

// Max number of Rx slots, values 1 to 8 
const TUint8 KMultimodeTsyDataMaxRxTSDefValue = 0x3;

// Max number of Tx slots, values 1 to 8
const TUint8 KMultimodeTsyDataMaxTxTSDefValue = 0x2;

// Max number of Sum slots, values 2 to 5 and NA
const TUint8 KMultimodeTsyDataTotalRxTxTSDefValue = 0x4;

// MultimodeTsy data call capabilities - supported values
// See Multimode Etel API Definition Document, rev. 5.01

// Speed caps
const TUint32 KMultimodeTsyDataSpeedCaps 
        = RMobileCall::KCapsSpeedAutobauding
        | RMobileCall::KCapsSpeed2400
        | RMobileCall::KCapsSpeed4800
        | RMobileCall::KCapsSpeed9600
        | RMobileCall::KCapsSpeed14400
        | RMobileCall::KCapsSpeed19200
        | RMobileCall::KCapsSpeed28800
        | RMobileCall::KCapsSpeed38400
        | RMobileCall::KCapsSpeed43200
        | RMobileCall::KCapsSpeed56000;

const TUint32 KMultimodeTsyDataSpeedCapsVT
        = RMobileCall::KCapsSpeed64000;

// Protocol caps 
const TUint32 KMultimodeTsyDataProtocolCaps 
        = RMobileCall::KCapsProtocolV22bis 
        | RMobileCall::KCapsProtocolV32 
        | RMobileCall::KCapsProtocolV34
        | RMobileCall::KCapsProtocolV110
        | RMobileCall::KCapsProtocolV120
        | RMobileCall::KCapsProtocolX31FlagStuffing
        | RMobileCall::KCapsProtocolPIAFS;

const TUint32 KMultimodeTsyDataProtocolCapsVT
       =  RMobileCall::KCapsPstnMultimedia
        | RMobileCall::KCapsIsdnMultimedia;


// Service caps
const TUint32 KMultimodeTsyDataServiceCaps 
        = RMobileCall::KCapsDataCircuitAsynchronous
        | RMobileCall::KCapsDataCircuitAsynchronousRDI
        | RMobileCall::KCapsPADAsyncUDI
        | RMobileCall::KCapsPADAsyncRDI
        | RMobileCall::KCapsPacketAccessSyncUDI
        | RMobileCall::KCapsPacketAccessSyncRDI;

const TUint32 KMultimodeTsyDataServiceCapsVT
        = RMobileCall::KCapsDataCircuitSynchronous;


// QoS caps
const TUint32 KMultimodeTsyDataQoSCaps 
        = RMobileCall::KCapsTransparent
        | RMobileCall::KCapsNonTransparent
        | RMobileCall::KCapsTransparentPreferred
        | RMobileCall::KCapsNonTransparentPreferred;

// TchCodings caps
const TUint32 KMultimodeTsyDataTchCodingsCaps 
        = RMobileCall::KCapsTchCoding48
        | RMobileCall::KCapsTchCoding96
        | RMobileCall::KCapsTchCoding144;

// Asymmetry caps
const TUint32 KMultimodeTsyDataAsymmetryCaps 
        = RMobileCall::KCapsAsymmetryNoPreference
        | RMobileCall::KCapsAsymmetryDownlink
        | RMobileCall::KCapsAsymmetryUplink;

// RLPVersion caps
const TUint32 KMultimodeTsyDataRLPVersionCaps 
        = RMobileCall::KCapsRLPSingleLinkVersion0
        | RMobileCall::KCapsRLPSingleLinkVersion1
        | RMobileCall::KCapsRLPMultiLinkVersion2;
    
// V42bis caps
const TUint32 KMultimodeTsyDataV42bisCaps 
        = RMobileCall::KCapsV42bisTxDirection
        | RMobileCall::KCapsV42bisRxDirection
        | RMobileCall::KCapsV42bisBothDirections;

#endif // CSD_DEFS_H

// End of File
