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

#ifndef __MMFDEVSOUNDPROXY_H__
#define __MMFDEVSOUNDPROXY_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "MmfBtAudioClientServer.h"
#include "MmfBtAudioServerProxy.h"


class RMMFDevSoundProxy; // declared here.

NONSHARABLE_CLASS( RMMFDevSoundProxy ): public RMmfSessionBase
/**
*@internalTechnology
*/
	{
public:

	IMPORT_C TInt Open();
	IMPORT_C TInt SetDevSoundInfo();
	IMPORT_C TInt InitializeL(TMMFState aMode);
	IMPORT_C TInt InitializeL(TUid aHWDev, TMMFState aMode);
	IMPORT_C TInt InitializeL(CArrayPtr<TUid> /*aHWDevArray*/, TMMFState /*aMode*/);
	IMPORT_C TInt InitializeL(TFourCC aDesiredFourCC, TMMFState aMode);
	IMPORT_C TMMFCapabilities Capabilities();
	IMPORT_C TMMFCapabilities Config();
	IMPORT_C TInt SetConfigL(const TMMFCapabilities& aConfig);
	IMPORT_C TInt MaxVolume();
	IMPORT_C TInt Volume();
	IMPORT_C TInt SetVolume(TInt aVolume);
	IMPORT_C TInt MaxGain();
	IMPORT_C TInt Gain();
	IMPORT_C TInt SetGain(TInt aGain);
	IMPORT_C void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	IMPORT_C void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage);
	IMPORT_C void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage);
	IMPORT_C void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage);
	IMPORT_C void Close();
	IMPORT_C void PlayInitL();
	IMPORT_C void RecordInitL();
	IMPORT_C void PlayData();
	IMPORT_C void RecordData();
	IMPORT_C void Stop();
	IMPORT_C void Pause();
	IMPORT_C void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);
	IMPORT_C void PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	IMPORT_C void PlayDTMFStringL(const TDesC& aDTMFString);
	IMPORT_C void PlayToneSequenceL(const TDesC8& aData);
	IMPORT_C void PlayFixedSequenceL(TInt aSequenceNumber);
	IMPORT_C void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
								 TTimeIntervalMicroSeconds32& aToneOffLength,
								 TTimeIntervalMicroSeconds32& aPauseLength);
	IMPORT_C void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
	IMPORT_C void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings);
	IMPORT_C void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings);
	IMPORT_C TInt SamplesRecorded();
	IMPORT_C TInt SamplesPlayed();
	IMPORT_C void SetToneRepeats(TInt aRepeatCount, const TTimeIntervalMicroSeconds& aRepeatTrailingSilence);
	IMPORT_C void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	IMPORT_C const TDesC& FixedSequenceName(TInt aSequenceNumber);
	IMPORT_C void ConvertInitL();
	IMPORT_C void ConvertData();
	IMPORT_C TAny* CustomInterface(TUid aInterfaceId);
	IMPORT_C TInt FixedSequenceCount();

	IMPORT_C void ReceiveInitializeCompleteEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceiveInitializeCompleteEvent();
	IMPORT_C void ReceiveBufferToBeFilledEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceiveBufferToBeFilledEvent();
	IMPORT_C void ReceiveBufferToBeEmptiedEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceiveBufferToBeEmptiedEvent();
	IMPORT_C void ReceivePlayErrorEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceivePlayErrorEvent();
	IMPORT_C void ReceiveRecordErrorEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceiveRecordErrorEvent();
	IMPORT_C void ReceiveToneFinishedEvent(TMMFDevSoundProxyHwBufPckg& aSetPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceiveToneFinishedEvent();
	IMPORT_C void ReceiveSendEventToClientEvent(TMMFEventPckg& aEventPckg, TRequestStatus& aStatus);
	IMPORT_C void CancelReceiveSendEventToClientEvent();

	IMPORT_C void CancelReceiveEvents();
	IMPORT_C void SetBuffer(CMMFDataBuffer* aBuffer);
	IMPORT_C TInt GetRecordedBufferL(CMMFDataBuffer& aBuffer);

	IMPORT_C TInt RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData = KNullDesC8);
	IMPORT_C TInt CancelRegisterAsClient(TUid aEventType);
	IMPORT_C TInt GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData);
	IMPORT_C TInt WillResumePlay();
	IMPORT_C TInt SetClientThreadInfo(TThreadId& aTid);

private:
	TMMFDevSoundProxySettingsPckg iDspsPckg;
	CMMFDataBuffer* iBuffer;
	HBufC* iSeqName;
	};

#endif
