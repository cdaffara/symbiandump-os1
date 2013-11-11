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
// TestStepPlayerAllocFailNew.cpp
// This file contains testing steps for the Unit
// test of the MMF 'Deprecated' Audio Clients.
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//


#ifndef __TEST_STEP_PLAYER_ALLOCFAIL_NEW__
#define __TEST_STEP_PLAYER_ALLOCFAIL_NEW__

 
/**
 *
 * Alloc Failure Open sound specifying the filename
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerAllocFailNew :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailNew* NewL();
	CTestStepPlayerAllocFailNew();
	~CTestStepPlayerAllocFailNew();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};


/**
 *
 * Alloc Failure Open sound specifying the filename
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerAllocFailNewFilePlayer :	public	CTestStepUnitMMFAudClient,
												public  MMdaAudioPlayerCallback	
	{
public:
	static CTestStepPlayerAllocFailNewFilePlayer* NewL();
	CTestStepPlayerAllocFailNewFilePlayer();
	~CTestStepPlayerAllocFailNewFilePlayer();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	void StartNewFilePlayerSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * Alloc Fail Open sound specifying a Descriptor
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerAllocFailNewDescPlayer :	public	CTestStepUnitMMFAudClient,
												public  MMdaAudioPlayerCallback	
	{
public:
	static CTestStepPlayerAllocFailNewDescPlayer* NewL();
	CTestStepPlayerAllocFailNewDescPlayer();
	~CTestStepPlayerAllocFailNewDescPlayer();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	HBufC8* iBuf;
	};

class CTestStepPlayerAllocFailPlay :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailPlay* NewL();
	CTestStepPlayerAllocFailPlay();
	~CTestStepPlayerAllocFailPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	void PlayAndStartSchedulerL();

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TBool iHasPlayed;
	};

/**
 *
 * Alloc Failure Open sound specifying the priority preference
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepPlayerAllocFailNewPriorPrefer :	public	CTestStepUnitMMFAudClient,
												public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailNewPriorPrefer* NewL();
	CTestStepPlayerAllocFailNewPriorPrefer();
	~CTestStepPlayerAllocFailNewPriorPrefer();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	TInt iTestCase;
	CMdaAudioPlayerUtility* iPlayer;
	};
*/
/**
 *
 * Alloc Failure Open sound specifying the preference
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
/*
class CTestStepPlayerAllocFailNewPrefer :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerAllocFailNewPrefer* NewL();
	CTestStepPlayerAllocFailNewPrefer();
	~CTestStepPlayerAllocFailNewPrefer();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMdaAudioPlayerUtility* iPlayer;
	};
*/
/**
 *
 * Alloc Failure Play sound
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

#endif //(__TEST_STEP_PLAYER_ALLOCFAIL_NEW__)
