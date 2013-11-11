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

#ifndef TSU_MDF_AUDIOCODECS_RECORD_H
#define TSU_MDF_AUDIOCODECS_RECORD_H

#include "tsu_mdf_audiocodecs_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepAudioCodecs;	
class CRecordAudioFile;
class CMdfTimerRecordStop;	

class RTestStepAudioCodecRecord : public RTestStepAudioCodecs
	{
public:
	RTestStepAudioCodecRecord();
	virtual TVerdict DoTestStepL();
	void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass);
private:
	CRecordAudioFile* iRecordAudioFile;
	TVerdict iVerdict;
	};

// For Record tests	
class CRecordAudioFile : public CActive, public MMMFHwDeviceObserver
	{
public:
enum TRecordAudioFileState
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
	static CRecordAudioFile* NewL(RTestStepAudioCodecs* aParent);
	~CRecordAudioFile();
	void StartL();
	TInt SetUpRecordFile();
	void CloseRecordFile();
	TInt WriteDataToFile(TDes8& aHwDataBuffer);
    void SetState(TRecordAudioFileState aState);
    void ReadNextBuffer(CMMFBuffer& aHwDataBuffer);
    void CleanupAndSetDeviceError(TPtrC16 aText);
    
    // From MMMFHwDeviceObserver
    virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped();
	virtual void Error(TInt aError=KErrNone);
	
	// From CActive
	void RunL();
	void DoCancel();
		
private:
	CRecordAudioFile(RTestStepAudioCodecs* aParent);
	void ConstructL();	

private:
	// data section
	RFs iFileServerSession;
	RFile iRecordFile;
	TRecordAudioFileState iState;	
	CMMFHwDevice* iHwDevice;
	RTestStepAudioCodecs* iParent;
	TInt iBufferSize;
	TInt iWrittenDataTotal;
	TInt iBuffersWrittenCount;
	TInt iLastBufferSize;
	CMdfTimerRecordStop* iRecordTimer;
	TBool iStoppedReceived;
	};		
	
class CMdfTimerRecordStop : CTimer
	{
public:
	static CMdfTimerRecordStop* NewL(CRecordAudioFile* aParent,
		CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime);	
	void Start();	
protected:
	TInt RunError(TInt aError);	
	void RunL();
private:
	CMdfTimerRecordStop(CRecordAudioFile* aParent,
		CMMFHwDevice* aHwDevice, TTimeIntervalMicroSeconds32 aWaitTime);
	CRecordAudioFile* iParent;
	CMMFHwDevice* iHwDevice;
	TTimeIntervalMicroSeconds32 iWaitTime;
	};
	

#endif // TSU_MDF_AUDIOCODECS_RECORD_H


