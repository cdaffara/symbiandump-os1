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


#ifndef TESTSTEPCONVERT_H
#define TESTSTEPCONVERT_H

 
/**
 *
 * Alloc Failure New Convert utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepConvertAllocFailNew :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertAllocFailNew* NewL();
	CTestStepConvertAllocFailNew();
	~CTestStepConvertAllocFailNew();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	};

/**
 *
 * Alloc Failure New Convert utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepConvertNew :	public	CTestStepUnitMMFAudClient,
							public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertNew* NewL();
	CTestStepConvertNew();
	~CTestStepConvertNew();
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
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
class CTestStepConvertAllocFailOpen :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertAllocFailOpen* NewL();
	CTestStepConvertAllocFailOpen();
	~CTestStepConvertAllocFailOpen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	void OpenAndStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2;
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
class CTestStepConvertOpen :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertOpen* NewL();
	CTestStepConvertOpen();
	~CTestStepConvertOpen();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	//void OpenFileStartSchedulerL();
	TVerdict TestPlayL();
	TVerdict TestRecordL();
	TVerdict CompareFilesL(const TDesC& aCreatedFilename, const TDesC& aExpectedFilename, TInt aLength);

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2;
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
class CTestStepConvertOpenLocFormat :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertOpenLocFormat* NewL();
	CTestStepConvertOpenLocFormat();
	~CTestStepConvertOpenLocFormat();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	//void OpenFileStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2;
	TMdaClipLocation* iClipLocationSrc;
	TMdaAuClipFormat iAuClipFormat;
	TMdaPcm16BitAuCodec iAu16bitCodec;
	TMdaWavClipFormat iWavClipFormat;
	TBool iHasStartedConverting;
	TBool iHasFinishedConverting;
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
class CTestStepConvertOpenLocFormat2 :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertOpenLocFormat2* NewL();
	CTestStepConvertOpenLocFormat2();
	~CTestStepConvertOpenLocFormat2();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	//void OpenFileStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	CFileMan* iFileMan;
	TPtrC iFileName2;
	TMdaClipLocation* iClipLocationSrc;
	TMdaClipLocation* iClipLocationTrgt;
	TMdaAuClipFormat iAuClipFormat;
	TMdaWavClipFormat iWavClipFormat;
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
class CTestStepConvertOpenLocFormat3 :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertOpenLocFormat3* NewL();
	CTestStepConvertOpenLocFormat3();
	~CTestStepConvertOpenLocFormat3();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	//void OpenFileStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	CFileMan* iFileMan;
	TPtrC iFileName2;
	TPtrC iFileNameWav1;
	TPtrC iFileNameWav2;

	TMdaClipLocation* iClipLocationSrc;
	TMdaClipLocation* iClipLocationTrgt;

	TMdaRawAudioClipFormat iRawClipFormat;
	TMdaU8PcmRawAudioCodec iRawAudioCodec;
	TMdaSL16RawAudioCodec iRawAudio16Codec;

	TMdaWavClipFormat iWavClipFormat;
	TMdaPcmWavCodec   iWavAudioPcmCodec;
	TMdaMulawWavCodec iWavAudioulawCodec;

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
class CTestStepConvertOpenLocFormat4 :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertOpenLocFormat4* NewL();
	CTestStepConvertOpenLocFormat4();
	~CTestStepConvertOpenLocFormat4();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	//void OpenFileStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	CFileMan* iFileMan;
	TMdaClipLocation* iClipLocationSrc;

	TMdaClipLocation* iClipLocationTrgtDes;
	RPointerArray<TDes8> iBuffer;

	TMdaRawAudioClipFormat iRawClipFormat;
	TMdaU8PcmRawAudioCodec iRawAudioCodec;
	TMdaSL16RawAudioCodec iRawAudio16Codec;

	TMdaWavClipFormat iWavClipFormat;
	TMdaPcmWavCodec   iWavAudioPcmCodec;
	TMdaMulawWavCodec iWavAudioulawCodec;

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
/*
class CTestStepConvertType :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertType* NewL();
	CTestStepConvertType();
	~CTestStepConvertType();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
*/
	// from MMdaObjectStateChangeObserver */
	//void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
/*
private:
	//void OpenFileStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2;
	CMdaAudioType* iAudioType;
	};
*/

/**
 *
 * Test Open
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepConvertAllocFailType :	public	CTestStepUnitMMFAudClient,
										public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertAllocFailType* NewL();
	CTestStepConvertAllocFailType();
	~CTestStepConvertAllocFailType();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
*/ //	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
/*
private:
	//void OpenFileStartSchedulerL();

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TPtrC iFileName2;
	};
*/

/**
 *
 * GetImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepConvertGetImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertGetImplementationInfo* NewL();
	CTestStepConvertGetImplementationInfo();
	~CTestStepConvertGetImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TMdaClipLocation* iClipLocationSrc;
	TMdaClipLocation* iClipLocationDest;
	TInt iNum;
	TBool iHasStartedConverting;
	TBool iHasFinishedConverting;
	TPtrC iFileName2;
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
class CTestStepConvertSendCustomCommandSync :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertSendCustomCommandSync* NewL();
	CTestStepConvertSendCustomCommandSync();
	~CTestStepConvertSendCustomCommandSync();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TInt iNum;
	TBool iHasStartedConverting;
	TBool iHasFinishedConverting;
	TMdaClipLocation* iClipLocationSrc;
	TMdaClipLocation* iClipLocationTrgt;
	TPtrC iFileName2;
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
class CTestStepConvertSendCustomCommandAsync :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertSendCustomCommandAsync* NewL();
	CTestStepConvertSendCustomCommandAsync();
	~CTestStepConvertSendCustomCommandAsync();
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
	CMdaAudioConvertUtility* iConvert;
	TMdaClipLocation* iClipLocationSrc;
	TMdaClipLocation* iClipLocationTrgt;

	CAsyncObserver* iAsyncObserver;
	TInt iNum;
	TBool iHasStartedConverting;
	TBool iHasFinishedConverting;
	TPtrC iFileName2;
	};

/**
 *
 * CheckImplementationInfo
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepConvertCheckImplementationInfo :	public	CTestStepUnitMMFAudClient,
								public	MMdaObjectStateChangeObserver
	{
public:
	static CTestStepConvertCheckImplementationInfo* NewL();
	CTestStepConvertCheckImplementationInfo();
	~CTestStepConvertCheckImplementationInfo();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);

private:
	TInt iError;
	CMdaAudioConvertUtility* iConvert;
	TMdaClipLocation* iClipLocationSrc;
	TMdaClipLocation* iClipLocationDest;
	TInt iNum;
	TBool iHasStartedConverting;
	TBool iHasFinishedConverting;
	TPtrC iFileName2;
	};

class MConvAudioObserver
	{
public:
	//* Signal completion of test with associated error
	virtual void ConvertComplete(TInt aError) = 0;
	};

class CConvertAudio : public CBase, 
				      public MMdaObjectStateChangeObserver
	{
public:
	static CConvertAudio* NewL(MConvAudioObserver* aObserver);
	
	virtual ~CConvertAudio();
	
	void ConvertL(const TDesC& aFromFileName, const TDesC& aToFileName);
	
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
protected:
	CConvertAudio(MConvAudioObserver* aObserver);
	void ConstructL();
	
	virtual void OnOpenL();

protected:
	MConvAudioObserver*const iObserver;
	CMdaAudioConvertUtility* iUtility;
	};

class RTestStepConvertAudio : public RAsyncTestStep,
					   public MConvAudioObserver
	{
public:
	static RTestStepConvertAudio* NewL(const TDesC& aStepName, 
	 								   const TDesC& aFromFileName,
	                            	   const TDesC& aToFileName,
	                            	   const TDesC& aReferenceFileName);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	void CloseTest();
	
	// from MConvAudioObserver
	void ConvertComplete(TInt aError);
	TBool CheckConversionL();
protected:
	RTestStepConvertAudio(const TDesC& aStepName, 
	               const TDesC& aFromFileName,
	               const TDesC& aToFileName,
	               const TDesC& aReferenceFileName);
	
	TFileName iFromFileName;
	TFileName iToFileName;
	TFileName iReferenceFileName;
	CConvertAudio* iConverter;
	RFs iFs;
	};

class CConvertPanic : public CConvertAudio
	{
public:	
	static CConvertPanic* NewL(MConvAudioObserver* aObserver);
	
	virtual ~CConvertPanic();
protected:
	void OnOpenL();
	CConvertPanic(MConvAudioObserver* aObserver);
	void ConstructL();
	};
	
class RTestStepConvertPanic: public RTestStepConvertAudio
	{
public:	
	static RTestStepConvertPanic* NewL(const TDesC& aStepName, 
	 								   const TDesC& aFromFileName,
	                            	   const TDesC& aToFileName,
	                            	   const TDesC& aReferenceFileName);
    void KickoffTestL();                  	  
protected:
	RTestStepConvertPanic(const TDesC& aStepName, 
	               const TDesC& aFromFileName,
	               const TDesC& aToFileName,
	               const TDesC& aReferenceFileName);	                            	   
	// from MConvAudioObserver
	void ConvertComplete(TInt aError);
	};

#endif //(TESTSTEPCONVERT_H)
