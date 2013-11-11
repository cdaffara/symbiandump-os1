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


#ifndef __TEST_STEP_PLAYER_ALLOCFAIL_OPEN__
#define __TEST_STEP_PLAYER_ALLOCFAIL_OPEN__

#include "mmfclientaudioplayer.h"

/**
 *
 * Alloc Failure Open sound specifying the filename
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerAllocFailOpenFile :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailOpenFile* NewL(TBool aUseFileHandle);
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aErrorcCode, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aErrorcCode);

private:
	CMdaAudioPlayerUtility* iPlayer;
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
class CTestStepPlayerAllocFailOpenDesc :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailOpenDesc* NewL();
	CTestStepPlayerAllocFailOpenDesc();
	~CTestStepPlayerAllocFailOpenDesc();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	void OpenDescStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	HBufC8* iBuf;
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
class CTestStepPlayerAllocFailOpenUrl :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailOpenUrl* NewL();
	CTestStepPlayerAllocFailOpenUrl();
	~CTestStepPlayerAllocFailOpenUrl();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};
#endif //(__TEST_STEP_PLAYER_ALLOCFAIL_OPEN__)
