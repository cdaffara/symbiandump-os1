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
// VideoFrameMessage.cpp
// 
//

#include <mmf/common/mmfcontrollerframework.h>
#include <fbs.h>
#include "MMFVideoFrameMessage.h"

/**
Handles decoding the video frame message and providing a reference to the bitmap
to modify for the user of the class.
Provides a callback to return the message to the client
*/
CMMFVideoFrameMessage* CMMFVideoFrameMessage::NewL(TMMFMessage& aMessage)
	{
	CMMFVideoFrameMessage * self = new (ELeave) CMMFVideoFrameMessage(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFbsBitmap& CMMFVideoFrameMessage::GetBitmap()
	{
	return *iBitmap;
	}
void CMMFVideoFrameMessage::FrameReady(TInt aErr)
	{
	iMessage.Complete(aErr);
	}

CMMFVideoFrameMessage::CMMFVideoFrameMessage(TMMFMessage& aMessage)
	: iMessage(aMessage)
	{
	}

void CMMFVideoFrameMessage::ConstructL()
	{
	TPckgBuf<TMMFVideoConfig> pckg;
	iMessage.ReadData1FromClientL(pckg);
	iFbsSession.Connect();
	iBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBitmap->Duplicate(pckg().iFrameBitmapServerHandle));
	}

CMMFVideoFrameMessage::~CMMFVideoFrameMessage()
	{
	delete iBitmap;
	iFbsSession.Disconnect();
	}


