/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef T_CMMFDEVVIDEODATA_H
#define T_CMMFDEVVIDEODATA_H

//Epoc includes
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <videoplayhwdevice.h>
#include <devvideoplay.h>
#include <devvideobase.h> 
#include <bitdev.h>   // for display device 
#include <fbs.h>  
//User Includes
#include "datawrapperbase.h"
#include "t_devvideoconstants.h"
#include "H264DecTestEngine.h"

enum TVideoDecoderOutput 
	{
	EDecodedFile = 0,
	EScreenOutput
	};

//Forward declarations
class CT_CMMFDevVideoPlay;

//Engine observer handle Async request completeion and release waitclass of Test scriptor

class CEngineObserver:  public CBase, public MVDecEngineObserver
{
public:
    void MvdeStreamEndReached();
    void MvdeSetError(TInt aError);
    ~CEngineObserver() {}
    CEngineObserver(CT_CMMFDevVideoPlay& aTestClass);
      
private:
    CT_CMMFDevVideoPlay& iTestClass;
    
};
/**
 * Test Active Notification class
 *
 */
class CT_CMMFDevVideoPlay : public CDataWrapperBase, MMMFDevVideoPlayObserver 
	{
public:
	
	~CT_CMMFDevVideoPlay();
	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	static	CT_CMMFDevVideoPlay*	NewL();		
	void DestroyData();
	TInt ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer, TInt aSize);
	TInt ReadOneCodedPicture(TVideoInputBuffer* aCodedInBuffer);
	TInt ReadRawPicture();
	void OpenFileL(TFileName& aInFileName);
	void OpenFileL(TFileName& aOutFileName, TFileName& aInFileName);
	void CloseFile();
	void ListFrameSizeL(TVideoCodec aCodec);
	void AbortDirectScreenAccess();
    void StartDirectScreenAccessL();
    void HandleRuntimeError(TInt aError);
    void SaveAndReturnPicture();
    void GetReturnedPicture();
    void FillAndSendBufferL();
    void GetInputBuffer();
    void HandleNewBufferL();  
    void SetWindowRect(TInt aX, TInt aY, TSize aSize);
    void CreateFiles(const TTEFSectionName& aSection);
    void InitializeParamsL(const TTEFSectionName& aSection);    
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
protected:
	CT_CMMFDevVideoPlay();
	void ConstructL();
private:
	void DoCmdInitialize(const TInt aAsyncErrorIndex);
	void DoCmdStart(const TInt aAsyncErrorIndex);
	void DoCmdStop();
	void DoCmdPause();
	void DoCmdResume();
	void DoCmdSetRotateOptionsL(const TTEFSectionName& aSection);
	void DoCmdSetScaleOptionsL(const TTEFSectionName& aSection);
	void DoCmdSetPosition(const TTEFSectionName& aSection);
	void DoCmdSetClockSource(const TTEFSectionName& aSection);
	void DoCmdNewL(const TTEFSectionName& aSection);
	void DoCmdSelectDecoderL(const TTEFSectionName& aSection);
	void DoCmdSetInputFormatL(const TTEFSectionName& aSection);
	void DoCmdSelectPostProcessorL(const TTEFSectionName& aSection);
	void DoCmdGetOutputFormatListL(const TTEFSectionName& aSection);
	void DoCmdPostProcessorInfoLC();
	void DoCmdFindCommonFormat();
	void DoCmdSetOutputFormatL(const TTEFSectionName& aSection);
	void DoCmdGetHeaderInformationL(const TTEFSectionName& aSection);
	void DoCmdSetVideoDestScreenL();
	void DoCmdGetBufferOptions();
	void DoCmdSetBufferOptionsL();
	void DoCmdSetPostProcessTypesL(const TTEFSectionName& aSection);
	void DoCmdReturnHeader();
	void DoCmdDestructor();
	void DoCmdAbortDirectScreenAccess();
	void DoCmdStartDirectScreenAccessL(const TTEFSectionName& aSection);
	void DoCmdUtilityGetEnvironmentL();

	void DoCmdH264CreateDecTest(const TTEFSectionName& aSection);
	void DoCmdH264SelectPostProcessor(const TTEFSectionName& aSection);
	void DoCmdH264SetPostProcessTypes(const TTEFSectionName& aSection);
	void DoCmdH264SetInputFormatCompr(const TTEFSectionName& aSection);
	void DoCmdH264SetOutputFormat(const TTEFSectionName& aSection);
	void DoCmdH264SetBufferOptions(const TTEFSectionName& aSection);
	void DoCmdH264ListFrameSize();
	void DoCmdH264SetVideoDestScreen(const TTEFSectionName& aSection);
	void DoCmdH264SetWindowRect(const TTEFSectionName& aSection);
	void DoCmdH264Initialize();
	void DoCmdH264Start(const TTEFSectionName& aSection);
	void DoCmdH264Stop();
	void DoCmdH264Pause();
	void DoCmdH264Resume();
	void DoCmdH264IOFrameNumMatch();
	void DoCmdH264FindCommonFormats();
	void DoCmdH264GetHeaderInformation(const TTEFSectionName& aSection);
	void DoCmdH264EnableSynchronization(const TTEFSectionName& aSection);
	void DoCmdH264SetScaleOptions(const TTEFSectionName& aSection);
	void DoCmdH264SetRotateOptions(const TTEFSectionName& aSection);
	void DoCmdH264SynchronizeDecoding(const TTEFSectionName& aSection);
	void DoCmdH264Delete();
	void DoCmdH264SetPosition(const TTEFSectionName& aSection);
	void DoCmdH264AbortDirectScreenAccess();
private:
	/**
	 * Wrapped object
	 */
	CMMFDevVideoPlay* iDevvp; 
	/**
	 * For selected decoder
	 */
    THwDeviceId iDecHWDevId; 
    /**
	 * For selected Post processor
	 */
    THwDeviceId iPostProcId;
    /**
	 * Size of video buffer
	 */
    TInt	iInBuffSize; 
    /**
	 * Control the number of inputs
	 */
    TBool	iInputEnd;
    /**
	 * Direct Screen Access
	 */
    TBool	iDsaStarted;
    /**
	 * For synchronize the clock(fps)
	 */
    TBool	iSynchronized;
    /**
	 * Listed frames
	 */
    TBool	iFrameListed;
    /**
	 * The size of the picture
	 */
    TSize	iPictureSize;
    /**
	 * Async error
	 */
    TInt	iAsyncErrorIndex;
    /**
	 * Width and height of the video
	 */
    TRect	iDispRect;
    /**
	 * For management the Open files
	 */
	TBool	iFileOpen;
	/**
	 * for get the coded buffer
	 */
	TVideoInputBuffer* iCodedInBuffer;
	/**
	 * Output buffer
	 */
    TVideoPicture* iOutBuffer;
    /**
	 * 
	 */
    TVideoPicture* iRawInBuffer;
    /**
	 * For synch the time
	 */
    TInt64	iTimeStamp;
    /**
	 * Time interval in each frame
	 */
    TInt64	iFrameTimeInterval;
    /**
	 * Buffer control
	 */
    TInt	iSentBuffCount;
    /**
	 * Buffer control
	 */
    TInt	iReturnedBuffCount;
    /**
	 * Loss Pictures
	 */
    TInt	iPictureLoss;
    /**
	 * The end of the stream
	 */
    TBool	iStreamEnd;
    /**
	 * Type of codec
	 */
    TVideoCodec iCodecType;
    /**
	 * Management of the time between frames
	 */
    CSystemClockSource* iClock;
    /**
	 * Direct Access at the Screen
	 */
    TBool	iDirectScreenAccess;
    /**
	 * for handle the buffer
	 */
    TBool	iCIBuffMgmtOn;
    /**
	 * For measurement of the frame
	 */
    TBool	iFrameMeasurement;
    /**
	 * for H264
	 */
    RArray<TInt> iFrameSizeList;
    /**
	 * frame rate(fps)
	 */
    TInt iFramerate;
    /**
	 *  File Server
	 */
    RFs   iFs;  
    /**
	 * Input file
	 */
    RFile iInFile;
    /**
	 * Ouput file
	 */
    RFile iOutFile;	
    /**
	 *  Fbs
	 */
    CFbsScreenDevice* iScreenDevice;  
    /**
	 * Common format between Post processor and decoder
	 */
    TUncompressedVideoFormat iCommonFormat;
    /**
	 * Decoder read from the ini file
	 */
    TUid iSelectDecoder;
    /**
	 * Post processor read from the ini file
	 */
    TUid iSelectPostProcessor;
    /**
	 * Formats Decoder
	 */
    RArray<TUncompressedVideoFormat> iDecFormats;
    /**
	 * Post processor formats
	 */
    RArray<TUncompressedVideoFormat> iPPFormats;
    /**
	 * Information about the post processor
	 */
    CPostProcessorInfo* iInfo; 
    /**
	 * if exist information for header info
	 */
    TVideoPictureHeader* iHeaderInfo;
    /**
	 * Buffer options
	 */
    CMMFDevVideoPlay::TBufferOptions iBufferOptions;
    /**
	 * The common format
	 */
    TBool iCommonFormatFound;  
    /**
     * To Control the errors in GetInputBuffer
     */
    TBool iErrorInputBuffer;
    /**
	 * Enum for codecs
	 */   
    static const TEnumEntryTable iCodecs[];
    /**
	 * Enum for Display mode
	 */
    static const TEnumEntryTable iDisplayModes[];
    /**
	 * Enum for rotations
	 */
    static const TEnumEntryTable iRotations[];
    /**
	 * Enum for Video Data Unit Types
	 */
    static const TEnumEntryTable iTVideoDataUnitType[];
    /**
	 * Enum for Video Data Unit Encapsulation
	 */
    static const TEnumEntryTable iTVideoDataUnitEncapsulation[];
    
    /**
     * H264
     */
    CVDecTestEngine* iEngine;
    CEngineObserver* iObserver;
    TBool iUse264;
    TInt iFrameRate;
	};
	
	
#endif // T_CMMFDEVVIDEODATA_H

