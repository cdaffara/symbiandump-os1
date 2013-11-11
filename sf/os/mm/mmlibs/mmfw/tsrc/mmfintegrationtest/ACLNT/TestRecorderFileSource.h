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
// The tests are for opening the audio file with TMMFileSource/TMMFileHandleSource
// and recording.
// Header file: Basic tests.
// 
//

/**
 @file TestRecorderFileSource.h
*/

#ifndef __TESTRECORDERFILESOURCE_H__
#define __TESTRECORDERFILESOURCE_H__

#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include "TSI_MMFACLNT.h"

enum TOpenFileType
{
	EFileName = 1,
	EFileHandle
};

/**
 * Open a file based clip and record.
 *
 * @class CTestMmfAclntRecFileSource
 *
 */
class CTestMmfAclntRecFileSource :  public CTestMmfAclntCodecTest,
									public MMdaObjectStateChangeObserver,
									public MMdaAudioPlayerCallback,
									public MMdaAudioOutputStreamCallback
	{
public:
	static CTestMmfAclntRecFileSource* NewL(const TDesC& aTestName, 
        const TDesC& aSectName, const TDesC& aKeyName, TOpenFileType aOpenFileType, 
        const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse);

    static CTestMmfAclntRecFileSource* NewLC(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, TOpenFileType aOpenFileType, const TTestFormat aFormat = ENone, 
        const TBool aCreateFile = EFalse);

    virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);

	// from MMdaAudioOutputStreamCallback
	virtual void MaoscOpenComplete(TInt aError);
	virtual void MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void MaoscPlayComplete(TInt aError);

private:
	CTestMmfAclntRecFileSource(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, TOpenFileType aOpenFileType, const TTestFormat aFormat = ENone, 
        const TBool aCreateFile = EFalse);
	
private:
	TInt iError;
	TBool iCreateFile;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	TOpenFileType iOpenFileType;		// How to open the file?
	};


/**
 * Open a file based clip and record and then play it 
 * specifying different sample rate / number of channels
 *
 * @class CTestMmfAclntRecPlayFileSource
 *
 */
class CTestMmfAclntRecPlayFileSource :  public CTestMmfAclntCodecTest, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntRecPlayFileSource* NewL(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, const TTestFormat aFormat, TOpenFileType aOpenFileType);

    virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntRecPlayFileSource(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, 
        const TTestFormat aFormat, TOpenFileType aOpenFileType);
	
private:
	TInt iError;
	TInt iCurrentState;
	TBuf<25> iSectName;					// Section name for retrieving filename
	TBuf<25> iKeyName;					// Key name for retrieving filename
	TOpenFileType iOpenFileType;		// How to open the file?
	};

#endif //__TESTRECORDERFILESOURCE_H__

