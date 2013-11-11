/*
* Copyright (c) 2008,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of EDID data types.
*
*/

#ifndef EDIDDATATYPES_H
#define EDIDDATATYPES_H

#include <e32base.h>

/** Every EDID-block is 128 bytes long (padded to 128bytes if shorter) */
const TUint KEdidParserSizeOfEdidBlock = 128;

/** EDID header size */
const TUint KEdidParserSizeOfEdidHeader = 8;

/** Valid base edid header */
const TUint8 KBaseEdidHeader[KEdidParserSizeOfEdidHeader] = {
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };

/** Year of manufacture stored in EDID data is "Year of Manufacture - 1990" */
const TUint KEdidBaseYear = 1990;

/** Padding byte */
const TUint KEdidPaddingByte = 0x00;

/** DDC segment size */
const TUint KEdidParserSizeOfSegment = 256;

const TUint KEdidParserManufacturerCodeLength = 3;

/** EDID data block including extension */
typedef TUint8 TDataBlock[KEdidParserSizeOfSegment];

/** One extension data block array (block size is 128 bytes) */
typedef TUint8 TExtDataBlock[KEdidParserSizeOfEdidBlock];

/** EDID manufacturer code */
typedef TBuf8<KEdidParserManufacturerCodeLength> TEdidVendorCode;

/** Aspect Ratio */
struct TEdidAspectRatio
    {
    TUint8 iLandscape;
    TUint8 iPortrait;
    };

/** Screen Size (Horizontal and vertical in cm) */
struct TEdidScreenSize
    {
    TUint8 iHorizontal;
    TUint8 iVertical;
    };

/** Product vendor information */
struct TEdidVendorProductInfo
    {
    TUint16 iManufacturerCode;
    TUint16 iProductId;
    TUint8  iSerial[4];
    TUint8  iWeek;
    TUint8  iYear;
    };

/**
 * EDID block structure
 */
struct TEdidBaseBlock
    {
    TUint8 iHeader[8];                         // 8 Bytes
    TEdidVendorProductInfo iVendorProductInfo; // 10 Bytes
    TUint8 iVersion;                           // 1 Byte
    TUint8 iRevision;                          // 1 Byte
    TUint8 iVideoInputDefinition;              // 1 Byte
    TEdidScreenSize iScreenSize;               // 2 Bytes
    TUint8 iDisplayTransferCharasteristics;    // 1 Byte
    TUint8 iFeatureSupportByte;                // 1 Byte
    TUint8 iColorCharasteristics[10];          // 10 Bytes
    TUint8 iEstablishedTimings[3];             // 3 Bytes
    TUint8 iStdTimingIdentifications[16];      // 16 Bytes
    TUint8 iTimingDescriptor[18];              // 18 Bytes
    TUint8 iSecondTimingDescriptor[18];        // 18 Bytes
    TUint8 iMonitorDescriptor[18];             // 18 Bytes
    TUint8 iMonitorRangeLimitsDescriptor[18];  // 18 Bytes
    TUint8 iExtensionNumber;                   // 1 Byte
    TUint8 iChecksum;                          // 1 Byte
    };

/**
 * Stereo Viewing Support
 */
enum TEdidStereoViewingSupport
    {
    /** Normal Display – No Stereo. The value of bit 0 is "don't care" */
    ENormalDisplay = 0,
    /** Field sequential stereo, right image when stereo sync signal = 1 */
    EFieldSequentialStereoRightWhenStereoSyncSignal1,
    /** Field sequential stereo, left image when stereo sync signal = 1 */
    EFieldSequentialStereoLeftWhenStereoSyncSignal1,
    /** 2-way interleaved stereo, right image on even lines */
    ETwoWayInterleavedStereoRightImageOnEvenLines,
    /** 2-way interleaved stereo, left image on even lines */
    ETwoWayInterleavedStereoLeftImageOnEvenLines,
    /** 4-way interleaved stereo */
    EFourWayInterleaverStereo,
    /** Side-by-Side interleaved stereo */
    ESideBySideInterleavedStere,
    /**  Unknown */
    EUnknownStereoViewingSupport = 0xff
    };

/**
 * Sync Signal Definitions
 */
enum TEdidSyncSignalDefinitions
    {
    /** Analog: */
    EAnalogCompositeSyncWithoutSerrationsSyncSyncOnGreenSignalOnly = 0,
    EAnalogCompositeSyncWithSerrationsSyncSyncOnGreenSignalOnly,
    EAnalogCompositeSyncWithoutSerrationsSyncOnAllThreeVideoSignals,
    EAnalogCompositeSyncWithSerrationsSyncOnAllThreeVideoSignals,
    EAnalogBipolarAnalogCompositeSyncWithoutSerrationsSyncSyncOnGreenSignalOnly,
    EAnalogBipolarAnalogCompositeSyncWithoutSerrationsSyncOnAllThreeVideoSignals,
    EAnalogBipolarAnalogCompositeSyncWithSerrationsSyncSyncOnGreenSignalOnly,
    EAnalogBipolarAnalogCompositeSyncWithSerrationsSyncOnAllThreeVideoSignals,
    /** Digital: */
    EDigitalCompositeSyncWithoutSerrations,
    EDigitalCompositeSyncWithSerrations,
    EDigitalSeparateSyncVerticalSyncIsNegativeHorizontalSyncIsNegative,
    EDigitalSeparateSyncVerticalSyncIsNegativeHorizontalSyncIsPositive,
    EDigitalSeparateSyncVerticalSyncIsPositiveHorizontalSyncIsNegative,
    EDigitalSeparateSyncVerticalSyncIsPositiveHorizontalSyncIsPositive,
    /** Unknown: */
    EUnknownSignalDefinition
    };

/**
 * EDID descriptor block type
 */
enum TEdidDescriptorBlockType
    {
    EEdidDescriptorBlockFirstTiming         = 1,
    EEdidDescriptorBlockSecondTiming        = 2,
    EEdidDescriptorBlockMonitorTiming       = 3,
    EEdidDescriptorBlockMonitorRangeLimit   = 4
    };

/**
 * EDID descriptor block
 */
struct TEdidDescriptorBlock
    {
    TUint16 iPixelClock;                        //16 bits
    TUint16 iHorizontalAddressableVideoPixels;  //12 bit long   (in pixels)
    TUint16 iHorizontalBlanking;                //12 bit long
    TUint16 iVerticalAddressableVideoPixels;    //12 bit long   (in pixels)
    TUint16 iVerticalBlanking;                  //12 bit long
    TUint16 iHorizontalFrontPorch;              //12 bit long
    TUint16 iHorizontalSyncPulse;               //12 bit long
    TUint16 iVerticalFrontPorch;                //12 bit long
    TUint16 iVerticalSyncPulse;                 //12 bit long
    TUint16 iHorizontalAddressableImageSize;    //12 bit long   (in millimeters)
    TUint16 iVerticalAddressableImageSize;      //12 bit long   (in millimeters)
    TUint8  iHorizontalBorder;                  //8 bits
    TUint8  iVerticalBorder;                    //8 bits
    TBool   iInterlacedVideo;                   //1 bit
    TEdidStereoViewingSupport iStereoSupport;
    TEdidSyncSignalDefinitions iSyncs;          //sync signal definitions
    };

/**
 * EDID extension types
 */
enum TEdidExtensionTagCode
    {
    ECea861Ext  = 0x02,     // CEA-861 Series Timing Extension
    EVTBExt     = 0x10,     // Video Timing Block Extension (VTB-EXT)
    EEDID20     = 0x20,     // EDID 2.0 Extension
    EDIExt      = 0x40,     // Display Information Extension (DI-EXT)
    ELSExt      = 0x50,     // Localized String Extension (LS-EXT)
    EDPVLExt    = 0x60,     // Digital Packet Video Link Extension (DPVL-EXT)
    EUnknownExt = 0xff      // Extension is unknown
    };

/**
 * Established timings 1
 */
enum TEdidEstablishedTimings1
    {
    E800x600_60Hz   = 0x01, // 0000 0001
    E800x600_56Hz   = 0x02, // 0000 0010
    E640x480_75Hz   = 0x04, // 0000 0100
    E640x480_72Hz   = 0x08, // 0000 1000
    E640x480_67Hz   = 0x10, // 0001 0000
    E640x480_60Hz   = 0x20, // 0010 0000
    E720x400_88Hz   = 0x40, // 0100 0000
    E720x400_70Hz   = 0x80  // 1000 0000
    };

/**
 * Established timings 2
 */
enum TEdidEstablishedTimings2
    {
    E1280x1024_75Hz   = 0x01, // 0000 0001
    E1024x768_75Hz    = 0x02, // 0000 0010
    E1024x768_70Hz    = 0x04, // 0000 0100
    E1024x768_60Hz    = 0x08, // 0000 1000
    E1024x768_87Hz    = 0x10, // 0001 0000
    E832x624_75Hz     = 0x20, // 0010 0000
    E800x600_75Hz     = 0x40, // 0100 0000
    E800x600_72Hz     = 0x80, // 1000 0000
    };

#endif // EDIDDATATYPES_H
