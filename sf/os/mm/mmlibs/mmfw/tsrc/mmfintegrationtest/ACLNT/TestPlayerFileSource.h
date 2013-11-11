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
// and playing.
// Header file: Basic tests.
// 
//

/**
 @file TestPlayerFileSource.h
*/

#ifndef __TESTPLAYERFILESOURCE_H__
#define __TESTPLAYERFILESOURCE_H__


#include "TSI_MMFACLNT.h"

//
// CTestMmfAclntFileSource
//

/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntFileSource
 *
 */
class CTestMmfAclntFileSource :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntFileSource* NewL(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, const TBool aPlay,const TInt aExpectedError);

    static CTestMmfAclntFileSource* NewLC(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, const TBool aPlay, const TInt aExpectedError);

    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntFileSource(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError);

private:
	TInt iError;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	const TBool iPlay;					// Does this test require playing the audio
	const TInt	iExpectedError;			// Is this a negative test
	};

//
// CTestMmfAclntFileHandleSource
//

/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntFileHandleSource
 *
 */
class CTestMmfAclntFileHandleSource :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntFileHandleSource* NewL(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, const TBool aPlay, const TInt aExpectedError);

    static CTestMmfAclntFileHandleSource* NewLC(const TDesC& aTestName, const TDesC& aSectName,
        const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError);

    virtual TVerdict DoTestStepL();

    // from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntFileHandleSource(const TDesC& aTestName, const TDesC& aSectName, 
        const TDesC& aKeyName, const TBool aPlay, const TInt aExpectedError);

private:
	TInt iError;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	const TBool iPlay;					// Does this test require playing the audio
	const TInt	iExpectedError;			// Is this a negative test
	};

#endif
