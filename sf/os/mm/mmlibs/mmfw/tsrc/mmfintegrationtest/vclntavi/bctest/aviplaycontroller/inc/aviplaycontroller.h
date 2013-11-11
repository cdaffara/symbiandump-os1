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

#ifndef AVIPLAYCONTROLLER_H
#define AVIPLAYCONTROLLER_H

//standard epoc32 includes
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <e32std.h>
#include <e32base.h>
#include <fbs.h>
#include <bitstd.h>
#include <bitdev.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/devvideo/devvideoplay.h>
#include <mmf/server/sounddevice.h>
#include <mmf/server/mmfaudiooutput.h>
#include <mmf/server/mmfcodec.h> 
#include "avireader.h"
#include "aviplaycontrolleruids.hrh"
	
class CAviReader;
class MAviReaderObserver;

const TInt KNumSampleRates = 9;

struct TSampleRateTable
  	{
	TInt iRate;
	TMMFSampleRate iRateEnum;
	};

//TAviPlayControllerPanics is an enumeration with the following entries:
//EBadState indicates a state violation.
//EBadCall indicates a bad call
enum TAviPlayControllerPanics
	{
	EBadCall,
	EBadState
	};


//The public API of CAviPlayController.It mainly implements the public APIs in CMMFController. It also implements PlayCustomInterfaces 
//to support extended functionality during playing.This class is reponsible for controlling the audio and video data transfer from file to
//devvideoplay and devsound.
class CAviPlayController: public CMMFController, 
                          public MMMFVideoPlayControllerCustomCommandImplementor,
						  public MMMFVideoControllerCustomCommandImplementor,
						  public MMMFAudioPlayDeviceCustomCommandImplementor,
						  public MMMFDevVideoPlayObserver,
						  public MDevSoundObserver,
						  public MAviReaderObserver,
						  public MMMFResourceNotificationCustomCommandImplementor,
 					  	  public MMMFVideoSetInitScreenCustomCommandImplementor
						   
	{
public:

	static CAviPlayController* NewL();
	~CAviPlayController();
	// from CMMFController Implementation
    virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void ResetL();
	virtual void PrimeL();
    virtual void PrimeL(TMMFMessage& aMessage);
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL();
	virtual TTimeIntervalMicroSeconds PositionL() const;
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds DurationL() const;
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);
	//MMMFVideoPlayControllerCustomCommandImplementor
    void MvpcPrepare();
	void MvpcGetFrameL(MMMFVideoFrameMessage& aMessage);
	void MvpcGetAudioEnabledL(TBool& aEnabled);
	void MvpcSetDisplayWindowL(const TRect& aWindowRect, const TRect& aClipRect);
	void MvpcUpdateDisplayRegionL(const TRegion& aRegion);
	void MvpcDirectScreenAccessEventL(const TMMFDSAEvent aDSAEvent);
	void MvpcRefreshFrameL();
	void MvpcPlayL(const TTimeIntervalMicroSeconds& aBegin, const TTimeIntervalMicroSeconds& aEnd);
	void MvpcGetLoadingProgressL(TInt& aPercentage);
	void MvpcSetRotationL(TVideoRotation aRotation);
	void MvpcGetRotationL(TVideoRotation& aRotation);
	void MvpcSetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering);
	void MvpcGetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering);
	void MvpcSetCropRegionL(const TRect& aCropRegion);
	void MvpcGetCropRegionL(TRect& aCropRegion);
	// MMMFVideoControllerCustomCommandImplementor 
	void MvcGetAudioCodecL(TFourCC& aCodec);
	void MvcGetVideoBitRateL(TInt& aBitRate);
	void MvcGetAudioBitRateL(TInt& aBitRate);
	void MvcSetFrameRateL(TReal32 aFramesPerSecond);
	void MvcGetFrameRateL(TReal32& aFramesPerSecond);
	void MvcGetVideoFrameSizeL(class TSize &aVideoFrameSize);
	void MvcGetVideoMimeTypeL(TDes8& aMimeType);
	// from MMMFAudioPlayDeviceCustomCommandImplemetor 
	void MapdSetVolumeL(TInt aVolume);
	void MapdGetMaxVolumeL(TInt& aMaxVolume);
	void MapdGetVolumeL(TInt& aVolume);
	void MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	void MapdSetBalanceL(TInt aBalance);
	void MapdGetBalanceL(TInt& aBalance);

	//from MMMFDevVideoPlayObserver
    void MdvpoNewBuffers();
    void MdvpoReturnPicture(TVideoPicture *aPicture);
    void MdvpoSupplementalInformation(const TDesC8 &aData, const TTimeIntervalMicroSeconds &aTimestamp, const TPictureId &aPictureId);
    void MdvpoPictureLoss();
    void MdvpoPictureLoss(const TArray< TPictureId > &aPictures);
    void MdvpoSliceLoss(TUint aFirstMacroblock, TUint aNumMacroblocks, const TPictureId &aPicture);
    void MdvpoReferencePictureSelection(const TDesC8 &aSelectionData);
    void MdvpoTimedSnapshotComplete(TInt aError, TPictureData *aPictureData, const TTimeIntervalMicroSeconds &aPresentationTimestamp, const TPictureId &aPictureId);
    void MdvpoFatalError(TInt aError);
    void MdvpoNewPictures();
    void MdvpoInitComplete(TInt aError);
    void MdvpoStreamEnd();
    // from MDevSoundObserver
    void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	//from  MMMFResourceNotificationCustomCommandImplementor,for audio resource notification 
	virtual void MarnRegisterAsClientL(TUid aEventType, const TDesC8& aNotificationRegistrationData);
	virtual void MarnCancelRegisterAsClientL(TUid aEventType);
	virtual void MarnGetResourceNotificationDataL(TUid aEventType, TDes8& aNotificationData);
	virtual void MarnWillResumePlayL();		
	// MMMFVideoSetInitScreenCustomCommandImplementor
	void MvsdSetInitScreenNumber(TInt aScreenNumber);
public:
   //TControllerState is an enumeration within the namespace  CAviVideoPlayController
   //The enumeration provides a set of control states:
   //EStopped is the initial state the controller resides in after it has been loaded
   //EPrimed is the state the controller is in when it has been associated with datapaths but is not streaming data
   //EPlaying is the state the controller is in when it is streaming data
   enum TControllerState
		{
		EStopped,
		EPrimed,
		EAudioReadyToPlay,
		EPlaying
		};
private:
    CAviPlayController();
   	void StartPrimeL();
	TBool SelectFirstSuitableDecoderL(
		const RArray<TUid>& aDecodersArray,
		const TUncompressedVideoFormat& aRequiredOutputFormat);
	void ConstructL();
	void SendErrorToClient(TInt aError);
	void CopyFrameL(CFbsBitmap& aBitmap);
	void CheckDevSoundPresentL();
	void CheckDevVideoPresentL()const;
	void CheckAviReaderPresentL()const;
	void CheckForInitComplete();
	void Panic(TInt aPanicCode);
	void StopVideoL();
	void StopAudioL();
	void StopAviReaderL();
	void RecreateDevVideoAfterFatalErrorL();
	void UpdateClipRegion();

	//from MAviReaderObserver
	void AudioBufferFilled();
	void VideoBufferFilled(TVideoInputBuffer* aBuffer);

private:
	class CMMFMessageHolder : public CBase
		{
		public:
			static CMMFMessageHolder* NewL(TMMFMessage& aMessage);
			void Complete(TInt aError);
		private:
			CMMFMessageHolder(TMMFMessage& aMessage);
			TMMFMessage iMessage;
		};
	
	class CSourceSinkEventHandler : public MAsyncEventHandler
		{
		public:
			CSourceSinkEventHandler(CAviPlayController& aParent);
			virtual ~CSourceSinkEventHandler();
		public:
			virtual TInt SendEventToClient(const TMMFEvent& aEvent);
		private:
			CAviPlayController& iParent;
		};
	
private:
	CMMFDevVideoPlay *iDevVideoPlay;
	CMMFDevSound* iDevSound;
	TControllerState iState;
	CAviReader* iAviReader;
	CMMFClip* iClip;
	CSourceSinkEventHandler *iEventHandler;
	TMMFPrioritySettings iPrioritySettings;
	THwDeviceId iDecoderDeviceId;
	CMMFMessageHolder* iMessage;
    CFbsScreenDevice* iScreenDev;
	CFbsBitGc* iScreenGc;
	CFbsBitmap* iBitmap;
	TRect iScreenRect;
	TRect iClipRect;
	RRegion iDisplayRegion;
	RRegion iDerivedClipRegion;
	TRect iCropRect;
	TVideoRotation iRotation;
	TReal32 iWidthPercentage;
	TReal32 iHeightPercentage;
	TBool iAntiAliasFiltering;
	TBool iDevVideoInitialized;
	TBool iDevSoundInitialized;
	TSize iFrameSize;
	TBool iAudioEnabled;
	TUid iDecoder;
	TBool iVideoDecoderInitialized;
	TInt iScreenNumber;
	TBool iVideoFatalError;
 	};

#endif 
