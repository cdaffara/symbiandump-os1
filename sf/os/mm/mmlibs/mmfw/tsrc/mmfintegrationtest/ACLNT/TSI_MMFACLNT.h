
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file TSI_MMFACLNT.h
*/

#ifndef TSI_MMFACLNT_H__
#define TSI_MMFACLNT_H__

#include <e32base.h>
#include <badesca.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <utf.h>
#include <testframework.h>

#include <mdaaudiosampleplayer.h>
#include <mdaaudiosampleeditor.h>
#include <mdaaudiotoneplayer.h>
#include <mmfformatimplementationuids.hrh>


#include "toneTest.h"

class CTestSuite ;

/**
 * Global Data used within tests
 *
 */
_LIT(KDTMFString,"01234, 56789, ABCDEF#*");
_LIT(KShortDTMFString,"0");
_LIT(KInvalidDTMFString,"XVB4, ?>, !1231\x00ac|");
_LIT(KDefault,"C:\\AclntITestData\\xxxxx.xxx"); 


#define _LIT8CORRUPTDATA__(name,data) const static TLitPtrC8 name={(sizeof(data) | (1<<5)),REINTERPRET_CAST(const TUint8*,data)}
_LIT8CORRUPTDATA__(KCorruptFixedSequenceData,KFixedSequenceTestSequenceData);

#define NUMBER_OF_REPEATS 3

const TInt KTwelveOfMicroSecond = 83333;
const TInt KTenPartOfSec	= 100000 ;
const TInt KOneSecond		= 1000000;	// Used in a time out function, 1 second (in microSeconds)#
const TInt KTwoSeconds		= KOneSecond*2;
const TInt KTenSeconds		= 10000000;	// Used in a time out function, 10 seconds (in microSeconds)
const TInt KFiveSeconds		= 5000000;	// 5 seconds (in microSeconds)
const TInt KFifteenSeconds	= 15000000;	// 15 seconds (in microSeconds)
const TInt KExpectedDeviation= 1500000; // 1.5 second tolerance for timed tests
const TInt KNameBufSize		= 25;
const TInt KIDBufSize       = 50;
const TInt KExpectedDurationDeviation = 3000;	// 3 millisecond tolerance for duration

//Amendment (for CTestMmfAclntConPosition)
const TInt KDeviation= 1000000; // 1.0 second deviation


//const TInt KPcm16FrameInterval = 371424;
//const TInt KPcm16FrameInterval = 371512;
//const TInt KPcm16FrameInterval = 278639;
// frame size is now variable, depending on the sample rate
//const TUint KFrameSize8K = 2048;	// frame size for 8Khz
const TUint KFrameSize11K = 3072;	// frame size for 11Khz
const TUint KPcmBitsPerSample8 = 8;
const TUint KPcmChannels = 1;
const TUint KSampleRate11K = 11025;
const TInt KPcm16FrameInterval = 
	(((KFrameSize11K * 1000000 / KSampleRate11K) /
	 KPcmBitsPerSample8 * KPcmChannels)
	* 8);


const TInt KFixedSequenceCount = 6;
const TInt KMaxVolume = 65535;

const TInt KMaxBalance = 100;
const TInt KMinBalance = -100;
const TInt KRecSize = 10000;
const TInt KToneFrequency = 880;
const TInt KToneFrequencyTwo = 1880;
const TInt KSoundFileLength = 8359183;
const TInt KInvalidNumber =	6234623;

//pinched from MmfAudioInput.h:
const TUint KAudioInputMinFrameSize = 0x800; //2K
const TUint KAudioInputMaxFrameSize = 0x4000; //16K
const TUint KAudioInputDeltaFrameSize = 0x400;  //1K
const TUint KAudioInputDefaultFrameSize = 0x1000;  //4K
const TUint KAudioInputFramesPerSecond = 4;
//pinched from gsm610.h:
const TUint	KGsmEncodedFrameSize = 65;




/**
 * @ TMmfAclntWavEncodeClipFormat, provides a class to support interfaces that require a 
 * @ TMdaClipFormat.
 */
const TUid KMmfUidEncodeFormatWAV = {KMmfUidFormatWAVWrite};
class TMmfAclntWavEncodeClipFormat : public TMdaClipFormat
	{
public:
	inline TMmfAclntWavEncodeClipFormat();
	};
inline TMmfAclntWavEncodeClipFormat::TMmfAclntWavEncodeClipFormat() : 
TMdaClipFormat(KMmfUidEncodeFormatWAV,sizeof(TMmfAclntWavEncodeClipFormat)) {}

/**
 * @ TMmfAclntWavDecodeClipFormat, provides a class to support interfaces that require a 
 * @ TMdaClipFormat.
 */
const TUid KMmfUidDecodeFormatWAV = {KMmfUidFormatWAVRead};
class TMmfAclntWavDecodeClipFormat : public TMdaClipFormat
	{
public:
	inline TMmfAclntWavDecodeClipFormat();
	};
inline TMmfAclntWavDecodeClipFormat::TMmfAclntWavDecodeClipFormat() : 
TMdaClipFormat(KMmfUidDecodeFormatWAV,sizeof(TMmfAclntWavDecodeClipFormat)) {}


/**
 * @ MCallbackHandlerObserver, provides an Observer
 * @ for monitoring callbacks.
 */
class MCallbackHandlerObserver
	{
public:
	virtual void MchoComplete(TInt aID, TInt aError)=0;
	};

/**
 * CCallbackHandler provides an Observer
 * for monitoring MMdaAudioPlayerCallback callbacks.
 *
 * @ class CPlayerCallbackHandler
 *
 */
class CPlayerCallbackHandler : public MMdaAudioPlayerCallback
	{
public:
	CPlayerCallbackHandler(const TInt aID, MCallbackHandlerObserver* aMchObserver) : iMchObserver(aMchObserver), iID(aID)  {};
	TInt ID();	

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
	
private:
	MCallbackHandlerObserver* iMchObserver;
	TInt iID;
	};

/**
 * CCallbackHandler provides an Observer for 
 * monitoring MMdaObjectStateChangeObserver callbacks. 
 *
 * @class CStateCallbackHandler
 *
 */
class CStateCallbackHandler : public MMdaObjectStateChangeObserver
	{
public:
	CStateCallbackHandler(const TInt aID, MCallbackHandlerObserver* aMchObserver) : iMchObserver(aMchObserver), iID(aID) {};
	TInt ID();	

	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode);

private:
	MCallbackHandlerObserver* iMchObserver;
	TInt iID;
	};


/**
 * provides an Observer for monitoring MMdaAudioToneObserver callbacks.
 *
 * @class CToneCallbackHandler
 *
 */
class CToneCallbackHandler : public MMdaAudioToneObserver
	{
public:
	CToneCallbackHandler(const TInt aID, MCallbackHandlerObserver* aMchObserver) : iMchObserver(aMchObserver), iID(aID)  {};
	TInt ID();	

	// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	MCallbackHandlerObserver* iMchObserver;
	TInt iID;
	};

/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @class CActiveListener
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener();
	void InitialiseActiveListener();
	// From CActive
	virtual void RunL();
	virtual void DoCancel() {};
	};


/**
 * It's a base class for all test steps.
 *
 * @class CTestMmfAclntStep 
 * @brief Test class that enables tests.
 *
 */
class CTestMmfAclntStep : public CTestStep
	{
public:
	CTestMmfAclntStep();
	static void CleanUp(TAny *aPtr) ;
	void SetTestSuite(const CTestSuite* aTestSuite );

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds);
	TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDivation);

	// tests may optionally implement pre- and post-ambles
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

protected:
	const CTestSuite* iTestSuite ;
	CActiveScheduler* iActiveScheduler;
	};

/**
 * Test base class that adds codec specific
 * functionalty (applys to tests that create files).
 *
 * @class CTestMmfAclntCodecTest
 * @codec specific test base class.
 *
 */
class CTestMmfAclntCodecTest : public CTestMmfAclntStep
	{
public:
	enum TTestFormat
		{
		ENone,
		EPcm16Wav,
		EMulawRaw,
		E16BitAu,
		EAlawAu,
		EPcm16Au,
		EImaAdpcmWav,
		EImasPcmWav,
		EPcm8,
		EPcmU8,
		EPcmU16,
		EAlawWav,
		EEpocWve,
		EGsmWav
		};
public:
	void SetupFormatL(const TTestFormat aFormat);
	TVerdict DoTestStepPostambleL();
	
protected:
	TMdaClipFormat* iFormat;
	TMdaPackage* iCodec;
	TMdaAudioDataSettings iAudioSettings;
	TTestFormat iTestFormat;
	};


TAny	GetDriveName(TDes& aFileName);
const TInt	KSizeBuf = 256;


#endif// TSI_MMFACLNT_H__

