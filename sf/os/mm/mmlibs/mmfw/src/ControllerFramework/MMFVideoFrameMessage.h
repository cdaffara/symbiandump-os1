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
// VideoFrameMessage.h
// 
//

#ifndef __MMF_VIDEO_FRAME_MESSAGE_H__
#define __MMF_VIDEO_FRAME_MESSAGE_H__

#include <fbs.h>
#include <mmf/common/mmfvideo.h>



/**
Handles decoding the video frame message 
Provides a callback to return the message to the client
*/
class CMMFVideoFrameMessage : public CBase, public MMMFVideoFrameMessage
	{
public:
	static CMMFVideoFrameMessage* NewL(TMMFMessage& aMessage);
	~CMMFVideoFrameMessage();

	void ConstructL();

	//From MMMFVideoFrameMessage
	virtual CFbsBitmap& GetBitmap();
	virtual void FrameReady(TInt aErr);
private:
	CMMFVideoFrameMessage(TMMFMessage& aMessage);
private:
	TMMFMessage iMessage;
	CFbsBitmap* iBitmap;
	RFbsSession iFbsSession;
	};
#endif
