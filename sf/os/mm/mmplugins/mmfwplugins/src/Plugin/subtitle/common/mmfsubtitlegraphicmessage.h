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

/**
 @file
 @internalComponent
*/

#ifndef MMFSUBTITLEGRAPHICMESSAGE_H
#define MMFSUBTITLEGRAPHICMESSAGE_H

#include <e32std.h>
#include <s32strm.h>

/*
Defines the message classes for communication between the MMF CRP 
subtitle rendering client and the WSERV CRP graphic plug-in
*/


enum TSubtitleGraphicMessage
	{
	ESubtitleCrpMessageInit 		= 10,
	ESubtitleCrpMessageInitSimple 	= 20,
	ESubtitleCrpMessageClear 		= 30,
	ESubtitleCrpMessageDrawFrame 	= 40,	
	ESubtitleCrpMessageSwapFrame 	= 50
	};

/**
Base class for all CRP messages
*/
class TSubtitleGraphicMessageBase
{
public:
	TSubtitleGraphicMessageBase(TSubtitleGraphicMessage aMsgType);

protected:
	TInt8 iMsgType;
};

/**
Initialise the CRP
Passes two bitmap handles to the CRP
@see CMMFSubtitleGraphic::Initialize(TInt aBuffer1, TInt aBuffer2);
*/
class TSubtitleCrpMsgInit : public TSubtitleGraphicMessageBase
	{
public:
	TSubtitleCrpMsgInit();
	TSubtitleCrpMsgInit(TInt aBitmapHandle1, TInt aBitmapHandle2);

public:
	TInt iBitmapHandle1;
	TInt iBitmapHandle2;
	};

/**
Initialise the CRP
@see CMMFSubtitleGraphic::Initialize()
*/
class TSubtitleCrpMsgInitSimple : public TSubtitleGraphicMessageBase
	{
public:
	TSubtitleCrpMsgInitSimple();
	};

/**
Request that the CRP clears all content from the window
@see CMMFSubtitleGraphic::Clear()
*/
class TSubtitleCrpMsgClear : public TSubtitleGraphicMessageBase
	{
public: 
	TSubtitleCrpMsgClear();
	};

/**
Requests that the CRP draws a frame indicated by the CFBSBitmap handle provided
@see CMMFSubtitleGraphic::DrawFrame(TInt aFrameHandle, TRect& aDirtyRegion,
     TTimeIntervalMicroSeconds& aDisplayDuration)
*/
class TSubtitleCrpMsgDrawFrame : public TSubtitleGraphicMessageBase
	{
public:
	TSubtitleCrpMsgDrawFrame();
	TSubtitleCrpMsgDrawFrame(TInt aBitmapHandle, const TRect& aDirtyRegion, const TTimeIntervalMicroSeconds& aDisplayDuration);

public:	
	TInt iBitmapHandle;
	TRect iDirtyRegion;
	TTimeIntervalMicroSeconds iDisplayDuration;
	};

/**
Request that the CRP draw the content in the current back buffer.  Assumes that the CRP
has been intialized with TSubtitleCrpMsgInit
@see CMMFSubtitleGraphic::SwapFrame(TInt aExpectedBuffer, 
 	TTimeIntervalMicroSeconds& aDisplayDuration, const TRect& aDirtyRegion)
*/
class TSubtitleCrpMsgRenderSwapFrame : public TSubtitleGraphicMessageBase
	{
public:
	TSubtitleCrpMsgRenderSwapFrame();
	TSubtitleCrpMsgRenderSwapFrame(TInt aExpectedFrame, const TRect& aDirtyRegion, const TTimeIntervalMicroSeconds& aDisplayDuration);
	
public:
	TInt iExpectedFrame;
	TTimeIntervalMicroSeconds iDisplayDuration;
	TRect iDirtyRegion;
	};
	
#endif
