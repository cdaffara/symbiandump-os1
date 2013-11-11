// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef CHAR_A3F_DEVSOUND_TESTBASE_H
#define CHAR_A3F_DEVSOUND_TESTBASE_H

// system includes
#include <testframework.h>
#include <mmf/server/sounddevice.h>

// user includes
#include "char_a3f_devsound_compliancesuiteconstants.h"

// forward declaration
class CA3FDevSoundToneClient;
class CA3FDevSoundPlayClient;
class CA3FDevSoundRecordClient;

class CAsyncWriteBufferToFile;

/*
 Base class for all DevSound test steps in this suite.
 */
class RA3FDevSoundTestBase: public RAsyncTestStep, public MDevSoundObserver 
	{
public:		
	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	void CallStopTest(TInt aError);

protected:
	RA3FDevSoundTestBase(const TDesC& aTestName);
	virtual void DoKickoffTestL() = 0;
	virtual void DoTimerCallback();
	static TInt TimerCallback(TAny*);
	void StartTimer(TTimeIntervalMicroSeconds32 aWaitTime);
	// from MDevSoundObserver
	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
	void BufferToBeFilled(CMMFBuffer* aBuffer);
	void PlayError(TInt aError);
	void BufferToBeEmptied(CMMFBuffer* aBuffer);
	void RecordError(TInt aError);
	void ConvertError(TInt aError);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void SendEventToClient(const TMMFEvent& aEvent);
	// Executes events of DevSound using a Finite State Machine
	virtual void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError) = 0;

	void SampleRateFromTIntToTMMFSampleRate(TInt aSampleRate, TMMFSampleRate &aESampleRate);
	void SampleRateFromTUintToString(TUint aSampleRate, TDes& aStringSampleRate);
	void ChannelsFromTUintToString(TUint aChannels, TDes& aStringChannels);
	void EncodingFromStringToTFourCC(const TDesC& aFourCCString);
	void PrintSupportedCapabilities(TUint aSampleRate,TUint aChannel);
	
protected:
	TBuf<KSizeBuf> iFourCCString;
	TFourCC iFourCCCode;
	TMmfDevSoundState iDevSoundState;
	CA3FDevSoundToneClient* iDevsoundToneClient;
	CA3FDevSoundPlayClient* iDevsoundPlayClient;
	CA3FDevSoundRecordClient* iDevsoundRecordClient;
	CMMFDevSound* iMMFDevSound;
	CPeriodic* iTimer;
	CMMFBuffer* iBuffer;
	// Handle for the audio file for playing and recording
	RFile iFile;
	// Handle to a file server session
	RFs iFs;
	CAsyncWriteBufferToFile* iAsyncWriteBTFAO;
	};

class TSampleRateToTMMFSampleRate
	{
public:
	TInt iTIntSampleRate;
	TMMFSampleRate iTMMFSampleRate;
	};

class TSampleRateToString
	{
public:
	TUint iTUIntSampleRate;
	TPtrC iTPtrSampleRate;
	};

class TChannelsToString
	{
public:
	TUint iTUIntChannels;
	TPtrC iTPtrChannels;
	};
/*
 *
 * CAsyncWriteBufferToFile
 *
 */

class CAsyncWriteBufferToFile : public CActive
	{
public:
	static CAsyncWriteBufferToFile* NewL(RFile& aFile, CMMFDevSound* aDevSound, RA3FDevSoundTestBase& aTestStep);
	~CAsyncWriteBufferToFile();
	void Start(CMMFDataBuffer* aBuffer);

private:
	CAsyncWriteBufferToFile(RFile& aFile, CMMFDevSound* aDevSound, RA3FDevSoundTestBase& aTestStep);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	RFile& iFile;
	CMMFDevSound* iDevSound;
	RA3FDevSoundTestBase& iTestStep;
	};

#endif	// CHAR_A3F_DEVSOUND_TESTBASE_H

