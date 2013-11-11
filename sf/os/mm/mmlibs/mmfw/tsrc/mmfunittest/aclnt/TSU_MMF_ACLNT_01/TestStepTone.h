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


#ifndef __TEST_STEP_TONE__
#define __TEST_STEP_TONE__

 
const TInt KHeapSizeToneTestEKA2 = 128000; // Heapsize for tone tests on EKA2
const TInt KTestTimeout = 2000000; // Currently used only by CTestStepTonePlayDualToneInvalidFreq
 
/**
 *
 * Alloc Failure New Tone utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneAllocFailNew :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver //,
									//public	MDevSoundObserver
	{
public:
	static CTestStepToneAllocFailNew* NewL();
	CTestStepToneAllocFailNew();
	~CTestStepToneAllocFailNew();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
	// from MMdaAudioToneObserver??
	//virtual void DevSoundInitFinished(TInt aError);
	//virtual void ToneFinished(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Test New Tone utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneNew :	public	CTestStepUnitMMFAudClient,
							public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneNew* NewL();
	CTestStepToneNew();
	~CTestStepToneNew();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Test New Tone utility with various Priority and Preference params
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneNewPriorPref :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneNewPriorPref* NewL();
	CTestStepToneNewPriorPref();
	~CTestStepToneNewPriorPref();
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Tone utility, Prepare to play
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayTone :	public	CTestStepUnitMMFAudClient,
										public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayTone* NewL();
	CTestStepTonePrepareToPlayTone();
	~CTestStepTonePrepareToPlayTone();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Test Tone Prepare DTMF String 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayDTMFString :	public	CTestStepUnitMMFAudClient,
												public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayDTMFString* NewL();
	CTestStepTonePrepareToPlayDTMFString();
	~CTestStepTonePrepareToPlayDTMFString();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	TBool iPlayComplete;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Test Tone Prepare File Sequence 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayFileSeq :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayFileSeq* NewL();
	CTestStepTonePrepareToPlayFileSeq();
	~CTestStepTonePrepareToPlayFileSeq();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

protected:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Test Tone Prepare DRM File Sequence 
 * (INC087436)
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayDRMFileSeq :	public	CTestStepTonePrepareToPlayFileSeq
	{
public:
	static CTestStepTonePrepareToPlayDRMFileSeq* NewL();
	CTestStepTonePrepareToPlayDRMFileSeq();
	~CTestStepTonePrepareToPlayDRMFileSeq();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	};

/**
 *
 * Test Tone Prepare Descriptor Sequence 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayDescSeq :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayDescSeq* NewL();
	CTestStepTonePrepareToPlayDescSeq();
	~CTestStepTonePrepareToPlayDescSeq();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Test Tone Prepare Fixed Sequence 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayFixedSeq :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayFixedSeq* NewL();
	CTestStepTonePrepareToPlayFixedSeq();
	~CTestStepTonePrepareToPlayFixedSeq();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Alloc Failure New Tone utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayTonePlay :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayTonePlay* NewL();
	CTestStepTonePrepareToPlayTonePlay();
	~CTestStepTonePrepareToPlayTonePlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	TBool iHasPlayed;
	};

/**
 * class used to encapsulate information about a dual tone 
 */
class TToneInfo
	{
public:
	TInt iFreq1;
	TInt iFreq2;
	TInt iDuration;
	};

class CTestStepTonePrepareToPlayDualTonePlay :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayDualTonePlay* NewL();
	CTestStepTonePrepareToPlayDualTonePlay();
	~CTestStepTonePrepareToPlayDualTonePlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt GetTune(const TDesC& aDtmfString, RArray<TToneInfo>& aTones);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	TBool iHasPlayed;
	};

class CTestStepTonePlayDualToneInvalidFreq :	public	CTestStepUnitMMFAudClient,
											public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePlayDualToneInvalidFreq* NewL(const TDesC& aName);
	CTestStepTonePlayDualToneInvalidFreq(const TDesC& aName);
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

	virtual void TimeoutExpired();
	static TInt TimeoutExpiredStaticCallback(TAny* aPtr);

private:
	TInt iPrepareError;
	TInt iPlayError;
	CMMFMdaAudioToneUtility* iTone;
	};


/**
 *
 * Cancel Play Tone utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneCancelPlay :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneCancelPlay* NewL();
	CTestStepToneCancelPlay();
	~CTestStepToneCancelPlay();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

//-----------------------------------------------------------------
/**
 *
 * Cancel Prepare Tone utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneCancelPrepare :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneCancelPrepare* NewL();
	CTestStepToneCancelPrepare();
	~CTestStepToneCancelPrepare();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};
//-----------------------------------------------------------------

/**
 *
 * set DTMF lengths
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneSetDTMFLengths :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneSetDTMFLengths* NewL();
	CTestStepToneSetDTMFLengths();
	~CTestStepToneSetDTMFLengths();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Set Volume Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneSetVolume :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneSetVolume* NewL();
	CTestStepToneSetVolume();
	~CTestStepToneSetVolume();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Get Volume Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneGetVolume :	public	CTestStepUnitMMFAudClient,
								public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneGetVolume* NewL();
	CTestStepToneGetVolume();
	~CTestStepToneGetVolume();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Set Priority Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneSetPriority :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneSetPriority* NewL();
	CTestStepToneSetPriority();
	~CTestStepToneSetPriority();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Set Repeats Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneSetRepeats :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneSetRepeats* NewL();
	CTestStepToneSetRepeats();
	~CTestStepToneSetRepeats();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Set Volume Ramp Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneSetVolumeRamp :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneSetVolumeRamp* NewL();
	CTestStepToneSetVolumeRamp();
	~CTestStepToneSetVolumeRamp();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Get Fixed Sequence number and name Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneGetFixedSeq :	public	CTestStepUnitMMFAudClient,
									public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneGetFixedSeq* NewL();
	CTestStepToneGetFixedSeq();
	~CTestStepToneGetFixedSeq();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	};

/**
 *
 * Get State Tone Utility
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepToneState :	public	CTestStepUnitMMFAudClient,
							public	MMdaAudioToneObserver
	{
public:
	static CTestStepToneState* NewL();
	CTestStepToneState();
	~CTestStepToneState();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);


private:
	TInt iError;
	CMMFMdaAudioToneUtility* iTone;
	TBool iHasPlayed;
	};


// Negative testing

/**
 *
 * Test Tone Prepare DTMF String 
 *
 * @lib "TSU_MMF_ACLNT_01.lib"
 *
 * @xxxx
 *
 */
class CTestStepTonePrepareToPlayIllegalDTMFString :	public	CTestStepUnitMMFAudClient,
													public	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePrepareToPlayIllegalDTMFString* NewL();
	CTestStepTonePrepareToPlayIllegalDTMFString();
	~CTestStepTonePrepareToPlayIllegalDTMFString();
	virtual enum TVerdict DoTestStepPreambleL(void);
	virtual enum TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();

	// from MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iError;
	TBool iPlayComplete;
	CMMFMdaAudioToneUtility* iTone;
	};


/**
 *
 * Test Playing a zero length tone on CMdaAudioToneUtility  
 * (DEF044042)
 * 
 * @lib "TSU_MMF_ACLNT_01.lib"
 * 
 * @xxxx
 *
 */
class CTestStepTonePlayZeroLength : public	CTestStepUnitMMFAudClient,
									public 	MMdaAudioToneObserver
	{
public:
	static CTestStepTonePlayZeroLength* NewL();
	CTestStepTonePlayZeroLength();
	~CTestStepTonePlayZeroLength();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
	virtual TVerdict DoTestStepL();
	// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
private:
	TInt iError;
	CMdaAudioToneUtility* iTone;
	};

class CToneActiveObject;
class RTestStepTonePlayBeforeInitialized : public RAsyncTestStep,
										  public MMdaAudioToneObserver
	{
friend class CToneActiveObject;

public:
	static RTestStepTonePlayBeforeInitialized* NewL();
	RTestStepTonePlayBeforeInitialized();

	void KickoffTestL();
	void CloseTest();
	
	void MatoPrepareComplete(TInt aError);
	void MatoPlayComplete(TInt aError);
private:
	CMMFMdaAudioToneUtility* iTone;
	CToneActiveObject* iToneActiveObject;
	};

class CToneActiveObject : public CActive
	{
public:
	CToneActiveObject(RTestStepTonePlayBeforeInitialized& aTest);
	~CToneActiveObject();
	
	void Activate();
	void RunL();
	void DoCancel();
	
private:
	RTestStepTonePlayBeforeInitialized& iTest;
	};
	
	
class RTestStepTonePlayWhenBusy : public RAsyncTestStep,
								  public MMdaAudioToneObserver
	{
public:
	static RTestStepTonePlayWhenBusy* NewL();
	
	RTestStepTonePlayWhenBusy();
	
	void KickoffTestL();
	void CloseTest();
	
	void MatoPrepareComplete(TInt aError);
	void MatoPlayComplete(TInt aError);
private:
	CMdaAudioToneUtility* iTone;
	CBusyTestUnit* iBusy;
	TInt iBusyPercent; // how busy to make things in %age
	TInt iBusyDelay;   // delay before busy time
	TInt iBusyLength;  // length of busy period
	TInt iToneLength;
	};

#endif //(__TEST_STEP_TONE__)
