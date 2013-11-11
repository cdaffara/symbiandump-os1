// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef VIDEOPLAYER2_H
#define VIDEOPLAYER2_H

#include <videoplayer.h>

class MMMFSurfaceEventHandler;

/**
@publishedPartner
@released

Extension class from CVideoPlayerUtility to allow graphics surface support.
*/
class CVideoPlayerUtility2 : public CVideoPlayerUtility
	{
public:

	~CVideoPlayerUtility2();
	IMPORT_C static CVideoPlayerUtility2* NewL(MVideoPlayerUtilityObserver& aObserver,
											  TInt aPriority,
											  TInt aPref);
										
	IMPORT_C void AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, 
									RWindow& aWindow, const TRect& aVideoExtent, 
									const TRect& aWindowClipRect);

	IMPORT_C void AddDisplayWindowL(RWsSession& aWs, CWsScreenDevice& aScreenDevice, RWindow& aWindow);

 	IMPORT_C void RemoveDisplayWindow(RWindow& aWindow);
 	
 	IMPORT_C void SetVideoExtentL(const RWindow& aWindow, const TRect& aVideoExtent);
	
	IMPORT_C void SetWindowClipRectL(const RWindow& aWindow, const TRect& aWindowClipRect);
	
	IMPORT_C void SetRotationL(const RWindow& aWindow, TVideoRotation aRotation);
	
	IMPORT_C TVideoRotation RotationL(const RWindow& aWindow);
	
	IMPORT_C void SetScaleFactorL(const RWindow& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage);
	
	IMPORT_C void GetScaleFactorL(const RWindow& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage);
	
	IMPORT_C void SetAutoScaleL(const RWindow& aWindow, TAutoScaleType aScaleType);

    IMPORT_C void SetAutoScaleL(const RWindow& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos);

    IMPORT_C void AddDisplayL(RWsSession& aWs, TInt aDisplay, MMMFSurfaceEventHandler& aEventHandler);
    
    IMPORT_C void RemoveDisplay(TInt aDisplay);    
    
    IMPORT_C void SetExternalDisplaySwitchingL(TInt aDisplay, TBool aControl);

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	IMPORT_C TBool SubtitlesAvailable();
	
	IMPORT_C void EnableSubtitlesL();
	
	IMPORT_C void DisableSubtitles();
	
	IMPORT_C TLanguage SubtitleLanguageL();
	
	IMPORT_C TArray<TLanguage> SupportedSubtitleLanguagesL();
	
	IMPORT_C void SetSubtitleLanguageL(TLanguage aLanguage);
	
	IMPORT_C void RedrawSubtitle(RWindow& aWindow, const TRect &aRect);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

private:
	friend class CTestStepUnitMMFVidClient;
	};

#endif // VIDEOPLAYER2_H
