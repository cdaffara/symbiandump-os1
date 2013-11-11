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


#ifndef __TEST_STEP_RECORDER__
#define __TEST_STEP_RECORDER__

#include <mdaaudiosampleplayer.h>
#include <mdaaudiotoneplayer.h>
#include <mdaaudiooutputstream.h>

const TInt KMaxMethods = 10;  //max number of dispatch methods
/**
 *
 * Test New Recorder utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderNew :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderNew* NewL();
	CTestStepRecorderNew();
	~CTestStepRecorderNew();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};


/**
 *
 * Test Open
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderOpen :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpen* NewL();
	CTestStepRecorderOpen();
	~CTestStepRecorderOpen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TMdaClipLocation* iClipLocation;
	TMdaClipFormat* iFormat;
	HBufC8* iBuf;
	TPtrC iFileName2, iFileName3;
	};

/**
 *
 * Test Open sound from file
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderOpenFile :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpenFile* NewL(TBool aOpenFileTwice);
	CTestStepRecorderOpenFile(TBool aOpenFileTwice);
	~CTestStepRecorderOpenFile();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileNames[3];
	TBool iOpenFileTwice; //Additional test for DEF051958
	};

/**
 *
 * Test Open sound specifying a descriptor
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderOpenDesc :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpenDesc* NewL();
	CTestStepRecorderOpenDesc();
	~CTestStepRecorderOpenDesc();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	HBufC8* iBuf;
	TPtr8* iDes8;
	TPtrC iFileNames[3];
	};

/**
 *
 * Test Open sound specifying a url
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderOpenUrl :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpenUrl* NewL();
	CTestStepRecorderOpenUrl();
	~CTestStepRecorderOpenUrl();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iURL;
	TPtrC iMimeType;
	};

/**
 *
 * Test Open sound specifying a Uri Tag 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderOpenviaUriTag :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpenviaUriTag* NewL();
	CTestStepRecorderOpenviaUriTag();
	~CTestStepRecorderOpenviaUriTag();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iURL;
	TPtrC iMimeType;
	};
	
class CTestStepRecorderOpenTryNextCtrl :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpenTryNextCtrl* NewL();
	CTestStepRecorderOpenTryNextCtrl();
	~CTestStepRecorderOpenTryNextCtrl();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iURL;
	TPtrC iMimeType;
	};

/**
 *
 * Test Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecord :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRecord* NewL();
	CTestStepRecorderRecord();
	~CTestStepRecorderRecord();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileName2, iFileName3;
	TBool iHasRecorded;
	TTimeIntervalMicroSeconds* iBeginning;
	};


/**
 *
 * Test Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordWav :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRecordWav* NewL();
	CTestStepRecorderRecordWav();
	~CTestStepRecorderRecordWav();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

protected:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasRecorded;
	TTimeIntervalMicroSeconds* iBeginning;
	};



/**
 *
 * Test Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordAu : public CTestStepRecorderRecordWav
	{
public:
	CTestStepRecorderRecordAu();
 	virtual enum TVerdict DoTestStepPreambleL(void);
	static CTestStepRecorderRecordAu* NewL();
	};

/**
 *
 * Test Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordRaw : public CTestStepRecorderRecordWav
	{
public:
    CTestStepRecorderRecordRaw();
 	virtual enum TVerdict DoTestStepPreambleL(void);
	static CTestStepRecorderRecordRaw* NewL();

	};


/**
 *
 * Test Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordWavGsm : public CTestStepRecorderRecordWav
	{
public:
	CTestStepRecorderRecordWavGsm();
 	virtual enum TVerdict DoTestStepPreambleL(void);
 	virtual TVerdict DoTestStepL();
	static CTestStepRecorderRecordWavGsm* NewL();
	};


/**
 *
 * Test Record sound Fail
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordWavGsmFail : public CTestStepRecorderRecordWav
	{
public:
	CTestStepRecorderRecordWavGsmFail();
 	virtual enum TVerdict DoTestStepPreambleL(void);
 	virtual TVerdict DoTestStepL();
	static CTestStepRecorderRecordWavGsmFail* NewL();
	};


/**
 *
 * Test Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordPause :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRecordPause* NewL(const TDesC& aTestName);
	CTestStepRecorderRecordPause(const TDesC& aTestName);
	~CTestStepRecorderRecordPause();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileName2, iFileName3;
	TBool iHasRecorded;
	TTimeIntervalMicroSeconds* iBeginning;
	};


/**
 *
 * Test Reopen sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderReopen :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderReopen* NewL();
	CTestStepRecorderReopen();
	~CTestStepRecorderReopen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Reopen sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderPosPlay :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderPosPlay* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderPosPlay(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderPosPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName3;
	CFileMan* iFileMan;
	TTimeIntervalMicroSeconds iBeginning;
	TTimeIntervalMicroSeconds iMiddle;
	TTimeIntervalMicroSeconds iEnd;
	};

/**
 *
 * Test record-open-play sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordPlay :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRecordPlay* NewL();
	CTestStepRecorderRecordPlay();
	~CTestStepRecorderRecordPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasRecorded;
	};

/**
 *
 * Test Set Window Play sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderWindowPlay :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderWindowPlay* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderWindowPlay(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderWindowPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName3;
	CFileMan* iFileMan;
	TTimeIntervalMicroSeconds iBeginning;
	TTimeIntervalMicroSeconds iMiddle;
	TTimeIntervalMicroSeconds iEnd;
	};

/**
 *
 * Test Set Window Play sound, SetWindow to different value and play again
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderWindowPlayTwice:	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderWindowPlayTwice* NewL(const TDesC& aTestName);
	CTestStepRecorderWindowPlayTwice(const TDesC& aTestName);
	~CTestStepRecorderWindowPlayTwice();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CFileMan* iFileMan;
	TInt iSetPlayWindowStart;
	TInt iSetPlayWindowEnd;
	};

/**
 *
 * Test Clear Window Play sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderClearWindowPlay :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderClearWindowPlay* NewL();
	CTestStepRecorderClearWindowPlay();
	~CTestStepRecorderClearWindowPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TTimeIntervalMicroSeconds iBeginning;
	TTimeIntervalMicroSeconds iMiddle;
	TTimeIntervalMicroSeconds iEnd;
	TBool iHasStartedPlaying;
	TBool iHasFinishedPlaying;
	};

/**
 *
 * Test Recorder Duration 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderDuration :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderDuration* NewL();
	CTestStepRecorderDuration();
	~CTestStepRecorderDuration();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileNames[3];
	};

/**
 *
 * Test Recorder Audio Type 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderType :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderType* NewL();
	CTestStepRecorderType();
	~CTestStepRecorderType();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileName2;
	//CMdaAudioType* iAudType;
	};

/**
 *
 * Test Recorder State 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderState :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderState* NewL();
	CTestStepRecorderState();
	~CTestStepRecorderState();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasRecorded;
	TBool iHasPlayed;
	};

/**
 *
 * Test Recorder Repeats 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRepeats :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRepeats* NewL();
	CTestStepRecorderRepeats();
	~CTestStepRecorderRepeats();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasPlayed;
	};

/**
 *
 * Test Recorder Set Volume Ramp 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderVolRamp :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderVolRamp* NewL();
	CTestStepRecorderVolRamp();
	~CTestStepRecorderVolRamp();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasStartedPlaying;
	TBool iHasFinishedPlaying;
	};

/**
 *
 * Test Recorder Set/Get (Max)Volume  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderVol :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderVol* NewL();
	CTestStepRecorderVol();
	~CTestStepRecorderVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasStartedPlaying;
	TBool iHasFinishedPlaying;
	};

/**
 *
 * Test Recorder Set/Get PlaybackBalance  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderBalancePlay :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderBalancePlay* NewL();
	CTestStepRecorderBalancePlay();
	~CTestStepRecorderBalancePlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder Set/Get RecordBalance  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderBalanceRecord :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderBalanceRecord* NewL();
	CTestStepRecorderBalanceRecord();
	~CTestStepRecorderBalanceRecord();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder Get Gain  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderGain :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderGain* NewL();
	CTestStepRecorderGain();
	~CTestStepRecorderGain();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	TInt iGain;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder Crop(Beg, End)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderCrop :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderCrop* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderCrop(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderCrop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void CropAndStartSchedulerL(const TTimeIntervalMicroSeconds& /* aBeginning */,
								const TTimeIntervalMicroSeconds& /* end */ );
	void CropAndStartSchedulerL(TTimeIntervalMicroSeconds /*aCropPoint*/, TBool /*aCropEnd*/);
private:
	TInt iError;
	TInt64 iOrigDuration;
	TBool iHasCropped;
	TBuf<100> iFileNameTmp;
	TBuf<100> iOutputFileName;
	CFileMan* iFileMan;
	TBool iIsConverterTest;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	};



/**
 *
 * Test Recorder Crop Descriptor (Beg, End)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecCropDescriptor : public	CTestStepUnitMMFAudClient,
								   public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecCropDescriptor* NewL(const TDesC& aTestName, TBool aUseOldAPi  );
	CTestStepRecCropDescriptor(const TDesC& aTestName, 	TBool aUseOldAPi );
	~CTestStepRecCropDescriptor();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
	void ReadFileToDescriptorL(const TDesC& aFilename );


private:
	void CropAndStartSchedulerL(const TTimeIntervalMicroSeconds& /* aBeginning */,
								const TTimeIntervalMicroSeconds& /* end */ );
	void CropAndStartSchedulerL(TBool /*aCropEnd*/);
private:
	TInt iError;
	TInt64 iOrigDuration;
	TBool iHasCropped;
	TBuf<100> iFileNameTmp;
	TPtrC iFileName3;
	CFileMan* iFileMan;
	CMMFMdaAudioRecorderUtility* iRecorder;
	HBufC8* iDescHBuf;				// HBuf for holding descriptor data
	TPtr8* iDes8;
	TBool iIsOldAPiTest;
	};

/**
 *
 * Test Recorder SetPriority
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSetPriority :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSetPriority* NewL();
	CTestStepRecorderSetPriority();
	~CTestStepRecorderSetPriority();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void CropAndStartSchedulerL(const TTimeIntervalMicroSeconds& /* aBeginning */,
								const TTimeIntervalMicroSeconds& /* end */ );

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder RecordTimeAvailable
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderRecordTimeAvail :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRecordTimeAvail* NewL();
	CTestStepRecorderRecordTimeAvail();
	~CTestStepRecorderRecordTimeAvail();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder SetMaxWriteLength
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSetMaxWriteLen :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSetMaxWriteLen* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderSetMaxWriteLen(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderSetMaxWriteLen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName3;
	CFileMan* iFileMan;
	};

/**
 *
 * Test Recorder Get Number of Metadata Entries
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderGetNumMeta :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderGetNumMeta* NewL();
	CTestStepRecorderGetNumMeta();
	~CTestStepRecorderGetNumMeta();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileNames[3];
	};

/**
 *
 * Test Recorder Get a Metadata Entry
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderGetMeta :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderGetMeta* NewL();
	CTestStepRecorderGetMeta();
	~CTestStepRecorderGetMeta();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL(TInt);

private:
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileNames[3];
	};

/**
 *
 * Test Recorder Add a Metadata Entry
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAddDelMeta :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAddDelMeta* NewL();
	CTestStepRecorderAddDelMeta();
	~CTestStepRecorderAddDelMeta();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder Delete(remove) a Metadata Entry
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepRecorderDelMeta :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderDelMeta* NewL();
	CTestStepRecorderDelMeta();
	~CTestStepRecorderDelMeta();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
*/
	//void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
/*
private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};
*/

/**
 *
 * Test Recorder Replace a Metadata Entry
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderReplaceMeta :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderReplaceMeta* NewL();
	CTestStepRecorderReplaceMeta();
	~CTestStepRecorderReplaceMeta();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Test Recorder Destination Data Type(s)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderDestinDataType :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderDestinDataType* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderDestinDataType(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderDestinDataType();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2, iFileName3;
	CFileMan* iFileMan;
	};

/**
 *
 * Test Recorder supported/Destination Bit Rate
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderBitRate :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderBitRate* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderBitRate(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderBitRate();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2, iFileName3;
	CFileMan* iFileMan;
	};

/**
 *
 * Test Recorder supported/Destination Sample Rate
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSampleRate :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSampleRate* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderSampleRate(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderSampleRate();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2, iFileName3;
	CFileMan* iFileMan;
	};

/**
 *
 * Test Recorder supported/Destination Format
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepRecorderFormat :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderFormat* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderFormat(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderFormat();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError, iNum;
	TBool iIsConverterTest;
	CMdaAudioConvertUtility* iConvert;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TPtrC iFileName2, iFileName3;
	CFileMan* iFileMan;
	};

/**
 *
 * Test Recorder supported/Destination Channels
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderChannels :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderChannels* NewL(const TDesC& aTestName, TBool aIsConverterTest);
	CTestStepRecorderChannels(const TDesC& aTestName, TBool aIsConverterTest);
	~CTestStepRecorderChannels();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TBool iIsConverterTest;
	TInt iError, iNum;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CTimer* iTimer;                        // timer added to improve asynch processing
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2, iFileName3;
	CFileMan* iFileMan;
	};
	
	
/**
 *
 * GetImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderGetImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderGetImplementationInfo* NewL();
	CTestStepRecorderGetImplementationInfo();
	~CTestStepRecorderGetImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iIsOpen;
	};


/**
 *
 * SendCustomCommandSync
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSendCustomCommandSync :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSendCustomCommandSync* NewL();
	CTestStepRecorderSendCustomCommandSync();
	~CTestStepRecorderSendCustomCommandSync();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TInt iNum;
	TBool iIsOpen;
	};


/**
 *
 * SendCustomCommandAsync
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSendCustomCommandAsync :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSendCustomCommandAsync* NewL();
	CTestStepRecorderSendCustomCommandAsync();
	~CTestStepRecorderSendCustomCommandAsync();
	void Close();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
    
   	class CAsyncObserver : public CActive
		{
	public:
		CAsyncObserver() : CActive(EPriorityStandard) {CActiveScheduler::Add(this);}
		void RunL()
			{
			CActiveScheduler::Stop();
			}
		void DoCancel(){;}
		TRequestStatus& ActiveStatus(){SetActive();return iStatus;}
		};


private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CAsyncObserver* iAsyncObserver;
	TInt iNum;
	TBool iIsOpen;
	};

/**
 *
 * ReloadingTest
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderReloadingTest :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver,
								public  MAudioLoadingObserver
	{
public:
	static CTestStepRecorderReloadingTest* NewL();
	CTestStepRecorderReloadingTest();
	~CTestStepRecorderReloadingTest();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
	// from MAudioLoadingObserver
	virtual void MaloLoadingStarted();
	virtual void MaloLoadingComplete();
    
private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TInt iNum;
	TBool iIsOpen;
	TBool iCorrectEventReceived;
	TBool iEventReceived;
	TBool iLoadingComplete;
	TBool iLoadingStarted;
	};


/**
 *
 * Test Record-play-record-play
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderPlayRecordPlay :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver,
								public MMdaAudioPlayerCallback

	{
public:
	static CTestStepRecorderPlayRecordPlay* NewL();
	CTestStepRecorderPlayRecordPlay();
	~CTestStepRecorderPlayRecordPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// from MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);

private:
	void OpenAndStartSchedulerL(TPtrC aFileName);
	void RecordPlayRecordTestL(TPtrC aFileName);
	void DeleteFileL(TPtrC aFileName);

private:
	CFileMan* iFileMan;
	TInt iError;
	TPtrC iFileNameWav;
	TPtrC iFileNameAu;
	//TPtrC iFileNameRaw;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * Test Recording error 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderError :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver							
	{
public:
	static CTestStepRecorderError* NewL();
	void ConstructL();
	CTestStepRecorderError();
	~CTestStepRecorderError();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

protected:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CMMFMdaAudioRecorderUtility* iRecorderHighPriority;
	TBool iHasRecorded;
	TPtrC iFileName2, iFileName3;
	TTimeIntervalMicroSeconds* iBeginning;

	 //[ fsm states for this test ]
	 enum TestState
	 {
      TestStateIdle, // initial state
	  TestStateOne,  
	  TestStateTwo,
	  TestStateThree,
	  TestStateFour,
	  TestStateTerminal,
	 };

	 // set up typedef for pointer to dispatch function
	 typedef void (CTestStepRecorderError::*pCallBack)(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	 pCallBack iStates[KMaxMethods]; //[ State table to be used for fsm dispatching ]

	 //[ actual test state ]
	 TestState iTestState ;
	 void SetState( TestState& aState );
	 TestState GetTestState() const ;
	 TBool IsValidState() const;
     
	 //[ Print Callback state ]
	 void PrintState(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode );
	 
	 //[finite state machine dispatch methods ]
	 void IdleStateChangeEventL(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
 	 void StateOneChangeEventL(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
 	 void StateTwoChangeEventL(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	 void StateThreeChangeEventL(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	 void StateFourChangeEventL(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
 	 void StateTerminalChangeEventL(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	};


const TInt KToneFrequency = 2000;
const TInt KTwoSeconds = 1000000;
const TInt KQuarterScond = 250000;

/**
 *
 * Test playing two files simutaneously
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSimultaneousPlay1 :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver,
									public MMdaAudioToneObserver
	{
public:
	static CTestStepRecorderSimultaneousPlay1* NewL();
	CTestStepRecorderSimultaneousPlay1();
	~CTestStepRecorderSimultaneousPlay1();
	void Close();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
private:
	TInt iError;
	TInt iCurrentState;
	TInt iErrorTone;
	TPtrC iFileName;
	CMdaAudioToneUtility* iToneUtility;
	CMdaAudioRecorderUtility* iRecorderUtility;

	};

//

/**
 *
 * Test: interrupt a low priority recorder with a high priority tone
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSimultaneousPlay2 :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver,
									public MMdaAudioToneObserver
	{
public:
	static CTestStepRecorderSimultaneousPlay2* NewL();
	CTestStepRecorderSimultaneousPlay2();
	~CTestStepRecorderSimultaneousPlay2();
	void Close();	
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
private:
	TInt iError;
	TInt iCurrentState;
	TInt iErrorTone;
	TPtrC iFileName;
	CMdaAudioToneUtility* iToneUtility;
	CMdaAudioRecorderUtility* iRecorderUtility;
	};

//

/**
 *
 * Test: interrupt a low priority recorder with a high priority recorder
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSimultaneousPlay3 :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSimultaneousPlay3* NewL();
	CTestStepRecorderSimultaneousPlay3();
	~CTestStepRecorderSimultaneousPlay3();
	void Close();	
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName1;
	TPtrC iFileName2;
	CMdaAudioRecorderUtility* iRecorderUtility1;
	CMdaAudioRecorderUtility* iRecorderUtility2;

	};

//

/**
 *
 * Interrupt a low priority recorder with a higher priority recorder
 * playing the same file
 * 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSimultaneousPlay4 :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSimultaneousPlay4* NewL();
	CTestStepRecorderSimultaneousPlay4();
	~CTestStepRecorderSimultaneousPlay4();
	void Close();	
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	TInt iCurrentState1;	// current state of first recorder utility
	TInt iCurrentState2;	// current state of second recorder utility
	TPtrC iFileName1;
	CMdaAudioRecorderUtility* iRecorderUtility1;
	CMdaAudioRecorderUtility* iRecorderUtility2;

	};

//

/**
 *
 * Open a low priority recorder and a higher priority recorder
 * with the same file & get the duration in each case
 * DEF038653
 * 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSimultaneousPlay5 :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSimultaneousPlay5* NewL();
	CTestStepRecorderSimultaneousPlay5();
	~CTestStepRecorderSimultaneousPlay5();
	void Close();	
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	TInt iCurrentState1;	// current state of first recorder utility
	TInt iCurrentState2;	// current state of second recorder utility
	TPtrC iFileName1;
	CMdaAudioRecorderUtility* iRecorderUtility1;
	CMdaAudioRecorderUtility* iRecorderUtility2;

	};

//

/**
 *
 * Test Playing once, getting the position after two seconds
 * waiting for play to complete and then playing again
 * Verify that the time take to play both time is roughly the same
 * i.e. that getting the position doesn't affect the second play's 
 * starting point (DEF037064)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderPlayTwice :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver,
									public MMdaAudioToneObserver
	{
public:
	static CTestStepRecorderPlayTwice* NewL();
	CTestStepRecorderPlayTwice();
	~CTestStepRecorderPlayTwice();
	void Close();	
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
private:
	TInt iError;
	TInt iCurrentState;
	TInt iErrorTone;
	TPtrC iFileName;
	CMdaAudioRecorderUtility* iRecorderUtility;

	};


/**
 *
 * Play a clip & periodically rewind & advance the position.
 * (akin to pressing the Rewind/FastForward buttons in the Audio app)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderPlayFF :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	enum TSchedulerStopReasons {EUnknown, EStateChange, ETimer};
public:
	static CTestStepRecorderPlayFF* NewL(const TDesC& aTestName);
	CTestStepRecorderPlayFF(const TDesC& aTestName);
	~CTestStepRecorderPlayFF();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// Callback timer used because we want client utility to 
	// receive callbacks from controller while timer is running
	class CCallbackTimer : public CTimer
		{
	public:
		CCallbackTimer(CTestStepRecorderPlayFF& aTest);
		~CCallbackTimer();
		static CCallbackTimer* NewL(CTestStepRecorderPlayFF& aTest);
		void RunL();
	private:
		CTestStepRecorderPlayFF& iTest;
		};

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	TInt iState;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CCallbackTimer* iTimer;

public:
	TSchedulerStopReasons iSchedulerStopReason;
	};


/**
 * INC042545 - Phone freezes when playing *.wav audio with FF and Stop.
 * Play a clip from the middle to EOF, then set the position to the clip's 
 * duration, call play and then immediately read the position & check it's 
 * not longer than the file's duration
 *
 *
 */
class CTestStepRecorderPlayEOF :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	enum TSchedulerStopReasons {EUnknown, EStateChange, ETimer};
public:
	static CTestStepRecorderPlayEOF* NewL(const TDesC& aTestName);
	CTestStepRecorderPlayEOF(const TDesC& aTestName);
	~CTestStepRecorderPlayEOF();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// Callback timer used because we want client utility to 
	// receive callbacks from controller while timer is running
	class CCallbackTimer : public CTimer
		{
	public:
		CCallbackTimer(CTestStepRecorderPlayEOF& aTest);
		~CCallbackTimer();
		static CCallbackTimer* NewL(CTestStepRecorderPlayEOF& aTest);
		void RunL();
	private:
		CTestStepRecorderPlayEOF& iTest;
		};

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	TInt iState;
	CMMFMdaAudioRecorderUtility* iRecorder;
	CCallbackTimer* iTimer;

public:
	TSchedulerStopReasons iSchedulerStopReason;
	};


/**
 *
 * Test Open MP3 sound from file
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderOpenMp3File :	public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderOpenMp3File* NewL();
	CTestStepRecorderOpenMp3File();
	~CTestStepRecorderOpenMp3File();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	class TMp3FileInfo
		{
		public:
			TMp3FileInfo(TDesC &aFileName, TVerdict aExpectedVerdict, TBool aExpectedError) :
			  iFileName(aFileName), iExpectedVerdict(aExpectedVerdict), iExpectedError(aExpectedError)
				{
				}
		public:
			TPtrC iFileName;
			TVerdict iExpectedVerdict;
			TInt iExpectedError;
		};

private:
	void OpenAndStartSchedulerL();
	TVerdict DoNextTestStepL();

private:
	TInt iState;
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	RArray<TMp3FileInfo> iFileInfo;
	};

/**
 *
 * INC038974 - SetPlayWindow,play,pause,ClearPlayWindow,pause,stop,play
 * make sure second play starts from start of file
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPlayAndClearWindow :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioPlayerCallback,
									public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepPlayerPlayAndClearWindow* NewL();
	CTestStepPlayerPlayAndClearWindow();
	~CTestStepPlayerPlayAndClearWindow();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TVerdict DoTestStepPlayerL();
	TVerdict DoTestStepRecorderL();

private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName;
	CMdaAudioPlayerUtility* iPlayerUtility;
	CMdaAudioRecorderUtility* iRecorderUtility;
	};


/**
 *
 * Test Playing a file and after a while changing the position
 * Wait for end of file and check the time to play is as expected
 * (INC042196)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSetPosition :public	CTestStepUnitMMFAudClient,
									public	MMdaObjectStateChangeObserver,
									public MMdaAudioToneObserver
	{
public:
	static CTestStepRecorderSetPosition* NewL();
	CTestStepRecorderSetPosition();
	~CTestStepRecorderSetPosition();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
private:
	TInt iError;
	TInt iCurrentState;
	TInt iErrorTone;
	TPtrC iFileName;
	CMdaAudioRecorderUtility* iRecorderUtility;
	};


/** 
 Test Record sound 
 
 @lib "TSU_MMF_ACLNT_01.lib"
 
 
 
 */
class CTestStepRecorderRecordStop;
class CTestStepRecorderRecordStopTimer : public CTimer
	{
public:
	static CTestStepRecorderRecordStopTimer* NewL(CTestStepRecorderRecordStop* aTestStep, CMMFMdaAudioRecorderUtility* aRecorder);

	void RunL();

private:
	CTestStepRecorderRecordStopTimer(CTestStepRecorderRecordStop* aTestStep, CMMFMdaAudioRecorderUtility* aRecorder);

private:
	CTestStepRecorderRecordStop* iTestStep;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

class CTestStepRecorderRecordStop :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderRecordStop* NewL();
	CTestStepRecorderRecordStop();
	~CTestStepRecorderRecordStop();
	virtual enum TVerdict DoTestStepPreambleL();
	virtual enum TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	void IncrementAndLogDelay();

private:
	void RecordAndStartSchedulerL();

private:
	TInt iError;
	CFileMan *iFileMan;
	CMMFMdaAudioRecorderUtility* iRecorder;

	TBool iStopping;
	CTestStepRecorderRecordStopTimer* iTimer;

	TMdaFileClipLocation* iClipFileName;
	TMdaWavClipFormat iClipFormat;
	TMdaPcmWavCodec iClipCodec;

	TInt iDelay;
	TTimeIntervalMicroSeconds* iBeginning;
	};

/**
 *
 * Test Recorder correct playback after recording
 * INC047838
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CRecPlayMuteTest;
class CTestStepRecorderPlayAfterRecording :	public	CTestStepUnitMMFAudClient
	{
public:
	static CTestStepRecorderPlayAfterRecording* NewL();
	CTestStepRecorderPlayAfterRecording();
	~CTestStepRecorderPlayAfterRecording();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

private:
	TInt iError;
	CRecPlayMuteTest* iTestObject;
	};
/**
 *
 * INC048184 - SetPlayWindow,play,Stop,SetPosition,Stop
 * make sure play starts from start of file
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderPlayStopPosStop :	public	CTestStepUnitMMFAudClient,
									        public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderPlayStopPosStop* NewL();
	~CTestStepRecorderPlayStopPosStop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	CTestStepRecorderPlayStopPosStop();
	
private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName;
	CMdaAudioRecorderUtility* iRecorderUtility;
	};
// INC060658  CMdaAudioRecorderUtility::OpenDesL fails with KErrNotFound

class CRecUtilDesOpenTest;
class CTestStepRecUtilOpenDesByUid :	public	CTestStepUnitMMFAudClient
	{
public:
	static CTestStepRecUtilOpenDesByUid* NewL();
	CTestStepRecUtilOpenDesByUid();
	~CTestStepRecUtilOpenDesByUid();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
private:
	TInt iError;
	CRecUtilDesOpenTest* iTestObject;
	};


/**
 *
 * DEF061942  KERN-EXEC:15 in the AudioPolicy server
 *
 */
class CTestStepRecorderNotifyCheck :	public	CTestStepUnitMMFAudClient,
									    public	MMdaObjectStateChangeObserver,
									    public  MMMFAudioResourceNotificationCallback
	{
public:
	static CTestStepRecorderNotifyCheck* NewL();
	~CTestStepRecorderNotifyCheck();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
	// from MMMFAudioResourceNotificationCallback
	void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
private:
	CTestStepRecorderNotifyCheck();
	
private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName;
	CMdaAudioRecorderUtility* iRecorderUtility;
	};

/**
 *
 * DEF061821  - CMdaAudioRecorderUtility::PlayL() fail case when registered for notification 
 *
 */
class CTestStepNotifyRegister :	public	CTestStepUnitMMFAudClient,
									    public	MMdaObjectStateChangeObserver,
									    public  MMMFAudioResourceNotificationCallback,
									    public  MMdaAudioOutputStreamCallback,
									    public  MMdaAudioPlayerCallback
	{
public:
	static CTestStepNotifyRegister* NewL();
	~CTestStepNotifyRegister();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
	// from the MMMFAudioResourceNotificationCallback
	void MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);
	// from the MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);
	// from the MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);
private:
	CTestStepNotifyRegister();
	
	TVerdict DoClipUtilStepL();
	TVerdict DoStreamUtilStepL();
private:
	TInt iError;
	TInt iCurrentState;
	TPtrC iFileName;
	CBase* iPlayUtility;
	};

/**
 * DEF068366  - Using record utility, callback of PlaybackBalance APIs fail.
 * Test Recorder Get/Set/Record/Get PlaybackBalance  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderSetBalRecordGetBal:	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderSetBalRecordGetBal* NewL();
	CTestStepRecorderSetBalRecordGetBal();
	~CTestStepRecorderSetBalRecordGetBal();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};
	
/**
 *
 * DEF078081: TCK fail: CMMFMdaAudioPlayerUtility::Duration can't handle infinite sounds
 * This defect resulted in the call TMMFDurationInfo Duration(TTimeIntervalMicroSeconds& aDuration) 
 * being added to CMdaAudioPlayerUtility, CMdaAudioRecorderUtility
 *
 * This test case checks that the call returns TMMFDurationInfo::EMMFDurationInfoValid
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderDurationInformation :	public	CTestStepUnitMMFAudClient,
							      	    		public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderDurationInformation* NewL();
	CTestStepRecorderDurationInformation();
	~CTestStepRecorderDurationInformation();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
  
	// from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	TInt iError;
	CMdaAudioRecorderUtility* iRecorder;
 	};
 	
 /**
 *
 * DEF085172: Incorrect documentation of CMdaAudioRecorderUtility::SetVolume()
 *
 * Test Recorder Set/Get (Low or Max)Volume  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderLowMaxVol :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderLowMaxVol* NewL();
	CTestStepRecorderLowMaxVol();
	~CTestStepRecorderLowMaxVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
	
private:
	TInt iError;
	CMdaAudioRecorderUtility* iRecorder;
	TBool iHasStartedPlaying;
	TBool iHasFinishedPlaying;
	};

/**
 *
 * CheckPlayImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderCheckPlayImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderCheckPlayImplementationInfo* NewL();
	CTestStepRecorderCheckPlayImplementationInfo();
	~CTestStepRecorderCheckPlayImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iIsOpen;
	};

/**
 *
 * CheckRecordImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderCheckRecordImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderCheckRecordImplementationInfo* NewL();
	CTestStepRecorderCheckRecordImplementationInfo();
	~CTestStepRecorderCheckRecordImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iIsOpen;
	};
	
 /**
 *
 * DEF087609: APIREG CMdaAudioPlayerUtility Panic tests fail with wrong panic
 *
 * Test Player Set/Get (Low or Max)Volume  
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerLowMaxVol :	public	CTestStepUnitMMFAudClient,
									public MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerLowMaxVol* NewL();
	CTestStepPlayerLowMaxVol();
	~CTestStepPlayerLowMaxVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

		// from MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMdaAudioPlayerUtility* iPlayer;
	TBool iHasStartedPlaying;
	TBool iHasFinishedPlaying;
	};
	
/**
 *
 * DEF093014: T: CMdaAudioRecorderUtility::SetDestinationFormatL causes panic
 *
 * Open a file based clip and try to set the destination format by passing the wrong UID,
 * so that it should leave with KErrNotSupported.
 *
 * @class CTestMmfAclntRecFileInvDesForm
 *
 */
class CTestMmfAclntRecFileInvDesForm :	public CTestStepUnitMMFAudClient, 
										public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntRecFileInvDesForm* NewL();
	virtual TVerdict DoTestStepL();
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);	
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	
private:
	CTestMmfAclntRecFileInvDesForm();

private:
	TInt iError;
	TPtrC iFileName;
	CMdaAudioRecorderUtility* iRecUtil;
	};	
	
/**
 *
 * DEF096351: T: CMdaAudioRecorderUtility::SetMaxWriteLength(-1) gives KErrArgument
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderMaxWriteLength :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderMaxWriteLength* NewL();
	CTestStepRecorderMaxWriteLength();
	~CTestStepRecorderMaxWriteLength();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

protected:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};
	
class CRecordAudio : public CBase, 
				     public MMdaObjectStateChangeObserver
	{
public:
	static CRecordAudio* NewL();
	
	~CRecordAudio();
	
	void RecordL(const TDesC& aFileName, TInt aPriority);
	TInt Error()
		{
		return iError;
		}
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
protected:
	CRecordAudio();
	void ConstructL();
	void OnOpenL();
	static TInt TimerCallback(TAny*);
	void DoTimerCallback();
private:
	CMdaAudioRecorderUtility* iUtility;
	TInt iError;
	CPeriodic* iTimer;
	};
	
class RAudioConcurrentRecTest : public RAsyncTestStep
	{
public:
	static RAudioConcurrentRecTest* NewL(const TDesC& aStepName, TInt aRecPriority1, TInt aRecPriority2, TInt aRecPriority3);
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
private:
	RAudioConcurrentRecTest(const TDesC& aStepName, TInt aRecPriority1, TInt aRecPriority2, TInt aRecPriority3);

private:
	RThread iThread1;
	RThread iThread2;
	RThread iThread3;
	TInt iRecPriority1;
	TInt iRecPriority2;
	TInt iRecPriority3;
	};

/**
 *
 * Defect INC115421 Sound recorder : When a recorded file is played and when we tap on the file to play again the phone crashes and restarts.
 * This defect resulted in the function CMdaAudioRecorderUtility::SetVolume(TInt aVolume) returning a TInt error code
 * where it returned void previously.
 * 
 * Test that errors are returned properly from a call to CMdaAudioRecorderUtility::SetVolume.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderVolError :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
    {
public:
	static CTestStepRecorderVolError* NewL();
	CTestStepRecorderVolError();
	~CTestStepRecorderVolError();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorcCode);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iHasStartedPlaying;
	TBool iHasFinishedPlaying;
    };

class MRecordAudioObserver
	{
public:
	//Signal completion of test with associated error
	virtual void RecordError(TInt aError) = 0;
	};

class CRecorder : public CBase, 
			      public MMdaObjectStateChangeObserver
	{
public:
	static CRecorder* NewL(MRecordAudioObserver* aObserver);
	
	~CRecorder();
	
	void RecordL(const TDesC& aFileName, TInt aPriority);
	
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
protected:
	CRecorder(MRecordAudioObserver* aObserver);
	void ConstructL();
	void OnOpenL();
	static TInt TimerCallback(TAny*);
	void DoTimerCallback();
private:
	MRecordAudioObserver* iObserver;
	CMdaAudioRecorderUtility* iUtility;
	TInt iError;
	CPeriodic* iTimer;
	};

//DEF127630:Fix for INC108007 appears to close
class RAudioRecPanicTest :  public RAsyncTestStep,
							public MRecordAudioObserver
	{
public:
	static RAudioRecPanicTest* NewL(const TDesC& aStepName, const TDesC& aFileName);
	
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
	//from MRecordAudioObserver
	void RecordError(TInt aError);
private:
	RAudioRecPanicTest(const TDesC& aStepName, const TDesC& aFileName);

private:
	CRecorder* iRecorder;
	TFileName iFileName;
	RFs iFs;
	};

#endif //(__TEST_STEP_RECORDER__)
