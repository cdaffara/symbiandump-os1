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

#ifndef VIDEORECORDFILE_H
#define VIDEORECORDFILE_H

#include "tsu_mdf_videocodecs_step.h"
#include <mmf/devvideo/devvideorecord.h>

class CImageDecoder;

// For Record tests	
class CRecordVideoFile : public CActive, public MMMFDevVideoRecordProxy
	{
public:
enum TRecordVideoFileState
	{
	EHwDeviceInit,
	EHwDeviceStartEncode,
	EHwDeviceEncodeNextFrame,
	EHwDeviceLoadImages,
	EHwDeviceAllowToComplete,
	EHwDeviceEncodeCheckData,		
	EHwDeviceDone,
	EHwDeviceError
	};	
	
public:
	static CRecordVideoFile* NewL(RTestStepVideoCodecs* aParent);
	~CRecordVideoFile();
	
	void LoadCodecL();
	void StartRecordL();
	void CleanupAndSetDeviceError(TPtrC16 aText);
	
    // from MMMFDevVideoRecordObserver
	virtual void MdvrpReturnPicture(TVideoPicture* aPicture);
	virtual void MdvrpSupplementalInfoSent();
	virtual void MdvrpNewBuffer(TVideoOutputBuffer* aBuffer);
	virtual void MdvrpFatalError(CMMFVideoHwDevice* aDevice, TInt aError);
	virtual void MdvrpInitializeComplete(CMMFVideoHwDevice* aDevice, TInt aError);
	virtual void MdvrpStreamEnd();
	
	// From CActive
	void RunL();
	void DoCancel();
	
private:
	CRecordVideoFile(RTestStepVideoCodecs* aParent);
	void ConstructL();	

	void SetupSourceFileL();
	void SetupRecordFileL();
	void CloseRecordFile();
	void WriteDataToFile(const TDesC8& aHwDataBuffer);
    void SetState(TRecordVideoFileState aState);
    
    void EncodeNextFrameL();

private:
	// data section
	RFs iFileServerSession;
	RFile iRecordFile;
	TRecordVideoFileState iState;	
	RTestStepVideoCodecs* iParent;
	
	//DevVideoRecord
	CMMFVideoEncodeHwDevice* iVideoHwDevice;
	TInt iError;
	
	TInt iBufferSize;
	TInt iWrittenDataTotal;
	TInt iBuffersWrittenCount;
	TInt iLastBufferSize;
	TBool iStoppedReceived;
	
	TVideoPicture iVideoPicture;
	
	RFs iFs;
	CImageDecoder* iImageDecoder;
	CFbsBitmap** iBitmaps;
	TInt iNumFrames;
	TInt iCurrentFrame;
	TSize iFrameSize;
	};		
	

#endif // VIDEORECORDFILE_H


