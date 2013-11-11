// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef AVIRECORDCONTROLLER_H
#define AVIRECORDCONTROLLER_H

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <badesca.h> 
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/devvideo/devvideorecord.h>
#include <mmf/server/sounddevice.h>
#include <mmf/server/mmfaudioinput.h> 
#include <ecam.h>
#include "aviwriter.h"
#include "avirecordcontrolleruids.hrh"



//TAviControllerPanics is an enumeration with the following entries:
//EBadState indicates a state violation.
//EBadCall indicates a bad call.

enum TAviRecordControllerPanics
	{
	EBadCall,
	EBadState
	};
	

//The public API of CAviRecordController.It mainly implements the public APIs in CMMFController. It also implements RecordCustomInterfaces 
//to support extended functionality during recording.This class is reponsible for controlling the audio data transfer from DevSound and 
//video data from DevVideoRecord to the File Sink.
class CAviWriter;
class MAviWriterObserver;
class CAviRecordController:public CMMFController, 
                           public MMMFVideoControllerCustomCommandImplementor,
						   public MMMFVideoRecordControllerCustomCommandImplementor,
						   public MMMFAudioRecordDeviceCustomCommandImplementor,
						   public MDevSoundObserver,
						   public MMMFDevVideoRecordObserver,
						   public MCameraObserver2,
						   public MAviWriterObserver,
						   public MMMFVideoPixelAspectRatioCustomCommandImplementor,
						   public MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor,
						   public MMMFVideoRecordControllerExtCustomCommandImplementor
	{
public:	
	static CAviRecordController* NewL();
	~CAviRecordController();
	virtual void AddDataSourceL(MDataSource& aSource);
    virtual void AddDataSinkL(MDataSink& aSink);
    virtual void RemoveDataSourceL(MDataSource& aSource);
    virtual void RemoveDataSinkL(MDataSink& aSink);
    virtual void ResetL();
    virtual void PrimeL();
	virtual void PrimeL(TMMFMessage &aMessage);
	virtual void PlayL();
	virtual void PauseL();
	virtual void PauseL(TMMFMessage& aMessage);
	virtual void StopL();
	virtual void StopL(TMMFMessage &aMessage);
	virtual TTimeIntervalMicroSeconds PositionL() const; 
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition); 
	virtual TTimeIntervalMicroSeconds DurationL() const;
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);
	
	//from MMMFAudioRecordDeviceCustomCommandImplementor
	void MardSetGainL(TInt aGain); 
	void MardGetMaxGainL(TInt& aMaxGain); 
	void MardGetGainL(TInt& aGain); 
	void MardSetBalanceL(TInt aBalance); 
	void MardGetBalanceL(TInt& aBalance);
 	// from MMMFVideoControllerCustomCommandImplementor
	void MvcGetVideoFrameSizeL(TSize &aVideoFrameSize);
    void MvcGetVideoBitRateL(TInt &aBitRate);
    void MvcSetFrameRateL(TReal32 aFramesPerSecond);
    void MvcGetFrameRateL(TReal32 &aFramesPerSecond);
    void MvcGetVideoMimeTypeL(TDes8 &aMimeType); 
    void MvcGetAudioBitRateL(TInt &aBitRate);
    void MvcGetAudioCodecL(TFourCC &aCodec);
    // from MMMFVideoRecordControllerCustomCommandImplementor
    void MvrcPrepareL();
    void MvrcSetCameraHandleL(TInt aCameraHandle);
    void MvrcSetVideoCodecL(const TDesC8 &aVideoCodec);
    void MvrcGetSupportedSinkVideoTypesL(CDesC8Array &aDataTypes); 
    void MvrcSetVideoFormatL(TUid aVideoFormatUid);  
    void MvrcSetVideoBitRateL(TInt &aBitRate);  
    void MvrcSetVideoFrameSizeL(TSize aFrameSize);
    void MvrcAddMetaDataEntryL(const CMMFMetaDataEntry &aNewEntry); 
    void MvrcRemoveMetaDataEntryL(TInt aIndex); 
    void MvrcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry &aNewEntry);
    void MvrcSetMaxFileSizeL(TInt aMaxFileSize); 
    void MvrcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds &aTime);
    void MvrcSetAudioEnabledL(TBool aEnable);         
    void MvrcGetSupportedSinkAudioTypesL(RArray< TFourCC > &aDataTypes); 
    void MvrcSetAudioBitRateL(TInt &aBitRate);
    void MvrcSetAudioCodecL(TFourCC aAudioCodec);  
    void MvrcGetAudioEnabledL(TBool &aEnabled);
    //MDevSoundObserver mixin implementations
    void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError); 
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError); 
	void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	void RecordError(TInt aError); 
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	//MMMFDevVideoRecordObserver mixin implementations
    void MdvroReturnPicture(TVideoPicture *aPicture);
    void MdvroSupplementalInfoSent();
    void MdvroNewBuffers();
    void MdvroFatalError(TInt aError);
    void MdvroInitializeComplete(TInt aError);
    void MdvroStreamEnd();
    //MCameraObserver2 mixin implementations
	void HandleEvent(const TECAMEvent &aEvent);
	void ViewFinderReady(MCameraBuffer &aCameraBuffer,TInt aError);
	void ImageBufferReady(MCameraBuffer &aCameraBuffer,TInt aError);	
	void VideoBufferReady(MCameraBuffer &aCameraBuffer,TInt aError);
 	//from MAviWriterObserver
 	void BufferEmptied();
	void AudioWritten(TBool abVal);
	// from MMMFVideoPixelAspectRatioCustomCommandImplementor
	void MvparSetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio);
	void MvparGetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio);
	void MvparGetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios);
	// from MMMFVideoAudioSamplingRateAndChannelConfigCustomCommandImplementor
	void MvasrccGetAudioChannelsL(TUint& aChannels);
	void MvasrccSetAudioChannelsL(const TUint aChannels);
	void MvasrccGetSupportedAudioChannelsL(RArray<TUint>& aChannels);
	void MvasrccSetAudioSampleRateL(const TUint aSampleRate);
	void MvasrccGetAudioSampleRateL(TUint& aSampleRate);
	void MvasrccGetSupportedAudioSampleRatesL(RArray<TUint>& aSampleRate);
	// from MMMFVideoRecordControllerExtCustomCommandImplementor
    virtual void MvrecSetVideoEnabledL(TBool aEnabled);
    virtual TBool MvrecVideoEnabledL() const;
    virtual void MvrecSetVideoQualityL(TInt aQuality);
    virtual TInt MvrecVideoQualityL() const;
    virtual void MvrecSetVideoFrameRateFixedL(TBool aFixedFrameRate);
    virtual TBool MvrecVideoFrameRateFixedL() const;	

//TControllerState is an enumeration within the namespace  CAviRecordController
//The enumeration provides a set of control states:
//EStopped is the initial state the controller resides in after it has been loaded
//EPrimed is the state the controller is in when it has been associated with datapaths but is not streaming data
//ERecording is the state the controller is in when it is streaming data
//EPausing is the state the controller is in when it is waiting for devsound initialization before it is actually paused.
//EPaused is the state the controller is in when it is paused
public:
	enum TControllerState
        {
        EStopped,
        EPrimed,
        ERecording,
        EPausing,
        EPaused
        };

private:
    //MessageHolder
	class CMMFMessageHolder : public CBase
		{
	public:
		static CMMFMessageHolder* NewL(TMMFMessage& aMessage);
		void Complete(TInt aError);
	private:
		CMMFMessageHolder(TMMFMessage& aMessage);
		TMMFMessage iMessage;
		};
    
   class CAsyncEventHandler : public MAsyncEventHandler
		{
	public:
		CAsyncEventHandler(CAviRecordController& aParent);
		virtual ~CAsyncEventHandler();
	public:
		virtual TInt SendEventToClient(const TMMFEvent& aEvent);
	private:
		CAviRecordController& iParent;
		};

protected:
    CAviRecordController();
    void ConstructL();
    void WriteHeaderL();
    void CheckDevSoundPresentL() const;
	void CheckDevVideoPresentL() const;
	void CheckAviWriterPresentL() const;
	void CheckCameraPresentL()const;
	void SendErrorToClient(TInt aError);
	void CheckForInitComplete();
	void Panic(TInt aPanicCode);
	void CheckForStopComplete();
	void StartPrimeL();		

private:
   	TControllerState iState;
    CMMFDevSound* iDevSound;
    CMMFDevVideoRecord* iDevVideoRecord;
    CMMFClip* iClip;
    CAviWriter* iAviWriter;
    CCamera *iCamera;
    CAsyncEventHandler* iEventHandler;
    CMMFMessageHolder* iMessage;
    TMMFPrioritySettings  iPrioritySettings;
    THwDeviceId iEncoderDeviceId;
	TBool iDevVideoRecordInitialized; 
	TBool iDevSoundInitialized;
	TUid iVideoFormatUid;
	HBufC8* iVideoCodecName;
	TFourCC iAudioCodecName;
	TInt iVideoBitRate;
	TSize iFrameSize;
	TReal32 iFrameRate;
	TBool iAudioEnabled;
	TInt iCameraHandle;
	TInt iMaxFileSize;
	TInt iVideoInputEnd;  //Identifies end of the video stream
	TBool iDevSoundStop;
	TUncompressedVideoFormat iVideoFormat;
	TBool ibAudioWritten;
	RArray<TSize> iVideoFrameSizes;
	RArray<TReal32> iVideoFrameRates;
	TBool iVideoEncoderInitialized;
	TVideoAspectRatio iAspectRatio; // Variable introduced because the Avi controller uses bitmap instead of  YUV (class TYuvFormat). 
									// If support for YUV is ever implemented then this variable can be removed, provided TYuvFormat is used to hold the values for Pixel Aspect Ratio.
	TUint iAudioChannels;			// Variable added because AVI record controller has the values hard coded in.
	TUint iAudioSampleRate;			// Variable added because AVI record controller has the values hard coded in.
    };
	     
#endif 
