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
// TestStepAudInStream.cpp
// This file contains testing steps for the Unit
// test of the MMF 'Deprecated' Audio Clients.
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//


#ifndef __TEST_STEP_AUD_IN_STREAM__
#define __TEST_STEP_AUD_IN_STREAM__

#include "TestStepUnitMMFAudClient.h"
#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include <mdaaudiosampleplayer.h>

 
const TInt KRecBufferSize = 1024;   // use 1K buffer
// Note, increasing the number of buffers can fail the testframework with KErrNoMemory
const TInt KRecNumBuffer = 15; // buffer to store data read from file	

// parameter structure for conversion tests
class TStreamInConvParameters
	{	
public:
	const TText* iTestName;
	TUint32 iEncoding;
	};

// base class for the audio Input stream tests
class CTestStepAudInStream : public CTestStepUnitMMFAudClient,
							 public MMdaAudioInputStreamCallback
{													           
public:
	static CTestStepAudInStream* NewL();
	~CTestStepAudInStream();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	void ProcessDataL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

protected:
	CTestStepAudInStream();
	void ConstructL();
	void StartReadL();
protected:
	CMMFMdaAudioInputStream*	iAudInStream;
	RFs						iFs;
	RFile					iFile;
	TBuf8<KRecBufferSize>		iBufferList[KRecNumBuffer];
	TInt					iStartBuf;
	TInt					iEndBuf;
	TInt					iError;

protected:
	class CWriteBufferActive : public CActive
		{
		friend class CTestStepAudInStream;
	public:
		CWriteBufferActive();
		~CWriteBufferActive();
		virtual void RunL();
		virtual void DoCancel();
		void Write(const TDesC8& /*aBuffer*/);
	public:
		CTestStepAudInStream*	iParent;
		};

protected:
	CWriteBufferActive*		iWriteBufferActive;

friend class CWriteBufferActive;
	};

/**
 *
 * New Input stream
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamNew :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamNew* NewL();
	CTestStepAudInStreamNew();
	~CTestStepAudInStreamNew();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};


/**
 *
 * Input stream Set Audio properties
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamSetProp :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetProp* NewL();
	CTestStepAudInStreamSetProp();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	~CTestStepAudInStreamSetProp();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};



/**
 *
 * Input stream Open
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamOpen :	public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamOpen* NewL();
	CTestStepAudInStreamOpen();
	~CTestStepAudInStreamOpen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Input stream Max/Set/(get) Gain
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamSetGain :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetGain* NewL();
	CTestStepAudInStreamSetGain();
	~CTestStepAudInStreamSetGain();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};

/**
 *
 * Input stream Gain
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepAudInStreamGain :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamGain* NewL();
	CTestStepAudInStreamGain();
	~CTestStepAudInStreamGain();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};
*/
/**
 *
 * Input stream MaxGain
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepAudInStreamMaxGain :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamMaxGain* NewL();
	CTestStepAudInStreamMaxGain();
	~CTestStepAudInStreamMaxGain();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};
*/

/**
 *
 * Input stream Set/(get) Balance
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamSetBalance :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetBalance* NewL();
	CTestStepAudInStreamSetBalance();
	~CTestStepAudInStreamSetBalance();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};

/**
 *
 * Input stream Balance
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepAudInStreamBalance :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamBalance* NewL();
	CTestStepAudInStreamBalance();
	~CTestStepAudInStreamBalance();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};
*/

/**
 *
 * Input stream Set Priority
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamSetPrior :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetPrior* NewL();
	CTestStepAudInStreamSetPrior();
	~CTestStepAudInStreamSetPrior();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMMFMdaAudioInputStream* iAudInStream;
	TInt iError;
	};

/**
 *
 * Input stream Write
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamRead :	public	CTestStepAudInStream, public MMdaAudioOutputStreamCallback, public MMdaAudioPlayerCallback
	{
public:
	static CTestStepAudInStreamRead* NewL(TUint aTestIndex);
	CTestStepAudInStreamRead(TUint aTestIndex);
	~CTestStepAudInStreamRead();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
public: // from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
		//from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

private:
	TBool iIsPlaying;
	const TStreamInConvParameters* iTestParameters;
	};

/**
 *
 * Input stream Stop
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamStop :	public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamStop* NewL();
	CTestStepAudInStreamStop();
	~CTestStepAudInStreamStop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Input stream Position
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamPosition :	public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamPosition* NewL();
	CTestStepAudInStreamPosition();
	~CTestStepAudInStreamPosition();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	};

/**
 *
 * Input stream GetBytes
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamGetBytes :	public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamGetBytes* NewL();
	CTestStepAudInStreamGetBytes();
	~CTestStepAudInStreamGetBytes();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Input stream DataType
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamDataType :	public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamDataType* NewL();
	CTestStepAudInStreamDataType();
	~CTestStepAudInStreamDataType();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};


/**
 *
 * Open sound specifying a Descriptor
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepPlayerNewDescPlayer :	public	CTestStepUnitMMFAudClient,
										public  MMdaAudioPlayerCallback	
	{
public:
	static CTestStepPlayerNewDescPlayer* NewL();
	CTestStepPlayerNewDescPlayer();
	~CTestStepPlayerNewDescPlayer();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcRecordComplete(TInt aError);

private:
	TInt iError;
	CMdaAudioPlayerUtility* iPlayer;
	HBufC8* iBuf;
	};
*/
/**
 *
 * Open sound specifying the priority preference
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepPlayerNewPriorPrefer :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerNewPriorPrefer* NewL();
	CTestStepPlayerNewPriorPrefer();
	~CTestStepPlayerNewPriorPrefer();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcRecordComplete(TInt aError);

private:
	TInt iError;
	TInt iTestCase;
	CMdaAudioPlayerUtility* iPlayer;
	};
*/

/**
 *
 * New Input stream
 *
 * DEF040281 - Audio input streaming crashes on completion on hardware device
 * (The crash was due to a missing destructor in CMdaAudioInputStream
 * which was causing a memory leak)
 */
class CTestStepAudInStreamMemLeak :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamMemLeak* NewL();
	CTestStepAudInStreamMemLeak();
	~CTestStepAudInStreamMemLeak();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	CMdaAudioInputStream* iAudInStream;
	TInt iError;
	};
	


 /*
 * Test new audio stream apis to provide bit rate support
 * Reference DevSound does not support this custom interface to results are always KErrNotSupported
 * 
 */
	
class RTestStepAudInStreamBitRate :	public	RTestStep,
							public	MMdaAudioInputStreamCallback
									
	{
public:
	RTestStepAudInStreamBitRate();
	// Setup states	
	void CreateTestStatesL();
	// Cleanup
	void Close();	
	
	TVerdict OpenL();
	TVerdict DoTestStepL();
	
	void TestBitRates();
	
	void SetSectionFailed(const TDesC& aMessage);
	void SetSectionPassed(const TDesC& aMessage);

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

private:
	TBool iTestVerdictSet;
	CMdaAudioInputStream* iAudInStream;
	TInt iError;
	};

/**
 *
 * Input stream test bitrate interface
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudInStreamTestBitrate :	public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamTestBitrate* NewL();
	CTestStepAudInStreamTestBitrate();
	~CTestStepAudInStreamTestBitrate();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};


/**
 *
 * Input stream test bitrate interface OOM
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class RTestStepAudInStreamBitRateOOM :	public	RTestStep,
										public	MMdaAudioInputStreamCallback
									
	{
public:
	~RTestStepAudInStreamBitRateOOM();
	TVerdict DoTestStepL();
	
	TVerdict TestBitRateInterfaceL();
	static RTestStepAudInStreamBitRateOOM* NewL();
	
	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

protected:
	RTestStepAudInStreamBitRateOOM();

private:
	CMMFMdaAudioInputStream* iAudInStream;
	};

//
// RAsyncTestStep
//
/**
 *
 * Base Async Test class
 *
 */
class CFsmTrigger;
NONSHARABLE_CLASS (CTestStepAudInStreamAsyncTestStep):	public RAsyncTestStep,
					   									public MMdaAudioInputStreamCallback
	{
public:
	CTestStepAudInStreamAsyncTestStep(CTestSuite* aSuite);

	// state machine
	virtual void Fsm() = 0;

	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	
	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

protected:	
 	enum TTestState
		{
		EStateOpen = 0,
		EStateRecordInit,
		EStateRecord,
		EStateRequestStop,
		EStateStopTest,
		EStateStopping
		};
	TTestState iTestState;
	
	static const TUint KNumBuffers = 4; //4 buffers are recorded usually in the given time.
	static const TUint KBufferSize = 4096;//Default buffer size for any pcm recording
	TBuf8<KBufferSize> iBufferList[KNumBuffers];
	TUint iBufferCount;

	CMdaAudioInputStream*	iAudInStream;
	// AO to invoke the Fsm
	CFsmTrigger* iTrigger;
};
	
/**
 *
 * Input stream Timer utility class
 *
 */
NONSHARABLE_CLASS (CFsmTrigger):	public CActive
	{
public:
	static CFsmTrigger* NewL(CTestStepAudInStreamAsyncTestStep& aObserver);
	virtual ~CFsmTrigger();
	
	// From CActive
	virtual void RunL();
	virtual void DoCancel();

	void StartTimer(TTimeIntervalMicroSeconds32 aDelay);
	
protected:
	CFsmTrigger(CTestStepAudInStreamAsyncTestStep& aObserver);
	void ConstructL();

private:
	RTimer iTimer;
	CTestStepAudInStreamAsyncTestStep& iObserver;
	};
	
/**
 *
 * Input stream test RequestStop api
 *
 */
NONSHARABLE_CLASS (CTestStepAudInStreamTestReqStop):	public CTestStepAudInStreamAsyncTestStep
	{
public:
	static CTestStepAudInStreamTestReqStop* NewL(CTestSuite* aSuite);

	// from CTestStepAudInStreamAsyncTest
	void Fsm();
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
	// from MMdaAudioInputStreamCallback
	void MaiscOpenComplete(TInt aError);
	void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	void MaiscRecordComplete(TInt aError);
		
private:
	CTestStepAudInStreamTestReqStop(CTestSuite* aSuite);
	};
	
/**
 *
 * Input stream test RequestStop then Stop apis
 *
 */
NONSHARABLE_CLASS (CTestStepAudInStreamTestReqStopThenStop):	public CTestStepAudInStreamAsyncTestStep
	{
public:
	static CTestStepAudInStreamTestReqStopThenStop* NewL(CTestSuite* aSuite);
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);
	
	// from CTestStepAudInStreamAsyncTest
	virtual void Fsm();
	
private:
	CTestStepAudInStreamTestReqStopThenStop(CTestSuite* aSuite);
	};
	/**
	 *
	 * Input stream test calling Stop from MaiscBufferCopied
	 *
	 */
NONSHARABLE_CLASS (CTestStepAudInStreamTestStopFromBufferCopied):	public CTestStepAudInStreamAsyncTestStep
		{
	public:
		static CTestStepAudInStreamTestStopFromBufferCopied* NewL(CTestSuite* aSuite);
		
		// from MMdaAudioInputStreamCallback
		virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
		
		// from CTestStepAudInStreamAsyncTest
		virtual void Fsm();
	private:
		CTestStepAudInStreamTestStopFromBufferCopied(CTestSuite* aSuite);
		};

/**
 *
 * Input stream Destroy
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
NONSHARABLE_CLASS (CTestStepAudInStreamDestroy):	public CTestStepAudInStreamAsyncTestStep
		{
	public:
		static CTestStepAudInStreamDestroy* NewL(CTestSuite* aSuite);
		
		// from MMdaAudioInputStreamCallback
		virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
		
		// from CTestStepAudInStreamAsyncTest
		virtual void Fsm();
	private:
		CTestStepAudInStreamDestroy(CTestSuite* aSuite);
		
		};


#endif //(__TEST_STEP_AUD_IN_STREAM__)
