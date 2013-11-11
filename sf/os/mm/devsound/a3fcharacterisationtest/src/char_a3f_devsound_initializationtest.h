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

// User includes
#include "char_a3f_devsound_testbase.h"

#ifndef CHAR_A3F_DEVSOUND_INITIALIZATIONTEST_H
#define CHAR_A3F_DEVSOUND_INITIALIZATIONTEST_H

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0001
 */
class RA3FDevSoundToneInitializationTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneInitializationTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneInitializationTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0002
 */
class RA3FDevSoundToneCancelInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneCancelInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneCancelInitializeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	void InitializeComplete(TInt aError);
	
private:
	TBool iInitCompleteReceived;
	TMMFSampleRate iESampleRate;
	TInt iSampleRate;
	TInt iChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0003
 */
class RA3FDevSoundToneInitializeToPlayTonesTwiceTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneInitializeToPlayTonesTwiceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneInitializeToPlayTonesTwiceTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0004
 */
class RA3FDevSoundToneInitializeToPlayTonesThenAudioTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneInitializeToPlayTonesThenAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneInitializeToPlayTonesThenAudioTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0005
 */
class RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0006
 */
class RA3FDevSoundToneInitializeTFourCCTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneInitializeTFourCCTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneInitializeTFourCCTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0007
 */
class RA3FDevSoundInitializeInvalidModeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundInitializeInvalidModeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundInitializeInvalidModeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0008
 */
class RA3FDevSoundPlayInitializationTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitializationTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayInitializationTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0009
 */
class RA3FDevSoundPlayCancelInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayCancelInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayCancelInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	void InitializeComplete(TInt aError);
	
private:
	TBool iInitCompleteReceived;
	TMMFSampleRate iESampleRate;
	TInt iSampleRate;
	TInt iChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0010
 */
class RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0011
 */
class RA3FDevSoundPlayInitializeToPlayAudioTwiceTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitializeToPlayAudioTwiceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayInitializeToPlayAudioTwiceTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0012
 */
class RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0013
 */	
class RA3FDevSoundPlayInitializeInvalidTFourCCTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitializeInvalidTFourCCTest* NewL(const TDesC& aTestName);
	
private:
	RA3FDevSoundPlayInitializeInvalidTFourCCTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0014
 */
class RA3FDevSoundRecordInitializationTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitializationTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordInitializationTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0015
 */
class RA3FDevSoundRecordCancelInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordCancelInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundRecordCancelInitializeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	void InitializeComplete(TInt aError);
	
private:
	TBool iInitCompleteReceived;
	TMMFSampleRate iESampleRate;
	TInt iSampleRate;
	TInt iChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0016
 */
class RA3FDevSoundRecordInitializeThenPlayToneInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitializeThenPlayToneInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordInitializeThenPlayToneInitializeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0017
 */
class RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0018
 */
class RA3FDevSoundRecordInitializeToRecordAudioTwiceTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitializeToRecordAudioTwiceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundRecordInitializeToRecordAudioTwiceTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0019
 */
class RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0020
 */
class RA3FDevSoundPlayDataWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayDataWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayDataWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0021
 */
class RA3FDevSoundPlayDTMFStringWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayDTMFStringWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayDTMFStringWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBuf16<KMaxStringLength> iDTMFString;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0022
 */
class RA3FDevSoundPlayDualToneWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayDualToneWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayDualToneWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0023
 */
class RA3FDevSoundPlayFixedSequenceWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayFixedSequenceWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayFixedSequenceWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0024
 */
class RA3FDevSoundPlayInitWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayInitWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0025
 */
class RA3FDevSoundPlayToneWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayToneWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayToneWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TInt iDuration;
	TInt iFrequencyTone1;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0026
 */
class RA3FDevSoundPlayToneSequenceWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayToneSequenceWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayToneSequenceWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0027
 */
class RA3FDevSoundRecordDataWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordDataWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundRecordDataWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0028
 */
class RA3FDevSoundRecordInitWithoutInitializeTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitWithoutInitializeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundRecordInitWithoutInitializeTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0029
 */
class RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0030
 */
class RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0031
 */
class RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0032
 */
class RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0033
 */
class RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-INIT-0034
 */
class RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds(const TDesC& aTestName);	
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};	
#endif //CHAR_A3F_DEVSOUND_INITIALIZATIONTEST_H

