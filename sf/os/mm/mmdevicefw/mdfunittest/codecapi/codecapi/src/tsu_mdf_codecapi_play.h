// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Play related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_play.h
*/

#ifndef TSU_MDF_CODECAPI_PLAY_H
#define TSU_MDF_CODECAPI_PLAY_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepCodecApi;
class CPlayAudioFile;

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0100-HP
 * Play an audio file from start to finish.
 *
 * @class RTestStepCodecApiPlay 
 *
 */
class RTestStepCodecApiPlay : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiPlay();
	virtual TVerdict DoTestStepL();
	virtual void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass);
protected:
	CPlayAudioFile* iPlayAudioFile;
	TVerdict iVerdict;
	};

/**
 * 
 * Active player for RTestStepCodecApiPlay and derived test classes.
 *
 * @class CPlayAudioFile 
 *
 */
class CPlayAudioFile : public CActive, public MMMFHwDeviceObserver
	{
public:
enum TPlayAudioFileState
	{
	EHwDeviceCreateAndInit,
	EHwDeviceStartDecode,
	EHwDeviceStartEncode,
	EHwDeviceAllowToComplete,
	EHwDeviceEncodeCheckData,		
	EHwDeviceDone,
	EHwDeviceError
	};	
public:
	static CPlayAudioFile* NewL(RTestStepCodecApi* aParent);
	~CPlayAudioFile();
	virtual void RunL();
	virtual void DoCancel();
	void StartL();
	TInt ReadTestFileInBuffer();
    void SetState(TPlayAudioFileState aState);
    void ReadNextBuffer(CMMFBuffer& aHwDataBuffer);
    
    virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped();
	virtual void Finished();
	virtual void Error(TInt aError=KErrNone);
	
	void CleanupAndSetDeviceError(TPtrC16 aText);
	
protected:
	CPlayAudioFile(RTestStepCodecApi* aParent);
private:
	void ConstructL();	

protected:
	TPlayAudioFileState iState;	
	CMMFHwDevice* iHwDevice;
	RTestStepCodecApi* iParent;
	TBool iFinished;
private:
	RFs iFileServerSession;
	HBufC8* iSourceFile;
	TInt iSourceFilePos;
	TBool iStoppedReceived;
	};

#endif // TSU_MDF_CODECAPI_PLAY_H


