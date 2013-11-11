// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHAR_A3F_DEVSOUND_RECORDTEST_H
#define CHAR_A3F_DEVSOUND_RECORDTEST_H

// system includes
#include "char_a3f_devsound_testbase.h"

#include "char_a3f_devsound_playrecordclients.h"


/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0001
 */
class RA3FDevSoundRecordConfigurationSetConfigTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetConfigTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetConfigTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iSampleRate;
	TInt iChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0002
 */
class RA3FDevSoundRecordConfigurationSetPriorityTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetPriorityTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetPriorityTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TMMFCapabilities iCapabilities;
	TInt iPriority;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0003
 */
class RA3FDevSoundRecordConfigurationSetGainTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetGainTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetGainTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iGain;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0004
 */
class RA3FDevSoundRecordConfigurationSetRecordBalanceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetRecordBalanceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetRecordBalanceTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iLRecordBalance;
	TInt iRRecordBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0005
 */	
class RA3FDevSoundRecordSetGainWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordSetGainWhileRecordingTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundRecordSetGainWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();

protected:
	TInt iDuration;
	TBool iCalledSetGain;
	};	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0006
 */
class RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iGain;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0007
 */
class RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iGain;
	TInt iExGain;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0008
 */
class RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iLRecordBalance;
	TInt iRRecordBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0009
 */
class RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iLRecordBalance;
	TInt iRRecordBalance;
	TInt iExLSpeakerBalance;
	TInt iExRSpeakerBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0010
 */
class RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iGain;
	TBool iInitializedToRecord;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0011
 */
class RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iGain;
	TBool iInitializedToRecord;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0012
 */
class RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iLRecordBalance;
	TInt iRRecordBalance;
	TBool iInitializedToRecord;
	};
		
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0013
 */
class RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:	
	TInt iLRecordBalance;
	TInt iRRecordBalance;
	TBool iInitializedToRecord;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0014
 */
class RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TInt iSampleRate;
	TInt iChannels;
	TBool iCallConfig;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0015
 */
class RA3FDevSoundQueryRecordCapabilitiesTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordCapabilitiesTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordCapabilitiesTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0016
 */
class RA3FDevSoundQueryRecordConfigTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordConfigTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordConfigTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TMMFSampleRate iExSampleRate;
	TInt iExCapRate;
	TInt iExCapChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0017
 */
class RA3FDevSoundQueryRecordBalanceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordBalanceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordBalanceTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iExLRecordBalance;
	TInt iExRRecordBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0018
 */
class RA3FDevSoundQueryRecordSupportedOutputDataTypesTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordSupportedOutputDataTypesTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordSupportedOutputDataTypesTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	RArray<TFourCC> iSupportedFourCCs;
	TInt iCurrentTypeInTest;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0019
 */
class RA3FDevSoundQueryRecordMaxGainTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordMaxGainTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordMaxGainTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iExGain;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0020
 */	
class RA3FDevSoundQueryRecordSamplesRecordedTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordSamplesRecordedTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordSamplesRecordedTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iExSamplesRecorded;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0021
 */	
class RA3FDevSoundQueryRecordGainTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordGainTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordGainTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0022
 */	
class RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iQueryRecordingSettings;
	TInt iExCapChannels;
	TInt iExCapRate;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0023
 */	
class RA3FDevSoundQueryRecordConfigWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordConfigWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordConfigWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iQueryRecordingSettings;
	TMMFSampleRate iExSampleRate;
	TInt iExCapRate;
	TInt iExCapChannels;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0024
 */	
class RA3FDevSoundQueryRecordBalanceWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordBalanceWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordBalanceWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iQueryRecordingSettings;
	TInt iExLRecordBalance;
	TInt iExRRecordBalance;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0025
 */	
class RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	
private:
	TInt iDuration;
	TBool iQueryRecordingSettings;
	TInt iCurrentTypeInTest;
	RArray<TFourCC> iSupportedFourCCs;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0026
 */	
class RA3FDevSoundQueryRecordMaxGainWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordMaxGainWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordMaxGainWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iQueryRecordingSettings;
	TInt iExGain;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0027
 */	
class RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iQueryRecordingSettings;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0028
 */	
class RA3FDevSoundQueryRecordGainWhileRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundQueryRecordGainWhileRecordingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundQueryRecordGainWhileRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TInt iGain;
	TBool iQueryRecordingSettings;
	};	
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0029
 */	
class RA3FDevSoundRecordingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordingTest* NewL(const TDesC& aTestName);
protected:
	RA3FDevSoundRecordingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
protected:
	TInt iDuration;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW0030
 */
class RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest(const TDesC& aTestName);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iRecordAgainToConfirmInitializedState;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0031
 */
class RA3FDevSoundRecordPreemptionTest : public RA3FDevSoundTestBase, public MA3FDevsoundRecordAudioClientObserver
	{
public:
	static RA3FDevSoundRecordPreemptionTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordPreemptionTest(const TDesC& aTestName);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeEmptiedCallback(TInt aError);
	void ClientRecordErrorCallback(TInt aError);
	void ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void RecordError(TInt aError);
	
private:
	TInt iDuration;
	TBool iOtherClientStarted;
	TFileName iOtherFilename;
	TBool iRecordStopped;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0032
 */
class RA3FDevSoundRecordRejectionTest : public RA3FDevSoundTestBase, public MA3FDevsoundRecordAudioClientObserver
	{
public:
	static RA3FDevSoundRecordRejectionTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordRejectionTest(const TDesC& aTestName);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeEmptiedCallback(TInt aError);
	void ClientRecordErrorCallback(TInt aError);
	void ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TBool iOtherClientStarted;
	TFileName iOtherFilename;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0033
 */
class RA3FDevSoundRecordDataWithoutRecordInitTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordDataWithoutRecordInitTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordDataWithoutRecordInitTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0034
 */	
class RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
protected:
	TInt iDuration;
	};
	

#endif // CHAR_A3F_DEVSOUND_RECORDTEST_H

