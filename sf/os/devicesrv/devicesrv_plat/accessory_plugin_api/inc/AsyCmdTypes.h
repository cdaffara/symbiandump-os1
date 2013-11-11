/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Command types, profile bit masks and parameter definitions.
*
*/


#ifndef ASYCMDTYPES_H
#define ASYCMDTYPES_H

// INCLUDES
#include <AccPolNameRecord.h>
#include <AccPolGenericID.h>

// CONSTANTS

// MACROS

// DATA TYPES

struct TASYCommandParamRecord
    {
    TAccPolNameRecord iNameRecord; // Can be NULL
    TInt64            iCmdValue;   // Content depends on command:
                                   // - ECmdSetValueBool: contains value to be set
                                   // - ECmdSetValueTInt: contains value to be set
                                   // - ECmdGetSupportedBTProfiles: contains BT Device Address
    TAccPolGenericID  iGenericID;
    };

struct TASYBTAccInfo
    {
    TUint16                 iDeviceInfo; // Device model number of the accessory. 0=Non-proprietary.
    TUint32                 iDeviceType; // Must be a Device type bit mask declared in AccPolGenericid.h
    TUint32                 iProfiles;   // Must be a Profiles bit mask declared in AsyCmdTypes.h
    TUint32                 iReserved;   // Used bits: KAccInfoDRM, KAccInfoLatency.
    TFixedArray<TUint16, 8> iSupportedFeatures; //Index positions are decrared in TAccInfoIndex                                     
    };

// Profiles bit mask definitions
// These definitions corresponds to name definitions in (AccPolCommonNameValuePairs.h)
const TUint32 KAccInfoHSP   =   1; // 0000 0000 0000 0000 0000 0000 0000 0001  Headset Profile (HSP)
const TUint32 KAccInfoHFP   =  64; // 0000 0000 0000 0000 0000 0000 0100 0000  Handsfree Profile (HFP)
const TUint32 KAccInfoAVRCP = 128; // 0000 0000 0000 0000 0000 0000 1000 0000  Audio Video Remote Control Profile (AVRCP)
const TUint32 KAccInfoA2DP  = 512; // 0000 0000 0000 0000 0000 0010 0000 0000  Advanced Audio Distribution Profile (A2DP)

//Constants for TASYBTAccInfo::iSupportedFeatures indexes
enum TAccInfoIndex
    {
    EAccInfoHSPIndex,  //index 0: supported features for HSP
    EAccInfoHFPIndex,  //index 1: supported features for HFP
    EAccInfoAVRCPIndex,//index 2: supported features for AVRCP
    EAccInfoA2DPIndex  //index 3: supported features for A2DP
                       //index 4-7: reserved
    };

// Bit mask definitions for TASYBTAccInfo::iReserved.
const TUint32 KAccInfoDRM   =   1; //Remote accessory supports capturing i.e. DRM protected audio output is not allowed.
const TUint32 KAccInfoLatency   =   2; //Remote accessory supports latency.

typedef TPckgBuf<TASYCommandParamRecord> TASYCmdParams;
typedef TPckgC<TASYBTAccInfo> TASYBTAccInfoPckgC;

// Process command ids
enum TProcessCmdId
    {
    ECmdSetValueBool,
    ECmdSetValueTInt,
    ECmdGetValueBool,
    ECmdGetValueTInt,
    ECmdGetValueTDes8,
    ECmdProcessCommandInit,
    ECmdGetSupportedBTProfiles,
    ECmdAccessoryUpdated, //Informs change of the connected accessory cababilitis        
    ECmdGetObjectValue, 
    ECmdSetObjectValue, 
    ECmdBadFunction = 0xffff // Error tracking
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // ASYCMDTYPES_H

// End of File
