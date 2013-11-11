// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_SWCDWRAP_PLAYHWDEVOBSERVER_H__
#define __TSU_MMF_SWCDWRAP_PLAYHWDEVOBSERVER_H__

#include "TSU_MMF_SWCDWRAP_HwDevObserver.h"

class CTestPlayMMFHwDeviceObserver : public CTestMMFHwDeviceObserver
	{
public:
	CTestPlayMMFHwDeviceObserver(CMMFHwDevice* aHwDevice);
	~CTestPlayMMFHwDeviceObserver();

	//MMFHwDeviceObserver
	virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer);
	virtual void Stopped();

	TInt FillBufferFromFileSync(const TDesC& aFileName, TUint aBufferSize);
	TInt ReadBufferDirectFromFileAsync(const TDesC& aFileName, TUint aNumberOfBuffers);
	
	//callback from CTestFileReader
	virtual void FileReadComplete(TInt aError);

	//gets Read File handle
	RFile& ReadFile();

	void CancelFileRead();

	TBool IsStopped();

	void StopActiveSchedulerOnStop(TBool aStopActiveSchedulerOnStop = ETrue);
	void CallPauseAndResumeAfterSendingLastBuffer();
private:
	TInt FillThisHwBufferFromDes(CMMFDataBuffer& aHwDataBuffer);
	TInt FillThisHwBufferFromFile(CMMFDataBuffer& aHwDataBuffer);
private:
	CTestFileReader* iFileReader;
	TUint iBufferPosition;
	RFile iSourceFile;
	HBufC8* iDataBuffer;
	TBool iAsyncMode;
	TBool iStopped;
	TBool iStopActiveSchedulerOnStop;
	TBool iCallPauseAndResumeAfterSendingLastBuffer;
	};

#endif /* __TSU_MMF_SWCDWRAP_PLAYHWDEVOBSERVER_H__ */
