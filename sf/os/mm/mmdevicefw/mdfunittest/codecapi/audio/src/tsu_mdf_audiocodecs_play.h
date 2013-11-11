// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSU_MDF_AUDIOCODECS_PLAY_H
#define TSU_MDF_AUDIOCODECS_PLAY_H

#include "tsu_mdf_audiocodecs_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepAudioCodecs;
class CPlayAudioFile;

class RTestStepAudioCodecPlay : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecPlay();
	virtual TVerdict DoTestStepL();
	void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass);
private:
	CPlayAudioFile* iPlayAudioFile;
	TVerdict iVerdict;
	};

// For Play tests
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
	static CPlayAudioFile* NewL(RTestStepAudioCodecs* aParent);
	~CPlayAudioFile();	
	void StartL();
	TInt ReadTestFileInBuffer();
    void SetState(TPlayAudioFileState aState);
    void ReadNextBuffer(CMMFBuffer& aHwDataBuffer);
    void CleanupAndSetDeviceError(TPtrC16 aText);
    
    // From MMMFHwDeviceObserver
    virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped();
	virtual void Error(TInt aError=KErrNone);
		
protected:
	// From CActive
	void RunL();
	void DoCancel();	
	
private:
	CPlayAudioFile(RTestStepAudioCodecs* aParent);
	void ConstructL();	

private:
	// data section
	RFs iFileServerSession;
	TPlayAudioFileState iState;	
	HBufC8* iSourceFile;
	TInt iSourceFilePos;
	CMMFHwDevice* iHwDevice;
	RTestStepAudioCodecs* iParent;
	TBool iStoppedReceived;
	};

#endif // TSU_MDF_AUDIOCODECS_PLAY_H


