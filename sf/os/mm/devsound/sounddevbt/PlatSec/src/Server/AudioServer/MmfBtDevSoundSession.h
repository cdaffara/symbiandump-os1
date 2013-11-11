// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFBTDEVSOUNDSESSION_H__
#define __MMFBTDEVSOUNDSESSION_H__

#include <e32base.h>
#include <e32std.h>
#include <mmfbase.h>
#include <mmfutilities.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <mmf/server/sounddevice.h>
#include "MmfBtAudioClientServer.h"


class CMMFAudioServer;
class CMMFDevSoundServer;
class CMMFDevSoundSvrImp;

class CMMFDevSoundSession : public CMmfIpcSession, public MDevSoundObserver
	{
public:
	static CMMFDevSoundSession* NewL(RServer2& aPolicyServerHandle);
	~CMMFDevSoundSession();

	void CreateL(const CMmfIpcServer& aServer);
	void ServiceL(const RMmfIpcMessage& aMessage);
	TInt DevSoundSessionId(void) {return iDevSoundSessionId;};
	void SendEventToClient(/*TMMFAudioServerEvent& aEvent*/);
	TBool CheckClientCapabilities();


	TBool DoInitialize1L(const RMmfIpcMessage& aMessage);
	TBool DoInitialize2L(const RMmfIpcMessage& aMessage);
	TBool DoInitialize3L(const RMmfIpcMessage& aMessage);
	TBool DoInitialize4L(const RMmfIpcMessage& aMessage);
	TBool DoCapabilitiesL(const RMmfIpcMessage& aMessage);
	TBool DoConfigL(const RMmfIpcMessage& aMessage);
	TBool DoSetConfigL(const RMmfIpcMessage& aMessage);
	TBool DoMaxVolumeL(const RMmfIpcMessage& aMessage);
	TBool DoVolumeL(const RMmfIpcMessage& aMessage);
	TBool DoSetVolumeL(const RMmfIpcMessage& aMessage);
	TBool DoMaxGainL(const RMmfIpcMessage& aMessage);
	TBool DoGainL(const RMmfIpcMessage& aMessage);
	TBool DoSetGainL(const RMmfIpcMessage& aMessage);
	TBool DoGetPlayBalanceL(const RMmfIpcMessage& aMessage);
	TBool DoSetPlayBalanceL(const RMmfIpcMessage& aMessage);
	TBool DoGetRecordBalanceL(const RMmfIpcMessage& aMessage);
	TBool DoSetRecordBalanceL(const RMmfIpcMessage& aMessage);
	TBool DoPlayInitL(const RMmfIpcMessage& aMessage);
	TBool DoRecordInitL(const RMmfIpcMessage& aMessage);
	TBool DoPlayDataL(const RMmfIpcMessage& aMessage);
	TBool DoRecordDataL(const RMmfIpcMessage& aMessage);
	TBool DoStopL(const RMmfIpcMessage& aMessage);
	TBool DoPauseL(const RMmfIpcMessage& aMessage);
	TBool DoPlayToneL(const RMmfIpcMessage& aMessage);
	TBool DoPlayDualToneL(const RMmfIpcMessage& aMessage);
	TBool DoPlayDTMFStringL(const RMmfIpcMessage& aMessage);
	TBool DoPlayToneSequenceL(const RMmfIpcMessage& aMessage);
	TBool DoPlayFixedSequenceL(const RMmfIpcMessage& aMessage);
	TBool DoSetDTMFLengthsL(const RMmfIpcMessage& aMessage);
	TBool DoSetVolumeRampL(const RMmfIpcMessage& aMessage);
	TBool DoGetSupportedInputDataTypesL(const RMmfIpcMessage& aMessage);
	TBool DoCopyFourCCArrayDataL(const RMmfIpcMessage& aMessage);
	TBool DoGetRecordedBufferL(const RMmfIpcMessage& aMessage);
	TBool DoSamplesRecordedL(const RMmfIpcMessage& aMessage);
	TBool DoSamplesPlayedL(const RMmfIpcMessage& aMessage);
	TBool DoSetToneRepeatsL(const RMmfIpcMessage& aMessage);
	TBool DoSetPrioritySettingsL(const RMmfIpcMessage& aMessage);
	TBool DoFixedSequenceNameL(const RMmfIpcMessage& aMessage);
	TBool DoConvertInitL(const RMmfIpcMessage& aMessage);
	TBool DoConvertDataL(const RMmfIpcMessage& aMessage);
	TBool DoFixedSequenceCountL(const RMmfIpcMessage& aMessage);
	TBool DoGetSupportedOutputDataTypesL(const RMmfIpcMessage& aMessage);

	TBool DoInitializeCompleteEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelInitializeCompleteEventL(const RMmfIpcMessage& aMessage);
	TBool DoBufferToBeFilledEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelBufferToBeFilledEventL(const RMmfIpcMessage& aMessage);
	TBool DoBufferToBeEmptiedEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelBufferToBeEmptiedEventL(const RMmfIpcMessage& aMessage);
	TBool DoPlayErrorEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelPlayErrorEventL(const RMmfIpcMessage& aMessage);
	TBool DoRecordErrorEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelRecordErrorEventL(const RMmfIpcMessage& aMessage);
	TBool DoToneFinishedEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelToneFinishedEventL(const RMmfIpcMessage& aMessage);
	TBool DoSendEventToClientEventL(const RMmfIpcMessage& aMessage);
	TBool DoCancelSendEventToClientEventL(const RMmfIpcMessage& aMessage);
	TBool DoRegisterAsClientL(const RMmfIpcMessage& aMessage);
	TBool DoCancelRegisterAsClientL(const RMmfIpcMessage& aMessage);
	TBool DoGetResourceNotificationDataL(const RMmfIpcMessage& aMessage);
	TBool DoWillResumePlayL(const RMmfIpcMessage& aMessage);
	TBool DoSetClientThreadInfoL(const RMmfIpcMessage& aMessage);
	
    void InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode);
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode);
	void InitializeL(MDevSoundObserver& aDevSoundObserver, CArrayPtr<TUid> aHWDevArray, TMMFState aMode);
	void InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode);

	TMMFCapabilities Capabilities();
	TMMFCapabilities Config() const;
	void SetConfigL(const TMMFCapabilities& aCaps);
	TInt MaxVolume();
	TInt Volume();
	void SetVolume(TInt aVolume);
	TInt MaxGain();
	TInt Gain();
	void SetGain(TInt aGain);
	void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage);
	void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage);
	void PlayInitL();
	void RecordInitL();
	void PlayData();
	void RecordData();
	void Stop();
	void Pause();
	TInt SamplesRecorded();
	TInt SamplesPlayed();
	void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	void PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	void PlayDTMFStringL(const TDesC& aDTMFString);
	void PlayToneSequenceL(const TDesC8& aData);
	void PlayFixedSequenceL(TInt aSequenceNumber);
	void SetToneRepeats(TInt aRepeatCount,
					const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);
	void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength);

	void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	void ConvertInitL();
	void ConvertData();
	TAny* CustomInterface(TUid aInterfaceId);
	TInt FixedSequenceCount();
	const TDesC& FixedSequenceName(TInt aSequenceNumber);
	void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;
	void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const;

	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	
	TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	TInt CancelRegisterAsClient(TUid aEventType);
	TInt GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData);
	TInt WillResumePlay();
	
protected:
	CMMFDevSoundSession();
	void ConstructL(RServer2& aPolicyServerHandle);
	CMMFDevSoundSvrImp* iBody;
	
private:
	TInt iDevSoundSessionId;
	RMmfIpcMessage* iBTBFMessage;
	RMmfIpcMessage* iBTBEMessage;
	RMmfIpcMessage* iPEMessage;
	RMmfIpcMessage* iREMessage;
	RMmfIpcMessage* iTFMessage;
	RMmfIpcMessage* iICMessage;
	RMmfIpcMessage* iSETCMessage;
	CMMFDataBuffer* iBufferPlay;
	CMMFDataBuffer* iBufferRecord;
	RArray<TFourCC> iArray;
	HBufC* iDTMFString;
	HBufC8* iToneSeqBuf;
	TBool iClientHasCaps;
	};
	
	

#endif
