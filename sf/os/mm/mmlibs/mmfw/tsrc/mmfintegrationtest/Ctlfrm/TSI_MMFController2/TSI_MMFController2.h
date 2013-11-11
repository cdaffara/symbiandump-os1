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
// Test controller for use with MMF Controller Integration Tests (TSI_MMFCtlFrm)
// 
//


#ifndef TSI_MMFCONTROLLER2_H
#define TSI_MMFCONTROLLER2_H

#include <e32std.h>
#include <e32base.h>
#include <e32twin.h>

#include <ecom/implementationproxy.h>

#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>

#include <testframework.h>

#include "TSI_MMFController2UIDs.hrh"
#include "TSI_MMFCustomCommands.h"
#include <mmf/common/mmfstandardcustomcommands.h>
#include "TSI_MmfCodes.h"

#define KTextBufLen	64

// UID for custom command parser
#define KTSIMmfCustomCommandParserUid          0x101F72B4

enum TTSIMmfController2Mode
	{
	ENormal = 0,
	EFailure = 1,
	EMemoryFailure = 2
	};

class CTSIMmfController2 : public CMMFController,
						   public CTSIMmfCustomCommandImplementor,
						   public MMMFAudioPlayDeviceCustomCommandImplementor,
						   public MMMFAudioRecordDeviceCustomCommandImplementor,
						   public MMMFAudioPlayControllerCustomCommandImplementor,
						   public MMMFAudioRecordControllerCustomCommandImplementor,
						   public MMMFAudioControllerCustomCommandImplementor,
						   public MMMFVideoControllerCustomCommandImplementor
						   
	{
public:
	static CTSIMmfController2* NewL(void);
	static CTSIMmfController2* NewLC(void);


	// implement CMMFController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void RemoveSourceSinkL();
	virtual void ResetL();
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

	virtual void MvcGetVideoFrameSizeL(TSize& aVideoFrameSize);
	virtual void MvcSetFrameRateL(TReal32 aFramesPerSecond);
	virtual void MvcGetFrameRateL(TReal32& aFramesPerSecond);
	virtual void MvcGetFrameL(TInt aFrameNumber);
	virtual void MvcGetVideoDimensionL(TRect& aVideoDimension);
	virtual void MvcGetAudioCodecL(TFourCC& aCodec);
	virtual void MvcGetVideoBitRateL(TInt& aBitRate);
	virtual void MvcGetAudioBitRateL(TInt& aBitRate);
	virtual void MvcSetDisplayWindowL(const TPoint& aWindow, const TRect& aWindowRect);
	virtual void MvcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	virtual void MvcRemoveMetaDataEntryL(TInt aIndex);
	virtual void MvcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);
	virtual void MvcSetSinkFormatL(TUid aVideoFormatUid);
	virtual void MvcSetSinkCodecsL(TFourCC aVideoCodec, TFourCC aAudioCodec);
	virtual void MvcGetVideoMimeTypeL(TDes8& aMimeType);

	TInt GetMemberFunctionCalled(TMMFMessage& aMessage);
	void SetControllerMode(TMMFMessage& aMessage);

	TInt GetHeapAddress(TMMFMessage& aMessage);
	TInt GetThreadId(TMMFMessage& aMessage);
	TInt GetPluginUid(TMMFMessage& aMessage);

private:
	CTSIMmfController2(void);
	~CTSIMmfController2(void);
	void ConstructL(void);

	virtual TInt SetAudioQuality(TInt aQuality);
	virtual TInt GetAudioQuality(TInt& aQuality);

private:
	MDataSource* iDataSource;
	MDataSink* iDataSink;
	TBool iSourceAdded;
	TBool iSinkAdded;
	void* iLogFunction;                 
	TTSIMmfController2Mode iControllerMode; 
	// Controller can be in EFailure (Failure tests) or ENormal (Default) Mode, 
	TBuf8<KTextBufLen> iMemFunctionText;
	// Text to be returned to show which function was called last
	
	};

#endif
