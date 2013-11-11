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
// Header file: Record related tests for PREQ1024 OpenMax Vorbis PU.
// 
//

/**
 @file tsu_mdf_omxvorbiscodecs_record.h
*/

#ifndef TSU_MDF_OMXVORBISCODECS_RECORD_H
#define TSU_MDF_OMXVORBISCODECS_RECORD_H

#include "tsu_mdf_omxvorbiscodecs_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepOmxVorbisCodecs;	
class CRecordAudioFile;
class CMTSU_MDF_OMXVORBISCODECS_STEP_HdfTimerRecordStop;	

/**
 * 
 * Test Case : MM-MDF-OMXVORBISCODECS-U-0002-HP
 * Record a Vorbis file
 *
 * @class RTestStepOmxVorbisCodecRecord 
 *
 */
class RTestStepOmxVorbisCodecRecord : public RTestStepOmxVorbisCodecs
	{
public:
	RTestStepOmxVorbisCodecRecord();
	virtual TVerdict DoTestStepL();
	void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass);
private:
	CRecordAudioFile* iRecordAudioFile;
	TVerdict iVerdict;
	};

/**
 * 
 * Active recorder for RTestStepOmxVorbisCodecRecord and derived test classes.
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
	//EHwDevicePlayFile, //used?
	EHwDeviceAllowToComplete,
	EHwDeviceEncodeCheckData,		
	EHwDeviceDone,
	EHwDeviceError
	};	
	
public:
	static CRecordAudioFile* NewL(RTestStepOmxVorbisCodecs* aParent);
	~CRecordAudioFile();
	void RunL();
	void DoCancel();
	void StartL();
	TInt SetUpRecordFile();
	void CloseRecordFile();
	TInt WriteDataToFile(TDes8& aHwDataBuffer);
    void SetState(TRecordAudioFileState aState);
	TInt ReadTestFileInBuffer();
    void ReadNextBuffer(CMMFBuffer& aHwDataBuffer);
    
    virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped();
	virtual void Error(TInt aError=KErrNone);
	
	void CleanupAndSetDeviceError(TPtrC16 aText);
	
private:
	CRecordAudioFile(RTestStepOmxVorbisCodecs* aParent);
	void ConstructL();	

private:
	// data section
	RFs iFileServerSession;
	RFile iRecordFile;
	TRecordAudioFileState iState;	
	HBufC8* iSourceFile;
	TInt iSourceFileReadPos;
	TBool iFinished;
	CMMFHwDevice* iHwDevice;
	RTestStepOmxVorbisCodecs* iParent;
	TInt iWrittenDataTotal;
	TInt iBuffersWrittenCount;
	TInt iLastBufferSize;
	TBool iStoppedReceived;
	};		
	
#endif // TSU_MDF_OMXVORBISCODECS_RECORD_H


