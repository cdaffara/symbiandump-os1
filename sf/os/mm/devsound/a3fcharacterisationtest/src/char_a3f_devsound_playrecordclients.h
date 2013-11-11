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

#ifndef CHAR_A3F_DEVSOUND_PLAYRECORDCLIENTS_H
#define CHAR_A3F_DEVSOUND_PLAYRECORDCLIENTS_H

#include <mmf/server/sounddevice.h>
#include <testframework.h>
#include "char_a3f_devsound_compliancesuiteconstants.h"

class CAsyncWriteBufferToFile2;

class MA3FDevsoundPlayAudioClientObserver
	{
public:
	virtual void ClientInitializeCompleteCallback(TInt aError) = 0;
	virtual void ClientBufferToBeFilledCallback(TInt aError) = 0;
	virtual void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer) = 0;
	virtual void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)=0;
	};


class CA3FDevSoundPlayClient : public CBase, public MDevSoundObserver
	{
public:
	static CA3FDevSoundPlayClient* NewL(MA3FDevsoundPlayAudioClientObserver &aObserver,TFourCC aFourccCode, RFs& aFs, CTestSuite* aSuite = NULL);
	virtual ~CA3FDevSoundPlayClient();

	// New functions
	void SetPriority(TInt priority);
	TInt PlayInit(const TDesC& aFileName);
	TInt PlayAudio();
	void StopPlay();
	void DeleteClient();
	
private:
	CA3FDevSoundPlayClient(MA3FDevsoundPlayAudioClientObserver &aObserver, TFourCC aFourccCode, RFs& aFs, CTestSuite* aSuite);
	void ConstructL();

	TInt Fsm(TMmfDevSoundEvent aDevSoundEvent);
	// from MDevSoundObserver
	void BufferToBeFilled(CMMFBuffer *aBuffer);
	void BufferToBeEmptied(CMMFBuffer *aBuffer);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void ConvertError(TInt aError);
	void InitializeComplete(TInt aError);
	void PlayError(TInt aError);
	void RecordError(TInt aError);
	void ToneFinished(TInt aError);

	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
			TRefByValue<const TDesC16> aFmt,...);
	
	TMmfDevSoundState iDevSoundState;
	CMMFDevSound* iDevSound;
	MA3FDevsoundPlayAudioClientObserver &iObserver;
	CMMFBuffer* iBuffer;
	TFileName iFilename;
	// Handle to a file server session
	RFs iFs;
	// Handle for the audio file for playing
	RFile iFile;
	TFourCC iFourccCode;
	CTestSuite* iSuite;
	};

class MA3FDevsoundRecordAudioClientObserver
	{
public:
	virtual void ClientInitializeCompleteCallback(TInt aError) = 0;
	virtual void ClientBufferToBeEmptiedCallback(TInt aError) = 0;
	virtual void ClientRecordErrorCallback(TInt aError) = 0;
	virtual void ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError)=0;
	};


class CA3FDevSoundRecordClient : public CBase, public MDevSoundObserver
	{
public:
	static CA3FDevSoundRecordClient* NewL(MA3FDevsoundRecordAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite = NULL);
	virtual ~CA3FDevSoundRecordClient();

	// New functions
	void SetPriority(TInt priority);
	TInt InitializeRecordClient(const TDesC& aFile);
	TInt RecordAudio();
	void StopRecord();
	
private:
	CA3FDevSoundRecordClient(MA3FDevsoundRecordAudioClientObserver &aObserver, TFourCC aFourccCode, RFs &aFs, CTestSuite* aSuite);
	void ConstructL();
	void WriteDataToFile();
	TInt Fsm(TMmfDevSoundEvent aDevSoundEvent);
	// from MDevSoundObserver
	void BufferToBeFilled(CMMFBuffer *aBuffer);
	void BufferToBeEmptied(CMMFBuffer *aBuffer);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void ConvertError(TInt aError);
	void InitializeComplete(TInt aError);
	void PlayError(TInt aError);
	void RecordError(TInt aError);
	void ToneFinished(TInt aError);
	
	void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
			TRefByValue<const TDesC16> aFmt,...);

	TMmfDevSoundState iDevSoundState;
	CMMFDevSound* iDevSound;
	MA3FDevsoundRecordAudioClientObserver &iObserver;
	CMMFBuffer* iBuffer;
	TFileName iFilename;
	// Handle for the audio file for recording
	RFile iFile;
	// Handle to a file server session
	RFs iFs;
	TFourCC iFourccCode;
	CAsyncWriteBufferToFile2* iAsyncWriteBTFAO;
	CTestSuite* iSuite;
	};
	
/*
 *
 * CAsyncWriteBufferToFile2
 *
 */

class CAsyncWriteBufferToFile2 : public CActive
	{
public:
	static CAsyncWriteBufferToFile2* NewL(RFile& aFile, CMMFDevSound* aDevSound);
	~CAsyncWriteBufferToFile2();
	void Start(CMMFDataBuffer* aBuffer);

private:
	CAsyncWriteBufferToFile2(RFile& aFile, CMMFDevSound* aDevSound);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	RFile& iFile;
	CMMFDevSound* iDevSound;

	};

/*
 *
 * TIntegrationTestLog16OverflowClient
 *
 */
class TIntegrationTestLog16OverflowClient :public TDes16Overflow
	{
public:
	// TDes16Overflow pure virtual
	virtual void Overflow(TDes16& aDes);
	};
	
#endif	//CHAR_A3F_DEVSOUND_PLAYRECORDCLIENTS_H

