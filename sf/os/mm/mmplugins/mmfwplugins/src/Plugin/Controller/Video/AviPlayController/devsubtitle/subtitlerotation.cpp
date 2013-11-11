// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "subtitlerotation.h"

CSubtitleRotation::CSubtitleRotation(MMMFDevSubtitleCallback& aCallback):
	CActive(EPriorityStandard),
	iMMFDevSubtitleCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CSubtitleRotation::~CSubtitleRotation()
	{
	Cancel();
	delete iRotator;
	}

CSubtitleRotation* CSubtitleRotation::NewL(MMMFDevSubtitleCallback& aCallback)
	{
	CSubtitleRotation* self = CSubtitleRotation::NewLC(aCallback);
	CleanupStack::Pop(self);
	return self;	
	}
	
CSubtitleRotation* CSubtitleRotation::NewLC(MMMFDevSubtitleCallback& aCallback)
	{
	CSubtitleRotation* self = new (ELeave)  CSubtitleRotation(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
void CSubtitleRotation::ConstructL()
	{
	iRotator = CBitmapRotator::NewL();
	}
	
void CSubtitleRotation::DoCancel()
	{
	iRotator->Cancel();
	}

void CSubtitleRotation::RunL()
	{
	iMMFDevSubtitleCallback.RotationComplete(iStatus.Int());
	}

void CSubtitleRotation::StartRotation(CFbsBitmap& aSrcBmp, CFbsBitmap& aTrgBmp, TVideoRotation aDisplayRotation)
	{
	switch(aDisplayRotation)
		{	
	case EVideoRotationClockwise270:	
		// Source is 90 degrees clockwise rotation so it requires a 270 degree clockwise rotation
		// to correctly orientate it to the display
		iRotator->Rotate(&iStatus, aSrcBmp, aTrgBmp, CBitmapRotator::ERotation270DegreesClockwise);
	break;
 
	case EVideoRotationClockwise180:
		iRotator->Rotate(&iStatus, aSrcBmp, aTrgBmp, CBitmapRotator::ERotation180DegreesClockwise);
	break;
 
	case EVideoRotationClockwise90:
		iRotator->Rotate(&iStatus, aSrcBmp, aTrgBmp, CBitmapRotator::ERotation90DegreesClockwise);
	break;
	
	default:
		// Programming error
		User::Invariant();
	break;
		}
	
	SetActive();
	}
