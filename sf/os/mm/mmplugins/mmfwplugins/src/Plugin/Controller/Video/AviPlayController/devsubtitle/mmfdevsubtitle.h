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

#ifndef MMFDEVSUBTITLE_H
#define MMFDEVSUBTITLE_H

#include <e32lang.h>
#include <e32std.h>
#include <mmf/common/mmfvideosubtitlecustomcommands.h>

#include "mmfdevsubtitlecallback.h"
#include "subtitlesource.h"
#include "subtitletimer.h"
#include "subtitlerotation.h"

// Forward declarations
class CMMFSubtitleGraphic;
class CSrtSubtitleDecoder;

NONSHARABLE_CLASS(CMMFDevSubtitle) : public CBase, public MMMFDevSubtitleCallback
	{
public:
	IMPORT_C static CMMFDevSubtitle* NewL(MSubtitleSource& aSubtitleSource);
	IMPORT_C static CMMFDevSubtitle* NewLC(MSubtitleSource& aSubtitleSource);
	IMPORT_C ~CMMFDevSubtitle();
	
	// from MMFDevSubtitleCallback
	void RotationComplete(TInt aResult);
	void SubtitleTimerExpired();
	
	IMPORT_C void AddSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfig);
	IMPORT_C void GetCrpParametersL(TInt aWindowId, TWsGraphicId& aCrpId, TRect& aRegion);
	IMPORT_C void GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages);
	IMPORT_C TTimeIntervalMicroSeconds Position();
	IMPORT_C void RemoveSubtitleConfigL(TInt aWindowId);
	
	IMPORT_C void SelectDecoderL(const TDesC& aRequestedDecoder);
	IMPORT_C void SetSubtitleLanguageL(TLanguage aLanguage);
	IMPORT_C void SetVideoPositionL(const TTimeIntervalMicroSeconds& aCurrentPosition);
	IMPORT_C void Start();
	IMPORT_C void Stop();
	IMPORT_C TLanguage SubtitleLanguageL();
	IMPORT_C void UpdateSubtitleConfigL(const TMMFSubtitleWindowConfig& aConfiguration);

private:
	CMMFDevSubtitle(MSubtitleSource& aSubtitleSource);
	void ConstructL();
	
	// Utility methods
	void DisplayCurrentSubtitleFrame();
	void ScheduleNextFrame();
	void TranslateWindowDimensions(const TRect& aSource, TRect& aTarget);
	void TranslateRegion(const TRect& aSource, TRect& aTarget);
	
	void CancelAndClear();
	void Panic(TInt aReason);
		
private:
	enum TSubtitleGraphicFrame
		{
		ESubtitleFrame1 = 1,
		ESubtitleFrame2 = 2
		};
		
	enum TDevSubtitleState
		{
		EDevSubtitleStateWaiting = 10,
		EDevSubtitleStateStopped = 20,
		EDevSubtitleStatePlaying = 30
		};
	
	enum TDevSubtitlePanic
		{
		EDevSubtitlePanicNoConfiguration = 10,
		EDevSubtitlePanicDecoderNotSet	 = 20,
		EDevSubtitlePanicAlreadyStarted	 = 30,
		EDevSubtitlePanicNotStarted  	 = 40,
		EDevSubtitlePanicInvalidFrame	 = 50
		};

private:
	/**
	Current subtitle region.
	*/	
	TRect iSubtitleRegion;
	
	/**
	Subtitle frames.
	*/
	CFbsBitmap* iSubtitleFrame1;
	CFbsBitmap* iSubtitleFrame2;
	
	/**
	Used for rotating subtitle frames as required
	*/
	CFbsBitmap *iRotationBuffer;
	
	/**
	Current frame set to display
	*/
	TSubtitleGraphicFrame iCurrentSubtitleFrame;
	
	/**
	Dirty region for current subtitle frame.  Used by CRP drawer for optimized drawing
	*/
	TRect iCurrentDirtyRegion;
	
	/**
	Subtitle Decoder
	*/
	CSrtSubtitleDecoder *iDecoder;
	
	/**
	Data stream source for the decoder
	*/
	MSubtitleSource& iSubtitleSource;
	
	/**
	CRP Subtitle graphic drawer
	*/
	CMMFSubtitleGraphic* iSubtitleGraphic;
	
	/**
	Helper classes
	*/
	CSubtitleTimer* iSubtitleTimer;
	CSubtitleRotation* iSubtitleRotation;
		
	TTime iRotationStartTime;
	TTimeIntervalMicroSeconds iSubtitleFrameDuration;
	TTimeIntervalMicroSeconds iSubtitleFrameStartTime;
	
	/**
	The current subtitle configuration
	*/
	TMMFSubtitleWindowConfig iCurrentSubtitleConfiguration;
	
	/**
	Video position when video was started
	*/
	TTimeIntervalMicroSeconds iVideoPositionAtStart;
	
	/**
	System time when video was started
	*/
	TTime iVideoPositionStartTime;
	
	/**
	System time when video was stopped
	*/
	TTimeIntervalMicroSeconds iVideoPositionStoppedTime;
	
	/**
	Current Devsubtitle state
	*/
	TDevSubtitleState iState;	
	};

#endif // MMFDEVSUBTITLE_H
