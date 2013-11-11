
// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Play record tests that use CMMFCodecs rather than CMMFHwDevices
// DEF039267
// 
//

/**
 @file TestOldCodec.h
*/

#ifndef TESTOLDCODEC_H__
#define TESTOLDCODEC_H__


#include "TestPlayerUtils.h"
#include "UseOldCodecAudioController/TestUseOldCodecAudioControllerUIDs.hrh"


/**
 *  Play and record an audio file using the UseOldCodecAudioController
 *  which uses CMMFCodecs and the 'null' pcm16->pcm16 hw device plugin an audio file.
 *
 * @class CTestMmfAclntOldCodecFile
 *
 */         
class CTestMmfAclntOldCodecFile :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntOldCodecFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay,const TInt aExpectedError);
	static CTestMmfAclntOldCodecFile* NewLC(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError);
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
	virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);
private:
	CTestMmfAclntOldCodecFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay, const TInt aExpectedError);	
private:
	TInt iError;
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	const TBool iPlay;					// Does this test require playing the audio
	const TInt	iExpectedError;			// Is this a negative test
	};

#endif
