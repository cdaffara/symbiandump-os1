/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __H264DecTestEngine_H
#define __H264DecTestEngine_H

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

#include "T_DevVideoConstants.h"
#include <Videoplayhwdevice.h>
#include <Devvideoplay.h>
#include <Devvideobase.h> 

//#define __CI_HEADERS__
//#define __SECUREOUTPUTCI__


#ifdef __CI_HEADERS__
#include <buffermanagementci.h>  // Custom interface buffre management

#endif


#ifdef __SECUREOUTPUTCI__
#include <secureoutputci.h>
#endif

#include <Bitdev.h>   // for display device 
#include <Fbs.h>      // for display device
#include <math.h> //pow

#ifdef __RESOURCENOTIFICATION__
#include <devvideostandardcustominterfaces.h>
#endif





//////////////////
// Utility function
//////////////////


// MACROS

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

const TUint KMaxNumberOfNals = 100;
//const TUint KDataChunkSize = 500000;


//#ifdef _DEBUG
#define DEBUGPRINT
//#endif

// Debug print macro
#ifdef DEBUGPRINT
#include <e32svr.h>
#define PRINT(x) RDebug::Print x;
#else
#define PRINT(x)
#endif

// ENumerations


enum TEngineState
     {
     EStateNone,
     ECreated,
     EInitialized,
     ERunning,	
     EPaused,	
     EStopped,	
     };

enum TYuvDataOptions
     {
     EYuv420Planner,
     EYuv422Interleaved
     };

enum THWDevice 
	{
	EPostProcessor,
	EDecoder
	};

enum TFLVFileType
	{
	EFLVNotSet,
	EFLVContainer,
	EFLVRaw
	};

// CLASS DECLARATION
class CVDecTestAO;
class CVDecTestEngine;
class MVDecEngineObserver;


/**
*  Video Decoder Test Engine Observer
*  
*  @lib 
*  @since 
*/
class MVDecEngineObserver
{
public:
	
	virtual void MvdeStreamEndReached() = 0;
	virtual void MvdeSetError(TInt aError) = 0;
};

/**
*  Video Decoder Test Engine 
*  implements common test execution block
*  @lib 
*  @since 
*/
class CVDecTestEngine : public CBase , public MMMFDevVideoPlayObserver 
#ifdef __CI_HEADERS__
,public MMmfVideoBufferManagementObserver
#endif
#ifdef __RESOURCENOTIFICATION__
,public MMmfVideoResourceObserver
#endif
    {
public:

     
     TEngineState iState;

public:
    CVDecTestEngine () { }
    ~CVDecTestEngine ();
    static CVDecTestEngine* NewL(MVDecEngineObserver& aTestClass);
    
    //info test method
    void SetCodecType(TVideoCodec aCodec);
    //void GetDecoderInfoL(const TDesC8& aMimeType, TBool aExactMatch);
    //void GetPostProcessorInfoL(TInt aCombination);
    void GetHeaderInformationL();
    
	void GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aDataUnitEncapsulation);
    //void GetHeaderInfoL(const TUid& aDecUid, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aDataUnitEncapsulation);
	void GetBitstreamCountersL();
	void PreDecoderBufferBytes();	
    void PictureBufferBytes();
    
    // Video test control methods
    //void SetUpL(TBool aScreenAcces);
    void SetUpL(TSize aSize, TBool aScreenAccess, TBool aCIBuffMgmt);
    void CreateDecoderL(const TUid& aDecUid, TVideoCodec aCodec, const TDesC8& aMimeType, TYuvDataOptions aOutFormat);
    void CreatePostProcessorL(const TUid& aPostprocUid, const TYuvFormat& aInputFormat, const TUncompressedVideoFormat& aOutputFormat, TUint32 aCombination);
    void CreateDecPostprocL(const TUid& aDecUid, TVideoCodec aCodec, const TUid& aPostprocUid, TUint32 aCombination);
    TInt Initialize();
    TInt InitializeAndDelete();
    TInt Start(TBool aInputEnd);
    TInt Stop();
    TInt Resume();
    TInt Pause();
    void FreezePicture(TTimeIntervalMicroSeconds aTimeStamp);
    void ReleaseFreeze(TTimeIntervalMicroSeconds aTimeStamp);
    void TearDown();    
    void SetFrameMatch(TBool aMatch);
    TInt FrameJump(TInt aNumToJump );
    void SetFrameDrop(TInt aFrameInterval, TInt aFrameNum, TBool aFrameDropMarker );
    void SetFLVPassword(TDesC8& aPassword); 
    void EnableInstantFpsL(TFileName& aOutFileName);
    
    // Select and set Postprocessing  
    void SetPostProcessTypesL(TInt aHWDevice, TUint32 aCombination);
    void SetInputCropOptionsL(TInt aHWDevice, TRect aCrop);
    void SetYuvToRgbOptionsL(TInt aHWDevice, TYuvToRgbOptions aOptions);
    void SetRotateOptionsL(TInt aHWDevice, TRotationType aRotation);
    void SetScaleOptionsL(TInt aHWDevice, TSize aSize, TBool aAntiAliasFiltering);
    void SetScaleOptionsL(TInt aNumFactor,TInt aDenoFactor, TBool aAntiAliasFiltering);
    void SetOutputCropOptionsL(TInt aHWDevice, TRect aCrop);
    void CommitL();
    void Revert();    
    
    void SetClockSource();
    void SetInputBufferSize(TInt aSize);
    void SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams);
    void SetPostProcSpecificOptionsL(TInt aHWDevice, const TDesC8& aOptions);
    void SetScreenClipRegion(TRegion& aRegion);
    void SetPauseOnClipFail(TBool aPause);    
    void IsPlaying();
    void GetPictureCounters();
    void NumFreeBuffers();
    void NumComplexityLevels(TInt aHWDevice);
    void InputEnd();
    void GetNewPictureInfo(TTimeIntervalMicroSeconds& aEarliestTimestamp, 
						   TTimeIntervalMicroSeconds& aLatestTimestamp);
    void GetTimedSnapshotL(const TUncompressedVideoFormat& aFormat, 
							const TTimeIntervalMicroSeconds& aPresentationTimestamp); 
	void GetTimedSnapshotL(const TUncompressedVideoFormat& aFormat, const TPictureId& aPictureId);						
    void CancelTimedSnapshot();
    void GetSupportedSnapshotFormatsL();
    
    // Complexity level
    void SetComplexityLevel(TInt aHWDevice, TInt aLevel);
    void GetComplexityLevelInfo(TInt aHWDevice, TInt aLevel);
    
    //DSA
    void StartDirectScreenAccessL();
    void StartDirectScreenAccessL(TRect aVideoRect, TRect aClipRegion);
    void AbortDirectScreenAccess();    
    void Redraw();
    
    void SetWindowRect(TInt aX, TInt aY, TSize aSize);
    void SetSecondScreenAccess(TBool aStatus);
    
    // snap shot methods
    void GetSnapshotL(TUncompressedVideoFormat& aFormat);
    
    void EnableSynchronization(TInt aFrameRate);
    void EnableFrameMeasurementTest(TBool aEnable);
    void SetSecureOutputL(TBool aSecure);
    void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition);
    void ResetPosition();
    TTimeIntervalMicroSeconds PlaybackPosition();
    void DecodingPosition();
    void ListFrameSizeL(RArray<TInt> aFramesizes);
    TInt GetFrameSizeCount();
    //void GetPictureCounter();
    void SetLandscapeMode();
    void SelectDecoderL(const TUid& aUid);
    void SelectPostProcessorL(const TUid& aUid);
    void SetInputFormatL(TInt aHwDev, const TUncompressedVideoFormat& aFormat);
    void SetInputFormatL(TInt aHwDev, const CCompressedVideoFormat& aFormat, 
						 TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation,
						 TBool aDataInOrder);
	void SetOutputFormatL(TInt aHwDev, const TUncompressedVideoFormat& aFormat);						 
    void SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions);
    void GetBufferOptions();
    void SynchronizeDecoding(TBool aSynch);
    void SetVideoDestScreenL(TBool aDest);
    void SetYuvToRgbOptionsL(TInt aHWDevice, const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat);
    void FindCommonFormatsL();
    TInt FindDecodersL(const TDesC8& aMimeType, TUint32 aPostProcType, TBool aExactMatch, TUid& aUid);
    TInt FindPostProcessorsL(TUid& aUid, TUint32 aPostProcType);
    void GetDecoderListL();
    void ConfigureDecoderL(TVideoPictureHeader& aVideoPictureHeader);
    void ConfigureDecoderL();
    void GetOutputFormatListL(TInt aHWDevice);
    void GetPostProcessorListL();
    void VideoDecoderInfoL(TUid aVideoDecoder);
    void PostProcessorInfoL(TUid aPostProcessor);
    
    TReal ConvertToRealL(const TDesC& aPtr);
    
    //File operations
    void OpenFileL(TFileName& aInFileName);
    void OpenFileL(TFileName& aOutFileName, TFileName& aInFileName);
    void CloseFile();
    void CorruptInput(TInt aFilter, TInt aInterval, TInt aFrameNum, TInt aPct);
    void CorruptEngineL();
    void CorruptEngineL( TVideoInputBuffer* aCodedInBuffer ); 
    TInt CreateCorruptedFileL(); 
    void OpenFileToWriteL(TFileName& aInFileName);
    TInt ReadVC1FrameL(TVideoInputBuffer* aCodedInBuffer);
    TInt ReadMPEG4FrameL(TVideoInputBuffer* aCodedInBuffer); 
    
    TInt SupportsFormatL(const TUid& aUid, const CCompressedVideoFormat& aFormat);
	TInt SupportsFormatInfoL(const TUid& aUid, const CCompressedVideoFormat& aFormat);		
	TInt SupportsAccelerationL(TInt aHwDev, const TUid& aUid);		
	TInt SupportsDirectDisplayL(TInt aHwDev, const TUid& aUid);		
	TInt SupportsMaxBitrateL(const TUid& aUid, TInt aBitrate);		
	TInt SupportsMaxPictureSizeL(const TUid& aUid, TSize aSize);		
	TInt SupportsMaxPictureRateL(const TUid& aUid, TPictureRateAndSize& aRateAndSize);			
	TInt SupportsPictureLossL(const TUid& aUid);		
	TInt SupportsSliceLossL(const TUid& aUid);
 	TInt SupportsOutputFormatL(TInt aHwdev, const TUncompressedVideoFormat& aFormat);
 	void SetSlowClient(TBool aOption);
 	void SetFastClient(TBool aOption);
 	TInt SetAUFormat();
 	TInt MoveFileL(const TDesC &anOld, const TDesC &aNew);
 	TInt CopyFileL(const TDesC &anOld, const TDesC &aNew); 	
 	
    
    // For Active objects
    void HandleNewBufferL();
        
    // From DevVideo Play observer 
    void MdvpoNewBuffers();
    void MdvpoReturnPicture(TVideoPicture* aPicture);
    void MdvpoSupplementalInformation(const TDesC8& aData, const TTimeIntervalMicroSeconds& aTimestamp, const TPictureId& aPictureId);
    void MdvpoPictureLoss();
    void MdvpoPictureLoss(const TArray<TPictureId>& aPictures);
    void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId& aPicture);
    void MdvpoReferencePictureSelection(const TDesC8& aSelectionData);
    void MdvpoTimedSnapshotComplete(TInt aError, TPictureData* aPictureData, const TTimeIntervalMicroSeconds& aPresentationTimestamp, const TPictureId& aPictureId);
    void MdvpoNewPictures();
    void MdvpoFatalError(TInt aError);
    void MdvpoInitComplete(TInt aError);
    void MdvpoStreamEnd();
   
    // Buffer Management Custom Interface observer 
#ifdef __CI_HEADERS__    
    void MmvbmoNewBuffers();
    void MmvbmoReleaseBuffers();
#endif
#ifdef __RESOURCENOTIFICATION__
	void MmvroResourcesLost(TUid aMediaDevice);
    void MmvroResourcesRestored(TUid aMediaDevice);
#endif
 
protected:
    // Own test functions
    void GetInputBufferL();
    void FillAndSendBufferL();
    void GetReturnedPictureL();
    void SaveAndReturnPicture();
    TInt ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer);
    TInt ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer, TInt aSize);
    TInt ReadRawPicture();
     
private:
	TUint8 ReadByteFromFile();
	void GoBackInFile(TUint aBytes);
	void ReadDataToBuffer(TVideoInputBuffer* aCodedInBuffer, TInt aSize, TBool aDiscard = EFalse);
	TBool CheckAndFillDataChunk();
	TBool ReadItemLE(unsigned char *readBuffer, int Size, unsigned char* pItem);
	TInt LoadNextFrameL(TVideoInputBuffer* aCodedInBuffer);
    void ConstructL(MVDecEngineObserver& aTestClass);
    void AssertTIntEqualL(TInt aExpected, TInt aActual);
    void PrintUncompressedFormat(const TUncompressedVideoFormat& aFormat);
    
private:
    CMMFDevVideoPlay* iDevvp;			  //points to DevVideo Play
    THwDeviceId iDecHWDevId;              //Hwdevise ID of selected Decoder
    THwDeviceId iPostProcId;              //Hwdevise ID of selected postprocessor
    TUid iPostProcessorUid;
    TUid iDecoderUid;
    CVDecTestAO* iDecTestAO;              
    TBool iRunning;                       //State of scheduler
    TYuvFormat iRawFormat;                //Used for Decoder output, postprocessor input 
    TRgbFormat iDispFormat;               // Display Options
    TRect iDispRect;                        
    TInt iError;
    TVideoInputBuffer* iCodedInBuffer;     // Pointer to Decoder input buffer
    TVideoInputBuffer* iCorruptedInBuffer;   //Pointer to corrupted input for decoder
    TInt iInBuffSize;                 // Size of decoder input buffer
    TVideoPicture* iOutBuffer;             // Pointer to picuture class for output data in memory buffer output 
    TVideoPicture* iOutBufferTemp;             // Pointer to picuture class for output data in memory buffer output 
    TVideoPicture* iRawInBuffer;            // Used for postprocessor input
    HBufC8* iRawDataArea;                  // Pointer to Heap buffer, filled with input data to postprocessor
    TInt64 iTimeStamp; 
    TInt64 iFrameTimeInterval;              
    TInt iSentBuffCount;                    //Number of frames sent to target HwDevice    
    TInt iReturnedBuffCount;                //Number of frames returned from target HwDevice
    TInt iPictureLoss;                      // number of picture loss
    TBool iInputEnd;
    TBool iStreamEnd; 
    TVideoCodec iCodecType;                
    TSize iPictureSize; 
    CSystemClockSource* iClock;            //Clock source, based on System clock
    TBool iSynchronized;
    TBool iDirectScreenAccess;
    TBool iCIBuffMgmtOn;
    TBool iDisableInputEnd;
    TBool iFrameMatch;              // input frame number should match output frame number
    TInt  iFrameJump;
    TInt  iFrameJumpCounter;
    TInt iFrameWriteLength;
    TInt iCurrentFilePos;
#ifdef __CI_HEADERS__     	   
    MMmfVideoBufferManagement* iCIBuffMgmt;  
#endif    
    MVDecEngineObserver* iTestClass;
    TBool iFrameMeasurement;
    TBool iDsaStarted;
    TBool iLandscapeMode;
    TInt iNoBuffAvailable;
    RArray<TInt> iFrameSizeList;
    TPictureData iPictureDataSnapshot;
    TBuf8<128> iInFileName;
    TBool iSetSlowClient;
    TBool iSetFastClient;
    TBool iSetAUFormat;
    TBool iOutFileOpen;
    TUint8* iAUBuffer;
    TUint32* iNalOffsetArray;
    TUint32* iNalLengthArray;
    TReal32 iFrameTime;
    
    TUint8* iDataChunk;
    TUint8* iReadDataChunk;
    TUint8* iWriteDataChunk;
    TUint iDataReadFromFile;
    TUint iDataThreshold;
    TBool iEndOfData;
    TBool iLastFrame;
    TBool iUseSecondScreen;
    
    //File IO
    RFs   iFs;
    RFile iInFile;                         // Input data file
    RFile iOutFile;                        // Output data file
    RFile iOutCorruptedFile;
    RFile iLogFile;
	TBool iDeleteDecoderFromInitComp;
	
	TInt iFlvCounter;
	TBool iCorruptMarker;
	TInt iFilter;
	TInt iInterval;
	TInt iFrameNum;
	TInt iPercentage;
	TInt iFrameCounter;
	TInt iCorruptCounter;
	TInt iFrameDropCounter;
	TInt iFrameDropInterval;
	TInt iFrameDropNum;
	TInt iFrameDropNumCounter;
	TBool iFrameDropMarker;
	TFileName iOutFileName;
	TFileName iOutCorruptedFileName;
	TBool iOutCorruptedFileOpen;
	TBuf8<128> iPasswordForFLV;
	TBool iSetPassword;
    TBool iInstantFpsEnabled;
    RArray<TTimeIntervalMicroSeconds> iInstantFpsList;
    RFile iFpsFile;

	
	// Memory check
	TInt iMemAlloc;
	TInt iMemDelete;
	
    
    TInt32 iDataChunkSize;
    //Fbs

    CFbsScreenDevice* iScreenDevice;
    
    CActiveSchedulerWait* iScheduler;      //Nested scheduler
    
    TTime							iTime;
	TDateTime						iTimeBeforeDecoding;
	TDateTime						iTimeAfterDecoding;	
	//TInt							iNumFramesDecoded;
	//TUint64							iTotalTime;
	//TBool 							iIsBeforeTimeUsed;
	TUint64							iBeforeDecodingTime;
	RArray<TUint64>					iBeforeDecoingTimesQueue;
    TUint							iFrame;
    //	TInt							iLength;
	//TBool							iFirstTime;
	TBool                           iNumFreeBufsFirstTime;
	TTimeIntervalMicroSeconds		iTimeToPlay;
    TTimeIntervalMicroSeconds		iPresentationTimestamp;
	TBool							iNoOfFreeBuffFlag;
	
	
	TInt tempFrame;

	TFLVFileType                    iFLVFileType;
    };




/**
*  Video Decoder Test Engine AO
*  Provides async services
*  @lib 
*  @since 
*/

class CVDecTestAO : public CActive
{
	public:
		CVDecTestAO(CVDecTestEngine* aTestEngine);
  		~CVDecTestAO();
  		void RequestData();
	private:
		void RunL();
  		void DoCancel();

	private:
  		CVDecTestEngine* iTestEngine;
  	    
};




#endif
