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

#include <graphics/wsgraphicscontext.h>
#include "mmfsubtitlegraphicdrawer.h"

// Compensation value in microseconds to account for differences in the requested callback time and the actual
// time the callback is delivered
static const TInt KClockDrift=100000;

CMMFSubtitleGraphicDrawer* CMMFSubtitleGraphicDrawer::NewL()
    {
    CMMFSubtitleGraphicDrawer* self = new(ELeave) CMMFSubtitleGraphicDrawer();  
    return self;
    }
    
CMMFSubtitleGraphicDrawer::~CMMFSubtitleGraphicDrawer()
    {
    delete iBitmap1;
    delete iBitmap2;
    delete iTempBitmap;
    }

void CMMFSubtitleGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment& aEnv, const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& /*aData*/)
    {
    // Note this method is called by the baseclass not CMMFSubtitleGraphicDrawer
    BaseConstructL(aEnv, aId, aOwner);
    
    // Construct bitmap objects.  Do this here as we can leave usefully with
    // any construction errors
	iBitmap1 = new (ELeave) CFbsBitmap();
	iBitmap2 = new (ELeave) CFbsBitmap();
	iTempBitmap = new (ELeave) CFbsBitmap();
    }


// From CwsGraphicDrawer
void CMMFSubtitleGraphicDrawer::HandleMessage(const TDesC8& aData)
    {   
    // Retreive message type from buffer
    TInt8 msgType = aData[0];
    
    switch (msgType)
    	{    	
	case ESubtitleCrpMessageInit:
		{
		iCaptureRegion = ETrue;
		TPckgBuf<TSubtitleCrpMsgInit> message;
		message.Copy(aData);
		ProcessMessageInit(message());
		}
		break;
		
	case ESubtitleCrpMessageInitSimple:
		iCaptureRegion = ETrue;
		iBitmap1Valid=EFalse;
    	iBitmap2Valid=EFalse;
    	iTempBitmapValid=ETrue;
		break;
		
	case ESubtitleCrpMessageDrawFrame:
		{
		iState = ESubtitleGraphicStateDrawFrame;
		TPckgBuf<TSubtitleCrpMsgDrawFrame> message;
		message.Copy(aData);
		ProcessMessageDrawFrame(message());
		}
		break;
		
	case ESubtitleCrpMessageSwapFrame:
		{
		iState = ESubtitleGraphicStateSwapFrame;
		TPckgBuf<TSubtitleCrpMsgRenderSwapFrame> message;
		message.Copy(aData);
        ProcessMessageSwapFrame(message());
		}
        break;
    	
    case ESubtitleCrpMessageClear:
    	iDisplayClearDue.UniversalTime();
    	iState = ESubtitleGraphicStateClear;
    	break;
		
	default:
		// Silently ignore unrecognised messages
		// Clear messages for example require no further processing
		// Set the state to waiting so DoDraw will do nothing.
		iState = ESubtitleGraphicStateWaiting;
	break;
    	}
    
    // Note clear messages require no further processing we just need to invalidate the display

	// Ask WSERV to redraw the CRP
    Invalidate();
    }

void CMMFSubtitleGraphicDrawer::DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& /*aData*/) const
	{	
	// For some crp rendering states we need to ensure that this DoDraw has been called
	// in response to Invalidate() and not generated in response to a portion of the CRP
	// becoming invald for some reason (menus, dialogs etc)
	
	// Store the invalid region as our subtitle region to complete initialization.
	if (iCaptureRegion)
		{
		iSubtitleRegion = aRect;
		iCaptureRegion = EFalse;
		}
	
	// If the current state is waiting we do dont need to do anything
	if ((iSubtitleRegion.IsEmpty())||(iState==ESubtitleGraphicStateWaiting))
		{
		return;
		}
	
	switch(iState)
	    {   	
	case ESubtitleGraphicStateDrawFrame:
	case ESubtitleGraphicStateSwapFrame:	
		// Blt new content
		DoBitBlt(aGc, iDirtyRegion);
		
		if ( iDisplayDuration.Int64() > 0 )
			{
			// Schedule a clear frame to remove this subtitle after the requested time
		    // Note clear messages require no further processing we just need to schedule a redraw
			aGc.ScheduleAnimation(iSubtitleRegion, iDisplayDuration.Int64());
			iDisplayClearDue.UniversalTime();
			iDisplayClearDue=iDisplayDuration.Int64()+iDisplayClearDue.Int64()-KClockDrift;
			iState = ESubtitleGraphicStateClear;
			}
		else
			{
			// Redraw this frames content until instructed to stop
			iState = ESubtitleGraphicStateRefreshContent;
			}
	    break;
	
	case ESubtitleGraphicStateRefreshContent:
		// Refresh current frame content
		DoBitBlt(aGc, iDirtyRegion);
		break;
		
	case ESubtitleGraphicStateClear:
		{
		// Clear is due, check to see if the clear is due now
		TTime timeNow;
		timeNow.UniversalTime();
		TInt diff=iDisplayClearDue.Int64()-timeNow.Int64();
		
		if ( diff > 0 )
			{
			// Clear is not due do a standard bitblt		
			DoBitBlt(aGc, iDirtyRegion);
			}
		else
			{
			// No further processing required as WSERV clears the region for us
			iState = ESubtitleGraphicStateWaiting;
			}
		}
		break;
		
	default:
		// Should never happen
#ifdef _DEBUG
		RDebug::Printf("CMMFSubtitleGraphicDrawer::DoDraw() - Invalid State in Drawer should never happen");
		User::Invariant();
#endif
	    break;
	    }
	}

/**
Renders the current bitmap frame
*/
void CMMFSubtitleGraphicDrawer::DoBitBlt(MWsGc& aGc, const TRect& aRect) const
	{
	MWsGraphicsContext* context = aGc.ObjectInterface<MWsGraphicsContext>();
	if(!context)
		{
		RDebug::Printf("CMMFSubtitleGraphicDrawer::DoBitBlt can not get MWsGraphicsContext");
		return;
		}
	context->Push();
	
	switch (iCurrentFrame)
		{
		case ESubtitleGraphicFrame1:
			if (iBitmap1Valid)
				{
				context->BitBlt(iSubtitleRegion.iTl + aRect.iTl, *iBitmap1, aRect);
				}
			break;
		
		case ESubtitleGraphicFrame2:
			if (iBitmap2Valid)
				{
				context->BitBlt(iSubtitleRegion.iTl + aRect.iTl, *iBitmap2, aRect);
				}
			break;
		
		case ESubtitleGraphicFrame3:
			if (iTempBitmapValid)
				{
				context->BitBlt(iSubtitleRegion.iTl + aRect.iTl, *iTempBitmap, aRect);
				}
			break;
		
		default:
			break;
		}

	context->Pop();	
	}

/**
Process initialization message from client.  Readies the CRP for drawing
@param aMessage Message data received from client
*/
void CMMFSubtitleGraphicDrawer::ProcessMessageInit(TSubtitleCrpMsgInit& aMessage)
	{
	// Duplicate client bitmap handles
	// These are verified by the client before they are sent
	iBitmap1->Reset();
	TInt err = iBitmap1->Duplicate(aMessage.iBitmapHandle1);
	
	// This should never be a problem but check in debug modes just in case
	// We dont do this in release mode as it panics the window server...
	__ASSERT_DEBUG(KErrNone==err, User::Invariant());
	
	iBitmap2->Reset();
	err = iBitmap2->Duplicate(aMessage.iBitmapHandle2);
	// This should never be a problem but check in debug modes just in case
	// We dont do this in release mode as it panics the window server...
	__ASSERT_DEBUG(KErrNone==err, User::Invariant());
	
	iCurrentFrame=ESubtitleGraphicFrame1;
	iBitmap1Valid=ETrue;
	iBitmap2Valid=ETrue;
	iTempBitmapValid=EFalse;
	}

/**
Process draw frame message from client.  Draw the bitmap provided.
@param aMessage Message data received from client
*/
void CMMFSubtitleGraphicDrawer::ProcessMessageDrawFrame(TSubtitleCrpMsgDrawFrame& aMessage)
	{
	// Duplicate client bitmap handle
	iTempBitmap->Reset();
	iTempBitmap->Duplicate(aMessage.iBitmapHandle);
	
    iOldDirtyRegion=iDirtyRegion;
	iDirtyRegion=aMessage.iDirtyRegion;
		
    iDisplayDuration=aMessage.iDisplayDuration;
    iCurrentFrame=ESubtitleGraphicFrame3;
	}

/**
Process swap frame message from client.  Draws the current backbuffer provided.
@param aMessage Message data received from client
*/
void CMMFSubtitleGraphicDrawer::ProcessMessageSwapFrame(TSubtitleCrpMsgRenderSwapFrame& aMessage)
	{
	__ASSERT_DEBUG((aMessage.iExpectedFrame==ESubtitleGraphicFrame1)||(aMessage.iExpectedFrame==ESubtitleGraphicFrame2), User::Invariant());
	
	iDisplayDuration = aMessage.iDisplayDuration;
	iOldDirtyRegion=iDirtyRegion;
	iDirtyRegion = aMessage.iDirtyRegion;
	
	// Swap the frame pointer
	iCurrentFrame = static_cast<TSubtitleGraphicFrame>(aMessage.iExpectedFrame);
	}
