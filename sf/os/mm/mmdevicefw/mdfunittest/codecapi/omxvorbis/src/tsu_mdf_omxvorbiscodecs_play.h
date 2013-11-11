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
// Header file: Play related tests for PREQ1024 OpenMax Vorbis PU.
// 
//

/**
 @file tsu_mdf_omxvorbiscodecs_play.h
*/

#ifndef TSU_MDF_OMXVORBISCODECS_PLAY_H
#define TSU_MDF_OMXVORBISCODECS_PLAY_H

#include "tsu_mdf_omxvorbiscodecs_step.h"
#include <mmf/server/mmfhwdevice.h>
#include <oggvorbiscodec/oggutil.h>

class RTestStepOmxVorbisCodecs;
class CPlayAudioFile;

/**
 * 
 * Test Case : MM-MDF-OMXVORBISCODECS-U-0001-HP
 * Play a Vorbis file
 *
 * @class RTestStepOmxVorbisCodecPlay 
 *
 */
class RTestStepOmxVorbisCodecPlay : public RTestStepOmxVorbisCodecs
	{
public:
	RTestStepOmxVorbisCodecPlay();
	virtual TVerdict DoTestStepL();
	void SetVerdict(TPtrC16 aText, TVerdict aVerdict = EPass);
private:
	CPlayAudioFile* iPlayAudioFile;
	TVerdict iVerdict;
	
	};

/**
 * 
 * Active player for RTestStepOmxVorbisCodecPlay and derived test classes.
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
	//EHwDevicePlayFile, //used?
	EHwDeviceAllowToComplete,
	EHwDeviceEncodeCheckData,		
	EHwDeviceDone,
	EHwDeviceError
	};	
public:
	static CPlayAudioFile* NewL(RTestStepOmxVorbisCodecs* aParent);
	~CPlayAudioFile();
	void RunL();
	void DoCancel();
	void StartL();
	TInt ReadTestFileInBuffer();
    void SetState(TPlayAudioFileState aState);
    void ReadNextBuffer(CMMFBuffer& aHwDataBuffer);

	TInt SetUpRecordFile();
	void CloseRecordFile();
	void AddWavHeader();
	TInt WriteDataToFile(TDes8& aHwDataBuffer);
    
    virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped();
	virtual void Error(TInt aError=KErrNone);
	
	void CleanupAndSetDeviceError(TPtrC16 aText);
	
private:
	CPlayAudioFile(RTestStepOmxVorbisCodecs* aParent);
	void ConstructL();	

private:
	// data section
	RFs iFileServerSession;
	TPlayAudioFileState iState;	
	HBufC8* iSourceFile;
	TInt iSourceFilePos;
	TInt iSourceFileReadPos;
	TBool iFinished;
	CMMFHwDevice* iHwDevice;
	RTestStepOmxVorbisCodecs* iParent;
	TBool iStoppedReceived;

	// for record file
	RFile iRecordFile;
	TInt iWrittenDataTotal;
	TInt iBuffersWrittenCount;

	// for Ogg pager
	COggPager* iPager;
	COggStream* iStream;
	TOggPage iPage;
	TOggPacket iPacket;
	};

#endif // TSU_MDF_OMXVORBISCODECS_PLAY_H


