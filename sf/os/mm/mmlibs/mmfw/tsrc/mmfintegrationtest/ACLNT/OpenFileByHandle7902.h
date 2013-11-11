
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
// Header file: Basic  tests.
// 
//

/**
 @file OpenFileByHandle7902.h
*/

#ifndef OPENFILEBYHANDLE7902_H__
#define OPENFILEBYHANDLE7902_H__

#include "TestRecorder.h"
#include "TSI_MMFACLNT.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7902
 * 
 */

class CTestMmfAclntOpenFile7902 :  public CTestMmfAclntCodecTest,
								   public MMdaObjectStateChangeObserver,
								   public MMdaAudioPlayerCallback,
								   public MMdaAudioOutputStreamCallback
	{
public:
	static CTestMmfAclntOpenFile7902* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse );
	static CTestMmfAclntOpenFile7902* NewLC( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse );
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();
	virtual void MoscoStateChangeEvent( CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode );

	// from MMdaAudioPlayerCallback
public:
	virtual void MapcInitComplete( TInt aError, const TTimeIntervalMicroSeconds& aDuration );
	virtual void MapcPlayComplete( TInt aError );

	// from MMdaAudioOutputStreamCallback
public:
	virtual void MaoscOpenComplete( TInt aError );
	virtual void MaoscBufferCopied( TInt aError, const TDesC8& aBuffer );
	virtual void MaoscPlayComplete( TInt aError );

protected:
	CTestMmfAclntOpenFile7902( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse );

	TVerdict PerformTestStepL();
	
	TInt iError;
	TBool iCreateFile;
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	} ;


#endif	// OPENFILEBYHANDLE7902_H__
