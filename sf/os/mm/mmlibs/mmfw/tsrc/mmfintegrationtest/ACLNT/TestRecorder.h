
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
 @file TestRecorder.h
*/

#ifndef TESTRECORDER_H
#define TESTRECORDER_H

#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include "TSI_MMFACLNT.h"


/**
 * Record API tests parent class
 *
 * @class CTestMmfAclntRecord
 */
class CTestMmfAclntRecord : public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecord(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	virtual TVerdict DoTestStepL();
	virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil)=0;

protected:
	TInt iError;
	CBase* iObject; 
	TInt iPreviousState; 
	TInt iCurrentState;
	TBool iNegative;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	};


/**
 * Set gain to maximum and enquire gain.
 *
 * CTestMmfAclntRecGain
 *
 */
class CTestMmfAclntRecGain : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecGain(const TDesC& aTestName, const TDesC& aSectName, const TInt aGain, TBool aNegative) ;
	static CTestMmfAclntRecGain* NewL(const TDesC& aTestName, const TDesC& aSectName, const TInt aGain, TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);

private:
	TInt iGain;
	} ;

/**
 * Reading the format of a file with unknown extension.
 *
 * CTestMmfFormatRead
 *
 */
class CTestMmfFormatRead : public CTestMmfAclntRecord
	{
public:
	CTestMmfFormatRead(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	static CTestMmfFormatRead* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	TVerdict DoTestStepL();
	} ;

/**
 * Enquire state at each state transition.
 *
 * CTestMmfAclntRecStateTran
 *
 */
class CTestMmfAclntRecStateTran : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecStateTran(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecStateTran* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	TVerdict DoTestStepL();

private:
	CMdaAudioClipUtility::TState iReportedState;
	} ;


/**
 * Audio croping.
 *
 * CTestMmfAclntCrop
 *
 */
class CTestMmfAclntCrop : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntCrop(const TDesC& aTestName, const TDesC& aSectName, const TBool aCropToEnd, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	//virtual TVerdict DoTestStepL();
	static CTestMmfAclntCrop* NewL(const TDesC& aTestName, const TDesC& aSectName, const TBool aCropToEnd, TBool aNegative);

private:
	TBool iCropToEnd;
	} ;


/**
 * Open, Close and open a new audio clip.
 *
 * @class CTestMmfAclntRecCloseOpen
 *
 */
class CTestMmfAclntRecCloseOpen : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecCloseOpen(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecCloseOpen* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	};



/**
 * Open a file based clip and record.
 *
 * @class CTestMmfAclntRecFile
 *
 */
class CTestMmfAclntRecFile :  public CTestMmfAclntCodecTest,
							  public MMdaObjectStateChangeObserver,
							  public MMdaAudioPlayerCallback,
							  public MMdaAudioOutputStreamCallback
	{
public:
	static CTestMmfAclntRecFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse);
	static CTestMmfAclntRecFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	// from MMdaAudioPlayerCallback
public:
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

	// from MMdaAudioOutputStreamCallback
public:
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	CTestMmfAclntRecFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse);
	
private:
	TInt iError;
	TBool iCreateFile;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;

/**
 * Open a file based clip and record, forcing a format
 *
 * @class CTestMmfAclntRecFileForceFormat
 *
 */
class CTestMmfAclntRecFileForceFormat :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntRecFileForceFormat* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	static CTestMmfAclntRecFileForceFormat* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();

	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecFileForceFormat(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	
private:
	TMdaClipFormat iFormat;
	TInt iError;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;

/**
 * Open a descriptor and record
 *
 * @class CTestMmfAclntRecDes
 *
 */
class CTestMmfAclntRecDes : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecDes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse) ;
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntRecDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	static CTestMmfAclntRecDes* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
private:
	HBufC8* iAudio;
	TPtr8 iAudioPtr;
	TInt iError;
	TInt iSize;
	TUint iFrameSize;
	TInt iHeaderSize;
	TBool iCreateDes;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;

//------------------------------------------------------------------ 

/**
 * Open a URL  
 *
 * @class CTestMmfAclntRecUrl
 *
 */
class CTestMmfAclntRecUrl : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	static CTestMmfAclntRecUrl* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	static CTestMmfAclntRecUrl* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
private:
	TInt iError;
	TBuf<25> iSectName;			// Section name for retrieving filename
	TBuf<25> iKeyName;			// Key name for retrieving filename
	};


/**
 * Set and enquire position
 *
 * @class CTestMmfAclntRecPosition
 *
 */
class CTestMmfAclntRecPosition : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecPosition(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecPosition* NewL(const TDesC& aTestName, const TDesC& aSectName, const TTimeIntervalMicroSeconds& aPosition, TBool aNegative);

private:
	TTimeIntervalMicroSeconds iPosition;
	//TBool iNegative;
	} ;

/**
 * Enquire duration
 *
 * @class CTestMmfAclntRecDuration
 *
 */
class CTestMmfAclntRecDuration : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecDuration(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecDuration* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	};


/**
 * Set meta-information
 *
 * @class CTestMmfAclntRecSetMeta
 *
 */
class CTestMmfAclntRecSetMeta: public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecSetMeta(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecSetMeta* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	};


/**
 * Set priority
 *
 * @class CTestMmfAclntRecPriority
 *
 */
class CTestMmfAclntRecPriority: public CTestMmfAclntStep, public MCallbackHandlerObserver
	{
public:
	CTestMmfAclntRecPriority(const TDesC& aTestName,const TDesC& aSectName);
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
	virtual void MchoComplete(TInt aID, TInt aError);
	
private:
	CMdaAudioRecorderUtility* iRecUtil[2];
	CStateCallbackHandler* iCallback[2];
	TInt iCallbackCount;
	TInt iFirstCallback;
	TInt iError;
	TBuf<25> iSectName;					// Section name for retrieving filename
	};

/**
 * Set and Enquire balance
 *
 * @class CTestMmfAclntRecBalance
 *
 */
class CTestMmfAclntRecBalance: public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecBalance(const TDesC& aTestName, const TDesC& aSectName, const TInt aBalance, TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecBalance* NewL(const TDesC& aTestName, const TDesC& aSectName, const TInt aBalance, TBool aNegative);
private:
	TVerdict CheckPlaybackBalance(CMdaAudioRecorderUtility* aRecUtil);
	TVerdict CheckRecordBalance(CMdaAudioRecorderUtility* aRecUtil);

private:
	TInt iBalance;
	};


/**
 * Set maximum length of file in bytes and record
 *
 * @class CTestMmfAclntRecLength
 *
 */
class CTestMmfAclntRecLength: public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecLength(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecLength* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	};


/**
 * Query recordable formats
 *
 * @class CTestMmfAclntRecFormats
 *
 */
class CTestMmfAclntRecFormats: public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecFormats(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecFormats* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	};

//
// NEGATIVE TESTS
// 

/** 
 * Record utility - Delete object before record operation has completed.
 *
 * @class CTestMmfAclntRecDelete
 *
 */
class CTestMmfAclntRecDelete : public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecDelete(const TDesC& aTestName, const TDesC& aSectName) ;
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepL();
	// MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	TInt iError;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	} ;


/**
 * test audio volume
 *
 * @class CTestMmfAclntAudioRecordVolume
 *
 */
class CTestMmfAclntAudioRecordVolume : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntAudioRecordVolume(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative,const TInt aVolume);
	static CTestMmfAclntAudioRecordVolume* NewL(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative, const TInt aVolume=-1);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);

private:
	TInt iVolume;
	};


/**
 * test audio ramp
 *
 * @class CTestMmfAclntAudioRecordRamp
 *
 */
class CTestMmfAclntAudioRecordRamp : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntAudioRecordRamp(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative);
	static CTestMmfAclntAudioRecordRamp* NewL(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
private:
	CMdaAudioClipUtility::TState iReportedState;
	};



/**
 * test number of destination channels
 *
 * @class CTestMmfAclntRecordDestChannels
 *
 */
class CTestMmfAclntRecordDestChannels : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecordDestChannels(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aNumberOfChannels,const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse) ;
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntRecordDestChannels* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aNumberOfChannels, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	static CTestMmfAclntRecordDestChannels* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TInt aNumberOfChannels, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
private:
	TInt iNumberOfChannels;
	HBufC8* iAudio;
	TInt iError;
	TInt iSize;
	TInt iFrameSize;
	TInt iHeaderSize;
	TBool iCreateDes;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;

/**
 * test set priority
 *
 * @class CTestMmfAclntRecordSetPriority
 *
 */
class CTestMmfAclntRecordSetPriority : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecordSetPriority(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	static CTestMmfAclntRecordSetPriority* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	};


/**
 * test Destination Sample Rate
 *
 * @class CTestMmfAclntRecordDestSampleRate
 *
 */

class CTestMmfAclntRecordDestSampleRate : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecordDestSampleRate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse) ;
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntRecordDestSampleRate* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	static CTestMmfAclntRecordDestSampleRate* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
private:
	HBufC8* iAudio;
	TInt iError;
	TInt iSize;
	TInt iFrameSize;
	TInt iHeaderSize;
	TBool iCreateDes;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;

/**
 * test Destination Bit Rate
 *
 * @class CTestMmfAclntRecordDestBitRate
 *
 */

class CTestMmfAclntRecordDestBitRate : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecordDestBitRate(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	static CTestMmfAclntRecordDestBitRate* NewL(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	};


/**
 * test Destination Data Type
 *
 * @class CTestMmfAclntRecordDestDataType
 *
 */
class CTestMmfAclntRecordDestDataType : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntRecordDestDataType(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse) ;
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntRecordDestDataType* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	static CTestMmfAclntRecordDestDataType* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
private:
	TFourCC iDataType;
	HBufC8* iAudio;
	TInt iError;
	TInt iSize;
	TInt iFrameSize;
	TInt iHeaderSize;
	TBool iCreateDes;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	};


/**
 * test Destination Format
 *
 * @class CTestMmfAclntRecordDestFormat
 *
 */

class CTestMmfAclntRecordDestFormat : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecordDestFormat(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative,const TUid aFormat);
	static CTestMmfAclntRecordDestFormat* NewL(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative,const TUid aFormat);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
private:
	TUid iFormat;
	};


/**
 * test Audio Device Mode
 *
 * @class CTestMmfAclntRecordAudioDeviceMode
 *
 */

class CTestMmfAclntRecordAudioDeviceMode : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecordAudioDeviceMode(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	static CTestMmfAclntRecordAudioDeviceMode* NewL(const TDesC& aTestName, const TDesC& aSectName,TBool aNegative);
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	};




//
// CMdaAudioInputStream API Integration Tests //
//
const TInt KRecBufferSize = 1024;   // use 1K buffer
const TInt KRecNumBuffer = 50; // buffer to store data read from file
const TInt KRecBufferSglBufSize = KRecBufferSize * 30;

/**
 * timer support
 *
 * @class CCallBackTimer
 *
 */

class CCallBackTimer : public CTimer
	{
public:
	static CCallBackTimer* NewL(TCallBack aCallBack, TPriority aPriority = EPriorityStandard);
private:
	virtual void RunL();
private:
	CCallBackTimer(TCallBack aCallBack, TPriority aPriority);
private:
	TCallBack iCallBack;
	};


/**
 * Base class for the audio Input stream tests
 *
 * @class CTestStepAudInStream
 *
 */
class CTestStepAudInStream : public CTestMmfAclntStep,
							 public MMdaAudioInputStreamCallback
{													           
public:
	static CTestStepAudInStream* NewL();
	virtual ~CTestStepAudInStream();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	virtual void ProcessDataL();
	
	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

protected:
	CTestStepAudInStream();
	void ConstructL();
	virtual void StartReadL();
	virtual void Read1BufL(); //Used for the RecordInit tests

	
protected:
	CMdaAudioInputStream*	iAudInStream;
	RFs						iFs;
	RFile					iFile;
	TBuf8<KRecBufferSize>	iBufferList[KRecNumBuffer];
	TInt					iStartBuf;
	TInt					iEndBuf;
	TInt					iError;

	enum TState { EStopped, ERecording };
	TState					iState;

protected:
	class CWriteBufferActive : public CActive
		{
		friend class CTestStepAudInStream;
	public:
		CWriteBufferActive();
		~CWriteBufferActive();
		virtual void RunL();
		virtual TInt RunError(TInt aError);
		virtual void DoCancel();
		void Write(const TDesC8& /*aBuffer*/);
	public:
		CTestStepAudInStream*	iParent;
		};

protected:
	CWriteBufferActive*		iWriteBufferActive;

friend class CWriteBufferActive;
	};


//Requesting client has MultimediaDD capability or None.
class CTestStepAudInStreamNoUECap : public CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamNoUECap* NewL(const TDesC& aTestName);
	CTestStepAudInStreamNoUECap(const TDesC& aTestName);
	~CTestStepAudInStreamNoUECap();
	virtual TVerdict DoTestStepL();
	
	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/);
	virtual void MaiscRecordComplete(TInt aError);
private:
	TInt iMaiscRecordCompleteError;
	};
	

//Requesting client has UserEnvironment capability or All -TCB capabilities.
class CTestStepAudInStreamWithUECap : public CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamWithUECap* NewL(const TDesC& aTestName);
	CTestStepAudInStreamWithUECap(const TDesC& aTestName);
	~CTestStepAudInStreamWithUECap();
	virtual TVerdict DoTestStepL();
	
	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);
	};	
	

/**
 * Test Gain
 *
 * @class CTestStepAudInStreamSetGain
 *
 */
class CTestStepAudInStreamSetGain :	public	CTestMmfAclntStep,
									public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetGain* NewL(const TDesC& aTestName);
	CTestStepAudInStreamSetGain(const TDesC& aTestName);
	~CTestStepAudInStreamSetGain();
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
	
/**
 * Test Priority
 *
 * @class CTestStepAudInStreamSetPriority
 *
 */
class CTestStepAudInStreamSetPriority :	public	CTestMmfAclntStep,
										public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetPriority* NewL(const TDesC& aTestName);
	CTestStepAudInStreamSetPriority(const TDesC& aTestName);
	~CTestStepAudInStreamSetPriority();
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

/**
 * Test Set/Get Balance
 *
 * @class CTestStepAudInStreamSetBalance
 *
 */
class CTestStepAudInStreamSetBalance :	public	CTestMmfAclntStep,
										public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetBalance* NewL(const TDesC& aTestName);
	CTestStepAudInStreamSetBalance(const TDesC& aTestName);
	~CTestStepAudInStreamSetBalance();
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


/**
 * Test Get Bytes
 *
 * @class CTestStepAudInStreamGetBytes
 *
 */
class CTestStepAudInStreamGetBytes : public	CTestStepAudInStream
	{
public:
	static CTestStepAudInStreamGetBytes* NewL(const TDesC& aTestName);
	CTestStepAudInStreamGetBytes(const TDesC& aTestName);
	~CTestStepAudInStreamGetBytes();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);

protected:
	TInt					iError;
	TUint					iBuffersUsed; //of the buffers given to AudioInputStream how many have been returned with data
	};



// Requesting that we use a single buffer
class CTestStepAudInStreamSglBuf : public CTestStepAudInStreamGetBytes
	{													           
public:
	static CTestStepAudInStreamSglBuf* NewL(const TDesC& aTestName);
	CTestStepAudInStreamSglBuf(const TDesC& aTestName);
	~CTestStepAudInStreamSglBuf();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepL();
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);
	virtual void ProcessDataL();
	
protected:
	CTestStepAudInStreamSglBuf();
	void Read1BufL();

private:	
	TBuf8<KRecBufferSglBufSize>	iSingleBuffer;
	TPtr8						iBufferPtr;
	};



/**
 * Test Set Properties
 *
 * @class CTestStepAudInStreamSetProp
 *
 */
class CTestStepAudInStreamSetProp :	public	CTestMmfAclntStep,
									public	MMdaAudioInputStreamCallback
	{
public:
	static CTestStepAudInStreamSetProp* NewL(const TDesC& aTestName);
	CTestStepAudInStreamSetProp(const TDesC& aTestName);
	~CTestStepAudInStreamSetProp();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioInputStreamCallback
	virtual void MaiscOpenComplete(TInt aError);
	virtual void MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaiscRecordComplete(TInt aError);
private:
	CMdaAudioInputStream*	iAudInStream;
	TInt					iError;
	};

//
//	CMdaAudioOutputStream Integration Tests //
//

const TInt KBufferSize = 4096;	// use 4K buffer on EKA2 due to difference in Sound Driver

const TInt KNumBuffer = 15; // buffer to store data read from file	

/**
 * Base class for the audio Output stream tests
 *
 * @class CTestStepAudOutStream
 *
 */
class CTestStepAudOutStream : public CTestMmfAclntStep,
							  public MMdaAudioOutputStreamCallback
{													           
public:
	static CTestStepAudOutStream* NewL();
	~CTestStepAudOutStream();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
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
	CMdaAudioOutputStream*	iAudOutStream;
	TBuf8<KBufferSize>		iBufferList[KNumBuffer];
	TInt					iStartBuf;
	TInt					iEndBuf;
	TInt					iError;
	TBool					iIsAllDataRead;
	TBool					iIsAllDataWritten;
	RFs						iFs;
	RFile					iFile;

protected:
	class CReadBufferActive : public CActive
		{
		friend class CTestStepAudOutStream;
	public:
		CReadBufferActive();
		~CReadBufferActive();
		virtual void RunL();
		virtual TInt RunError(TInt aError);
		virtual void DoCancel();
		void Read();
	public:
		TBuf8<KBufferSize>		iBuffer;
		CTestStepAudOutStream*	iParent;
		};

protected:
	CReadBufferActive*		iReadBufferActive;

friend class CReadBufferActive;
	};

/**
 * Test Set Properties
 *
 * @class CTestStepAudOutStreamSetProp
 *
 */
class CTestStepAudOutStreamSetProp :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamSetProp* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamSetProp(const TDesC& aTestName);
	virtual enum TVerdict DoTestStepPreambleL(void);
	~CTestStepAudOutStreamSetProp();
	virtual TVerdict DoTestStepL();
	};

/**
 * Test Open Stream
 *
 * @class CTestStepAudOutStreamOpen
 *
 */
class CTestStepAudOutStreamOpen :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamOpen* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamOpen(const TDesC& aTestName);
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepL();
	};


/**
 * Test Set Volume
 *
 * @class CTestStepAudOutStreamSetVol
 *
 */
class CTestStepAudOutStreamSetVol :	public	CTestMmfAclntStep,
									public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamSetVol* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamSetVol(const TDesC& aTestName);
	~CTestStepAudOutStreamSetVol();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	CMdaAudioOutputStream* iAudOutStream;
	TInt iError;
	};


/**
 * Test Set Balance
 *
 * @class CTestStepAudOutStreamSetBalance
 *
 */
class CTestStepAudOutStreamSetBalance :	public	CTestMmfAclntStep,
										public	MMdaAudioOutputStreamCallback
	{
public:
	static CTestStepAudOutStreamSetBalance* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamSetBalance(const TDesC& aTestName);
	~CTestStepAudOutStreamSetBalance();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	CMdaAudioOutputStream* iAudOutStream;
	TInt iError;
	};

/**
 * Test Set Stream Priority
 *
 * @class CTestStepAudOutStreamSetPrior
 *
 */
class CTestStepAudOutStreamSetPrior :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamSetPrior* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamSetPrior(const TDesC& aTestName);
	~CTestStepAudOutStreamSetPrior();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 * Test Stream Stop
 *
 * @class CTestStepAudOutStreamStop
 *
 */
class CTestStepAudOutStreamStop :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamStop* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamStop(const TDesC& aTestName);
	~CTestStepAudOutStreamStop();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 * Test Set Position
 *
 * @class CTestStepAudOutStreamPosition
 *
 */
class CTestStepAudOutStreamPosition :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamPosition* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamPosition(const TDesC& aTestName);
	~CTestStepAudOutStreamPosition();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 * Test Get Bytes
 *
 * @class CTestStepAudOutStreamGetBytes
 *
 */
class CTestStepAudOutStreamGetBytes :	public	CTestStepAudOutStream
	{
public:
	static CTestStepAudOutStreamGetBytes* NewL(const TDesC& aTestName);
	CTestStepAudOutStreamGetBytes(const TDesC& aTestName);
	~CTestStepAudOutStreamGetBytes();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};




/**
 * Base class for tests where no playback controller is available
 *
 * @class CTestMmfAclntRecNoPlayback
 *
 */
class CTestMmfAclntRecNoPlayback :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
	//This will set iError to iStatus from it's RunL and stop the scheduler
	class CTestMmfAclntAOCallback :  public CActive
		{
	public:
		CTestMmfAclntAOCallback(CTestMmfAclntRecNoPlayback* aParentTestStep) : CActive(0), iParentTestStep(aParentTestStep) {	CActiveScheduler::Add(this);}
		void PublicSetActive() {SetActive();}

	private:
		void RunL() {iParentTestStep->iError = iStatus.Int(); CActiveScheduler::Stop();}
		void DoCancel() {iParentTestStep->iError = KErrCancel; CActiveScheduler::Stop();};
		CTestMmfAclntRecNoPlayback* iParentTestStep;
		};
	friend class CTestMmfAclntAOCallback;


public:
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();


	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

protected:
	CTestMmfAclntRecNoPlayback(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);

	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC() = 0;
	virtual TVerdict RecordSuccessfull() = 0;

	//Descriptor tests are slightly different
	virtual TBool IsFileTest() {return ETrue;}

	CMdaAudioRecorderUtility* iRecorder;
	TInt iError;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	TPtrC iFilename;					//Filename of test
	} ;


/**
 * Concrete class for tests where no playback controller is available
 * and the format to use is forced through OpenL()
 *
 * @class CTestMmfAclntRecNoPlaybackForceFormat
 *
 */
class CTestMmfAclntRecNoPlaybackForceFormat :  public CTestMmfAclntRecNoPlayback
	{
public:
	static CTestMmfAclntRecNoPlaybackForceFormat* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	static CTestMmfAclntRecNoPlaybackForceFormat* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);


private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict RecordSuccessfull();
	
protected:
	CTestMmfAclntRecNoPlaybackForceFormat(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	TMdaClipFormat iFormat;
	};


class CTestMmfAclntRecNoPlaybackForceFormatDesc :  public CTestMmfAclntRecNoPlayback
	{
public:
	static CTestMmfAclntRecNoPlaybackForceFormatDesc* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	static CTestMmfAclntRecNoPlaybackForceFormatDesc* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);

	virtual ~CTestMmfAclntRecNoPlaybackForceFormatDesc() {delete iBuf;delete iFileDesc;}

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict RecordSuccessfull();
	virtual TBool IsFileTest() {return EFalse;}

protected:
	CTestMmfAclntRecNoPlaybackForceFormatDesc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat) :
	   CTestMmfAclntRecNoPlayback(aTestName, aSectName, aKeyName), iFormat(aFormat) {iHeapSize = 250000;}


	TMdaClipFormat iFormat;
	HBufC8* iBuf;
	TPtr8* iFileDesc;
	} ;



class CTestMmfAclntRecNoPlaybackFile :  public CTestMmfAclntRecNoPlayback
	{
public:
	static CTestMmfAclntRecNoPlaybackFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);
	static CTestMmfAclntRecNoPlaybackFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);

protected:
	CTestMmfAclntRecNoPlaybackFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName) :
	  CTestMmfAclntRecNoPlayback(aTestName, aSectName, aKeyName) {}

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict RecordSuccessfull();
	} ;



class CTestMmfAclntRecNoPlaybackDesc :  public CTestMmfAclntRecNoPlayback
	{
public:
	static CTestMmfAclntRecNoPlaybackDesc* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);
	static CTestMmfAclntRecNoPlaybackDesc* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);

	virtual ~CTestMmfAclntRecNoPlaybackDesc() {delete iBuf;delete iFileDesc;}

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict RecordSuccessfull();
	virtual TBool IsFileTest() {return EFalse;}

protected:
	CTestMmfAclntRecNoPlaybackDesc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName) :
	  CTestMmfAclntRecNoPlayback(aTestName, aSectName, aKeyName)  {iHeapSize = 250000;}


	HBufC8* iBuf;
	TPtr8* iFileDesc;
	} ;



class CTestMmfAclntRecNoPlaybackFileForceController :  public CTestMmfAclntRecNoPlayback
	{
public:
	static CTestMmfAclntRecNoPlaybackFileForceController* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);
	static CTestMmfAclntRecNoPlaybackFileForceController* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict RecordSuccessfull();

protected:
	CTestMmfAclntRecNoPlaybackFileForceController(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController) :
	  CTestMmfAclntRecNoPlayback(aTestName, aSectName, aKeyName), iController(aController) {}


	const TUid iController;
	} ;



class CTestMmfAclntRecNoPlaybackDescForceController :  public CTestMmfAclntRecNoPlayback
	{
public:
	static CTestMmfAclntRecNoPlaybackDescForceController* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);
	static CTestMmfAclntRecNoPlaybackDescForceController* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);

	virtual ~CTestMmfAclntRecNoPlaybackDescForceController() {delete iBuf;delete iFileDesc;}

protected:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict RecordSuccessfull();
	virtual TBool IsFileTest() {return EFalse;}

protected:
	CTestMmfAclntRecNoPlaybackDescForceController(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController) :
	  CTestMmfAclntRecNoPlayback(aTestName, aSectName, aKeyName), iController(aController)  {iHeapSize = 250000;}


	HBufC8* iBuf;
	TPtr8* iFileDesc;
	const TUid iController;
	} ;





/**
 * Base class for tests where no record controller is available
 *
 * @class CTestMmfAclntRecNoRecord
 *
 */
class CTestMmfAclntRecNoRecord :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
	//This will set iError to iStatus from it's RunL and stop the scheduler
	class CTestMmfAclntAOCallback :  public CActive
		{
	public:
		CTestMmfAclntAOCallback(CTestMmfAclntRecNoRecord* aParentTestStep) : CActive(0), iParentTestStep(aParentTestStep) {	CActiveScheduler::Add(this);}
		void PublicSetActive() {SetActive();}
	private:
		void RunL() {iParentTestStep->iError = iStatus.Int(); CActiveScheduler::Stop();}
		void DoCancel() {};
		CTestMmfAclntRecNoRecord* iParentTestStep;
		};
	friend class CTestMmfAclntAOCallback;

public:
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();


	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

protected:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC() = 0;
	virtual TVerdict PlaySuccessfull() = 0;

	//Descriptor tests are slightly different
	virtual TBool IsFileTest() {return ETrue;}

	CTestMmfAclntRecNoRecord(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);
	
	CMdaAudioRecorderUtility* iRecorder;
	TInt iError;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	TPtrC iFilename;					//Filename of test
	} ;


/**
 * Concrete class for tests where no record controller is available
 * and the format to use is forced through OpenL()
 *
 * @class CTestMmfAclntRecNoRecordForceFormat
 *
 */
class CTestMmfAclntRecNoRecordForceFormat :  public CTestMmfAclntRecNoRecord
	{
public:
	static CTestMmfAclntRecNoRecordForceFormat* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	static CTestMmfAclntRecNoRecordForceFormat* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);


private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict PlaySuccessfull();

protected:
	virtual TVerdict DoTestStepPreambleL() {return CTestMmfAclntRecNoRecord::DoTestStepPreambleL();}

	CTestMmfAclntRecNoRecordForceFormat(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);

	TMdaClipFormat iFormat;
	} ;


class CTestMmfAclntRecNoRecordForceFormatDesc :  public CTestMmfAclntRecNoRecordForceFormat
	{
public:
	static CTestMmfAclntRecNoRecordForceFormatDesc* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);
	static CTestMmfAclntRecNoRecordForceFormatDesc* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat);

	virtual TVerdict DoTestStepPreambleL() {delete iBuf;delete iFileDesc; return CTestMmfAclntRecNoRecordForceFormat::DoTestStepPreambleL();}

private:
	virtual TBool IsFileTest() {return EFalse;}
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict PlaySuccessfull();

protected:
	CTestMmfAclntRecNoRecordForceFormatDesc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TMdaClipFormat& aFormat) :
	   CTestMmfAclntRecNoRecordForceFormat(aTestName, aSectName, aKeyName, aFormat) {iHeapSize = 0x20000;}


	HBufC8* iBuf;
	TPtr8* iFileDesc;
	} ;




class CTestMmfAclntRecNoRecordFile :  public CTestMmfAclntRecNoRecord
	{
public:
	static CTestMmfAclntRecNoRecordFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);
	static CTestMmfAclntRecNoRecordFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);

protected:
	CTestMmfAclntRecNoRecordFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName) :
	  CTestMmfAclntRecNoRecord(aTestName, aSectName, aKeyName) {}

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict PlaySuccessfull();
	} ;



class CTestMmfAclntRecNoRecordDesc :  public CTestMmfAclntRecNoRecord
	{
public:
	static CTestMmfAclntRecNoRecordDesc* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);
	static CTestMmfAclntRecNoRecordDesc* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName);

	virtual ~CTestMmfAclntRecNoRecordDesc() {delete iBuf;delete iFileDesc;}

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict PlaySuccessfull();
	virtual TBool IsFileTest() {return EFalse;}

protected:
	CTestMmfAclntRecNoRecordDesc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName) :
	  CTestMmfAclntRecNoRecord(aTestName, aSectName, aKeyName)  {iHeapSize = 250000;}


	HBufC8* iBuf;
	TPtr8* iFileDesc;
	} ;



class CTestMmfAclntRecNoRecordFileForceController :  public CTestMmfAclntRecNoRecord
	{
public:
	static CTestMmfAclntRecNoRecordFileForceController* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);
	static CTestMmfAclntRecNoRecordFileForceController* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);

private:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict PlaySuccessfull();

protected:
	CTestMmfAclntRecNoRecordFileForceController(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController) :
	  CTestMmfAclntRecNoRecord(aTestName, aSectName, aKeyName), iController(aController) {}

	const TUid iController;
	} ;



class CTestMmfAclntRecNoRecordDescForceController :  public CTestMmfAclntRecNoRecord
	{
public:
	static CTestMmfAclntRecNoRecordDescForceController* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);
	static CTestMmfAclntRecNoRecordDescForceController* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController);

	virtual ~CTestMmfAclntRecNoRecordDescForceController() {delete iBuf;delete iFileDesc;}

protected:
	virtual CMdaAudioRecorderUtility* CMdaAudioRecorderUtilityLC();
	virtual TVerdict PlaySuccessfull();
	virtual TBool IsFileTest() {return EFalse;}

protected:
	CTestMmfAclntRecNoRecordDescForceController(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TUid aController) :
	  CTestMmfAclntRecNoRecord(aTestName, aSectName, aKeyName), iController(aController)  {iHeapSize = 250000;}


	HBufC8* iBuf;
	TPtr8* iFileDesc;
	const TUid iController;
	} ;


const TInt KSampleRate8K = 8000;
const TInt KFourSeconds = 4000000;

/**
 * Open a file based clip and record and then record again (i.e. append)
 *
 * @class CTestMmfAclntRecFileAppend
 *
 */
class CTestMmfAclntRecFileAppend :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	enum TFormatType
		{
		EWavFormat,
		EAuFormat
		};
	
	enum TCodecType
		{
		EAdpcmCodec,
		EPcmCodec
		};
public:
	static CTestMmfAclntRecFileAppend* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType, TBool aDiffSampleRate = EFalse);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();

	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecFileAppend(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType, TBool aDiffSampleRate = EFalse);
	
private:
	TFormatType iFormatType;
	TCodecType iCodecType;

	TBool iDiffSampleRate;

	TInt iError;
	TInt iCurrentState;

	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;


/**
 * INC038043
 * Open a file based clip and record and then play it 
 * specifying different sample rate / number of channels
 *
 * @class CTestMmfAclntRecFilePlay
 *
 */
class CTestMmfAclntRecFilePlay :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	enum TFormatType
		{
		EWavFormat,
		EAuFormat
		};
	
	enum TCodecType
		{
		EAdpcmCodec,
		EPcmCodec
		};
public:
	static CTestMmfAclntRecFilePlay* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();

	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecFilePlay(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType);
	
private:
	TFormatType iFormatType;
	TCodecType iCodecType;

	TInt iError;
	TInt iCurrentState;

	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;


/**
 * DEF039893
 * Gets balance prior to setting balance
 * @class CTestMmfAclntRecord
 */
class CTestMmfAclntRecordGetBalance : public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntRecordGetBalance* NewL(const TDesC& aTestName);	
	virtual TVerdict DoTestStepL();

	//MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);
private:
	CTestMmfAclntRecordGetBalance(const TDesC& aTestName);
	TInt iError;
	CBase* iObject; 
	TInt iPreviousState; 
	TInt iCurrentState;
	};


/**
 * DEF039308  No integration test to test playback of recorded raw file 
 * Open a RAW file based clip and record and then play it 
 * specifying different sample rate / number of channels
 *
 * @class CTestMmfAclntRecRawFilePlay
 *
 */
class CTestMmfAclntRecRawFilePlay :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	enum TFormatType
		{
		ERawFormat
		};
	
	enum TCodecType
		{
		EAdpcmCodec,
		EPcmCodec,
		EAlawCodec
		};
public:
	static CTestMmfAclntRecRawFilePlay* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();

	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecRawFilePlay(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TFormatType aFormatType, TCodecType aCodecType);
	
private:
	TFormatType iFormatType;
	TCodecType iCodecType;

	TInt iError;
	TInt iCurrentState;

	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;


/**
 * REQ2870
 * Sierra: AudioRecorderUtility to get AudioBitrare 
 * @class CTestMmfAclntRecSrcAuBitrate
 */

class CTestMmfAclntRecSrcAuBitrate : public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntRecSrcAuBitrate* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName1, const TDesC& aKeyName2);	
	virtual TVerdict DoTestStepL();
	//MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecSrcAuBitrate(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName1, const TDesC& aKeyName2);
	TInt iError;
	CBase* iObject; 
	TInt iPreviousState; 
	TInt iCurrentState;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyFileName;	// Key name for retrieving filename
	TBuf<KNameBufSize> iKeyBitRateName; // Key name for retrieving bit rate
	TInt iExpectedBitRate;
	} ;

/**
 * REQ2870
 * Sierra: AudioRecorderUtility to get AudioBitrare from a recorded file
 * @class CTestMmfAclntRecBitrateRecordedFile
 */
class CTestMmfAclntRecBitrateRecordedFile :  public CTestMmfAclntCodecTest,
											 public MMdaObjectStateChangeObserver
											
	{
public:
	static CTestMmfAclntRecBitrateRecordedFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecBitrateRecordedFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse);
	
private:
	TInt iError;
	TBool iCreateFile;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	} ;
	
/**
 * Record-Stop-Record
 * Added as part of CR1566
 * CTestMmfAclntRecStopRec
 *
 */
class CTestMmfAclntRecStopRec : public CTestMmfAclntRecord
	{
public:
	CTestMmfAclntRecStopRec(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioRecorderUtility* aRecUtil);
	static CTestMmfAclntRecStopRec* NewL(const TDesC& aTestName, const TDesC& aSectName, TBool aNegative);
	TVerdict DoTestStepL();

private:
	CMdaAudioClipUtility::TState iReportedState;
	} ;	

#endif //TESTRECORDER_H
