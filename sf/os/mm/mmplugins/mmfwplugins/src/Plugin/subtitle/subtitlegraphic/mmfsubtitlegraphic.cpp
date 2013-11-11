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
 @publishedPartner
 @prototype
*/

#include <s32mem.h>
#include "mmfsubtitlegraphic.h"
#include "mmfsubtitlegraphicmessage.h"

// Constants
const TUid KSubtitleGraphicDrawerImplId      = {0x10285C9C};
const TUint KMaxSupportedFrames				 = 2;

/**
Standard constructor for CMMFSubtitleGraphic
*/
CMMFSubtitleGraphic::CMMFSubtitleGraphic()
    {
    
    }

/**
Standard destructor
*/
EXPORT_C CMMFSubtitleGraphic::~CMMFSubtitleGraphic()
    {
	delete iTempBitmap;
    }

/**
Allocates and constructs a CMMFSubtitleGraphic object
*/
EXPORT_C CMMFSubtitleGraphic* CMMFSubtitleGraphic::NewL()
    {
    CMMFSubtitleGraphic* self = new(ELeave) CMMFSubtitleGraphic;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
Performs 2nd phase construction for CMMFSubtitleGraphic
*/
void CMMFSubtitleGraphic::ConstructL()
	{
	CWsGraphic::BaseConstructL(KSubtitleGraphicDrawerImplId, KNullDesC8);
	
	iTempBitmap = new (ELeave) CFbsBitmap();
	}


// From CWsGraphic
void CMMFSubtitleGraphic::OnReplace()
    {
    
    }

void CMMFSubtitleGraphic::HandleMessage(const TDesC8& /*aData*/)
	{
	
	}
// End from CWsGraphic

/**
Requests that the content rendering plugin clears all content from the window
*/
EXPORT_C void CMMFSubtitleGraphic::Clear()
	{
	TSubtitleCrpMsgClear clear;
	
    const TPckgC<TSubtitleCrpMsgClear> message(clear);
    
    SendMessage(message);
    
    // Flush window server msg queue.. msg will sit there otherwise...
    Flush();
	}

/**
Draws the bitmap frame indicated by aFrameHandle

@param aFrameHandle 	A CFbsBitmap handle, used as a source framebuffer by the CRP.
@param aDisplayDuration The time in microseconds that this frame should be 
                        displayed for.  The CRP will clear this frame after
                        aDisplayDuration microseconds.  Note if the display duration
                        is 0 the frame will be cleared when a subsequent frame is drawn,
                        or when Clear() is called.
@param aDirtyRegion 	The decoder returns the region of the subtitle frame
                     	that has been updated.  i.e. the region that contains 
                     	new subtitle content
@return  				KErrNone if successful, otherwise KErrArgument on an invalid
						aDirtyRegion/aFrameHandle, or KErrNotReady if the corresponding
						Initialize() function has not been called
*/
EXPORT_C TInt CMMFSubtitleGraphic::DrawFrame(TInt aFrameHandle, const TRect& aDirtyRegion, const TTimeIntervalMicroSeconds& aDisplayDuration)
	{
	if ((aDirtyRegion.Width()==0)||(aDirtyRegion.Height()==0))
		{
		return KErrArgument;
		}
	
	if ( KErrNone!=CheckBitmapHandle(aFrameHandle) )
		{
		return KErrArgument;
		}
		
	if ( ESubtitleGraphicStateInitSimple!=iState )
		{
		return KErrNotReady;
		}
	
	TSubtitleCrpMsgDrawFrame messageContent(aFrameHandle, aDirtyRegion, aDisplayDuration);
	TPckgBuf<TSubtitleCrpMsgDrawFrame> message(messageContent);
	
    SendMessage(message);
    
    // Flush window server msg queue.. msg will sit there otherwise...
    Flush();
    
    return KErrNone;
	}

/**
Requests that the CRP initializes.  The reference CRP duplicates and stores
both bitmap handles for future DrawFrame requests.  This function is to be 
used in conjunction with SwapFrame() only.

@param 	aBuffer1 A CFbsBitmap handle, will require duplication before use.  
		The reference CRP implementation treats this as the primary buffer 
		on startup, i.e. this handle should be used for the initial subtitle frame.
@param 	aBuffer2 A CFbsBitmap handle, will require duplication before use.
@return KErrNone if successful, otherwise KErrArgument on an invalid aBuffer1/aBuffer2
@see CFbsBitmap::Duplicate(TInt)
*/
EXPORT_C TInt CMMFSubtitleGraphic::Initialize(TInt aBuffer1, TInt aBuffer2)
	{
	if ( (CheckBitmapHandle(aBuffer1)!=KErrNone) || 
		 (CheckBitmapHandle(aBuffer2)!=KErrNone))
		{
		return KErrArgument;
		}
		
	iState = ESubtitleGraphicStateInit;
	
	TSubtitleCrpMsgInit messageContent(aBuffer1, aBuffer2);
	TPckgBuf<TSubtitleCrpMsgInit> message(messageContent);
	
    SendMessage(message);
    
    // Flush window server msg queue.. msg will sit there otherwise...
    Flush();
    
    return KErrNone;
	}

/**
Requests that the CRP initializes.  The reference CRP does nothing on 
receiving this request.  This function is to be used in conjunction with 
DrawFrame() only.
*/
EXPORT_C void CMMFSubtitleGraphic::Initialize()
	{
	TSubtitleCrpMsgInitSimple simple;
	TPckgBuf<TSubtitleCrpMsgInitSimple> message(simple);
	
	iState = ESubtitleGraphicStateInitSimple;
	   
    SendMessage(message);
    
    // Flush window server msg queue.. msg will sit there otherwise...
    Flush();
	}

/**
Requests that the CRP draws the requested internal buffer. 
The CRP maintains two internal buffers the handle for which are provided at initialisation
time. See CMMFSubtitleGraphic::Initialize(TInt aBuffer1, TInt aBuffer2).

@param 	aExpectedBuffer The index number of the internal frame the crp should draw.
@param 	aDisplayDuration The time in microseconds that this frame should be displayed for.
    					 The CRP will clear this frame after aDisplayDuration microseconds
@param   aDirtyRegion The decoder returns the region of the subtitle frame that has been
            		  updated.  i.e. the region that contains new subtitle content
@return KErrNone if successful.  KErrArgument if an invalid aExpectedBuffer, aDirtyRegion, or
aDisplayDuration are given.  KErrNotReady if the corresponding Initialize() function has not been called.
*/
EXPORT_C TInt CMMFSubtitleGraphic::SwapFrame(TUint aExpectedBuffer, const TRect& aDirtyRegion, const TTimeIntervalMicroSeconds& aDisplayDuration)
	{
	if ((aExpectedBuffer > KMaxSupportedFrames) || (aExpectedBuffer == 0))
		{
		return KErrArgument;
		}
		
	if ((aDirtyRegion.Width() <= 0)||(aDirtyRegion.Height() <= 0))
		{
		return KErrArgument;
		}
		
	if (ESubtitleGraphicStateInit!=iState)
		{
		return KErrNotReady;
		}
	
	if (aDisplayDuration < 0)
		{
		return KErrArgument;
		}
		
	TSubtitleCrpMsgRenderSwapFrame messageData(aExpectedBuffer, aDirtyRegion, aDisplayDuration);
	TPckgBuf<TSubtitleCrpMsgRenderSwapFrame> message(messageData);
		    
    SendMessage(message);
    
    // Flush window server msg queue.. msg will sit there otherwise...
    Flush();
    
    return KErrNone;
	}
	
/**
Checks that the given handle is valid
@param 	aHandle A bitmap handle
@return KErrNone if successful, otherwise KErrArgument if aHandle is invalid
@see CFbsBitmap::Handle()
*/	
TInt CMMFSubtitleGraphic::CheckBitmapHandle(TInt aHandle)
	{
	// Check framehandle is valid.
	TInt err = iTempBitmap->Duplicate(aHandle);
	
	if ( KErrUnknown==err )
		{
		return KErrArgument;
		}
	
	return err;
	}	
