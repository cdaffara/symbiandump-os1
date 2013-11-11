// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TS_CMMFVIDEOTESTCONTROLLER_H
#define TS_CMMFVIDEOTESTCONTROLLER_H

#include <e32std.h>
#include <e32base.h>
//#include <e32twin.h>


#include <ecom/implementationproxy.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>

#include <testframework.h>

#include <caf/caf.h>
using namespace ContentAccess;

#include "CmmfVideoTestControllerUIDs.hrh"
#include "TS_CMMFVideoTestCustomCommands.h"
#include <mmf/common/mmfstandardcustomcommands.h>
#include "TS_Codes.h"
#include <mmf/common/mmfdurationinfocustomcommands.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdurationinfocustomcommandsimpl.h>
#include <mmf/common/mmfdurationinfocustomcommandsenums.h>
#endif
#include <mmf/devvideo/videoplayhwdevice.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <graphics/surfacemanager.h>
#endif

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include <mmf/common/mmfvideosubtitlecustomcommands.h>
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

enum TMmfTestControllerPanic
	{
	EAddDataSourceFailure,
	ENotSupported
	};

enum TestControllerRPMode
	{
	EModeUnassigned = 0,
	EModeRecord,
	EModePlay
	};

enum TestControllerMode
	{
	EModeNormal,
	EModeFailure,
	EModeMemoryFailure,
	EModeNotSupported
	};

enum TestControllerState
	{
	EStateNotReady = 0,
	EStateOpen,
	EStatePlaying
	};

// Used to determine which error to set when doing a KTestSetError custom command
enum TestControllerErrorType
	{
	EMmfTC_OpenError,
	EMmfTC_PrepareError,
	EMmfTC_PlayError,
	EMmfTC_PrepareLeave
	};

// test constant which dictates record time available
const TInt KMMFVideoRecordTimeAvailable = 10000000;	// 10 seconds

class CMMFTestController : public CMMFController,
						   public MMMFTestCustomCommandImplementor,
						   public MMMFAudioPlayDeviceCustomCommandImplementor,
						   public MMMFAudioRecordDeviceCustomCommandImplementor,
						   //public MMMFAudioPlayControllerCustomCommandImplementor,
						   //public MMMFAudioRecordControllerCustomCommandImplementor,
						   //public MMMFAudioControllerCustomCommandImplementor,
						   public MMMFVideoControllerCustomCommandImplementor,
						   public MMMFVideoPlayControllerCustomCommandImplementor,
						   public MMMFVideoRecordControllerCustomCommandImplementor,
						   public MMMFVideoDRMExtCustomCommandImplementor,
						   public MMMFDurationInfoCustomCommandImplementor2
#ifdef SYMBIAN_BUILD_GCE
						,public MMMFVideoPlaySurfaceSupportCustomCommandImplementor
#endif
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
						,public MMMFVideoPlaySubtitleSupportCustomCommandImplementor
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
{
public:
	static CMMFTestController* NewL(void);
	static CMMFTestController* NewLC(void);


	//static CMMFController* NewL();
//implement CMMFController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void RemoveSourceSinkL();
	virtual void ResetL();//XXX - replacement for RemoveSourceSink()
	virtual void PrimeL();
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


	//--------------------------------------------//
	// MMMFAudioPlayDeviceCustomCommandImplemetor //
	//--------------------------------------------//
	virtual void MapdSetVolumeL(TInt aVolume);
	virtual void MapdGetMaxVolumeL(TInt& aMaxVolume);
	virtual void MapdGetVolumeL(TInt& aVolume);
	virtual void MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	virtual void MapdSetBalanceL(TInt aBalance);
	virtual void MapdGetBalanceL(TInt& aBalance);


	//-----------------------------------------------//
	// MMMFAudioRecordDeviceCustomCommandImplementor //
	//-----------------------------------------------//

	virtual void MardSetGainL(TInt aGain);
	virtual void MardGetMaxGainL(TInt& aMaxGain);
	virtual void MardGetGainL(TInt& aGain);
	virtual void MardSetBalanceL(TInt aBalance);
	virtual void MardGetBalanceL(TInt& aBalance);

/*
	//-------------------------------------------------//
	// MMMFAudioPlayControllerCustomCommandImplementor //
	//-------------------------------------------------//

	virtual void MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);
	virtual void MapcDeletePlaybackWindowL();


	//---------------------------------------------------//
	// MMMFAudioRecordControllerCustomCommandImplementor //
	//---------------------------------------------------//

	virtual void MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);
	virtual void MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& aMaxDuration);
	virtual void MarcSetMaxFileSizeL(TInt aMaxSize);
	virtual void MarcCropL(TBool aToEnd);
	virtual void MarcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	virtual void MarcRemoveMetaDataEntryL(TInt aIndex);
	virtual void MarcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);


	//---------------------------------------------//
	// MMMFAudioControllerCustomCommandImplementor //
	//---------------------------------------------//

	virtual void MacSetSourceSampleRateL(TUint aSampleRate);
	virtual void MacSetSourceNumChannelsL(TUint aNumChannels);
	virtual void MacSetSourceFormatL(TUid aFormatUid);
	virtual void MacSetSinkSampleRateL(TUint aSampleRate);
	virtual void MacSetSinkNumChannelsL(TUint aNumChannels);
	virtual void MacSetSinkFormatL(TUid aFormatUid);
	virtual void MacSetCodecL(TFourCC aSourceDataType, TFourCC aSinkDataType);
	*/
	
	//---------------------------------------------//
	// MMMFTestCustomCommandImplementor //
	//---------------------------------------------//
	virtual void MvtccCustomCommandSyncL(TMMFMessage& aMessage);
	virtual void MvtccCustomCommandSyncWithReturnL(TMMFMessage& aMessage);
	virtual void MvtccCustomCommandAsyncL(TMMFMessage& aMessage);
	virtual void MvtccCustomCommandAsyncWithReturnL(TMMFMessage& aMessage);
	virtual void MvtccCustomCommandRequestGeneralEventL(TMMFMessage& aMessage);
	virtual void MvtccCustomCommandSimulateReloadingL(TMMFMessage& aMessage);
	virtual void MvtccCustomCommandCheckUrlL(TMMFMessage& aMessage);
#ifdef SYMBIAN_BUILD_GCE
	void MvtccCustomCommandSimulateSurfaceCreatedL(TMMFMessage& aMessage);
	void MvtccCustomCommandSimulateParaChangedL(TMMFMessage& aMessage);
	void MvtccCustomCommandSimulateRemoveSurfaceL(TMMFMessage& aMessage);
#endif	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	void MvtccCustomCommandSetSubtitleDisplayCheckL(TMMFMessage& aMessage);
	void MvtccCustomCommandGetSubtitleCallCount(TMMFMessage& aMessage);
	void MvtccCustomCommandSimulateCrpReadyEvent(TMMFMessage& aMessage);
	void MvtccCustomCommandSetLanguageNotAvail(TMMFMessage& aMessage);
#endif
	//---------------------------------------------//
	// MMMFVideoControllerCustomCommandImplementor //
	//---------------------------------------------//

	virtual void MvcGetVideoDimensionL(TRect& aVideoDimension);
	virtual void MvcGetAudioCodecL(TFourCC& aCodec);
	virtual void MvcGetVideoBitRateL(TInt& aBitRate);
	virtual void MvcGetAudioBitRateL(TInt& aBitRate);
	virtual void MvcSetFrameRateL(TReal32 aFramesPerSecond);
	virtual void MvcGetFrameRateL(TReal32& aFramesPerSecond);
	virtual void MvcGetVideoFrameSizeL(class TSize &);
	virtual void MvcGetVideoMimeTypeL(TDes8& aMimeType);


	//---------------------------------------------//
	// MMMFVideoPlayControllerCustomCommandImplementor //
	//---------------------------------------------//
	virtual void MvpcPrepare();
	virtual void MvpcGetFrameL(MMMFVideoFrameMessage& aMessage);
	virtual void MvpcGetAudioEnabledL(TBool& aEnabled);
	virtual void MvpcSetDisplayWindowL(const TRect& aWindowRect, const TRect& aClipRect);
	virtual void MvpcUpdateDisplayRegionL(const TRegion& aRegion);
	virtual void MvpcDirectScreenAccessEventL(const TMMFDSAEvent aDSAEvent);
	virtual void MvpcRefreshFrameL();
	virtual void MvpcPlayL(const TTimeIntervalMicroSeconds& aBegin, const TTimeIntervalMicroSeconds& aEnd);
	virtual void MvpcGetLoadingProgressL(TInt& aPercentage);
	virtual void MvpcSetRotationL(TVideoRotation aRotation);
	virtual void MvpcGetRotationL(TVideoRotation& aRotation);
	virtual void MvpcSetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, TBool aAntiAliasFiltering);
	virtual void MvpcGetScaleFactorL(TReal32& aWidthPercentage, TReal32& aHeightPercentage, TBool& aAntiAliasFiltering);
	virtual void MvpcSetCropRegionL(const TRect& aCropRegion);
	virtual void MvpcGetCropRegionL(TRect& aCropRegion);

	//---------------------------------------------//
	// MMMFVideoRecordControllerCustomCommandImplementor //
	//---------------------------------------------//

	virtual void MvrcSetVideoFormatL(TUid aVideoFormatUid);
	virtual void MvrcSetVideoCodecL(const TDesC8& aVideoCodec);
	virtual void MvrcSetAudioCodecL(TFourCC aAudioCodec);
	virtual void MvrcSetVideoBitRateL(TInt& aBitRate);
	virtual void MvrcSetAudioBitRateL(TInt& aBitRate);
	virtual void MvrcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	virtual void MvrcRemoveMetaDataEntryL(TInt aIndex);
	virtual void MvrcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);
	virtual void MvrcSetMaxFileSizeL(TInt aMaxFileSize);
	virtual void MvrcSetVideoFrameSizeL(TSize aSize);
	virtual void MvrcSetAudioEnabledL(TBool aBool);
	virtual void MvrcPrepareL();
	virtual void MvrcSetCameraHandleL(TInt aCameraHandle);
	virtual void MvrcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);
	virtual void MvrcGetSupportedSinkAudioTypesL(RArray<TFourCC>& aDataTypes);
	virtual void MvrcGetSupportedSinkVideoTypesL(CDesC8Array& aDataTypes);
	virtual void MvrcGetAudioEnabledL(TBool& aEnabled);  //INC23777

	//---------------------------------------------//
	// MMMFDRMIntentCustomCommandImplementor		//
	//---------------------------------------------//
	virtual void MvdeGetFrameL(MMMFVideoFrameMessage& aMessage, TIntent aIntent);
	
    //----------------------------------------------//
	// 	MMMFDurationInfoCustomCommandImplementor2   //
	//----------------------------------------------//
	virtual TInt GetDurationInfo(TMMFDurationInfo& aDurationInfo);
	
#ifdef SYMBIAN_BUILD_GCE
	//--------------------------------------------------------//
	// 	MMMFVideoPlaySurfaceSupportCustomCommandImplementor   //
	//--------------------------------------------------------//
	void MvpssUseSurfacesL();
	void MvpssGetSurfaceParametersL(TSurfaceId& aSurfaceId, 
									TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio);
	void MvpssSurfaceRemovedL(const TSurfaceId& aSurfaceId);
#endif

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	//--------------------------------------------------------//
	// 	MMMFVideoPlaySubtitleSupportCustomCommandImplementor   //
	//--------------------------------------------------------//
	void MvpsusGetCrpParametersL(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect);
	void MvpsusAddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig);
	void MvpsusRemoveSubtitleConfigL(TInt aWindowId);
	void MvpsusUpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig);
	void MvpsusGetSubtitlesAvailableL(TBool& aAvailable);
	void MvpsusDisableSubtitlesL();
	void MvpsusEnableSubtitlesL();
	void MvpsusGetSubtitleLanguageL(TLanguage& aLanguage);
	void MvpsusGetSupportedSubtitleLanguagesL(RArray<TLanguage>& aAvailable);
	void MvpsusSetSubtitleLanguageL(TLanguage aLanguage);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	
	TInt SendEventToClient(const TMMFEvent& aEvent);
	void OpenComplete();
	void PlayComplete();
	void PrepareComplete();
	void FrameReady();
	void StartReloading();
	void CompleteReloading();
#ifdef SYMBIAN_BUILD_GCE
	void CreateSurfaceL();
	void SurfaceCreated();
	void ParaChanged();
	void RemoveSurface();
#endif
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	void CrpReady(TInt aWindowId);
	static TBool FindWindowByWindowId(const TInt* aWindowId, const TMMFSubtitleWindowConfig& aData);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	
	// helper functions to check the controller's state and passed parameters
	TInt GetMemberFunctionCalled(TMMFMessage& aMessage);
	TInt GetMemberFunctionCalled2(TMMFMessage& aMessage);
	void SetControllerMode(TMMFMessage& aMessage);
	TInt GetTmpInt1Passed(TMMFMessage& aMessage);
	TInt GetTmpInt2Passed(TMMFMessage& aMessage);
	TInt GetTmpReal32Passed(TMMFMessage& aMessage);
	TInt GetTmpTimeIntervalMicroPassed(TMMFMessage& aMessage);
	//TInt GetTmpMeta(TMMFMessage& aMessage);
	TInt GetTmpCodecPassed(TMMFMessage& aMessage);
	void SetTestError(TMMFMessage& aMessage);


private:

	class CAsyncCallback : public CActive
		{
	public:
		CAsyncCallback();
		~CAsyncCallback();

		void RequestCallback(TMMFMessage& aMessage, TInt aError);
		void RunL();
		void DoCancel();

	private:
		TMMFMessage* iMessage;
		TInt iError;
		};

	class CControllerTimer : public CTimer
		{
	public:
		enum TimerType
			{
			ETmrPlay,
			ETmrPrepareComplete,
			ETmrOpenComplete,
			ETmrFrameReady,
			ETmrStartReloading,
			ETmrReloading,
			ETmrSurfaceCreated,
			ETrmParaChanged,
			ETrmRemoveSurface,
			};
	public:
		static CControllerTimer* NewL(CMMFTestController* aController);
		void DelayOpen(TTimeIntervalMicroSeconds aDelay);
		void DelayPrepare(TTimeIntervalMicroSeconds aDelay);
		void DelayFrameReady(TTimeIntervalMicroSeconds aDelay);
		void StartReloading(const TTimeIntervalMicroSeconds& aInterval,TInt aPercentageIncrement);
		void PlayFor(TTimeIntervalMicroSeconds aDelay);
		void DelaySurfaceCreated(TTimeIntervalMicroSeconds aDelay);
		void DelayParaChanged(TTimeIntervalMicroSeconds aDelay);
		void DelayRemoveSurface(TTimeIntervalMicroSeconds aDelay);
		TInt LoadingProgress();
		void RunL();
	private:
		CControllerTimer(CMMFTestController* aController);
		CMMFTestController* iController;
		TMMFEvent iEvent;
		TimerType iType;
		TTimeIntervalMicroSeconds iInterval;
		TInt iPercentageComplete;
		TInt iPercentageIncrement;
		TInt iDisplayId;
		};


private:
	CMMFTestController(void);
	~CMMFTestController(void);
	void ConstructL(void);

	//virtual TInt SetAudioQuality(TInt aQuality);
	//virtual TInt GetAudioQuality(TInt& aQuality);

	MDataSource* iDataSource;
	MDataSink* iDataSink;
	TBool iSourceAdded;
	TBool iSinkAdded;
	void* iLogFunction;                 
	TestControllerMode iControllerMode; // Controller can be in EFailure (Failure tests) or ENormal (Default) Mode, 
	TBuf8<64> iMemFunctionText;         // Text to be returned to show which function was called last
	TBuf8<64> iMemFunctionText2;		// Same as above but for KLogFunction2
	
	// helper tmp variables used to hold incoming values
	CControllerTimer* iDelayEvent;
	CAsyncCallback* iAsyncCallback;
	TTimeIntervalMicroSeconds tmpTimeIntervalMicroSecs;
	TInt tmpInt1, tmpInt2;
	TReal32 tmpReal32;
	CMMFMetaDataEntry* tmpMeta;
	TFourCC tmpCodec;

	// State variables
	TestControllerState iState;	
	TTimeIntervalMicroSeconds iPosition;
	TTimeIntervalMicroSeconds iDuration;
	TTimeIntervalMicroSeconds iFrameLength;
	TInt iVolume;

	TUid iVideoFormatUid;
	HBufC8* iVideoCodec;
	TFourCC iAudioCodec;
	TInt iVideoBitRate;
	TInt iAudioBitRate;
	TInt iMaxFileSize;
	TSize iFrameSize;
	TBool iAudioEnabled;
	TInt iGain;
	TInt iCameraHandle;

	TTimeIntervalMicroSeconds iRecordTimeAvail;

	CDesC8Array* iSinkVideoMimeTypes;

	TBool iRecordPrepareComplete;
	TBool iPlayPrepareComplete;
	TestControllerRPMode iRecPlayMode;
	CFbsBitmap* iBitmap;
	MMMFVideoFrameMessage* iVideoFrameMessage;

	// Used for faking an error code
	TInt iOpenError;
	TInt iPrepareError;
	TInt iPrepareLeave;
	TInt iPlayError;
	
	TVideoRotation iRotation;
	TReal32 iWidthPercentage;
	TReal32 iHeightPercentage;
	TBool iAntiAliasFiltering;
	TRect iCropRegion;
	// DRM Intent
	TIntent iDRMIntent;
	TInt iFrameReadyError;

#ifdef SYMBIAN_BUILD_GCE
	// graphics surface
	TBool iSurfaceSupported;
	TBool iSurfaceAvailable;
	RArray<TInt> iDisplayIds;	
	TSurfaceId iSurfaceId;
	TSurfaceId iSurfaceId2;
	RSurfaceManager iSurfaceManager;
#endif

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	TBool iSubtitleAvailable;
	TBool iSubtitleEnabled;
	RArray<TMMFSubtitleWindowConfig> iSubtitleWindows;
	RArray<TMMFSubtitleWindowConfig> iExpectedSubtitleWindows;
	TInt iEnableSubtitleCallCount;
	TInt iDisableSubtitleCallCount;
	TInt iAddSubtitleConfigCallCount;
	TInt iRemoveSubtitleConfigCallCount;
	TInt iGetCrpParamCallCount;
	TInt iCrpReadyEventCount;
	TInt iUpdateSubtitleConfigCallCount;
	TLanguage iSubtitleLanguage;
	TBool iLangNotAvailable;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
};

#endif
