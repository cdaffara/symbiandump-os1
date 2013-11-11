/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Type definitions for Accessory Framework
*
*/


#ifndef ACCESSORYTYPES_H
#define ACCESSORYTYPES_H

//  INCLUDES
#include <AccPolGenericID.h>
#include <AccPolGenericIDArray.h>
#include <AccPolAccessoryMode.h>
#include <AccPolNameRecord.h>

// CONSTANTS

// HW device types used in Accessory Control API's settings part
const TUint32 KASNoDevice      = 0x0;    // 00000000000000000000
const TUint32 KASTTY           = 0x1;    // 00000000000000000001
const TUint32 KASLoopset       = 0x2;    // 00000000000000000010
const TUint32 KASHeadset       = 0x4;    // 00000000000000000100
const TUint32 KASHeadphones    = 0x8;    // 00000000000000001000
const TUint32 KASLineOut       = 0x10;   // 00000000000000010000
const TUint32 KASTVOut         = 0x20;   // 00000000000000100000
const TUint32 KASMusicStand    = 0x40;   // 00000000000001000000

// MACROS

// DATA TYPES
struct TAccValueTypeTBool
    {
    TBool iValue;
    };

struct TAccValueTypeTInt
    {
    TInt iValue;
    };

struct TAccValueTypeObject
    {
public:
    inline TAccValueTypeObject( TPtr8 aValue ):
        iValue( aValue )
        {
        }
public:
    TPtr8 iValue;
    };

struct TAccSrvGIDNameRecord
    {
    TAccPolGenericID  iGenericID;
    TAccPolNameRecord iNameRecord;
    };
    
//Used in ConnectAccessory() to pack parameters
//(for Accessory Server internal use)
struct TAccSrvConnectRecord
    {
    TAccSrvConnectRecord():iCapsPtr( NULL, 0, 0 ){};
    TInt  iNameValueBufSize;
    TUint iThreadID;
    TBool iEvaluateConnectionRules;
    TPtr8 iCapsPtr;  
    };
      
enum TAccAudioType
    {
    EAccMonoAudio,
    EAccStereoAudio
    };

typedef TPckg    <TAccPolGenericID>      TAccSrvGenericIDPckg;
typedef TPckgC   <TAccPolGenericID>      TAccSrvGenericIDPckgC;
typedef TPckgBuf <TAccPolGenericID>      TAccSrvGenericIDPckgBuf;
typedef TPckg    <TAccPolGenericIDArray> TAccSrvGenericIDArrayPckg;
typedef TPckgC   <TAccPolGenericIDArray> TAccSrvGenericIDArrayPckgC;
typedef TPckgC   <TAccAudioType>         TAccSrvAudioTypePckgC;
typedef TPckg    <TAccAudioType>         TAccSrvAudioTypePckg;
typedef TPckgBuf <TAccAudioType>         TAccSrvAudioTypePckgBuf;
typedef TPckg    <TAccPolAccessoryMode>  TAccSrvAccessoryModePckg;
typedef TPckgC   <TAccPolNameRecord>     TAccSrvNameRecordPckgC;
typedef TPckg    <TAccPolNameRecord>     TAccSrvNameRecordPckg;
typedef TPckgBuf <TAccPolNameRecord>     TAccSrvNameRecordPckgBuf;
typedef TPckg    <TAccValueTypeTBool>    TAccSrvValueTypeTBoolPckg;
typedef TPckgBuf <TAccValueTypeTBool>    TAccSrvValueTypeTBoolPckgBuf;
typedef TPckg    <TAccValueTypeTInt>     TAccSrvValueTypeTIntPckg;
typedef TPckgBuf <TAccValueTypeTInt>     TAccSrvValueTypeTIntPckgBuf;
typedef TPckgBuf <TAccSrvGIDNameRecord>  TAccSrvGIDNameRecordPckgBuf;
typedef TPckg    <TAccSrvGIDNameRecord>  TAccSrvGIDNameRecordPckg;
typedef TPckgBuf <TAccSrvConnectRecord>  TAccSrvConnectRecordPckgBuf;
typedef TPckg    <TAccSrvConnectRecord>  TAccSrvConnectRecordPckg;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // ACCESSORYTYPES_H

// End of File
