// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file contains step class declaration for DevVideo integration tests.
// 
//


#ifndef TSI_MMF_DEVVIDEO_STEP_H
#define TSI_MMF_DEVVIDEO_STEP_H

// system includes
#include <testframework.h>
#include <mmf/devvideo/videoplayhwdevice.h>

// forward declaration
class CTestSuiteDevVideo;

// size constant declarations
const TInt KNameBufSize = 25;
const TInt KSizeBuf = 256;
const TInt KHeaderBytesSize = 1024;
const TInt KInputBufferSize = 8192;

// Test compressed video format utility function
static inline CCompressedVideoFormat* GetTestCVFormatL( const TDesC8& aMimeType )
    {
    CCompressedVideoFormat* temp = CCompressedVideoFormat::NewL( aMimeType, KNullDesC8);
    return temp;
    }

// Position constants
const TInt KTestPosition = 12345;
const TInt KTestDecodePosition = 256;
const TInt KTestPlayPosition = 512;
const TInt KTestRecordPosition = 100000;

// Video header information
const TVideoDataUnitType KTestDataUnitType           = EDuCodedPicture;
const TVideoDataUnitEncapsulation KTestDataUnitEncap = EDuElementaryStream;
const TVideoPictureHeader::THeaderOptions KTestPictureHeaderOptions  = TVideoPictureHeader::EPresentationTimestamp;
const TVideoInputBuffer::TVideoBufferOptions KTestInputBufferOptions = TVideoInputBuffer::EPresentationTimestamp;
const TInt KTestInputBufferTimestamp = 1000000;
_LIT8(KTestInputBufferData, "I'm a lumber jack and I'm OK!");

// Invalid video HwDevice UIDs
const TUid KInvalidUidDevVideoTestDecodeHwDevice = {0x1001F7D3};
const TUid KInvalidUidDevVideoTestEncodeHwDevice = {0x1001F7D3};

// unsupported Video formats
const TUncompressedVideoFormat KTestPostProcInfoNegativeFormat = {EYuvRawData};

// Conversion matrices
const TYuvConversionMatrix KTestYuvMatrix1 = {{1, 2, 3}, {4, 5, 6, 7, 8, 9, 10, 11, 12}, {13, 14, 15}};
const TYuvConversionMatrix KTestYuvMatrix2 = {{-1, -2, -3}, {-4, -5, -6, -7, -8, -9, -10, -11, -12}, {-13, -14, -15}};
const TYuvFormat KTestYuvFormat1 = {EYuvRange0, EYuv420Chroma1, EYuvDataPlanar, const_cast <TYuvConversionMatrix*> (&KTestYuvMatrix1), const_cast <TYuvConversionMatrix*> (&KTestYuvMatrix2), 10, 20};

// XviD Devices stuff //
// XviD HwDevice UIDs
const TUid KUidDevVideoXvidDecodeHwDeviceUid = {0x10273775};
const TUid KUidDevVideoXvidEncodeHwDeviceUid = {0x10273777};
// XviD HwDevice PUs
const TUid KUidTestVideoPuXvidDecoderDevice = {0x102737C4};
const TUid KUidTestVideoPuXvidEncoderDevice = {0x102737C2};
//const TUid KUidTestVideoPuXvidDecoderDevice = {0x10273775};
//const TUid KUidTestVideoPuXvidEncoderDevice = {0x10273777};

//
// DevVideo test Devices stuff //
// DevVideo Test HwDevice UIDs
const TUid KUidTestVideoDecodeHwDeviceAdapter = {0x102737ED};
const TUid KUidTestVideoEncodeHwDeviceAdapter = {0x102737EB};
const TUid KUidDevVideoTestDecodeHwDevice1 = {0x101F7D93};
const TUid KUidDevVideoTestEncodeHwDevice1 = {0x101FFA02};
// DevVideo Test HwDevice PUs
const TUid KUidTestVideoDecodeStubPU = {0x102737E6};
const TUid KUidTestVideoEncodeStubPU = {0x102737E4};
//

// -- Decoder Info
const TUint32 KXvidDecoderInfoCount = 1;
_LIT8(KXvidDecoderMimeType, "video/mp4v-es2");
_LIT8(KTestDecoderMimeType, "video/x-symbiantest");
_LIT(KXvidDecoderInfoManufacturer, "Symbian Ltd.");
_LIT8(KXvidDecoderInfoCSInfo, "Coded by Symbian");
_LIT8(KXvidDecoderInfoISInfo, "Implemented by Symbian");
const TInt KXvidDecoderInfoVersionMaj = 0;
const TInt KXvidDecoderInfoVersionMin = 1;
const TInt KXvidDecoderInfoVersionBuild = 1;
const TInt KXvidDecoderInfoMaxPicSizeX = 640;
const TInt KXvidDecoderInfoMaxPicSizeY = 480;
const TInt KXvidDecoderInfoMaxPicRate = 30;

// -- Encoder Info
const TUint32 KXvidEncoderInfoCount = 1;
_LIT8(KXvidEncoderMimeType, "video/mp4v-es2");
_LIT8(KTestEncoderMimeType, "video/x-symbiantest");
_LIT(KXvidEncoderInfoManufacturer, "Symbian Ltd.");
_LIT8(KXvidEncoderInfoCSInfo, "Coded by Symbian");
_LIT8(KXvidEncoderInfoISInfo, "Implemented by Symbian");
const TInt KXvidEncoderInfoVersionMaj = 0;
const TInt KXvidEncoderInfoVersionMin = 1;
const TInt KXvidEncoderInfoVersionBuild = 1;
const TInt KXvidEncoderInfoMaxSizeX = 640;
const TInt KXvidEncoderInfoMaxSizeY = 480;
const TReal KXvidEncoderInfoMaxRate = 30.0;
const TUint KXvidEncoderInfoMaxUEPLevels = 1;
const TUint KXvidEncoderInfoMaxBitrate = KMaxTUint32;
const TUint KXvidEncoderInfoMaxILSSteps = 1;
const TUint32 KXvidEncoderInfoPictureOptions = 0;
const TUint KXvidNumBitrateLayers = 1;
const TVideoDataUnitType KXvidUnitType1 = EDuCodedPicture;
const TVideoDataUnitEncapsulation KXvidEncapType1 = EDuElementaryStream;
const TInt KXvidEncoderInfoMaxPicSizeX = 640;
const TInt KXvidEncoderInfoMaxPicSizeY = 480;
const TReal KXvidEncoderInfoMaxPicRate = 30.0;

// Test compressed XviD video format utility function
static inline CCompressedVideoFormat* GetXvidCVFormatL()
    {
    CCompressedVideoFormat* temp = CCompressedVideoFormat::NewL( KXvidDecoderMimeType, KNullDesC8 );
    return temp;
    }

// local function to get the drive name
TAny GetDriveName(TDes& aFilename);

// DevVideoPlay events
enum TMmfDevVideoPlayEvents
    {
    EDevVideoPlayInit = 0,
    EDevVideoPlayStart
    };

// DevVideoRecord events
enum TMmfDevVideoRecordEvents
    {
    EDevVideoRecordInit = 0,
    EDevVideoRecordStart
    };

// Type of test whether it is normal test or considers Uid, for example
enum TTestType
    {
    ENormalTest = 0,
    EUidTest,           // Specifies whether a test uses Uid or not, during initialization.
    EInvalidUidTest,    // Specifies whether a test uses invalid Uid for initialization.
    ENoVideoTypeTest    // Specifies a test with no video type set.
    };

/**
 *
 * RTestStepDevVideoBase
 * - base class for all test steps.
 *
 */
class RTestStepDevVideoBase : public RTestStep
    {
public:
    RTestStepDevVideoBase();

    // Set the suite name
    void SetTestSuite(const CTestSuiteDevVideo* aTestSuite);

protected:
    // Stores error values for testing
    TInt iError;

    // Section name for retrieving filename
    TBuf<KNameBufSize> iSectName;

    // Key name for retrieving filename
    TBuf<KNameBufSize> iKeyName;

    // Active Scheduler
    CActiveScheduler* iActiveScheduler;

    // Pointer to suite which owns this test
    const CTestSuiteDevVideo* iTestSuiteDevVideo;

   // Filename of video file
    TBuf<KSizeBuf> iFilename;

    // Creating and opening the video file for playing and recording
    RFile iFile;

    // Handle to a file server session
    RFs iFs;

    // Whether video HwDevice is reconfigured or not
    TBool iReConfigure;

    // Keep track on number of times MdvroStreamEnd/MdvroStreamEnd callback is called
    TUint iEndCount;

    TTestType iTestType;
    };

#endif    // TSI_MMF_DEVVIDEO_STEP_H
