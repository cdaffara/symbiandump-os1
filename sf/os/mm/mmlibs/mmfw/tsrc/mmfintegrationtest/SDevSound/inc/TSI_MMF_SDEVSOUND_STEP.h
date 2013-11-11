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

#ifndef __TSI_MMF_SDEVSOUND_STEP_H__
#define __TSI_MMF_SDEVSOUND_STEP_H__


#include <testframework.h>
#include <mdaaudiosampleplayer.h>
#include <mdaaudiosampleeditor.h>
#include <f32file.h>
#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include <mda/common/resource.h>


#include "TSI_MMF_SDEVSOUND_SUITE.h"


const TInt KMaxVolume = 0xFFFF;
const TInt KMaxGain   = 0xFFFF;


class CTestSuite;
class CTestSuiteSDevSound;

const TInt KNameBufSize			= 25;
const TInt KFiveSeconds			= 5000000;
const TInt KOneSecond			= 1000000;
const TInt KArrayGranularity	= 7;


_LIT(KDefault,"C:\\AclntITestData\\xxxxx.xxx"); 

/**
 *
 * CActiveListener provides the asynchronous operation
 * of an active object
 *
 * @lib "TSI_MMF_SDEVSOUND_SUITE.lib"
 *
 *
 */
class CActiveListener : public CActive
	{
public:
	CActiveListener() : CActive(CActive::EPriorityStandard) 
		{ 
		CActiveScheduler::Add(this);
		}
	void InitialiseActiveListener()
		{
		iStatus = KRequestPending; 
		SetActive();
		}
	// From CActive
	virtual void RunL()
		{
		CActiveScheduler::Stop();
		}
	virtual void DoCancel()
		{
		//Cancel();		//otherwise, create unwanted recursive problem
		CActiveScheduler::Stop();
		}
	~CActiveListener()
		{
		Cancel();
		}
	};

/**
 *
 * CTestStepSDevSound - Test class that enables SDevSound tests.
 * It's a base class for all test steps.
 *
 * @lib "TSI_MMF_SDEVSOUND_SUITE.lib"
 *
 *
 */
class CTestStepSDevSound : public CTestStep, public MDevSoundObserver
	{
public:
	CTestStepSDevSound();
	~CTestStepSDevSound();
	void SetTestSuite(const CTestSuiteSDevSound* aTestSuite);

protected:
	
// Require all tests to create iMMFDevSound in their DoTestStepL(), 
// becauses OpenAudioPolicyServer test doesn't want to create iMMFDevSound

	//virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();

	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);

	TVerdict InitializeDevSound(TMMFState aMode);
	TVerdict InitializeDevSound(TFourCC& aDataType, TMMFState aMode);
	void SetVolume(TInt aVolume);
	virtual TVerdict DigitalPlayback(TInt /*aNumSamples*/, TDesC& /*aFilename*/, TFourCC& /*aDataType*/){return EFail;}
	TVerdict PlayInit();
	TVerdict PlayData();

protected:

	typedef enum TCallback 
		{
		EInitComplete,
		EToneFinished,
		EBuffToFill,
		EPlayError,
		EBuffToEmpty,
		ERecError,
		EConvertMsg,
		EDeviceMsg
		};

	TFixedArray<TInt, KArrayGranularity> iCallbackArray;
	
	//pointer to suite which owns this test 
	const CTestSuiteSDevSound* iDevSoundTestSuite;

	//Active Scheduler
	CActiveScheduler* iAS;

	CActiveListener* iAL;
	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	TInt iCallbackError;
	CMMFBuffer* iBuffer;
	
	RChunk iChunk;
	};

//------------------------------------------------------------------
class CTestStepSDevSoundPlaySimpleTone : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundPlaySimpleTone(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------
class CTestStepSDevSoundPlayDTMFTones : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundPlayDTMFTones(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------
class CTestStepSDevSoundPlayDualTone : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundPlayDualTone(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------
class CTestStepSDevSoundPlayEOFPCM16 : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundPlayEOFPCM16(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	TVerdict DigitalPlayback(TInt aNumSamples, TDesC& aFilename, TFourCC& aDataType);

protected:	
	RFs iFs;
	RFile iFile;
	};

//------------------------------------------------------------------
class CTestStepSDevSoundPlayFile: public CTestStepSDevSound, public MMdaAudioPlayerCallback
	{
public:
	CTestStepSDevSoundPlayFile(const TDesC& aTestName, const TDesC& aKeyName);
	//TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);


private:
	TTimeIntervalMicroSeconds iDuration;
	TInt iError;
	TBuf<KNameBufSize> iKeyFileName;
	};

//------------------------------------------------------------------
class CTestStepSDevSoundRecordFileBase: public CTestStepSDevSound, public MMdaObjectStateChangeObserver
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
		};
		
		
	CTestStepSDevSoundRecordFileBase(const TDesC& aTestName,
								 	 const TDesC& aSectName,
								 	 const TDesC& aKeyName,
								 	 const TTestFormat aFormat,
								 	 const TBool aCreateFile);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	virtual TVerdict CheckRecordError();
	virtual ~CTestStepSDevSoundRecordFileBase();
	void Close();
	void SetupFormatL(const TTestFormat aFormat);
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
protected:
	TInt iError;
	TBool iCreateFile;
	TBuf<KNameBufSize> iSectName;					// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;					// Key name for retrieving filename
	TMdaClipFormat* iFormat;
	TMdaPackage* iCodec;
	TTestFormat iTestFormat;
	TMdaAudioDataSettings iAudioSettings;

	};

//------------------------------------------------------------------

//The requested client (testframework) has the required capability for recording
//---It has capability = UserEnvironment or ALL -TCB --
class CTestStepSDevSoundRecordFile : public CTestStepSDevSoundRecordFileBase
	{
public:

	CTestStepSDevSoundRecordFile(const TDesC& aTestName,
								 const TDesC& aSectName,
								 const TDesC& aKeyName,
								 const TTestFormat aFormat,
								 const TBool aCreateFile);
								 
	virtual TVerdict CheckRecordError();
	~CTestStepSDevSoundRecordFile();
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	};
	
//------------------------------------------------------------------

//The requested client (testframework) does not have the required capability for recording.
//---It has capability = None or MultimediaDD--
class CTestStepSDevSoundRecordFileNoCap : public CTestStepSDevSoundRecordFileBase
	{
public:

	CTestStepSDevSoundRecordFileNoCap(const TDesC& aTestName,
								 	  const TDesC& aSectName,
								 	  const TDesC& aKeyName,
								  	  const TTestFormat aFormat,
								 	  const TBool aCreateFile);
								 	  
	virtual TVerdict CheckRecordError();
	~CTestStepSDevSoundRecordFileNoCap();
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	};
//------------------------------------------------------------------

//------------------------------------------------------------------
class CTestStepSDevSoundPlayWithoutInitialize : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundPlayWithoutInitialize(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	TVerdict TestPlayData();
	};
//------------------------------------------------------------------

//------------------------------------------------------------------
class CTestStepSDevSoundRecordWithoutInitialize : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundRecordWithoutInitialize(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	TVerdict TestRecordData();
	};
//------------------------------------------------------------------

//------------------------------------------------------------------
class CTestStepSDevSoundClientThreadCap : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundClientThreadCap(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------
class CTestStepSDevSoundClientThreadInvalidId : public CTestStepSDevSound
	{
public:
	CTestStepSDevSoundClientThreadInvalidId(const TDesC& aTestName);
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepL();
	};

//------------------------------------------------------------------

TAny	GetDriveName(TDes& aFileName);
const TInt	KSizeBuf = 256;

#endif// __TSI_MMF_SDEVSOUND_STEP_H__
