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
 @file TestPlayerUtils.h
*/

#ifndef __TESTDRM_H__
#define __TESTDRM_H__


#include "TSI_MMFACLNT.h"


/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntDRMPlayerNoRights
 *
 */         
class CTestMmfAclntDRMPlayerNoRights :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntDRMPlayerNoRights* NewL();
	static CTestMmfAclntDRMPlayerNoRights* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntDRMPlayerNoRights();
	
private:
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};


/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntDRMPlayerRightsCount
 *
 */         
class CTestMmfAclntDRMPlayerRightsCount :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntDRMPlayerRightsCount* NewL();
	static CTestMmfAclntDRMPlayerRightsCount* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntDRMPlayerRightsCount();
	
private:
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};



/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntDRMRecorderNoRights
 *
 */         
class CTestMmfAclntDRMRecorderNoRights :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntDRMRecorderNoRights* NewL();
	static CTestMmfAclntDRMRecorderNoRights* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
    virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntDRMRecorderNoRights();
	
private:
	TInt iPreviousState;
	TInt iCurrentState;
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};


/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntDRMRecorderRightsCount
 *
 */         
class CTestMmfAclntDRMRecorderRightsCount :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntDRMRecorderRightsCount* NewL();
	static CTestMmfAclntDRMRecorderRightsCount* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
    virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntDRMRecorderRightsCount();
	
private:
	TInt iPreviousState;
	TInt iCurrentState;
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};

/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntDRMPlayerOpen
 *
 */         
class CTestMmfAclntDRMPlayerOpen :  public CTestMmfAclntStep, public MMdaAudioPlayerCallback
	{
public:
	static CTestMmfAclntDRMPlayerOpen* NewL();
	static CTestMmfAclntDRMPlayerOpen* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaAudioPlayerCallback
    virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    virtual void MapcPlayComplete(TInt aError);

private:
	CTestMmfAclntDRMPlayerOpen();
	
private:
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};

/**
 * Load and initialise an audio file.
 *
 * @class CTestMmfAclntDRMRecorderPlay
 *
 */         
class CTestMmfAclntDRMRecorderPlay :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntDRMRecorderPlay* NewL();
	static CTestMmfAclntDRMRecorderPlay* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
    virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntDRMRecorderPlay();
	
private:
	TInt iPreviousState;
	TInt iCurrentState;
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};


/**
 * Test that Audio conversion fails for protected files
 *
 * @class CTestMmfAclntDRMConverterFail
 *
 */         
class CTestMmfAclntDRMConverterFail :  public CTestMmfAclntStep, public MMdaObjectStateChangeObserver
	{
public:
	static CTestMmfAclntDRMConverterFail* NewL();
	static CTestMmfAclntDRMConverterFail* NewLC();
	virtual TVerdict DoTestStepL();
	// from MMdaObjectStateChangeObserver
    virtual void MoscoStateChangeEvent(CBase* aObject,TInt aPreviousState,TInt aCurrentState, TInt aErrorCode);

private:
	CTestMmfAclntDRMConverterFail();
	
private:
	TInt iPreviousState;
	TInt iCurrentState;
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;// Stores duration of audio
	};

#endif // __TESTDRM_H__
