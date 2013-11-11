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

#include <mmf/plugin/mmfsubtitlegraphic.h>
#include "mmfdevsubtitle.h"
#include "srtdecoder.h"

_LIT(KDecoderString, "srtdecoder");

CMMFDevSubtitle::CMMFDevSubtitle(MSubtitleSource& aSubtitleSource):
	iSubtitleSource(aSubtitleSource)
	{
	
	}

EXPORT_C CMMFDevSubtitle::~CMMFDevSubtitle()
	{
	delete iDecoder;
	delete iSubtitleFrame1;
	delete iSubtitleFrame2;
	delete iRotationBuffer;
	delete iSubtitleGraphic;
	delete iSubtitleTimer;
	delete iSubtitleRotation;
	}
	
EXPORT_C CMMFDevSubtitle* CMMFDevSubtitle::NewL(MSubtitleSource& aSubtitleSource)
	{
	CMMFDevSubtitle* self = NewLC(aSubtitleSource);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CMMFDevSubtitle* CMMFDevSubtitle::NewLC(MSubtitleSource& aSubtitleSource)
	{
	CMMFDevSubtitle* self = new (ELeave) CMMFDevSubtitle(aSubtitleSource);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CMMFDevSubtitle::ConstructL()
	{
	iSubtitleFrame1 = new (ELeave) CFbsBitmap();
	iSubtitleFrame2 = new (ELeave) CFbsBitmap();
	iRotationBuffer = new (ELeave) CFbsBitmap();
	
	iSubtitleTimer    = CSubtitleTimer::NewL(*this);
	iSubtitleRotation = CSubtitleRotation::NewL(*this);	
	
	iCurrentSubtitleFrame = ESubtitleFrame1;
	iState = EDevSubtitleStateWaiting;
	}

EXPORT_C void CMMFDevSubtitle::AddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig)
	{
	if (iState != EDevSubtitleStateWaiting)
		{
		TInt err = iCurrentSubtitleConfiguration.iWindowId == aConfig.iWindowId ? KErrArgument : KErrInUse;
		User::Leave(err);
		}
	
	UpdateSubtitleConfigL(aConfig);
	}
	
// From MMMFDevSubtitleCallback
void CMMFDevSubtitle::RotationComplete(TInt aResult)
	{
	if (aResult == KErrCancel)
		{
		return;
		}

	if ( KErrNone == aResult )	
		{
		TTime currentSystemTime;
		currentSystemTime.UniversalTime();
		TInt32 timeTakenRotation = currentSystemTime.Int64()-iRotationStartTime.Int64();
			
		//
		// Check to see if the subtitle is due now
		if ( timeTakenRotation > iSubtitleFrameStartTime.Int64() ) 
			{
			// Display the frame as fast as possible.
			
			// Update the subtitle frame duration to take account of how long rotation processing took
			iSubtitleFrameDuration = iSubtitleFrameDuration.Int64() - timeTakenRotation;
			
			if ( iSubtitleFrameDuration < 0 )
				{
				// Rotation took so long the frame is now no longer valid skip it
				ScheduleNextFrame();
				}
			else
				{
				// Frame is still valid display...
				SubtitleTimerExpired();	
				}
			}
		else
			{	
			// Frame is due in the future schedule a callback
			TTimeIntervalMicroSeconds32 temp(iSubtitleFrameStartTime.Int64()-timeTakenRotation);
			iSubtitleTimer->StartTimer(temp);
			}		
		}
		else
		{
		// Rotation failed, skip this frame and schedule the next one
		ScheduleNextFrame();	
		}
		
	}

void CMMFDevSubtitle::SubtitleTimerExpired()
	{
	__ASSERT_DEBUG(!iSubtitleRotation->IsActive(), User::Invariant());
	
	// Subtitle frame timer exposed draw the current frame.
	DisplayCurrentSubtitleFrame();
	
	// Swap  current subtitle frame from frame 1 to frame 2 or vice versa.
	iCurrentSubtitleFrame = (iCurrentSubtitleFrame == ESubtitleFrame1) ? ESubtitleFrame2 : ESubtitleFrame1;
	
	ScheduleNextFrame();
	}
	
// End - From MMMFDevSubtitleCallback
EXPORT_C void CMMFDevSubtitle::GetCrpParametersL(TInt aWindowId, TWsGraphicId& aCrpId, TRect& aRegion)
	{
	if (iState == EDevSubtitleStateWaiting)
		{
		User::Leave(KErrNotReady);
		}

	// Leave with KErrNotFound if the (only) added configuration is not the one we're retrieving parameters for.
	if (aWindowId != iCurrentSubtitleConfiguration.iWindowId)
		{
		User::Leave(KErrNotFound);
		}
	else if (aWindowId < 0)
		{
		User::Leave(KErrArgument);
		}
	
	aCrpId = iSubtitleGraphic->Id();
	aRegion = iSubtitleRegion;
	}

EXPORT_C void CMMFDevSubtitle::GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages)
	{
	if (!iDecoder)
		{
		User::Leave(KErrNotReady);
		}
	
	iDecoder->GetSupportedSubtitleLanguagesL(aLanguages);
	}

EXPORT_C TTimeIntervalMicroSeconds CMMFDevSubtitle::Position()
	{
	if (iState == EDevSubtitleStatePlaying)
		{
		TTime currentTime;
		currentTime.UniversalTime();
		return currentTime.MicroSecondsFrom(iVideoPositionStartTime - iVideoPositionAtStart);
		}
	else
		{
		// Return the last position when stopped
		return iVideoPositionStoppedTime;
		}
	}

EXPORT_C void CMMFDevSubtitle::RemoveSubtitleConfigL(TInt aWindowId)
	{
	if (iState == EDevSubtitleStateWaiting)
		{
		User::Leave(KErrNotReady);
		}
	
	// Leave with KErrNotFound if the (only) added configuration does not match the one being removed.
	if (aWindowId != iCurrentSubtitleConfiguration.iWindowId)
		{
		User::Leave(KErrNotFound);
		}
	
	// Return to waiting state.
	iState = EDevSubtitleStateWaiting;
	
	CancelAndClear();
	
	delete iSubtitleGraphic;
	iSubtitleGraphic = NULL;
	}
	
EXPORT_C void CMMFDevSubtitle::SelectDecoderL(const TDesC& aRequestedDecoder)
	{
	if (iState != EDevSubtitleStateWaiting)
		{
		User::Leave(KErrNotReady);
		}
	
	if ( aRequestedDecoder.Compare(KDecoderString) == 0 )
		{
		delete iDecoder;
		iDecoder = NULL;
		
		iDecoder = CSrtSubtitleDecoder::NewL(iSubtitleSource);
		}
	else
		{
		// We only support the SRT decoder
		User::Leave(KErrNotFound);
		}
	}

EXPORT_C void CMMFDevSubtitle::SetSubtitleLanguageL(TLanguage aLanguage)
	{
	if (!iDecoder)
		{
		User::Leave(KErrNotReady);
		}
	
	iDecoder->SetSubtitleLanguageL(aLanguage);
	}

EXPORT_C void CMMFDevSubtitle::SetVideoPositionL(const TTimeIntervalMicroSeconds& aCurrentPosition)
	{
	__ASSERT_ALWAYS(iDecoder, Panic(EDevSubtitlePanicDecoderNotSet));
	
	if (aCurrentPosition.Int64() < 0)
		{
		User::Leave(KErrArgument);
		}
	
	iVideoPositionAtStart = aCurrentPosition;
	iVideoPositionStartTime.UniversalTime();
	
	iDecoder->SetVideoPosition(aCurrentPosition);
	
	if (iState == EDevSubtitleStatePlaying)
		{	
		CancelAndClear();
		
		// Get next frame for drawing
		ScheduleNextFrame();
		}
	else if (iState == EDevSubtitleStateStopped)
		{
		iVideoPositionStoppedTime = aCurrentPosition;
		}
	}

EXPORT_C void CMMFDevSubtitle::Start()
	{
	__ASSERT_ALWAYS(iState != EDevSubtitleStatePlaying, Panic(EDevSubtitlePanicAlreadyStarted));
	__ASSERT_ALWAYS(iDecoder, Panic(EDevSubtitlePanicDecoderNotSet));
	
	// Ignore start command if a configuration has not been added yet.
	if (iState == EDevSubtitleStateStopped)
		{
		iState = EDevSubtitleStatePlaying; // Set the state to playing
		iVideoPositionStartTime.UniversalTime();
		iDecoder->Start();
		ScheduleNextFrame(); // Start playing from the current position
		}
	}
	
EXPORT_C void CMMFDevSubtitle::Stop()
	{
	if (iDecoder)
		{
		if ( iState == EDevSubtitleStatePlaying )
			{
			iState = EDevSubtitleStateStopped;
			
			// Reset video position.  Ignore trap because this function only leaves when input
			// parameter is less than zero.
			TRAP_IGNORE(SetVideoPositionL(0)); 
			
			CancelAndClear();
			
			iVideoPositionStoppedTime = Position();
			}
			
		iDecoder->Stop();
		}
	}

EXPORT_C TLanguage CMMFDevSubtitle::SubtitleLanguageL()
	{	
	if (!iDecoder)
		{
		User::Leave(KErrNotReady);
		}
	
	return iDecoder->SubtitleLanguageL();
	}
	
/**
Translate source window dimensions based on the current video rotation setting.
*/
void CMMFDevSubtitle::TranslateWindowDimensions(const TRect& aSource, TRect& aTarget)
	{
	switch (iCurrentSubtitleConfiguration.iRotation)
		{
		case EVideoRotationNone:
			aTarget = aSource;
			break;
			
		case EVideoRotationClockwise180:
			aTarget.iTl = TPoint(0, 0);
			aTarget.iBr = TPoint(aSource.Width(), aSource.Height());
			break;
			
		case EVideoRotationClockwise90:
		case EVideoRotationClockwise270:
			aTarget.iTl = TPoint(0, 0);
			aTarget.iBr = TPoint(aSource.Height(), aSource.Width());
			break;
			
		default:
			break;
		};
	}

/**
Translate source region based on the current video rotation setting.  This provides the correct
window coordinates for rendering
*/
void CMMFDevSubtitle::TranslateRegion(const TRect& aSource, TRect& aTarget)
	{
	TInt winHeight = iCurrentSubtitleConfiguration.iWindowClipRect.Height();
	TInt winWidth = iCurrentSubtitleConfiguration.iWindowClipRect.Width();

	switch (iCurrentSubtitleConfiguration.iRotation)
		{
		case EVideoRotationNone:
			aTarget = aSource;
			break;
				
		case EVideoRotationClockwise90:
			// Rotate the region and takes account of the clip rect origin.
			// When rotation is 90 degree, X coordinate is transformed into Y coordinate
			// by subtracting from the target width and X coordinate become Y coordinate
			aTarget.iTl = TPoint(winWidth - aSource.iBr.iY, aSource.iTl.iX);
			aTarget.iBr = TPoint(winWidth - aSource.iTl.iY, aSource.iBr.iX);
			aTarget.Move(iCurrentSubtitleConfiguration.iWindowClipRect.iTl);
			break;
			
		case EVideoRotationClockwise180:
			// Rotate the region and takes account of the clip rect origin.
			// When rotation is 180 degree, iTl and iBr are subtracted from target width 
			// and height and swapped
			aTarget.iTl = TPoint(winWidth - aSource.iBr.iX, winHeight - aSource.iBr.iY);
			aTarget.iBr = TPoint(winWidth - aSource.iTl.iX, winHeight - aSource.iTl.iY);
			aTarget.Move(iCurrentSubtitleConfiguration.iWindowClipRect.iTl);
			break;
		
		case EVideoRotationClockwise270:			
			// Rotate the region and takes account of the clip rect origin.
			// When rotation is 270 degree, Y coordinate is transformed into X coordinate
			// by subtracting from the target width and Y coordinate become X coordinate
			aTarget.iTl = TPoint(aSource.iTl.iY, winHeight - aSource.iBr.iX);
			aTarget.iBr = TPoint(aSource.iBr.iY, winHeight - aSource.iTl.iX);
			aTarget.Move(iCurrentSubtitleConfiguration.iWindowClipRect.iTl);
			break;
			
		default:
			break;
		};
	}

EXPORT_C void CMMFDevSubtitle::UpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfiguration)
	{
	if (!iDecoder)
		{
		User::Leave(KErrNotReady);
		}
	
	if (aConfiguration.iWindowClipRect.IsEmpty())
		{
		User::Leave(KErrArgument);
		}
	
	if (aConfiguration.iDisplayMode <  EColor16MA || aConfiguration.iDisplayMode >= EColorLast)
		{
		User::Leave(KErrArgument);
		}
	
	// If the state is not Waiting, e.g. a config has already been added, then the new window
	// ID must match the ID that was previously added.  
	if (iState != EDevSubtitleStateWaiting && iCurrentSubtitleConfiguration.iWindowId != aConfiguration.iWindowId)
		{
		User::Leave(KErrNotFound);
		}
	
	iCurrentSubtitleConfiguration = aConfiguration;
	
	// Cancel timer and rotation requests and clear the CRP
	CancelAndClear();
	
	if (iSubtitleGraphic)
		{
		delete iSubtitleGraphic;
		iSubtitleGraphic = NULL;
		}
	
	iSubtitleGraphic = CMMFSubtitleGraphic::NewL();	
	
	if ( iCurrentSubtitleConfiguration.iRotation == EVideoRotationNone )
		{
		User::LeaveIfError(iDecoder->CalculateSubtitleRegion(iCurrentSubtitleConfiguration.iWindowClipRect, iSubtitleRegion));
		}
	else
		{
		// Translate window dimensions to match rotation setings
		TRect tranScreenSize;
		TranslateWindowDimensions(iCurrentSubtitleConfiguration.iWindowClipRect, tranScreenSize);
		
		// Subtitle frames will require rotation before being displayed. Create the tempory buffer used
		// for generating rotated frames.
		TRect subtitleRegion;
		User::LeaveIfError(iDecoder->CalculateSubtitleRegion(tranScreenSize, subtitleRegion));
		
		// Need to translate the rotated region to the windows orientation for correct rendering
		TranslateRegion(subtitleRegion, iSubtitleRegion);
		
		iRotationBuffer->Reset();
		
		// Create the rotated region buffer
		User::LeaveIfError(iRotationBuffer->Create(subtitleRegion.Size(), EColor16MA));
		}
				
	// Use the display mode and subtitle region to create the subtitle frame bitmap objects
	// Note these two buffers are always in portrait mode as these are passed to the CRP for drawing
	iSubtitleFrame1->Reset();
	iSubtitleFrame2->Reset();
	User::LeaveIfError(iSubtitleFrame1->Create(iSubtitleRegion.Size(), EColor16MA));
	User::LeaveIfError(iSubtitleFrame2->Create(iSubtitleRegion.Size(), EColor16MA));
	
	if ( iCurrentSubtitleConfiguration.iRotation == EVideoRotationNone )
		{
		// Initialise the CRP
		User::LeaveIfError(iSubtitleGraphic->Initialize(iSubtitleFrame1->Handle(), iSubtitleFrame2->Handle()));
		}
	else
		{
		// Rotation invalidates bitmap handles so we need to 
		// Reinitialize the crp with the new bitmap handles
		iSubtitleGraphic->Initialize();
		}
	
	// Transition to stopped state if not in playing state.
	if (iState == EDevSubtitleStateWaiting)
		{
		iState = EDevSubtitleStateStopped;
		}
	else if (iState == EDevSubtitleStatePlaying)
		{
		ScheduleNextFrame();
		}
	}

void CMMFDevSubtitle::DisplayCurrentSubtitleFrame()
	{
	__ASSERT_ALWAYS(iState == EDevSubtitleStatePlaying, Panic(EDevSubtitlePanicNotStarted));
	
	if ( iCurrentSubtitleConfiguration.iRotation == EVideoRotationNone )
		{
		iSubtitleGraphic->SwapFrame(iCurrentSubtitleFrame, iCurrentDirtyRegion, iSubtitleFrameDuration);	
		}
	else
		{	
		// Rendering in rotation mode.. display the current frame using a drawframe method.
		// We need to do this because the CBitmapRotator utility resets the bitmap handle
		iSubtitleGraphic->DrawFrame(iSubtitleFrame1->Handle(), iCurrentDirtyRegion, iSubtitleFrameDuration);
		}
	}

void CMMFDevSubtitle::ScheduleNextFrame()
	{
	TInt err = KErrNone;
	
	if (iCurrentSubtitleConfiguration.iRotation != EVideoRotationNone)
		{
		TRAP(err, iDecoder->GetNextFrameL(*iRotationBuffer, iCurrentDirtyRegion, iSubtitleFrameStartTime, iSubtitleFrameDuration));
		iRotationStartTime.UniversalTime();
		
		// Translate the dirty region back to screen coodinates for correct rendering
		TRect temp(iCurrentDirtyRegion);
		TranslateRegion(temp, iCurrentDirtyRegion);		
		TSize size = iSubtitleFrame1->SizeInPixels();
		iCurrentDirtyRegion=TRect(TPoint(0,0), TPoint(size.iWidth, size.iHeight));
		
		if ( err == KErrNone )
			{		
			// We always rendering to frame1 as a target
			iSubtitleRotation->StartRotation(*iRotationBuffer, *iSubtitleFrame1, iCurrentSubtitleConfiguration.iRotation);
			}
		}
	else
		{
		switch (iCurrentSubtitleFrame)
			{
		case ESubtitleFrame1:
			TRAP(err, iDecoder->GetNextFrameL(*iSubtitleFrame1, iCurrentDirtyRegion, iSubtitleFrameStartTime, iSubtitleFrameDuration));
			break;
		
		case ESubtitleFrame2:
			TRAP(err, iDecoder->GetNextFrameL(*iSubtitleFrame2, iCurrentDirtyRegion, iSubtitleFrameStartTime, iSubtitleFrameDuration));
			break;
				
		default:
			// Should never happen
			Panic(EDevSubtitlePanicInvalidFrame);
			break;
			}
			
		if ( err == KErrNone )
			{
			// Schedule subtitle frame drawing
			TTimeIntervalMicroSeconds32 temp(iSubtitleFrameStartTime.Int64());
			iSubtitleTimer->StartTimer(temp);
			}
		}
	
	if (err == KErrEof)
		{
		// End of subtitle file reached reset
		iState=EDevSubtitleStateStopped;
		TRAP_IGNORE(SetVideoPositionL(0));
		}
	}


void CMMFDevSubtitle::CancelAndClear()
	{
	// Clear any current subtitle contents from the screen
	if (iSubtitleGraphic)
		{
		iSubtitleGraphic->Clear();
		}
	
	// Cancel any outstanding subtitle frames
	iSubtitleTimer->Cancel();

	// Cancel bitmap rotation.
	iSubtitleRotation->Cancel();
	}

void CMMFDevSubtitle::Panic(TInt aReason)
	{
	_LIT(KDevSubtitlePanicCat, "DEVSUBTITLE");
	User::Panic(KDevSubtitlePanicCat, aReason);
	}
