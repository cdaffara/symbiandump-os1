
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
 @file TestTone.h
*/

#ifndef TESTTONE_H__
#define TESTTONE_H__

#include "TSI_MMFACLNT.h"



/**
 * Play a tone
 *
 * @class CTestMmfAclntTone
 *
 */
class CTestMmfAclntTone : public  CTestMmfAclntStep, public MMdaAudioToneObserver
	{
public:
	CTestMmfAclntTone(const TDesC& aTestName, const TInt aExpectedResult=KErrNone) ;
	static CTestMmfAclntTone* NewL(const TDesC& aTestName, const TInt aExpectedResult=KErrNone);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

protected:
	TInt iError;
	TInt iExpectedResult;
	TInt iFrequency;
	TBool iStop;
	TTimeIntervalMicroSeconds iDuration;
	};


/**
 * Play a Dual tone
 *
 * @class CTestMmfAclntDualTone
 *
 */
class CTestMmfAclntDualTone : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntDualTone(const TDesC& aTestName) ;
	static CTestMmfAclntDualTone* NewL(const TDesC& aTestName);
	virtual TVerdict DoTestStepL();

protected:
	TInt iFreqOne;
	TInt iFreqTwo;
 	};


/**
 * Play a DTMF string
 *
 * @class CTestMmfAclntToneDtmf
 *
 */
class CTestMmfAclntToneDtmf : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntToneDtmf(const TDesC& aTestName,const TDesC& aDTMF,const TInt aExpectedResult=KErrNone) ;
	static CTestMmfAclntToneDtmf* NewL(const TDesC& aTestName,const TDesC& aDTMF,const TInt aExpectedResult=KErrNone);
	virtual TVerdict DoTestStepL();

protected:
	TBuf<KNameBufSize> iDTMF;
 	};


/** 
 * Play a tone file
 *
 * @class CTestMmfAclntToneFile
 *
 */
class CTestMmfAclntToneFile : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntToneFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName) ;
	static CTestMmfAclntToneFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();

protected:	
	TInt iFrequency;
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	};


/**
 * Play a tone from a descriptor.
 *
 * @class CTestMmfAclntToneDes
 *
 */
class CTestMmfAclntToneDes : public CTestMmfAclntTone 
	{
public:
	CTestMmfAclntToneDes(const TDesC& aTestName, const TDesC8& aDes, const TInt aExpectedResult);
	static CTestMmfAclntToneDes* NewL(const TDesC& aTestName, const TDesC8& aDes, const TInt aExpectedResult);
	virtual TVerdict DoTestStepL();

public:
	const TDesC8& iDes;
	} ;


/**
 * Play a predefined/fixed tone
 *
 * @class CTestMmfAclntToneFixed
 *
 */
class CTestMmfAclntToneFixed : public CTestMmfAclntTone 
	{
public:
	CTestMmfAclntToneFixed(const TDesC& aTestName,const TInt aTone);
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntToneFixed* NewL(const TDesC& aTestName,const TInt aTone);

protected:
	TInt iTone;
	} ;


/**
 * Playing a tone and playing an audio file.
 *
 * @class CTestMmfAclntToneAudio
 *
 */
class CTestMmfAclntToneAudio : public CTestMmfAclntStep, public MMdaAudioToneObserver, public MMdaAudioPlayerCallback
	{
public:
	CTestMmfAclntToneAudio() ;
	virtual TVerdict DoTestStepL();
	// From MMdaAudioToneObserver
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	TInt iPlayerError;
	TInt iToneError;
	TInt iCallbackCount;
	};

/**
 * Cancel tone play
 *
 * @class CTestMmfAclntToneCancelP
 *
 */
class CTestMmfAclntToneCancelP : public CTestMmfAclntTone 
	{
public:
	CTestMmfAclntToneCancelP(const TDesC& aTestName);
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneCancelP* NewL(const TDesC& aTestName);
	};

/**
 * Cancel tone prepare.
 *
 * @class CTestMmfAclntToneCancelIni
 *
 */
class CTestMmfAclntToneCancelIni : public CTestMmfAclntTone 
	{
public:
	CTestMmfAclntToneCancelIni(const TDesC& aTestName);
	virtual TVerdict DoTestStepL();
	static CTestMmfAclntToneCancelIni* NewL(const TDesC& aTestName);
	};

/**
 * Enquire sequence name
 *
 * @class CTestMmfAclntToneNames
 *
 */
class CTestMmfAclntToneNames : public CTestMmfAclntToneFixed
	{
public:
	CTestMmfAclntToneNames(const TDesC& aTestName) ;
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneNames* NewL(const TDesC& aTestName);
	};

/**
 * Enquire sequence count
 *
 * @class CTestMmfAclntToneCount
 *
 */
class CTestMmfAclntToneCount : public CTestMmfAclntToneFixed
	{
public:
	CTestMmfAclntToneCount(const TDesC& aTestName);
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneCount* NewL(const TDesC& aTestName);
	};

/**
 * Set volume to max and enquire volume
 *
 * @class CTestMmfAclntToneVolume
 *
 */
class CTestMmfAclntToneVolume : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntToneVolume(const TDesC& aTestName,const TInt aVolume) ;
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneVolume* NewL(const TDesC& aTestName,const TInt aVolume);

private:
	TInt iVolume;
	};

/**
 * Set volume ramp
 *
 * @class CTestMmfAclntVolumeRamp
 *
 */
class CTestMmfAclntVolumeRamp : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntVolumeRamp(const TDesC& aTestName, const TInt aRamp);
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntVolumeRamp* NewL(const TDesC& aTestName, const TInt aRamp);

private:
	TInt iRamp;
	};


/**
 * Set repeats
 *
 * @class CTestMmfAclntToneRepeat
 *
 */
class CTestMmfAclntToneRepeat : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntToneRepeat(const TDesC& aTestName) ;
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneRepeat* NewL(const TDesC& aTestName);
	};

/**
 * Configure tone on length, tone of length, pause length of DTMF Tones
 *
 * @class CTestMmfAclntToneLength
 *
 */
class CTestMmfAclntToneLength : public CTestMmfAclntToneDtmf
	{
public:
	CTestMmfAclntToneLength(const TDesC& aTestName) ;
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneLength* NewL(const TDesC& aTestName);
	};

/**
 * Set prioritys of tones
 *
 * @class CTestMmfAclntPriorityTones
 *
 */
class CTestMmfAclntPriorityTones : public CTestMmfAclntStep, public MCallbackHandlerObserver
	{
public:
	CTestMmfAclntPriorityTones() ;
	virtual TVerdict DoTestStepL();
	// MCallbackHandlerObserver
	void MchoComplete(TInt aID, TInt aError);

private:
	CMdaAudioToneUtility* iToneUtil[2];
	CToneCallbackHandler* iCallback[2];
	TInt iCallbackCount;
	TInt iFirstCallback;
	TInt iFrequency;
	TInt iError;
	};

/**
 * Set balance and enquire balance
 *
 * @class CTestMmfAclntToneBalance
 *
 */
class CTestMmfAclntToneBalance : public CTestMmfAclntTone
	{
public:
	CTestMmfAclntToneBalance(const TDesC& aTestName,const TInt aBalance) ;
	virtual TVerdict DoTestL(CMdaAudioToneUtility* aToneUtil);
	static CTestMmfAclntToneBalance* NewL(const TDesC& aTestName,const TInt aBalance);

private:
	TInt iBalance;
	};

//
// NEGATIVE TESTS
//


/** 
 * Set up tone on, off and pause length to illegal values.
 *
 * @class CTestMM_MMF_ACLNT_I_1155_HP
 *
 */
class CTestMmfAclntOnOffPause : public CTestMmfAclntStep, public MMdaAudioToneObserver
	{
public:
	CTestMmfAclntOnOffPause() ;
	virtual TVerdict DoTestStepL();
// From MMdaAudioToneObserver
public:
	virtual void MatoPrepareComplete(TInt aError);
	virtual void MatoPlayComplete(TInt aError);

private:
	TInt iFrequency;
	TInt iError;
	};


#endif
