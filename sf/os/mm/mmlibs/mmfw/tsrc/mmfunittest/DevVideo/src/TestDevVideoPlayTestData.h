// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: DevVideoPlay test data.
// 
//

/**
 @file TestDevVideoPlayTestData.h
*/

#ifndef __TESTDEVVIDEOPLAYTESTDATA_H__
#define __TESTDEVVIDEOPLAYTESTDATA_H__

#include "TSU_MMF_DevVideo.h"


// mime types
_LIT8(KTestMimeType1, "video/testvideo");

// optional data
_LIT8(KTestOptional1, "blah blah blah blah blah");

// test uncompressed video formats
const TUncompressedVideoFormat KTestVidFormat1 = {ERgbRawData};
const TUncompressedVideoFormat KTestVidFormat2 = {ERgbFbsBitmap};
const TUncompressedVideoFormat KTestVidFormat3 = {EYuvRawData};

// test unit and encapsulation types
const TVideoDataUnitType KTestUnitType1				= EDuCodedPicture;
const TVideoDataUnitEncapsulation KTestEncapType1	= EDuElementaryStream;

// test compressed video format utility function
static inline CCompressedVideoFormat* GetTestCVFormatL( const TDesC8& aMimeType )
	{
	CCompressedVideoFormat* temp = CCompressedVideoFormat::NewL( aMimeType, KTestOptional1 );
 	return temp;
	}

// test pre-process types
const TUint32 KTestProcessType1 = EPpInputCrop | EPpMirror | EPpRotate;
const TUint32 KTestProcessType2 = EPpScale | EPpOutputCrop | EPpOutputPad;

// input crop rectangles
const TInt KTestInputCropRectA = 0;
const TInt KTestInputCropRectB = 50;
const TInt KTestInputCropRectC = 100;
const TInt KTestInputCropRectD = 150;

// KTestInputCropRect1 = A, B, C, D;
// KTestInputCropRect2 = D, C, B, A;

// yuv to rgb options
const TYuvToRgbOptions KTestYuvToRgb1 = {50, 50, 50, 0.5, EDitherNone};
const TYuvToRgbOptions KTestYuvToRgb2 = {100, 100, 100, 1.0, EDitherErrorDiffusion};
static inline TBool CompareYuvRgbOptions(const TYuvToRgbOptions& a, const TYuvToRgbOptions& b)
	{
	if ( (a.iLightness == b.iLightness) && (a.iSaturation == b.iSaturation) &&		 
		 (a.iContrast == b.iContrast) && (a.iGamma == b.iGamma) &&
		 (a.iDitherType == b.iDitherType) )
		 return ETrue;
	else return EFalse;
	}


// conversion matrices
const TYuvConversionMatrix KTestYuvMatrix1 = {{1, 2, 3}, {4, 5, 6, 7, 8, 9, 10, 11, 12}, {13, 14, 15}};
const TYuvConversionMatrix KTestYuvMatrix2 = {{-1, -2, -3}, {-4, -5, -6, -7, -8, -9, -10, -11, -12}, {-13, -14, -15}};

static inline TBool CompareYuvMatrices(const TYuvConversionMatrix& a, const TYuvConversionMatrix& b)
	{
	TBool same = ETrue;
	TInt x = 0;

	for (x = 0; x < 3; x++)
		{
		if ( (a.iPostOffset[x] != b.iPostOffset[x]) ||
			 (a.iPreOffset[x] != b.iPreOffset[x]) )
			{
			same = EFalse;
			}
		}
		
	for (x = 0; x < 9; x++)
		{
		if (a.iMatrix[x] != b.iMatrix[x]) 
			{
			same = EFalse;
			}
		}
		
	return same;
	}

// yuv formats
const TYuvFormat KTestYuvFormat1 = {EYuvRange0, EYuv420Chroma1, EYuvDataPlanar, const_cast <TYuvConversionMatrix*> (&KTestYuvMatrix1), const_cast <TYuvConversionMatrix*> (&KTestYuvMatrix2), 10, 20};
const TYuvFormat KTestYuvFormat2 = {EYuvRange1, EYuv420Chroma2, EYuvDataInterleavedLE, const_cast <TYuvConversionMatrix*> (&KTestYuvMatrix2), const_cast <TYuvConversionMatrix*> (&KTestYuvMatrix1), 5, 10};

static inline TBool CompareYuvFormats(const TYuvFormat& a, const TYuvFormat& b)
	{
	if ( (a.iCoefficients == b.iCoefficients) && 
		 (a.iPattern == b.iPattern) &&		 
		 (a.iDataLayout == b.iDataLayout) && 
		 (CompareYuvMatrices(*a.iYuv2RgbMatrix, *b.iYuv2RgbMatrix)) &&
		 (CompareYuvMatrices(*a.iRgb2YuvMatrix, *b.iRgb2YuvMatrix)) &&
		 (a.iAspectRatioNum == b.iAspectRatioNum) &&
		 (a.iAspectRatioDenom == b.iAspectRatioDenom) )
		 return ETrue;
	else return EFalse;
	}

// rgb formats
const TRgbFormat KTestRgbFormat1 = ERgb16bit444;
const TRgbFormat KTestRgbFormat2 = EFbsBitmapColor16M;

// rotation types
const TRotationType KTestRotate1 = ERotate90Clockwise;
const TRotationType KTestRotate2 = ERotate90Anticlockwise;

// test scale size
const TInt KTestScaleX = 100; 
const TInt KTestScaleY = 50;

// output crop rectangles
const TInt KTestOutputCropRectA = 0;
const TInt KTestOutputCropRectB = 50;
const TInt KTestOutputCropRectC = 256;
const TInt KTestOutputCropRectD = 350;

// KTestOutputCropRect1 = A, B, C, D;
// KTestOutputCropRect2 = D, C, B, A;

// post-processor options
_LIT8(KTestPostProcOptions1, "abcdefghijklmno");
_LIT8(KTestPostProcOptions2, "123456-7-654321");

// buffer options creation utility function
static inline CMMFDevVideoPlay::TBufferOptions GetTestBufferOptions( void )
	{
	CMMFDevVideoPlay::TBufferOptions buffOptions;
 	
	buffOptions.iPreDecodeBufferSize = 10;
	buffOptions.iMaxPostDecodeBufferSize = 20;
	buffOptions.iPreDecoderBufferPeriod = 30;
	buffOptions.iPostDecoderBufferPeriod = 40;
	buffOptions.iMaxInputBufferSize = 50;
	buffOptions.iMinNumInputBuffers = 60;

	return buffOptions;
	}

// buffer options comparison utility function 
static inline TBool CompareBufferOptions(const CMMFDevVideoPlay::TBufferOptions& a, const CMMFDevVideoPlay::TBufferOptions& b)
	{
	if ( (a.iPreDecodeBufferSize == b.iPreDecodeBufferSize) && 
		 (a.iMaxPostDecodeBufferSize == b.iMaxPostDecodeBufferSize) &&		 
		 (a.iPreDecoderBufferPeriod == b.iPreDecoderBufferPeriod) && 
		 (a.iPostDecoderBufferPeriod == b.iPostDecoderBufferPeriod) &&
		 (a.iMaxInputBufferSize == b.iMaxInputBufferSize) &&
		 (a.iMinNumInputBuffers == b.iMinNumInputBuffers) )
		 return ETrue;
	else return EFalse;
	}

// Hrd/Vbv test data
const THrdVbvSpecification KTestHrdVbvSpec = EHrdVbv3GPP;
_LIT8(KTestHrdVbvParams, "not exactly a package of parameters but never mind");

// direct screen access test data
const TInt KTestDSARectA = 0;
const TInt KTestDSARectB = 100;
const TInt KTestDSARectC = 200;
const TInt KTestDSARectD = 300;

// KTestDSARect1 = A, B, C, D;
// KTestDSARect2 = D, C, B, A;

// KTestDSARegion1 = {KTestDSARect1};
// KTestDSARegion2 = {KTestDSARect2};

// position test data
const TInt KTestPosition = 12345;
const TInt KTestPositionFatal = 666;
const TInt KTestDecodePosition = 256;
const TInt KTestPlayPosition = 512;

// buffer bytes test data
const TUint KTestPreDecoderBytes = 1024;
const TUint KTestPictureBytes = 2048;

// picture counters creation utility function
static inline CMMFDevVideoPlay::TPictureCounters GetTestPictureCounters( void )
	{
	CMMFDevVideoPlay::TPictureCounters picCount;
 	
	picCount.iPicturesSkipped = 10;
	picCount.iPicturesDecoded = 20;
	picCount.iPicturesDisplayed = 30;
	picCount.iTotalPictures = 40;

	return picCount;
	}

// picture counters comparison utility function 
static inline TBool ComparePictureCounters(const CMMFDevVideoPlay::TPictureCounters& a, const CMMFDevVideoPlay::TPictureCounters& b)
	{
	if ( (a.iPicturesSkipped == b.iPicturesSkipped) && 
		 (a.iPicturesDecoded == b.iPicturesDecoded) &&		 
		 (a.iPicturesDisplayed == b.iPicturesDisplayed) && 
		 (a.iTotalPictures == b.iTotalPictures) )
		 return ETrue;
	else return EFalse;
	}

// bitstream counters creation utility function
static inline CMMFDevVideoPlay::TBitstreamCounters GetTestBitstreamCounters( void )
	{
	CMMFDevVideoPlay::TBitstreamCounters bitCount;
 	
	bitCount.iLostPackets = 256;
	bitCount.iTotalPackets = 512;
	
	return bitCount;
	}

// bitstream counters comparison utility function 
static inline TBool CompareBitstreamCounters(const CMMFDevVideoPlay::TBitstreamCounters& a, const CMMFDevVideoPlay::TBitstreamCounters& b)
	{
	if ( (a.iLostPackets == b.iLostPackets) && 
		 (a.iTotalPackets == b.iTotalPackets) )
		 return ETrue;
	else return EFalse;
	}

// test num buffers
const TUint KTestNumFreeBuffers = 4096;

// complexity levels
const TUint KTestComplexityLevel1 = 1;
const TUint KTestComplexityLevel2 = 3;
const TUint KTestNumComplexityLevels1 = 5;
const TUint KTestNumComplexityLevels2 = 10;

// complexity level info creation utility function
static inline CMMFDevVideoPlay::TComplexityLevelInfo GetTestLevelInfo( TInt a )
	{
	CMMFDevVideoPlay::TComplexityLevelInfo levInfo;
 	
	levInfo.iOptions = CMMFDevVideoPlay::TComplexityLevelInfo::EAvgPictureRate;
	levInfo.iAvgPictureRate = 1.5;
	levInfo.iPictureSize.SetSize(a * 100,a * 200);
	levInfo.iRelativeImageQuality = 0.5;
	levInfo.iRequiredMIPS = a;
	levInfo.iRelativeProcessTime = 0.3;
	
	return levInfo;
	}

// complexity level info comparison utility function 
static inline TBool CompareLevelInfos(const CMMFDevVideoPlay::TComplexityLevelInfo& a, const CMMFDevVideoPlay::TComplexityLevelInfo& b)
	{
	if ( (a.iOptions == b.iOptions) && 
		 (a.iAvgPictureRate == b.iAvgPictureRate) &&
		 (a.iPictureSize == b.iPictureSize) &&
		 (a.iRelativeImageQuality == b.iRelativeImageQuality) &&
		 (a.iRequiredMIPS == b.iRequiredMIPS) &&
		 (a.iRelativeProcessTime == b.iRelativeProcessTime) )
		 return ETrue;
	else return EFalse;
	}

// picture timestamp test data
const TInt KTestPictureTimestamp = 12345;

// input buffer test data
_LIT8(KTestBufferString, "Roobarb and Custard!");
const TUint32 KTestBufferOptions = TVideoInputBuffer::EDecodingTimestamp;
const TUint KTestBufferSize = 20;

// decoder info
const TUint32 KTestDecoderInfoCount = 3;

const TText8* const KTestDecoderInfoMimeArray[KTestDecoderInfoCount] = 
	{
	_S8("video/bunglevideo"), 
	_S8("video/zippyvideo"),
	_S8("video/georgevideo")
	};

_LIT(KTestDecoderInfoManufacturer, "fred bloggs plc");
_LIT(KTestDecoderInfoIdentifier, "video funkifier");

const TInt KTestDecoderInfoMaxSizeX = 256;
const TInt KTestDecoderInfoMaxSizeY = 512;

_LIT8(KTestDecoderInfoCSInfo, "coded by a blind monkey");
_LIT8(KTestDecoderInfoISInfo, "implemented by a baboon");

const TInt KTestDecoderInfoVersionMaj = 1;
const TInt KTestDecoderInfoVersionMin = 2;
const TInt KTestDecoderInfoVersionBuild = 3;
// KTestDecoderInfoVersion = Maj.Min.Build;

// post processor info
const TUint32 KTestPostProcInfoCount = 3;
_LIT(KTestPostProcInfoManufacturer, "honest sid inc");
_LIT(KTestPostProcInfoIdentifier, "video post-funkifier");

_LIT8(KTestPostProcInfoISInfo, "implemented by a lame pigeon");

const TInt KTestPostProcInfoVersionMaj = 1;
const TInt KTestPostProcInfoVersionMin = 2;
const TInt KTestPostProcInfoVersionBuild = 3;
// KTestPostProcInfoVersion = Maj.Min.Build;

const TUint32 KTestPostProcInfoRotations = ERotate90Clockwise | ERotate90Anticlockwise | ERotate180;

const TYuvToRgbCapabilities KTestPostProcInfoYuvToRgbCaps = {1, 2, 3, ETrue, EFalse, ETrue, EFalse, 4};
static inline TBool CompareYuvRgbCaps(const TYuvToRgbCapabilities& a, const TYuvToRgbCapabilities& b)
	{
	if ( (a.iSamplingPatterns == b.iSamplingPatterns) && (a.iCoefficients == b.iCoefficients) &&		 
		 (a.iRgbFormats == b.iRgbFormats) && (a.iLightnessControl == b.iLightnessControl) &&
		 (a.iSaturationControl == b.iSaturationControl) && (a.iContrastControl == b.iContrastControl) &&
		 (a.iGammaCorrection == b.iGammaCorrection) && (a.iDitherTypes == b.iDitherTypes) )
		 return ETrue;
	else return EFalse;
	}

const TUncompressedVideoFormat KTestPostProcInfoFormatArray[KTestPostProcInfoCount] =
	{{ERgbRawData}, {ERgbFbsBitmap}, {EYuvRawData}};

const TUint32 KTestPostProcInfoCombsArray[KTestPostProcInfoCount] =
	{KTestProcessType1, KTestProcessType2, EPpNoiseFilter | EPpColorEnhancement | EPpFrameStabilisation};

const TScaleFactor KTestPostProcInfoScaleFactorsArray[KTestPostProcInfoCount] =
	{{1, 2}, {1, 4}, {1,8}};

static inline TBool CompareScaleFactors(const TScaleFactor& a, const TScaleFactor& b)
	{
	if ((a.iScaleNum == b.iScaleNum) && (a.iScaleDenom == b.iScaleDenom))
		return ETrue;
	else return EFalse;
	}

const TUncompressedVideoFormat KTestPostProcInfoNegativeFormat = {EYuvRawData};
const TUint32 KTestPostProcInfoNegativeComb = EPpCustom;


// video header information
const TVideoDataUnitType KTestDataUnitType = EDuCodedPicture;
const TVideoDataUnitEncapsulation KTestDataUnitEncap = EDuElementaryStream;
const TVideoInputBuffer::TVideoBufferOptions KTestInputBufferOptions = TVideoInputBuffer::EPresentationTimestamp; 
const TVideoPictureHeader::THeaderOptions KTestPictureHeaderOptions = TVideoPictureHeader::EPresentationTimestamp;
const TInt KTestInputBufferTimestamp = 1000000;
const TInt KTestPictureHeaderTimestamp = 2000000;
_LIT8(KTestInputBufferData, "I'm a lumber jack and I'm OK!");

// TPictureData
const TInt KTestPictureDataSizeX = 57;
const TInt KTestPictureDataSizeY = 18;

// snapshot data
const TInt KTestSnapshotTimestamp = 1000;
const TPictureId KTestSnapshotId = {TPictureId::ETemporalReference, 999};

// clock deviation
const TUint KTestClockDeviationMS	 = 500000;		// 0.5 seconds
const TUint KTestClockBigDeviationMS = 2000000;		// 2 seconds
const TUint KTestClock2Seconds	  = 2000000;
const TUint KTestClock4Seconds	  = 4000000;
const TUint KTestClock18Seconds	  = 18000000;
const TUint KTestClock20Seconds	  = 20000000;
const TUint KTestClock22Seconds	  = 22000000;

// devvideorecord specific test data
const TInt KTestInputSize1X = 640; 
const TInt KTestInputSize1Y = 480;

const TInt KTestInputSize2X = 320; 
const TInt KTestInputSize2Y = 200;

const TInt KTestCamHandlePre = 1;
const TInt KTestCamHandleEnc = 2;
const TInt KTestCamHandleFatal = 666;

const TReal KTestPictureRate = 29.97;

// rgb data
const TRgbRange KTestRgbRange1 = ERgbRangeFull;
const TRgbRange KTestRgbRange2 = ERgbRange16to235;

// output pad
const TInt KTestPadX = 100;
const TInt KTestPadY = 200;
const TInt KTestPadPointX = 300;
const TInt KTestPadPointY = 400;

// color enhancement
const TColorEnhancementOptions KTestColorEnhance1 = {1, 2, 3};
const TColorEnhancementOptions KTestColorEnhance2 = {4, 5, 6};

static inline TBool CompareColorEnhancements(const TColorEnhancementOptions& a, const TColorEnhancementOptions& b)
	{
	if ( (a.iLightness == b.iLightness) && 
		 (a.iSaturation == b.iSaturation) && 
		 (a.iContrast == b.iContrast) )
		 return ETrue;
	else return EFalse;
	}

// custom pre process options
_LIT8(KTestCustomPreProc1, "rhubarb rhubarb rhubarb!");
_LIT8(KTestCustomPreProc2, "custard custard custard?");

const TReal KTestRandomAccessRate = 0.555;
const TUint KTestNumBitrateLayers = 369;

const TUint KTestLayer = 12;
const TScalabilityType KTestScaleType = EScalabilityQualityFG;

const TUint KTestMaxRefPics = 9;
const TUint KTestMaxPicDelay = 111;

// encoder buffer options creation utility function
static inline TEncoderBufferOptions GetTestEncBufferOptions( void )
	{
	TEncoderBufferOptions buffOptions;
 	
	buffOptions.iMaxPreEncoderBufferPictures = 10;
	buffOptions.iHrdVbvSpec = KTestHrdVbvSpec;
	buffOptions.iHrdVbvParams.Set(KTestHrdVbvParams);
	buffOptions.iMaxOutputBufferSize = 100;
	buffOptions.iMaxCodedPictureSize = 200;
	buffOptions.iMaxCodedSegmentSize = 300;
	buffOptions.iMinNumOutputBuffers = 400;

	return buffOptions;
	}

// encoder buffer options comparison utility function 
static inline TBool CompareEncBufferOptions(const TEncoderBufferOptions& a, const TEncoderBufferOptions& b)
	{
	if ( (a.iMaxPreEncoderBufferPictures == b.iMaxPreEncoderBufferPictures) && 
		 (a.iHrdVbvSpec == b.iHrdVbvSpec) &&		 
		 (a.iHrdVbvParams == b.iHrdVbvParams) && 
		 (a.iMaxOutputBufferSize == b.iMaxOutputBufferSize) &&
		 (a.iMaxCodedPictureSize == b.iMaxCodedPictureSize) &&
		 (a.iMaxCodedSegmentSize == b.iMaxCodedSegmentSize) &&
		 (a.iMinNumOutputBuffers == b.iMinNumOutputBuffers) )
		 return ETrue;
	else return EFalse;
	}

_LIT8(KTestCSEncoderOptions, "there's a moose in the hoose!");
_LIT8(KTestISEncoderOptions, "but did the moose chase the goose?");

_LIT8(KTestISInitOutput, "I am a mole...");
_LIT8(KTestCSInitOutput, "...and I live in a hole!");

const TUint KTestProtectLevels = 444;
const TUint KTestLevel = 11;
const TUint KTestBitrate = 192;
const TUint KTestStrength = EFecStrengthLow;

const TInt KTestLossBurstLength = 123456;
const TUint KTestLossRate = 8192;

const TReal KTestErrorRate = 1.11;
const TReal KTestStdDeviation = 2.22;

const TUint KTestSizeBytes = 8;
const TUint KTestSizeMacroblocks = 244;

// encoder rate control options creation utility function
static inline TRateControlOptions GetTestRateControlOptions( void )
	{
	TRateControlOptions rateOptions;
 	
	rateOptions.iControl = EBrControlPicture;
	rateOptions.iBitrate = 256;
	rateOptions.iPictureQuality = 512;
	rateOptions.iPictureRate = 1.234;
	rateOptions.iQualityTemporalTradeoff = 4.321;
	rateOptions.iLatencyQualityTradeoff = 5.678;

	return rateOptions;
	}

// encoder rate control options comparison utility function 
static inline TBool CompareRateControlOptions(const TRateControlOptions& a, const TRateControlOptions& b)
	{
	if ( (a.iControl == b.iControl) && 
		 (a.iBitrate == b.iBitrate) &&		 
		 (a.iPictureQuality == b.iPictureQuality) && 
		 (a.iPictureRate == b.iPictureRate) &&
		 (a.iQualityTemporalTradeoff == b.iQualityTemporalTradeoff) &&
		 (a.iLatencyQualityTradeoff == b.iLatencyQualityTradeoff) )
		 return ETrue;
	else return EFalse;
	}

// scalability
const TUint KTestNumSteps = 56;
const TInLayerScalabilityType KTestLayerScaleType = EInLScalabilityTemporal;
const TUint KTestBitrateShare1 = 666;
const TUint KTestBitrateShare2 = 777;
const TUint KTestPictureShare1 = 888;
const TUint KTestPictureShare2 = 999;

const TUint KTestPointPeriod = 2;

// settings output
_LIT8(KTestISSettingsOutput, "seven deadly sins...");
_LIT8(KTestCSSettingsOutput, "...seven ways to win");

// picture timestamp
const TInt KTestEncPictureTimestamp = 98765;

// supplemental info
_LIT8(KTestSuppInfo, "don't eat spaghetti with chopsticks...");
const TInt KTestSuppTime = 6000000;
const TInt KTestSuppTimeCancel = 666666666;

const TInt KTestRecordPosition = 999666333;

const TUint KTestFreeBuffers = 9;
const TUint KTestFreeBytes = 256;

// picture counters creation utility function
static inline CMMFDevVideoRecord::TPictureCounters GetTestEncPictureCounters( void )
	{
	CMMFDevVideoRecord::TPictureCounters picCount;

	picCount.iPicturesSkippedBufferOverflow = 10;
	picCount.iPicturesSkippedProcPower = 20;
	picCount.iPicturesSkippedRateControl = 30;
	picCount.iPicturesProcessed = 40;
	picCount.iInputPictures = 50;
	return picCount;
	}

// picture counters comparison utility function 
static inline TBool CompareEncPictureCounters(const CMMFDevVideoRecord::TPictureCounters& a, const CMMFDevVideoRecord::TPictureCounters& b)
	{
	if ( (a.iPicturesSkippedBufferOverflow == b.iPicturesSkippedBufferOverflow) && 
		 (a.iPicturesSkippedProcPower == b.iPicturesSkippedProcPower) &&		 
		 (a.iPicturesSkippedRateControl == b.iPicturesSkippedRateControl) && 
		 (a.iPicturesProcessed == b.iPicturesProcessed) &&
		 (a.iInputPictures == b.iInputPictures ))
		 return ETrue;
	else return EFalse;
	}

const TInt KTestEncInputPictures = 10;

const TInt KTestFrameStableX1 = 5;
const TInt KTestFrameStableY1 = 10;
const TInt KTestFrameStableX2 = 300;
const TInt KTestFrameStableY2 = 200;

const TPictureId KTestPictureId1 = {TPictureId::ETemporalReference, 999};
const TPictureId KTestPictureId2 = {TPictureId::ETemporalReference, 666};
const TPictureId KTestPictureId3 = {TPictureId::ETemporalReference, 333};

static inline TBool ComparePictureIDs(const TPictureId& a, const TPictureId&b)
	{
	return ((a.iIdType == b.iIdType) && (a.iId == b.iId));
	}

// slice loss
const TUint KTestFirstMacroblock = 50;
const TUint KTestNumMacroblocks = 3;

// ref picture data
_LIT8(KTestRefPictureInfo, "there once was an ugly duckling...");

// start / stop / pause / resume positions
const TInt KTestRecTimeStop	= 0;	

const TInt KTestRecTimeStartEnc = 10;
const TInt KTestRecTimePauseEnc = 20;
const TInt KTestRecTimeResumeEnc = 30;

const TInt KTestRecTimeStartPre = 40;
const TInt KTestRecTimePausePre = 50;
const TInt KTestRecTimeResumePre = 60;

// command to start buffer creation
_LIT8(KTestISEncBuffers, "get them buffers a rollin!");
_LIT8(KTestISEncBufferData, "steam rollin along!");

// encoder info
const TUint32 KTestEncoderInfoCount = 3;

const TText8* const KTestEncoderInfoMimeArray[KTestEncoderInfoCount] = 
	{
	_S8("video/bunglevideo"), 
	_S8("video/zippyvideo"),
	_S8("video/georgevideo")
	};

_LIT(KTestEncoderInfoManufacturer, "bodgitt plc");
_LIT(KTestEncoderInfoIdentifier, "video bodger");

_LIT8(KTestEncoderInfoCSInfo, "coded by a blind orangutan");
_LIT8(KTestEncoderInfoISInfo, "implemented by an imp");

const TInt KTestEncoderInfoVersionMaj = 11;
const TInt KTestEncoderInfoVersionMin = 22;
const TInt KTestEncoderInfoVersionBuild = 33;
// KTestEncoderInfoVersion = Maj.Min.Build;

const TInt KTestEncoderInfoMaxSizeX = 256;
const TInt KTestEncoderInfoMaxSizeY = 512;
const TUint KTestEncoderInfoMaxUEPLevels = 5;
const TUint KTestEncoderInfoMaxBitrate = 963;
const TUint KTestEncoderInfoMaxILSSteps = 555;
const TUint32 KTestEncoderInfoPictureOptions = TVideoPicture::ESceneCut | TVideoPicture::EEffectParameters | TVideoPicture::ETimestamp;

static inline void GetTestEncoderInfoRate(TInt aIndex, TPictureRateAndSize& aRate)
	{
	TPictureRateAndSize rate;
	TSize size(aIndex * 10, aIndex * 20);
	TReal rIndex = aIndex;

	rate.iPictureSize = size;
	rate.iPictureRate = rIndex / 10.0;

	aRate = rate;
	}

// Pre Processor Info
_LIT(KTestPreProcInfoManufacturer, "iced inc");
_LIT(KTestPreProcInfoIdentifier, "video munger");

_LIT8(KTestPreProcInfoISInfo, "implemented by a marsupial");

const TInt KTestPreProcInfoVersionMaj = 44;
const TInt KTestPreProcInfoVersionMin = 55;
const TInt KTestPreProcInfoVersionBuild = 66;
// KTestPreProcInfoVersion = Maj.Min.Build;
const TUint32 KTestPreProcInfoRotations = KTestRotate1 | KTestRotate2;
const TUint32 KTestPreProcInfoRgbRanges = KTestRgbRange1 | KTestRgbRange2;

const TYuvToYuvCapabilities KTestPreProcInfoYuvToYuvCaps = {1, 2, 3, 4};

static inline TBool CompareYuvYuvCapabilities(const TYuvToYuvCapabilities& a, const TYuvToYuvCapabilities& b)
	{
	return ((a.iInputSamplingPatterns == b.iInputSamplingPatterns) &&
			(a.iInputDataLayouts == b.iInputDataLayouts) &&
			(a.iOutputSamplingPatterns == b.iOutputSamplingPatterns) &&
			(a.iOutputDataLayouts == b.iOutputDataLayouts) );
	}

const TUncompressedVideoFormat KTestPreProcInfoOutputFormatArray[KTestPostProcInfoCount] =
	{{ERgbRawData}, {ERgbFbsBitmap}, {EYuvRawData}};

const TUncompressedVideoFormat KTestPreProcInfoInputFormatArray[KTestPostProcInfoCount] =
	{{ERgbFbsBitmap}, {EYuvRawData}, {ERgbRawData}};

const TUncompressedVideoFormat KTestPreProcInfoNegativeFormat = {ERgbRawData};

// Custom Interface UIDs
const TUid KUidCustomInterfaceOne	= {0x101F7DD1};	//Supported by decoder
const TUid KUidCustomInterfaceTwo	= {0x101F7DD2};	//Supported by post processor
const TUid KUidCustomInterfaceThree = {0x101F7DD3};	//Supported by encoder
const TUid KUidCustomInterfaceFour	= {0x101F7DD4};	//Supported by pre processor

#endif	// __TESTDEVVIDEOPLAYTESTDATA_H__
