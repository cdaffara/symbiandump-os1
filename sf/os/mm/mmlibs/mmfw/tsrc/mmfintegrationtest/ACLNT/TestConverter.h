
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
 @file TestConverter.h
*/


#ifndef TESTCONVERTER_H__
#define TESTCONVERTER_H__

#include "TSI_MMFACLNT.h"

//Original duration = 8359176 Us with 92160 bytes of 8bit per sample data + 44 header
//Window start = 2089794 Us, aligned to a frame = 1857596 Us
//Window end = 6269382 Us, aligned to a frame =6315828 Us
//Therefore window = 4179588, aligned to a frames =4458231 Us

//New file = 4458231/8359176 * 92160 * 16bits(2) + 44 = 98348
const TInt KConverterWindowFileSize = 98348;


//Add to KConverterWindowFileSize a full convert of the source file:-
//=92160 * 16bits(2) + KConverterWindowFileSize
const TInt KConverterWindowPlusNormalFileSize = 282668;


/**
 * Set config destination clip
 *
 * @class CTestMmfAclntConConfig
 *
 */
class CTestMmfAclntConConfig : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver 
	{
public:
	CTestMmfAclntConConfig(const TDesC& aTestName, const TTestFormat aFormat, const TBool aNegative);
	static CTestMmfAclntConConfig* NewL(const TDesC& aTestName, const TTestFormat aFormat, const TBool aNegative = EFalse);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	virtual TVerdict DoTestStepPreambleL();
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

protected:
	virtual void GetKeyName(TDes& aDes);
protected:
	TInt iError;
	TBool iThisIsNegativeTest;
	};


/**
 * Convert WAV to AU.
 *
 * @class CTestMmfAclntDoConv
 *
 */
class CTestMmfAclntDoConv : public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	enum FormatSpec
	{
	MdaFormatSpec,
	MmfFormatSpec
	};



	CTestMmfAclntDoConv(const TDesC& aTestName, const TDesC& aSectName, 
		const TDesC& aKeyName, const TDesC& aSectName2, 
		const TDesC& aKeyName2, 
		TInt	aBits, FormatSpec aFormat);
	
	//aBits 0  = TMdaPcmWavCodec::E8BitPcm
	//		1  = TMdaPcmWavCodec::E16BitPcm
	//		-1  - UnsupportedCodec (negative test)
	static CTestMmfAclntDoConv* NewL(const TDesC& aTestName, 
		const TDesC& aSectName, const TDesC& aKeyName, 
		const TDesC& aSectName2, const TDesC& aKeyName2, 
		TInt aBits, FormatSpec aFormat);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

protected:
	TInt iError;
	CBase* iObject;
	TInt iPreviousState;
	TInt iCurrentState;
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	TBuf<KNameBufSize> iSectName2;
	TBuf<KNameBufSize> iKeyName2;

	FormatSpec iFormat;
	TInt	iBits;
	};


/**
 * Closes an open file then opens a second file
 *
 * @class CTestMmfAclntConCloseOpen
 *
 */
class CTestMmfAclntConCloseOpen: public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConCloseOpen(const TDesC& aTestName) ;
	~CTestMmfAclntConCloseOpen(){} ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	static CTestMmfAclntConCloseOpen* NewL(const TDesC& aTestName);
	};

/**
 * Set convert window. Enquire window. Delete window
 *
 * @class CTestMmfAclntConWindow
 *
 */
class CTestMmfAclntConWindow: public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConWindow(const TDesC& aTestName) ;
	~CTestMmfAclntConWindow(){} ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	static CTestMmfAclntConWindow* NewL(const TDesC& aTestName);
protected:
	TVerdict CheckFileSize(const TDesC& aFilename, const TInt aExpectedSize);
	};


/**
 * Enquire duration
 *
 * @class CTestMmfAclntConDuration
 *
 */
class CTestMmfAclntConDuration: public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConDuration(const TDesC& aTestName) ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, 
			TInt aCurrentState, TInt aErrorCode) ;
	static CTestMmfAclntConDuration* NewL(const TDesC& aTestName);
protected:
	virtual void GetKeyName(TDes& aDes);
	};

/**
 * Set position and enquire position
 *
 * @class CTestMmfAclntConPosition
 *
 */
class CTestMmfAclntConPosition: public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConPosition(const TDesC& aTestName, const TTimeIntervalMicroSeconds aPosition);
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	static CTestMmfAclntConPosition* NewL(const TDesC& aTestName, const	TTimeIntervalMicroSeconds aPosition);

protected:
	void GetKeyName(TDes& aDes);
private:
	TTimeIntervalMicroSeconds iPosition;
	};


/**
 * Set config destination clip
 *
 * @class CTestMmfAclntConQuery
 *
 */
class CTestMmfAclntConQuery: public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConQuery(const TDesC& aTestName);
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	static CTestMmfAclntConQuery* NewL(const TDesC& aTestName);
	TVerdict DoTestStepL();
protected:
	void GetKeyName(TDes& aDes);
	};

//
// NEGATIVE TESTS
//

/** 
 * Conversion utility - Delete object before conversion operation has completed.
 *
 * @class CTestMmfAclntDelete
 *
 */
class CTestMmfAclntDelete : public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	CTestMmfAclntDelete() ;
	virtual TVerdict DoTestStepL();
	// MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	TInt iError;	
	};


/** 
 * Open new file while audio converter is converting. Open a new audio file.
 *
 * @class CTestMmfAclntOpenWhileConvert
 *
 */
class CTestMmfAclntOpenWhileConvert : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntOpenWhileConvert(const TDesC& aTestName) ;
	static CTestMmfAclntOpenWhileConvert* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	} ;
/**
 * @class CTestMmfAclntConvertDestBitRate
 */

class CTestMmfAclntConvertDestBitRate : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertDestBitRate(const TDesC& aTestName, TBool aNegative) ;
        static	CTestMmfAclntConvertDestBitRate* NewL(const TDesC& aTestName, TBool aNegative) ;
	TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);

protected:
	void GetKeyName(TDes& aDes);
	};


/**
 * @class CTestMmfAclntConvertDestDataType
 */
class CTestMmfAclntConvertDestDataType : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertDestDataType(const TDesC& aTestName, TBool aNegative);
        static CTestMmfAclntConvertDestDataType* NewL(const TDesC& aTestName, TBool aNegative);
	TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
protected:
	void GetKeyName(TDes& aDes);
private:
	TFourCC iDataType ;
	} ;


/**
 * @class CTestMmfAclntConvertDestFormat
 */
class CTestMmfAclntConvertDestFormat : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertDestFormat(const TDesC& aTestName, TBool aNegative);
	static CTestMmfAclntConvertDestFormat* NewL(const TDesC& aTestName, TBool aNegative);
	TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
protected:
	void GetKeyName(TDes& aDes);
	};


/**
 * @class CTestMmfAclntConvertDestSampleRate
 */
class CTestMmfAclntConvertDestSampleRate : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertDestSampleRate(const TDesC& aTestName, TBool aNegative);
	static CTestMmfAclntConvertDestSampleRate* NewL(const TDesC& aTestName, TBool aNegative);
	TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	TVerdict DoTestStepL() ;

protected:
	void GetKeyName(TDes& aDes);
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState,	TInt aCurrentState,
								TInt aErrorCode) ;
	};

/**
 * @class CTestMmfAclntConvertDestNumberOfChannels
 */
class CTestMmfAclntConvertDestNumberOfChannels : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertDestNumberOfChannels(const TDesC& aTestName, TBool aNegative);
	static CTestMmfAclntConvertDestNumberOfChannels* NewL(const TDesC& aTestName, TBool aNegative);
	TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
protected:
	void GetKeyName(TDes& aDes);
	};

/**
 * @class CTestMmfAclntConvertSourceInfo
 */
class CTestMmfAclntConvertSourceInfo : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertSourceInfo(const TDesC& aTestName, TBool aNegative);
	static CTestMmfAclntConvertSourceInfo* NewL(const TDesC& aTestName, TBool aNegative);
	TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
protected:
	void GetKeyName(TDes& aDes);
	};
/**
 * Audio convert croping.
 *
 * @class CTestMmfAclntConvertCrop
 *
 */
class CTestMmfAclntConvertCrop : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertCrop(const TDesC& aTestName, const TBool aCropToEnd, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	static CTestMmfAclntConvertCrop* NewL(const TDesC& aTestName, const TBool aCropToEnd, TBool aNegative);
	virtual TVerdict DoTestStepL() ;
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
private:
	TBool iCropToEnd;
	TPtrC iToFilename;
	TPtrC iToFilename2;
	} ;


/**
 * Set maximum length of file in bytes and convert
 *
 * @class CTestMmfAclntConvertLength
 *
 */
class CTestMmfAclntConvertLength: public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertLength(const TDesC& aTestName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConvertUtil);
	static CTestMmfAclntConvertLength* NewL(const TDesC& aTestName, TBool aNegative);
	};

/**
 * Set repeats
 *
 * @class CTestMmfAclntConvertRepeat
 *
 */
class CTestMmfAclntConvertRepeat : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertRepeat(const TDesC& aTestName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConvertUtil);
	static CTestMmfAclntConvertRepeat* NewL(const TDesC& aTestName, TBool aNegative);
	};

/**
 * Audio convert Stoping.
 *
 * @class CTestMmfAclntConvertStop
 *
 */
class CTestMmfAclntConvertStop : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertStop(const TDesC& aTestName, TBool aNegative) ;
	virtual TVerdict DoTestL(CMdaAudioConvertUtility* aConverter);
	static CTestMmfAclntConvertStop* NewL(const TDesC& aTestName, TBool aNegative);
	TVerdict DoTestStepL() ;
	// from MMdaObjectStateChangeObserver
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	} ;

/**
 * Open a descriptor and convert
 *
 * @class CTestMmfAclntConvertDes
 *
 */
class CTestMmfAclntConvertDes : public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver, public MMdaAudioPlayerCallback	
	{
public:
	CTestMmfAclntConvertDes(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse) ;
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntConvertDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	static CTestMmfAclntConvertDes* NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateDes = EFalse);
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	HBufC8* iAudio;
	TInt iError;
	TInt iSize;
	TInt iFrameSize;
	TInt iHeaderSize;
	TBool iCreateDes;
	TBuf<32> iSectName;					// Section name for retrieving filename
	TBuf<32> iKeyName;					// Key name for retrieving filename
	TUint iDataLengthSource;			// source data length - not including header
	TUint iSamples;
	TUint iChannels;
	TUint iFileSizeSink;				// expected file size including header
	TTimeIntervalMicroSeconds iDuration;

	} ;

/**
 * Set Urls
 *
 * @class CTestMmfAclntConvertUrl
 *
 */
class CTestMmfAclntConvertUrl : public CTestMmfAclntConConfig
	{
public:
	CTestMmfAclntConvertUrl(const TDesC& aTestName, TBool aNegative) ;
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntConvertUrl* NewL(const TDesC& aTestName, TBool aNegative);
	};

#endif
