// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_VIDEO_CALLBACK_H__
#define __MMF_VIDEO_CALLBACK_H__

#include <videoplayer.h>
#include <videorecorder.h>
#include <fbs.h>

/**
Active object utility class to allow the Callback to be called asynchronously.
*/
class CMMFVideoPlayerCallback : public CActive
	{
public:
	enum TCallbackEvent {
		EOpenCompleteEvent,
		EFrameReadyEvent,
		EPlayCompleteEvent
		};

public:
	CMMFVideoPlayerCallback(MVideoPlayerUtilityObserver& aCallback);
	~CMMFVideoPlayerCallback();
	void Signal(TInt aError, TCallbackEvent event);
	void SetFrame(CFbsBitmap& aBitmap);
	TRequestStatus& ActiveStatus();
private:
	void RunL();
	void DoCancel();
private:
	TCallbackEvent iEvent;
	MVideoPlayerUtilityObserver& iCallback;
	CFbsBitmap* iBitmap;
	TInt iError;
	};

/**
Active object utility class to allow the Callback to be called asynchronously.
*/
class CMMFVideoRecorderCallback : public CActive
	{
public:
	enum TCallbackEvent {
		EOpenCompleteEvent,
		EPrepareCompleteEvent,
		ERecordCompleteEvent
		};
public:
	CMMFVideoRecorderCallback(MVideoRecorderUtilityObserver& aCallback);
	~CMMFVideoRecorderCallback();
	void Signal(TInt aError, TCallbackEvent event);
private:
	void RunL();
	void DoCancel();
private:
	TCallbackEvent iEvent;
	MVideoRecorderUtilityObserver& iCallback;
	TInt iError;
	};

#endif //__MMF_VIDEO_CALLBACK_H__
