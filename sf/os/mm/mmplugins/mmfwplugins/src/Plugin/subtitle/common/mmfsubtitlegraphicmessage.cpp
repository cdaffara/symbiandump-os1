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

#include "mmfsubtitlegraphicmessage.h"

/**
TSubtitleGraphicMessageBase

@param aMsgType Message type identifier.
*/
TSubtitleGraphicMessageBase::TSubtitleGraphicMessageBase(TSubtitleGraphicMessage aMsgType) : iMsgType(aMsgType)
	{

	}

/**
Default constructor
*/
TSubtitleCrpMsgInit::TSubtitleCrpMsgInit() : 
										TSubtitleGraphicMessageBase(ESubtitleCrpMessageInit)
	{

	}

/**
Message constructor for CRP ESubtitleCrpMessageInit
@param aBitmapHandle1 Bitmap handle for frame 1 @see CFbsBitmap::Handle()
@param aBitmapHandle2 Bitmap handle for frame 2 @see CFbsBitmap::Handle()
*/
TSubtitleCrpMsgInit::TSubtitleCrpMsgInit(TInt aBitmapHandle1, TInt aBitmapHandle2) : 
										TSubtitleGraphicMessageBase(ESubtitleCrpMessageInit),
										iBitmapHandle1(aBitmapHandle1),
										iBitmapHandle2(aBitmapHandle2)
	{

	}

/**
Message constructor for CRP ESubtitleCrpMessageInitSimple
*/
TSubtitleCrpMsgInitSimple::TSubtitleCrpMsgInitSimple() :
						   TSubtitleGraphicMessageBase(ESubtitleCrpMessageInitSimple)
	{
	
	}

/**
Message constructor for CRP ESubtitleCrpMessageClear
*/
TSubtitleCrpMsgClear::TSubtitleCrpMsgClear() : TSubtitleGraphicMessageBase(ESubtitleCrpMessageClear)
	{

	}

/**
Message constructor for CRP ESubtitleCrpMessageDrawFrame
*/
TSubtitleCrpMsgDrawFrame::TSubtitleCrpMsgDrawFrame( ): 
													TSubtitleGraphicMessageBase(ESubtitleCrpMessageDrawFrame)
	{

	}
	
/**
Message constructor for CRP ESubtitleCrpMessageInit
@param aBitmapHandle Bitmap handle for drawframe request @see CFbsBitmap::Handle()
@param aDirtyRegion Dirty region in the source bitmap tha should be copied to the screen
@param aDisplayDuration Number of microseconds this frame should be displayed for
*/
TSubtitleCrpMsgDrawFrame::TSubtitleCrpMsgDrawFrame(TInt aBitmapHandle, 
												   const TRect& aDirtyRegion, 
												   const TTimeIntervalMicroSeconds& aDisplayDuration) : 
												   TSubtitleGraphicMessageBase(ESubtitleCrpMessageDrawFrame),
												   iBitmapHandle(aBitmapHandle),
												   iDirtyRegion(aDirtyRegion),
												   iDisplayDuration(aDisplayDuration)
	{
	
	}

/**
Message constructor for CRP ESubtitleCrpMessageSwapFrame
*/	
TSubtitleCrpMsgRenderSwapFrame::TSubtitleCrpMsgRenderSwapFrame():
		TSubtitleGraphicMessageBase(ESubtitleCrpMessageSwapFrame)
		{
		
		}

/**
Message constructor for CRP ESubtitleCrpMessageInit
@param 	aExpectedBuffer   The index number of the internal frame the crp should draw.
@param 	aDisplayDuration The time in microseconds that this frame should be displayed for. 
						 The CRP will clear this frame after aDisplayDuration microseconds
@param   aDirtyRegion    The decoder returns the region of the subtitle frame that has been
   				         updated.  i.e. the region that contains new subtitle content
*/
TSubtitleCrpMsgRenderSwapFrame::TSubtitleCrpMsgRenderSwapFrame(TInt aExpectedBuffer, 
									const TRect& aDirtyRegion,
									const TTimeIntervalMicroSeconds& aDisplayDuration):
									TSubtitleGraphicMessageBase(ESubtitleCrpMessageSwapFrame),
									iExpectedFrame(aExpectedBuffer),
									iDisplayDuration(aDisplayDuration),
									iDirtyRegion(aDirtyRegion)
	{
	
	}
