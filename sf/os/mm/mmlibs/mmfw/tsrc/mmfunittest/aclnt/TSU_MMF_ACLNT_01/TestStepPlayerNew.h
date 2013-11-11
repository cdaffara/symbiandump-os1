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
// TestStepPlayerNew.cpp
// This file contains testing steps for the Unit
// test of the MMF 'Deprecated' Audio Clients.
// Tests opening sounds of various formats
// from a file with various parameters and constructor calls
// 
//


#ifndef __TEST_STEP_PLAYER_NEW__
#define __TEST_STEP_PLAYER_NEW__

 
/**
 *
 * Open sound specifying the filename
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerNew :	public	CTestStepUnitMMFAudClient,
							public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerNew* NewL();
	CTestStepPlayerNew();
	~CTestStepPlayerNew();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	};


/**
 *
 * Open sound specifying the filename
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerNewFilePlayer :	public	CTestStepUnitMMFAudClient,
										public  MMdaAudioPlayerCallback	
	{
public:
	static CTestStepPlayerNewFilePlayer* NewL();
	CTestStepPlayerNewFilePlayer();
	~CTestStepPlayerNewFilePlayer();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	};

/**
 *
 * Open sound specifying a Descriptor
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerNewDescPlayer :	public	CTestStepUnitMMFAudClient,
										public  MMdaAudioPlayerCallback	
	{
public:
	static CTestStepPlayerNewDescPlayer* NewL();
	CTestStepPlayerNewDescPlayer();
	~CTestStepPlayerNewDescPlayer();
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

/**
 *
 * Open sound specifying the priority preference
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepPlayerNewPriorPrefer :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerNewPriorPrefer* NewL();
	CTestStepPlayerNewPriorPrefer();
	~CTestStepPlayerNewPriorPrefer();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPostambleL(void);

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	TInt iTestCase;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * Open sound specifying the preference
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepPlayerNewPrefer :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerNewPrefer* NewL();
	CTestStepPlayerNewPrefer();
	~CTestStepPlayerNewPrefer();
	virtual TVerdict DoTestStepL();
	virtual enum TVerdict DoTestStepPostambleL(void);

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
 *
 * Play sound
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepPlayerPlay :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlay* NewL();
	CTestStepPlayerPlay();
	~CTestStepPlayerPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TBool iHasPlayed;
	};

/**
 *
 * Play sound twice
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPlayTwice : public CTestStepUnitMMFAudClient,
								 public MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayTwice* NewL();
	CTestStepPlayerPlayTwice();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);
private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TBool iHasPlayed;
	};

/**
 *
 * Play sound three times
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerPlayThreeTimes : public CTestStepUnitMMFAudClient, public MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayThreeTimes* NewL();
	CTestStepPlayerPlayThreeTimes();
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/);
	virtual void MapcPlayComplete(TInt aError);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
protected:
	static TInt PlayThreeTimesCallBack(TAny* aPtr);
	void DoPlayThreeTimesCallBack();
protected:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TBool iHasPlayed;
	CPeriodic* iTimer;
	};

class CTestStepPlayerPlayAu :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayAu* NewL();
	CTestStepPlayerPlayAu();
	~CTestStepPlayerPlayAu();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	TBool iHasPlayed;
	};

/**
 *
 * Play, Close and Play sound
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepPlayerClosePlay : public	CTestStepUnitMMFAudClient,
								 public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerClosePlay* NewL();
	CTestStepPlayerClosePlay();
	~CTestStepPlayerClosePlay();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, 
								  const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	};


/**
 *
 * Play the .wav IMAADPCM Formats
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */

class CTestStepPlayerPlayWav :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerPlayWav* NewL();
	CTestStepPlayerPlayWav();
	~CTestStepPlayerPlayWav();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	TBool iHasPlayed;
	};

/**
 *
 * Test introduced to check the Fix for DEF056298.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */


class CTestStepCheckHeaderTag :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepCheckHeaderTag* NewL();
	CTestStepCheckHeaderTag();
	~CTestStepCheckHeaderTag();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
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
 * Test introduced to check the Fix for INC063833.
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepPlayerFileHandle :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioPlayerCallback
	{
public:
	static CTestStepPlayerFileHandle* NewL();
	CTestStepPlayerFileHandle();
	~CTestStepPlayerFileHandle();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioPlayerUtility* iPlayer;
	};

/**
  PDEF104196:V&V_PATOS_3_W950i_error when audio file is finished 	
 */ 
class RAudioPlayCompleteTest : public RAsyncTestStep, public MMdaAudioPlayerCallback
	{
public:
	static RAudioPlayCompleteTest* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	// from MMdaAudioPlayerCallback
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);	
protected:
	RAudioPlayCompleteTest(const TDesC& aStepName, const TDesC& aFileName);
	TFileName iFileName;
	CMdaAudioPlayerUtility* iUtility;
	TTimeIntervalMicroSeconds iDuration;
	TTime iPlayStartTime; 
	};
	

/**
 *PDEF131534: SPPR_PERF: Music player crashes when opening corrupted wav file 
 *Open a corrupt wav file , which has sampling rate 0xFFFFFFFF 	
 */ 
class RAudioOpenCorrupt : public RAsyncTestStep, public MMdaAudioPlayerCallback
	{
public:
	static RAudioOpenCorrupt* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	// from MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);	
private:
	RAudioOpenCorrupt(const TDesC& aStepName, const TDesC& aFileName);
	TFileName iFileName;
	CMdaAudioPlayerUtility* iUtility;
	};

/**
  DEF127630: Fix for INC108007 appears to close client down prematurely  
 */ 
class RAudioPlayPanicTest : public RAudioPlayCompleteTest
	{
public:
	static RAudioPlayPanicTest* NewL(const TDesC& aStepName, const TDesC& aFileName);
	virtual void KickoffTestL();
	void MapcPlayComplete(TInt aError);	
protected:
	RAudioPlayPanicTest(const TDesC& aStepName, const TDesC& aFileName);
	};
#endif //(__TEST_STEP_PLAYER_NEW__)
