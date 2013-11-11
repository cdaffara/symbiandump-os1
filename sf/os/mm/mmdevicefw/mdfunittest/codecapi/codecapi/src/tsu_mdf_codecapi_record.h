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
// Header file: Record related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_record.h
*/

#ifndef TSU_MDF_CODECAPI_RECORD_H
#define TSU_MDF_CODECAPI_RECORD_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepCodecApi;	
class CRecordAudioFile;
class CMdfTimerRecordStop;	

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0110-HP
 * Use the HwDeviceAdapter to record a file
 *
 * @class RTestStepCodecApiRecord 
 *
 */
class RTestStepCodecApiRecord : public RTestStepCodecApi
	{
public:
	RTestStepCodecApiRecord();
	virtual TVerdict DoTestStepL();
	void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass);
private:
	CRecordAudioFile* iRecordAudioFile;
	TVerdict iVerdict;
	};

/**
 * 
 * Active recorder for RTestStepCodecApiRecord and derived test classes.
 *
 * @class CRecordAudioFile 
 *
 */
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
	static CRecordAudioFile* NewL(RTestStepCodecApi* aParent);
	~CRecordAudioFile();
	void RunL();
	void DoCancel();
	void StartL();
	TInt SetUpRecordFile();
	void CloseRecordFile();
	TInt WriteDataToFile(TDes8& aHwDataBuffer);
    void SetState(TRecordAudioFileState aState);
    void ReadNextBuffer(CMMFBuffer& aHwDataBuffer);
    
    virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped();
	virtual void Error(TInt aError=KErrNone);
	
	void CleanupAndSetDeviceError(TPtrC16 aText);
	
private:
	CRecordAudioFile(RTestStepCodecApi* aParent);
	void ConstructL();	

private:
	// data section
	RFs iFileServerSession;
	RFile iRecordFile;
	TRecordAudioFileState iState;	
	CMMFHwDevice* iHwDevice;
	RTestStepCodecApi* iParent;
	TInt iBufferSize;
	TInt iWrittenDataTotal;
	TInt iBuffersWrittenCount;
	TInt iLastBufferSize;
	CMdfTimerRecordStop* iRecordTimer;
	TBool iStoppedReceived;
	};		
	
/**
 * 
 * Timer for those test classes requiring timed stop
 *
 * @class CMdfTimerRecordStop 
 *
 */
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
	

#endif // TSU_MDF_CODECAPI_RECORD_H


