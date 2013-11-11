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


#ifndef __TEST_STEP_RECORDER_ALLOCFAIL__
#define __TEST_STEP_RECORDER_ALLOCFAIL__

#include <mda/client/utility.h>
#include "mmfclientaudiorecorder.h"
 
/**
 *
 * Alloc Failure New Recorder utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailNew :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailNew* NewL();
	CTestStepRecorderAllocFailNew();
	~CTestStepRecorderAllocFailNew();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Alloc Failure Open sound from file
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailOpenFile :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailOpenFile* NewL(TBool aUseFileHandle);
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	CMdaAudioRecorderUtility* iRecorder;
	TBool iResultOK;
	TBool iAllocOK;
	TBool iUseFileHandle;

#ifdef __WINS__
	TBool iKErrGeneralIgnored; //Ignore KErrGeneral on EMULATOR
#endif
	};

/**
 *
 * Alloc Failure Open sound specifying a descriptor
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailOpenDesc :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailOpenDesc* NewL();
	CTestStepRecorderAllocFailOpenDesc();
	~CTestStepRecorderAllocFailOpenDesc();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenDescAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	HBufC8* iBuf;
	TPtr8*  iDes8;
	};

/**
 *
 * Alloc Failure Open sound specifying a url
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailOpenUrl :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailOpenUrl* NewL();
	CTestStepRecorderAllocFailOpenUrl();
	~CTestStepRecorderAllocFailOpenUrl();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenUrlAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};


/**
 *
 * Alloc Failure Play sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailOpen :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailOpen* NewL();
	CTestStepRecorderAllocFailOpen();
	~CTestStepRecorderAllocFailOpen();
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
	TMdaFileClipLocation* iClipLocation;
	TMdaClipFormat* iFormat;
	};

/**
 *
 * Alloc Failure Play sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailPlay :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailPlay* NewL();
	CTestStepRecorderAllocFailPlay();
	~CTestStepRecorderAllocFailPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void PlayAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};

/**
 *
 * Alloc Failure Record sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailRecord :	public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailRecord* NewL();
	CTestStepRecorderAllocFailRecord();
	~CTestStepRecorderAllocFailRecord();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void RecordAndStartSchedulerL();

private:
	TInt iError;
	TBool iHasRecorded;
	CMMFMdaAudioRecorderUtility* iRecorder;
	TBool iSimulateDriveRemoval;
	};

/**
 *
 * Alloc Failure Crop sound 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepRecorderAllocFailCrop :		public	CTestStepUnitMMFAudClient,
											public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepRecorderAllocFailCrop* NewL();
	CTestStepRecorderAllocFailCrop();
	~CTestStepRecorderAllocFailCrop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void CropAndStartSchedulerL();

private:
	TInt iError;
	TInt64 iOrigDuration;
	TBool iHasCropped;
	TBuf<100> iFileNameTmp;
	CFileMan* iFileMan;
	CMMFMdaAudioRecorderUtility* iRecorder;
	};




#endif //(__TEST_STEP_RECORDER_ALLOCFAIL__)
