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
//


#ifndef OGGPLAYCONTROLLER_H
#define OGGPLAYCONTROLLER_H


#include <e32base.h>
#include <ecom/implementationproxy.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfaudiooutput.h>
#include <mmf/common/mmfdrmcustomcommands.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include "OggPlayControllerUIDs.hrh"
#include "oggdecode.h"

#define KMMFFourCCCodeVORB TFourCC('V', 'O', 'R', 'B')
const TInt KNumSampleRates = 11;

struct TSampleRateTable
    	{
		TInt iRate;
		TMMFSampleRate iRateEnum;
		};

/*
 TMmfOggPlayControllerPanics is an enumeration with the panic codes internal to OggPlayController.
 EBadCall indicates that the particular call is not supposed to be made in the OggPlayController.
 */
enum TMmfOggPlayControllerPanics
	{
	EBadCall,
	};
	
class MOggDecodeObserver;

/**
@internalTechnology

The public API of COggPlayController. It mainly implements the public APIs in CMMFController. It also implements PlayCustomInterfaces 
to support extended functionality during playing. It is reponsible for controlling the data transfer from Source to DevSound.
*/
class COggPlayController : public CMMFController, 
                           public MMMFAudioPlayDeviceCustomCommandImplementor,
                           public MMMFAudioPlayControllerCustomCommandImplementor,
                           public MMMFDRMCustomCommandImplementor,
						   public MMMFResourceNotificationCustomCommandImplementor,
						   public MMMFAudioControllerCustomCommandImplementor,
						   public MDevSoundObserver,
						   public MOggDecodeObserver,
						   public MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor
	{
public:
    static COggPlayController* NewL();
    ~COggPlayController();
    
    void AddDataSourceL(MDataSource& aSource);
    void AddDataSinkL(MDataSink& aSink);
    void RemoveDataSourceL(MDataSource& aSource);
    void RemoveDataSinkL(MDataSink& aSink);
    
    void ResetL();
    void PrimeL();
    void PrimeL(TMMFMessage& aMessage);
	void PlayL();
    void PauseL(TMMFMessage& aMessage);
    void PauseL();
    void StopL();
    
    TTimeIntervalMicroSeconds PositionL() const;
    void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
    TTimeIntervalMicroSeconds DurationL() const;
    
    void SetPrioritySettings(const TMMFPrioritySettings& aSettings);
    void GetNumberOfMetaDataEntriesL(TInt& aNumber);
    CMMFMetaDataEntry* GetMetaDataEntryL(TInt aIndex);
    
    // MMMFAudioPlayDeviceCustomCommandImplementor
  	void MapdSetVolumeL(TInt aVolume);
    void MapdGetMaxVolumeL(TInt& aMaxVolume);
    void MapdGetVolumeL(TInt& aVolume);
    void MapdSetVolumeRampL(const TTimeIntervalMicroSeconds& aRampDuration);
    void MapdSetBalanceL(TInt aBalance);
    void MapdGetBalanceL(TInt& aBalance);
    
    // MMMFAudioPlayControllerCustomCommandImplementor
	void MapcSetPlaybackWindowL(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);
	void MapcDeletePlaybackWindowL();
	void MapcGetLoadingProgressL(TInt& aPercentageComplete);
    
    // from MMMFDRMCustomCommandImplementor
	TInt MdcExecuteIntent(ContentAccess::TIntent aIntent);
	TInt MdcEvaluateIntent(ContentAccess::TIntent aIntent);
	TInt MdcDisableAutomaticIntent(TBool aDisableAutoIntent);
	TInt MdcSetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
	
	// MMMFResourceNotificationCustomCommandImplementor 
	void MarnRegisterAsClientL(TUid aEventType,const TDesC8& aNotificationRegistrationData);
	void MarnCancelRegisterAsClientL(TUid aEventType);
	void MarnGetResourceNotificationDataL(TUid aEventType,TDes8& aNotificationData);
	void MarnWillResumePlayL();
	
	//from MMMFAudioControllerCustomCommandImplementor
 	void MacSetSourceSampleRateL(TUint aSampleRate);
 	void MacSetSourceNumChannelsL(TUint aNumChannels);
 	void MacSetSourceFormatL(TUid aFormatUid);
 	void MacSetSinkSampleRateL(TUint aSampleRate);
 	void MacSetSinkNumChannelsL(TUint aNumChannels);
 	void MacSetSinkFormatL(TUid aFormatUid);
 	void MacSetCodecL(TFourCC aSourceDataType, TFourCC aSinkDataType);
 	void MacSetSourceBitRateL(TUint aBitRate);
 	void MacSetSourceDataTypeL(TFourCC aDataType);
 	void MacSetSinkBitRateL(TUint aRate);
 	void MacSetSinkDataTypeL(TFourCC aDataType);
 	void MacGetSourceSampleRateL(TUint& aRate);
 	void MacGetSourceBitRateL(TUint& aRate);
 	void MacGetSourceNumChannelsL(TUint& aNumChannels);
 	void MacGetSourceFormatL(TUid& aFormat);
 	void MacGetSourceDataTypeL(TFourCC& aDataType);
 	void MacGetSinkSampleRateL(TUint& aRate);
 	void MacGetSinkBitRateL(TUint& aRate);
 	void MacGetSinkNumChannelsL(TUint& aNumChannels);
 	void MacGetSinkFormatL(TUid& aFormat);
 	void MacGetSinkDataTypeL(TFourCC& aDataType);
 	void MacGetSupportedSourceSampleRatesL(RArray<TUint>& aSupportedRates);
 	void MacGetSupportedSourceBitRatesL(RArray<TUint>& aSupportedRates);
 	void MacGetSupportedSourceNumChannelsL(RArray<TUint>& aSupportedChannels);
 	void MacGetSupportedSourceDataTypesL(RArray<TFourCC>& aSupportedDataTypes);
 	void MacGetSupportedSinkSampleRatesL(RArray<TUint>& aSupportedRates);
 	void MacGetSupportedSinkBitRatesL(RArray<TUint>& aSupportedRates);
 	void MacGetSupportedSinkNumChannelsL(RArray<TUint>& aSupportedChannels);
 	void MacGetSupportedSinkDataTypesL(RArray<TFourCC>& aSupportedDataTypes);
	
	//from MMMFAudioPlayControllerSetRepeatsCustomCommandImplementor
	TInt MapcSetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);
	
	//MDevSoundObserver mixin implementations	
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	
	//from MOggDecodeObserver
	void BufferFilled();
	
	//internal to COggPlayController
	static void LeaveIfNullL(TAny* aPtr);
	void SendErrorToClient(TInt aError);
	void StartPrimeL();
private:
	void DoFillThisBufferL(CMMFBuffer* aBuffer);
	void PlaySilence();

	//called when trailing silence playback is complete
	static TInt RepeatTrailingSilenceTimerComplete(TAny* aController);
	void DoRepeatTrailingSilenceTimerComplete();
	void ResetDevSoundTime();
private:
    COggPlayController();
    void ConstructL();
    
private:
    /**
	@internalTechnology
	
	This class provides the mechanism to store the client message during the asynchronous operation
	in the controller sothat it can be completed after the operation is completed. In specific, during PrimeL(), PauseL().
	*/
	class CMMFMessageHolder : public CBase
		{
	public:
		static CMMFMessageHolder* NewL(TMMFMessage& aMessage);
		void Complete(TInt aError);
	private:
		CMMFMessageHolder(TMMFMessage& aMessage);
		TMMFMessage iMessage;
		};
    /**
	@internalTechnology
	
	This class is used to receive events from the source and sink on behalf of the controller.
	
	The reason behind adding an extra class for receiving events is that COggPlayController cannot derive
	from both MAsyncEventHandler and MDevSoundObserver as there is a clash in the API they contain.
	*/
    class CSourceSinkEventHandler : public MAsyncEventHandler
		{
		public:
			CSourceSinkEventHandler(MDevSoundObserver& aParent);
			virtual ~CSourceSinkEventHandler();
		public:
			virtual TInt SendEventToClient(const TMMFEvent& aEvent);
		private:
			MDevSoundObserver& iParent;
		};
	/**
 	@internalTechnology
 	
 	The enumeration provides a set of controller states:
 	
 	EStopped is the initial state the controller resides in after it has been loaded
 	EPriming is the state the controller is in when it is waiting for the devsound to be initialized and configured the codec.
 	EPrimed is the state the controller is in when it has initialized devsound and configured the codec.
 	EPlaying is the state the controller is in when it is streaming data from source to devsound.
	EPausing is the state the controller is in when it is waiting for devsound initialization before it is actually paused.
	EPaused is the state the controller is in when it is paused
	ESeeking is the state the controller is in when it is waiting for devsound initialization before it is repositioned
	ESeek is the state the controller is in when it is repositioned.
	*/
    enum TControllerState
        {
        EStopped,
        EPriming,
        EPrimed,
        EPlaying,
        EPausing,
        EPaused,
        ESeeking,
        ESeek
        };
    TControllerState iState;
    TBool iDisableAutoIntent;
    //Source which is not owned by the controller
    CMMFClip* iClip;
    MDataSink* iSink;
    TTimeIntervalMicroSeconds iPlayWindowStart;
    TTimeIntervalMicroSeconds iPlayWindowEnd;
    COggDecode* iOggSource;
    CMMFMessageHolder*	iMessage;
    CSourceSinkEventHandler* iEventHandler;
    //Extracted from MDataSink passed by the MMF. Not owned by the controller
    CMMFDevSound* iMMFDevSound;
	TTimeIntervalMicroSeconds iStoredPosition;
    TMMFPrioritySettings    iPrioritySettings;
		
	/**
    The number of times to repeat the audio sample. 
    */
    TInt iNumberOfTimesToRepeat;
	    
	/**
    The number of times to repeat the audio sample. 
    */
    TInt iNumberOfTimesPlayed;
	
	/**
    The time interval of the trailing silence in microseconds.
	*/
	TTimeIntervalMicroSeconds iTrailingSilence;
	
	/**
    The time interval of the trailing silence left to play in microseconds.
	*/
	TTimeIntervalMicroSeconds iTrailingSilenceLeftToPlay;
	
	/**
    The time interval left for the play to complete
	*/
	TTimeIntervalMicroSeconds iTimeLeftToPlayComplete;
	/**
	Timer used for trailing silences when repeating
	*/
	CPeriodic* iRepeatTrailingSilenceTimer;

	/**
	Pointer to sourcebuffer
	**/
	CMMFBuffer* iSourceBuffer;
	TBool iVerifyPlayComplete;
    TTimeIntervalMicroSeconds iDevSoundResetTime;
    TInt iDevSoundResetSamples;
    TBool iGetTimePlayedSupported;
    CActiveSchedulerWait* iActiveSchedulerWait;
    TInt iInitializeState;
	};

#endif // OGGPLAYCONTROLLER_H
