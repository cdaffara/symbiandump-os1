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
// This file contains testing steps for the Unit
// test of the MMF 'Deprecated' Audio Clients.
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//


#ifndef TESTSTEPAUDOUTSTREAM_H
#define TESTSTEPAUDOUTSTREAM_H

#include <mmf/server/mmffile.h>
#include "TestStepUnitMMFAudClient.h"
#include <mdaaudiooutputstream.h>
#include <mdaaudiotoneplayer.h>


const TInt KBufferSize = 4096;	// use 4K buffer on EKA2 due to difference in Sound Driver
const TInt KImadPolicyNumberOfStreams = 2;  
const TInt KNumBuffer = 6; // buffer to store data read from file

// parameter structure for conversion tests
class TConvParameters
	{	
public:
	const TText* iTestName;
	TUint32 iEncoding;
	TUint	iSampleRate;
	TUint	iChannels;
	const TText* iFromFilename;
	TInt	iExpectedResult;
	};


// base class for the audio Output stream tests
class CTestStepAudOutStream : public CTestStepUnitMMFAudClient,
							  public MMdaAudioOutputStreamCallback
{													           
public:
	static CTestStepAudOutStream* NewL();
	~CTestStepAudOutStream();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	//virtual TInt HandleKeyEvent(/*TKeyCode aKeystroke*/);
	//virtual void StartL();
	//virtual const TDesC& Instructions();
	//void passParent(CTestStepUnitMMFAudClient& aParent);
	void ProcessDataL(TBuf8<KBufferSize>& aData);

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError); 

protected:
	CTestStepAudOutStream();
	void ConstructL();
	void ResetBufferList();

protected:
	CMMFMdaAudioOutputStream* iAudOutStream;
	TBuf8<KBufferSize>		iBufferList[KNumBuffer];
	TInt					iStartBuf;
	TInt					iEndBuf;
	TInt					iError;
	TBool					iIsAllDataRead;
	TBool					iIsAllDataWritten;

protected:
	class CReadBufferActive : public CActive
		{
		friend class CTestStepAudOutStream;
	public:
		CReadBufferActive();
		~CReadBufferActive();
		virtual void RunL();
		virtual void DoCancel();
		void Read();
		void Read(TInt aPos);
	public:
		TBuf8<KBufferSize>		iBuffer;
		CTestStepAudOutStream*	iParent;
		};

protected:
	CReadBufferActive*		iReadBufferActive;

friend class CReadBufferActive;
	};

/**
 *
 * New output stream
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamNew :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamNew* NewL();
	CTestStepAudOutStreamNew();
	~CTestStepAudOutStreamNew();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
private:
	CMMFMdaAudioOutputStream* iAudOutStream;
	};


/**
 *
 * Output stream Set Audio properties
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamSetProp :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamSetProp* NewL();
	CTestStepAudOutStreamSetProp();
	virtual enum TVerdict DoTestStepPreambleL(void);
	~CTestStepAudOutStreamSetProp();
	virtual TVerdict DoTestStepL();
	};

class CTestStepAudOutStreamMapChannels : public CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamMapChannels* NewL();
	CTestStepAudOutStreamMapChannels();
	TVerdict DoTestStepL();
	};

/**
 *
 * Output stream Set Audio properties - negative test
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamSetPropNeg :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamSetPropNeg* NewL();
	CTestStepAudOutStreamSetPropNeg();
	virtual enum TVerdict DoTestStepPreambleL(void);
	~CTestStepAudOutStreamSetPropNeg();
	virtual TVerdict DoTestStepL();
	};



/**
 *
 * Output stream Open
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamOpen :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamOpen* NewL();
	CTestStepAudOutStreamOpen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Output stream Open Zero
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamOpenZero :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamOpenZero* NewL();
	CTestStepAudOutStreamOpenZero();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Output stream Max/Set/(get) Volume
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamSetVol :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamSetVol* NewL();
	CTestStepAudOutStreamSetVol();
	~CTestStepAudOutStreamSetVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	CMMFMdaAudioOutputStream* iAudOutStream;
	TInt iError;
	};

/**
 *
 * Output stream Volume
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepAudOutStreamVol :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamVol* NewL();
	CTestStepAudOutStreamVol();
	~CTestStepAudOutStreamVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	TInt iError;
	};
*/
/**
 *
 * Output stream MaxVolume
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepAudOutStreamMaxVol :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamMaxVol* NewL();
	CTestStepAudOutStreamMaxVol();
	~CTestStepAudOutStreamMaxVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	TInt iError;
	};
*/

/**
 *
 * Output stream Set/(get) Balance
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamSetBalance :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamSetBalance* NewL();
	CTestStepAudOutStreamSetBalance();
	~CTestStepAudOutStreamSetBalance();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	CMMFMdaAudioOutputStream* iAudOutStream;
	TInt iError;
	};

/**
 *
 * Output stream Balance
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepAudOutStreamBalance :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamBalance* NewL();
	CTestStepAudOutStreamBalance();
	~CTestStepAudOutStreamBalance();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	TInt iError;
	};
*/

/**
 *
 * Output stream Set Priority
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamSetPrior :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamSetPrior* NewL();
	CTestStepAudOutStreamSetPrior();
	~CTestStepAudOutStreamSetPrior();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Output stream Write
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamWrite :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamWrite* NewL();
	CTestStepAudOutStreamWrite();
	~CTestStepAudOutStreamWrite();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Output stream Stop
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamStop :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamStop* NewL();
	CTestStepAudOutStreamStop();
	~CTestStepAudOutStreamStop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Output stream Position
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamPosition :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamPosition* NewL();
	CTestStepAudOutStreamPosition();
	~CTestStepAudOutStreamPosition();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Output stream GetBytes
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamGetBytes :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamGetBytes* NewL();
	CTestStepAudOutStreamGetBytes();
	~CTestStepAudOutStreamGetBytes();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	// from MMdaAudioOutputStreamCallback
public:
	void MaoscPlayComplete(TInt aError);
	};

/**
 *
 * Output stream DataType
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamDataType :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamDataType* NewL();
	CTestStepAudOutStreamDataType();
	~CTestStepAudOutStreamDataType();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	// from MMdaAudioOutputStreamCallback
	};

/**
 *
 * Test Output stream on differents formats 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamFormats :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamFormats* NewL(TUint aTestIndex);
	CTestStepAudOutStreamFormats(TUint aTestIndex);
	~CTestStepAudOutStreamFormats();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	TFileName SetFileNameL( const TDesC& aPathNameAndExtn );
	inline TMMFFileConfig& GetFileName() { return iFileName; }
	// from MMdaAudioOutputStreamCallback
private:
	const TConvParameters* iTestParameters;
	TMMFFileConfig	iFileName;// audio output file 
	};


/**
 *
 * DEF093436: Call to Configure in wrong place in CMMFImaAdpcmToPcm16CodecHwDevice::Start
 * PDEF092098: DevSound fails to update policy if error occurs in StartPlayDataL
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamImadPolicy;

class CTestStepAudOutStreamImadPolicyCallback :	 public MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamImadPolicyCallback* NewL(TInt aStreamId, CTestStepAudOutStreamImadPolicy* iParent);
	CTestStepAudOutStreamImadPolicyCallback(TInt aStreamId, CTestStepAudOutStreamImadPolicy* iParent);
	virtual ~CTestStepAudOutStreamImadPolicyCallback();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError); 
	
private:
	TInt iStreamId;	
	CTestStepAudOutStreamImadPolicy* iParent;
	};

class CTestStepAudOutStreamImadPolicy :	public CTestStepUnitMMFAudClient
	{													           
public:
	static CTestStepAudOutStreamImadPolicy* NewL(TUint aTestIndex);
	CTestStepAudOutStreamImadPolicy(TUint aTestIndex);
	~CTestStepAudOutStreamImadPolicy();

	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	TFileName SetFileNameL( const TDesC& aPathNameAndExtn );
	inline TMMFFileConfig& GetFileName() { return iFileName; }

	void OpenComplete(TInt aStreamId, TInt aError);
	void BufferCopied(TInt aStreamId, TInt aError, const TDesC8& aBuffer);
	void PlayComplete(TInt aStreamId, TInt aError); 
	
	void ProcessDataL(TInt aStreamId, TBuf8<KBufferSize>& aData);

protected:
	void ConstructL();
	void ResetBufferList();
	void CloseAll();

protected:
	CMMFMdaAudioOutputStream* iAudOutStream[KImadPolicyNumberOfStreams];
	CTestStepAudOutStreamImadPolicyCallback* iAudOutStreamCallback[KImadPolicyNumberOfStreams];
	TBuf8<KBufferSize>		iBufferList[KImadPolicyNumberOfStreams][KNumBuffer];
	TInt					iStartBuf[KImadPolicyNumberOfStreams];
	TInt					iEndBuf[KImadPolicyNumberOfStreams];
	TInt					iError[KImadPolicyNumberOfStreams];
	TBool					iIsAllDataRead[KImadPolicyNumberOfStreams];
	TBool					iIsAllDataWritten[KImadPolicyNumberOfStreams];
	TBool					iPlayComplete[KImadPolicyNumberOfStreams];
	RFile					iFile[KImadPolicyNumberOfStreams];
	
	const TConvParameters* iTestParameters;
	TMMFFileConfig	iFileName;// audio output file 
	TFileName iFsFileName;	// filename for RFs
	
protected:
	class CReadBufferActive : public CActive
		{
		friend class CTestStepAudOutStreamImadPolicy;
	public:
		CReadBufferActive(TInt aStreamId);
		~CReadBufferActive();
		virtual void RunL();
		virtual void DoCancel();
		void Read();
		void Read(TInt aPos);
	public:
		TBuf8<KBufferSize>		iBuffer;
		TInt					iStreamId;
		CTestStepAudOutStreamImadPolicy*	iParent;
		};

private:
	// which stream are we currently looking at
	TInt					iStreamId;	
	CReadBufferActive*		iReadBufferActive[2];
	enum TSchedulerStopType
		{
		ENotStopped = 0,
		EOpenComplete = 1,
		EPlayComplete = 2
		};
	TSchedulerStopType	iSchedulerStopType;
friend class CReadBufferActive;
	};

/**
 *
 * Output stream Play/Error
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamPlayError : public CTestStepAudOutStream, public MMdaAudioToneObserver
	{
public:
	static CTestStepAudOutStreamPlayError* NewL(TUint aTestIndex);
	CTestStepAudOutStreamPlayError(TUint aTestIndex);
	~CTestStepAudOutStreamPlayError();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError); 
	virtual void MaoscOpenComplete(TInt aError); 

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
private:
	const TConvParameters* iTestParameters;
	CMdaAudioToneUtility* iToneUtility;
	enum TSchedulerStopType
		{
		ENotStopped = 0,
		EStreamOpenComplete = 1,
		EStreamPlayComplete = 2,
		ETonePrepareComplete = 3,
		ETonePlayComplete = 4
		};
	TSchedulerStopType iSchedulerStopType;	
	TInt iBufferErrorCount;
	TBool iExpectingBufferErrors;
	};

/**
 *
 * Output stream new Panic
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepAudOutStreamConcurrentOpen : public CTestStepUnitMMFAudClient
	{
public:
	static CTestStepAudOutStreamConcurrentOpen* NewL();
	CTestStepAudOutStreamConcurrentOpen();
	~CTestStepAudOutStreamConcurrentOpen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
private:
	static TInt CreateThread(TAny* data);
	static void StartThreadL();
	};

class CTestAudioOutputStreamWrapper : public CBase, public MMdaAudioOutputStreamCallback
	{
public:
    static CTestAudioOutputStreamWrapper*	NewL( TInt aPriority=EMdaPriorityNormal);
    virtual ~CTestAudioOutputStreamWrapper();
protected:
    CTestAudioOutputStreamWrapper();
    void ConstructL(TInt aPriority);  
private:
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);	
private:
    CMdaAudioOutputStream*	iAudioOutputStream;    
	};

class MAudioOPStreamObserver
	{
public:
	//* Signal playing has stopped - for info
	virtual void PlayingStopped(TInt aError, TVerdict aVerdict)=0;
	};
	
class CAudioOPStreamAudio : public CBase, 
				            public MMdaAudioOutputStreamCallback
	{
	enum TState
		{
		EStateIdle,
		EStateOpeningOutput,
		EStateWriting
		};
public:
	static CAudioOPStreamAudio* NewL(MAudioOPStreamObserver* aObserver, TBool iKeepOpenAtEnd, TBool iAutoStop, TBool iNegativeTest);
	
	~CAudioOPStreamAudio();
	
	void PlayL();
	void Stop();
	void RequestStop();
	// from MMdaAudioOutputStreamCallback
	void MaoscOpenComplete(TInt aError);
	void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	void MaoscPlayComplete(TInt aError) ;
	
protected:
	CAudioOPStreamAudio(MAudioOPStreamObserver* aObserver, TBool iKeepOpenAtEnd, TBool iAutoStop, TBool iNegativeTest);
	void ConstructL();
	void Complete(TInt aError, TVerdict aVerdict);
	void WriteOutputStreamBuffersL();
private:
	MAudioOPStreamObserver*const iObserver;
	TState iState;
	static const TUint KNumBuffers = 4; 
	static const TUint KBufferSize = 4096;
	TBuf8<KBufferSize> iBufferList[KNumBuffers];
	TUint iBufferCount;
	CMdaAudioOutputStream* iOutputStream;
	RFs iFs;
	RFile iFile;
	TBool iKeepOpenAtEnd;
	TBool iAutoStop;
	TBool iWriteBufferAfterRequestStop;
	};

class RAudioOPStreamRequestStop : 	public RAsyncTestStep,
					 				public MAudioOPStreamObserver
	{
public:
	static RAudioOPStreamRequestStop* NewL(const TDesC& aStepName, TBool iKeepOpenAtEnd, TBool iAutoStop, TBool iNegativeTest, TBool aRestart=EFalse);
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
	void PlayingStopped(TInt aError, TVerdict aVerdict);
private:
	RAudioOPStreamRequestStop(const TDesC& aStepName, TBool iKeepOpenAtEnd, TBool iAutoStop, TBool iNegativeTest, TBool aRestart=EFalse);
	static TInt TimerCallback(TAny*);
	void DoTimerCallback();
	
	CAudioOPStreamAudio* iPlayer;
	CPeriodic* iTimer;
	TBool iKeepOpenAtEnd;
	TBool iAutoStop;
	TBool iWriteBufferAfterRequestStop;
	TBool iRestart;
	};
#endif //(TESTSTEPAUDOUTSTREAM_H)


