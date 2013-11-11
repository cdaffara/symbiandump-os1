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


#ifndef TS_CMMFTESTCONTROLLER_H
#define TS_CMMFTESTCONTROLLER_H

#include <e32std.h>
#include <e32base.h>
//#include <e32twin.h>


#include <ecom/implementationproxy.h>
//#include <w32std.h> // For RWindow (Needs Adding)

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>

//#include <TestFramework.h>

#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#include <mmf/common/mmfvideosubtitlecustomcommands.h>
#include <gdi.h>
#endif // SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

#include "TS_CMMFTestCustomCommands.h"
#include "CmmfTestControllerUIDs.hrh"
#include "TS_Codes.h"


const TUint KExpectedUint = 0x10111213;
const TUid KExpectedUid = {0x20212223};
const TInt32 KExpectedFourCC1 = 0x41424344;
const TInt32 KExpectedFourCC2 = 0x44434241;

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
_LIT8(KGetCrpParams, "MvpsusGetCrpParametersL called");
_LIT8(KUpdateSubConfig, "MvpsusUpdateSubtitleConfigL called");
_LIT8(KAddSubConfig, "MvpsusAddSubtitleConfigL called");
_LIT8(KRemoveSubConfig, "MvpsusRemoveSubititleConfigL called");
_LIT8(KSubAvailable, "MvpsusGetSubtitlesAvailableL called");
_LIT8(KDisableSub, "MvpsusDisableSubtitlesL called");
_LIT8(KEnableSub, "MvpsusEnableSubtitlesL called");
_LIT8(KGetSubLang, "MvpsusGetSubtitleLanguageL called");
_LIT8(KSetSubLang, "MvpsusSetSubtitleLanguageL called");
_LIT8(KGetSubLangSup, "MvpsusGetSupportedSubtitleLanguagesL called");

const TInt KSubtitleWindowId = 55;
const TInt KSubtitleWsGraphicId = 150;
#define KSubtitleCrpRect TRect(10,20,30,40)
const TDisplayMode KSubtitleDisplayMode = EColor16MAP;
const TVideoRotation KSubtitleRotation = EVideoRotationClockwise270;
const TLanguage KSubtitleLanguage0 = ELangCanadianEnglish;
const TLanguage KSubtitleLanguage1 = ELangAmerican;
const TLanguage KSubtitleLanguage2 = ELangEnglish;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

enum TestControllerMode
	{
	ENormal = 0,
	EFailure = 1,
	EMemoryFailure = 2
	};

class CMMFTestController : public CMMFController,
						   public MMMFTestCustomCommandImplementor,
						   public MMMFAudioPlayDeviceCustomCommandImplementor,
						   public MMMFAudioRecordDeviceCustomCommandImplementor,
						   public MMMFAudioPlayControllerCustomCommandImplementor,
						   public MMMFAudioRecordControllerCustomCommandImplementor,
						   public MMMFAudioControllerCustomCommandImplementor
#ifdef SYMBIAN_BUILD_GCE
						   , public MMMFVideoPlaySurfaceSupportCustomCommandImplementor //,
#endif // SYMBIAN_BUILD_GCE						   
						   //public MMMFVideoControllerCustomCommandImplementor 
						   //XXX - these custom commands are changing...
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
						   , public MMMFVideoPlaySubtitleSupportCustomCommandImplementor
#endif // SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
{
public:
	static CMMFTestController* NewL(void);
	static CMMFTestController* NewLC(void);

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

	//---------------------------------//
	// MMFTestCustomCommandsImplmentor //
	//---------------------------------//
	virtual TInt MtccSetAudioQuality(TInt aQuality);
	virtual TInt MtccGetAudioQuality(TInt& aQuality);
	virtual void MtccCustomCommandSyncL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandSyncWithReturnL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandAsyncL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandAsyncWithReturnL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandRequestGeneralEventL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandSimulateReloadingL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandCheckUrlL(TMMFMessage& aMessage);
	virtual void MtccCustomCommandCheckLongUrlL(TMMFMessage& aMessage);

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


	//-------------------------------------------------//
	// MMMFAudioPlayControllerCustomCommandImplementor //
	//-------------------------------------------------//

	virtual void MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);
	virtual void MapcDeletePlaybackWindowL();
	virtual void MapcGetLoadingProgressL(TInt& aPercentageComplete);


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
	virtual void MacSetSourceBitRateL(TUint aBitRate);
	virtual void MacSetSourceDataTypeL(TFourCC aDataType);
	virtual void MacSetSinkBitRateL(TUint aRate);
	virtual void MacSetSinkDataTypeL(TFourCC aDataType);
	virtual void MacGetSourceSampleRateL(TUint& aRate);
	virtual void MacGetSourceBitRateL(TUint& aRate);
	virtual void MacGetSourceNumChannelsL(TUint& aNumChannels);
	virtual void MacGetSourceFormatL(TUid& aFormat);
	virtual void MacGetSourceDataTypeL(TFourCC& aDataType);
	virtual void MacGetSinkSampleRateL(TUint& aRate);
	virtual void MacGetSinkBitRateL(TUint& aRate);
	virtual void MacGetSinkNumChannelsL(TUint& aNumChannels);
	virtual void MacGetSinkFormatL(TUid& aFormat);
	virtual void MacGetSinkDataTypeL(TFourCC& aDataType);
	virtual void MacGetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates);
	virtual void MacGetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates);
	virtual void MacGetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels);
	virtual void MacGetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes);
	virtual void MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates);
	virtual void MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates);
	virtual void MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels);
	virtual void MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes);


	//---------------------------------------------//
	// MMMFVideoControllerCustomCommandImplementor //
	//---------------------------------------------//

	virtual void MvcSetFrameRateL(TReal32 aFramesPerSecond);
	virtual void MvcGetFrameRateL(TReal32& aFramesPerSecond);
	virtual void MvcGetFrameL(TInt aFrameNumber);
	virtual void MvcGetVideoDimensionL(TRect& aVideoDimension);
	virtual void MvcSetDisplayWindowL(RWindow& /*aWindow*/, const TRect& aWindowRect);
	virtual void MvcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	virtual void MvcRemoveMetaDataEntryL(TInt aIndex);
	virtual void MvcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);


	//----------------------------------------------------//
	// MMMFVideoPlaySurfaceSupportCustomCommandImplemetor //
	//----------------------------------------------------//


#ifdef SYMBIAN_BUILD_GCE
	virtual void MvpssUseSurfacesL();
	virtual void MvpssGetSurfaceParametersL(TSurfaceId& aSurfaceId, 
							TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio);
	virtual void MvpssSurfaceRemovedL(const TSurfaceId& aSurfaceId);
#endif // SYMBIAN_BUILD_GCE	

	//--------------------------------------------------------//
	// 	MMMFVideoPlaySubtitleSupportCustomCommandImplementor  //
	//--------------------------------------------------------//
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	virtual void MvpsusGetCrpParametersL(TInt aWindowId, TWsGraphicId& aId, TRect& aCrpRect);
	virtual void MvpsusUpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig);
	virtual void MvpsusAddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig);
	virtual void MvpsusRemoveSubtitleConfigL(TInt aWindowId);
	virtual void MvpsusGetSubtitlesAvailableL(TBool& aAvailable);
	virtual void MvpsusDisableSubtitlesL();
	virtual void MvpsusEnableSubtitlesL();
	virtual void MvpsusGetSubtitleLanguageL(TLanguage& aLanguage);
	virtual void MvpsusGetSupportedSubtitleLanguagesL(RArray<TLanguage>& aAvailable);
	virtual void MvpsusSetSubtitleLanguageL(TLanguage aLanguage);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

	
	TInt GetMemberFunctionCalled(TMMFMessage& aMessage);
	void SetControllerMode(TMMFMessage& aMessage);
	void ThreadStackSize(TMMFMessage& aMessage);
	
	TInt StartReloading();
	TInt CompleteReloading();
	
	class CAsyncCallback : public CActive
		{
	public:
		enum State {
			EAsyncCallback = 0,
			ESimulateReloading
			};
	public:
		CAsyncCallback();
		~CAsyncCallback();

		void RequestCallback(TMMFMessage& aMessage, TInt aError);
		void RunL();
		void DoCancel();
		
		TInt LoadingProgress();

	private:
		TMMFMessage* iMessage;
		TTimeIntervalMicroSeconds iInterval;
		TInt iIncrement;
		TInt iProgress;
		TInt iError;
		State iState;
		};

	class CReloadingSimulator : public CTimer
		{
	public:
		enum State {
			ETmrStartReloading = 0,
			ETmrReloading
			};
	public:
		static CReloadingSimulator* NewL(CMMFTestController* aController);
		~CReloadingSimulator();

		void StartReloading(TTimeIntervalMicroSeconds aInterval, TInt aIncrement);
		void RequestCallback(TMMFMessage& aMessage, TInt aError);
		void RunL();
		void DoCancel();
		
		TInt LoadingProgress();

	private:
		CReloadingSimulator(CMMFTestController* aController);
		TMMFMessage* iMessage;
		TTimeIntervalMicroSeconds iInterval;
		TInt iIncrement;
		TInt iProgress;
		TInt iError;
		State iState;
		CMMFTestController* iController;
		};

private:
	CMMFTestController(void);
	~CMMFTestController(void);
	void ConstructL(void);
	
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	void SetSubtitlesAvailable(TMMFMessage& aMessage);
#endif // SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

private:
	RArray<MDataSource*> iDataSources;
	RArray<MDataSink*> iDataSinks;

	MDataSource* iLastDataSourceAdded;
	MDataSink* iLastDataSinkAdded;
	TBool iSourceAdded;
	TBool iSinkAdded;
	void* iLogFunction;                 
	TestControllerMode iControllerMode; // Controller can be in EFailure (Failure tests) or ENormal (Default) Mode, 
	TBuf8<64> iMemFunctionText;         // Text to be returned to show which function was called last
	TBool iTestPassed;
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	TInt iSubtitlesAvailableErr;
#endif // SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	
	CAsyncCallback* iAsyncCallback;
	CReloadingSimulator* iReloadingSimulator;
};

#endif
