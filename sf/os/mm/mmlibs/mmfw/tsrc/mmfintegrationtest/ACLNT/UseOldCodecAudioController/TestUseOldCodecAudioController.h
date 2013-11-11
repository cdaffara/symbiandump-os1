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
//

#ifndef __TESTUSEOLDCODECAUDIOCONTROLLER_H__
#define __TESTUSEOLDCODECAUDIOCONTROLLER_H__

// Standard EPOC32 includes required by this header file

#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>

// Public Media Server includes
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfdatasource.h>

#include <mmf/server/mmfdatapathproxy.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/server/mmfaudiooutput.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/common/mmfstandardcustomcommands.h>


const TInt KOneSecondInMicroSeconds(1000000);


class CMMFMessageHolder : public CBase
	{
public:
	static CMMFMessageHolder* NewL(TMMFMessage& aMessage)
		{return new(ELeave) CMMFMessageHolder(aMessage);}
	void Complete(TInt a) {iMessage.Complete(a);}
private:
	CMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage) {}
	TMMFMessage iMessage;
	};

/**
*
* The public API for clients of the MMFAudioController
*            
*/

class CMMFTestUseOldCodecAudioController : public CMMFController, 
							public MAsyncEventHandler,
							public MMMFAudioPlayDeviceCustomCommandImplementor,
							public MMMFAudioRecordDeviceCustomCommandImplementor,
							public MMMFAudioPlayControllerCustomCommandImplementor,
							public MMMFAudioRecordControllerCustomCommandImplementor,
							public MMMFAudioControllerCustomCommandImplementor
	{
public:
	static CMMFController* NewL();
	virtual ~CMMFTestUseOldCodecAudioController();

	void DevSoundInitialised(TInt aError);
	
	//implement CMMFController ecom plugin interface
	virtual void AddDataSourceL(MDataSource& aDataSource);
	virtual void AddDataSinkL(MDataSink& aDataSink);
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);

	virtual void ResetL();
	virtual void PrimeL();
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL() {} //not required
	virtual void StopL(TMMFMessage& aMessage);

	virtual TTimeIntervalMicroSeconds PositionL() const;
	virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	virtual TTimeIntervalMicroSeconds DurationL() const;
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);
	virtual CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);
	
	//from MAsyncEventHandler
	virtual TInt SendEventToClient(const TMMFEvent& aEvent);

	//from MMMFAudioPlayDeviceCustomCommandImplementor
	virtual void MapdSetVolumeL(TInt aVolume);
	virtual void MapdGetMaxVolumeL(TInt& aMaxVolume);
	virtual void MapdGetVolumeL(TInt& aVolume);
	virtual void MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
	virtual void MapdSetBalanceL(TInt aBalance);
	virtual void MapdGetBalanceL(TInt& aBalance);

	//from MMMFAudioRecordDeviceCustomCommandImplementor
	virtual void MardSetGainL(TInt aGain);
	virtual void MardGetMaxGainL(TInt& aMaxGain);
	virtual void MardGetGainL(TInt& aGain);
	virtual void MardSetBalanceL(TInt aBalance);
	virtual void MardGetBalanceL(TInt& aBalance);

	//from MMMFAudioPlayControllerCustomCommandImplementor
	virtual void MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);
	virtual void MapcDeletePlaybackWindowL();
	virtual void MapcGetLoadingProgressL(TInt& aPercentageComplete);

	//from MMMFAudioRecordControllerCustomCommandImplementor
	virtual void MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);
	virtual void MarcSetMaxDurationL(const TTimeIntervalMicroSeconds& aMaxDuration);
	virtual void MarcSetMaxFileSizeL(TInt aFileSize);
	virtual void MarcCropL(TBool aToEnd);
	virtual void MarcAddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);
	virtual void MarcRemoveMetaDataEntryL(TInt aIndex);
	virtual void MarcReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);

	//from MMMFAudioControllerCustomCommandImplementor
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

public:

/**
 TControllerState is an enumeration within the namespace  CMMFAudioController
 The enumeration provides a set of control states:
 EStopped is the initial state the controller resides in after it has been loaded
 EPrimed is the state the controller is in when it has been associated with datapaths but is not streaming data
 EPlaying is the state the controller is in when it is streaming data
*/
	enum TControllerState
		{
		EStopped,
		EPrimed,
		EPlaying
		};

protected:
	CMMFTestUseOldCodecAudioController();
protected:
	void ConstructL();
	void NegotiateL();

	void ConvertFromDevSoundCapsToSampleRatesL(const TMMFCapabilities& aDevSoundCaps,
											   RArray<TUint>& aSampleRates);
	void ConvertFromDevSoundCapsToNumChannelsL(const TMMFCapabilities& aDevSoundCaps,
											   RArray<TUint>& aNumChannels);
	void ConvertFromDevSoundCapsToDataTypesL(const TMMFCapabilities& aDevSoundCaps,
											 RArray<TFourCC>& aDataTypes);
	void ConvertFromSampleRateToDevSoundCapsL(TUint aSampleRate, TMMFCapabilities& aDevSoundCaps);
	void ConvertFromNumChannelsToDevSoundCapsL(TUint aNumChannels, TMMFCapabilities& aDevSoundCaps);
	void ConvertFromDataTypeToDevSoundCapsL(TFourCC aDataType, TMMFCapabilities& aDevSoundCaps);

	// state accessor functions
    TBool            SetState( TControllerState aState );
	TControllerState State() const ;
	TBool            IsValidStateTransition( TControllerState aState ) const;
	TBool            Invariant() const;
	TBool            IsValidState( TControllerState aState ) const ;
	TBool            ResetPostCondition() const;
	TBool			 SinkFormatRequired( MDataSink& aDataSink) const;
	TBool            SourceFormatRequired(MDataSource& aDataSource) const;
	void             CalculateLeftRightBalance( TInt& aLeft, TInt& aRight, TInt aBalance ) const;
	void			 CalculateBalance( TInt& aBalance, TInt aLeft, TInt aRight ) const;


	//This method is used to assist testing of the audio controller
	virtual TBool IsUnderTest(){return EFalse;}

private:
	/**
	* 
	* This class is required to ensure that CMMFTestUseOldCodecAudioController::PrimeL() runs synchronously 
	* as in some configurations it needs to wait on DevSound initialisation.  This class sits between the 
	* DevSound interface and the actual real DevSound client, propagating all the function calls to it.  
	* The only additional functionality it performs is to inform the CMMFTestUseOldCodecAudioController object 
	* when DevSound initialisation is complete.
	* 
	*/
	class TDevSoundEventHandler : public MDevSoundObserver
		{
	public:
		TDevSoundEventHandler();
		
		void SetInterceptedDevSoundObserver(MDevSoundObserver* aObserver);
		void SetEventObserver(CMMFTestUseOldCodecAudioController* aObserver);
		
		//from MDevSoundObserver
		virtual void InitializeComplete(TInt aError);
		virtual void ToneFinished(TInt aError);
		virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
		virtual void PlayError(TInt aError);
		virtual void BufferToBeEmptied(CMMFBuffer* aBuffer);
		virtual void RecordError(TInt aError);
		virtual void ConvertError(TInt aError);
		virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
		
	private:
		MDevSoundObserver* iInterceptedDevSoundObserver;	//Not owned
		CMMFTestUseOldCodecAudioController* iEventObserver; //Not owned
		};
	
private:
	MDataSource*             iDataSource;
	MDataSink*               iDataSink;
	CMMFDataPath*            iDataPath;
	CMMFFormatDecode*        iSourceFormat;
	CMMFFormatEncode*        iSinkFormat;
	TMediaId                 iMediaId;
	TControllerState         iState;
	TMMFPrioritySettings     iPrioritySettings;
	TBool                    iSourceAndSinkAdded;

	TBool                    iStoppingRecording;

	CMMFMessageHolder*       iStoppingMessage;
	TDevSoundEventHandler	 iDevSoundEventHandler;
	CActiveSchedulerWait*	 iWaitForAsyncService;
	};


#endif
