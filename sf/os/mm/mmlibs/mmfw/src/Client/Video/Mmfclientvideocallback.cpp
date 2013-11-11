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
// MmfVideoCallback.cpp
// 
//

#include "mmfvideocallback.h"

CMMFVideoPlayerCallback::~CMMFVideoPlayerCallback()
	{
	}

CMMFVideoPlayerCallback::CMMFVideoPlayerCallback(MVideoPlayerUtilityObserver& aCallback)
	: CActive(EPriorityStandard), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

void CMMFVideoPlayerCallback::SetFrame(CFbsBitmap& aBitmap) 
	{
	iEvent = EFrameReadyEvent;
	iBitmap = &aBitmap;
	}

void CMMFVideoPlayerCallback::RunL()
	{
	switch (iEvent)
		{
		case EOpenCompleteEvent: iCallback.MvpuoOpenComplete(iStatus.Int());
							break;
		case EFrameReadyEvent:	ASSERT(iBitmap);
								iCallback.MvpuoFrameReady(*iBitmap,iStatus.Int());
							break;
		case EPlayCompleteEvent: iCallback.MvpuoPlayComplete(iStatus.Int());
							break;
		default: ASSERT(EFalse);
			break;
		}
	}

void CMMFVideoPlayerCallback::DoCancel()
	{
	}

void CMMFVideoPlayerCallback::Signal(const TInt aReason, TCallbackEvent aEvent)
	{
	ASSERT(!IsActive());
	iEvent = aEvent;
	// Signal ourselves to run with the given completion code
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aReason);
	SetActive();
	}

TRequestStatus& CMMFVideoPlayerCallback::ActiveStatus()
	{
	SetActive();
	return iStatus;
	}


CMMFVideoRecorderCallback::~CMMFVideoRecorderCallback()
	{
	}

CMMFVideoRecorderCallback::CMMFVideoRecorderCallback(MVideoRecorderUtilityObserver& aCallback)
	: CActive(EPriorityStandard), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

void CMMFVideoRecorderCallback::RunL()
	{
	switch (iEvent)
		{
		case EOpenCompleteEvent: iCallback.MvruoOpenComplete(iStatus.Int());
							break;
		case EPrepareCompleteEvent: iCallback.MvruoPrepareComplete(iStatus.Int());
							break;
		case ERecordCompleteEvent: iCallback.MvruoRecordComplete(iStatus.Int());
							break;
		default: ASSERT(EFalse);
			break;
		}
	}

void CMMFVideoRecorderCallback::DoCancel()
	{
	}

void CMMFVideoRecorderCallback::Signal(const TInt aReason, TCallbackEvent aEvent)
	{
	ASSERT(!IsActive());
	iEvent = aEvent;
	// Signal ourselves to run with the given completion code
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aReason);
	SetActive();
	}
