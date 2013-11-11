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

#ifndef MMFSUBTITLEUTILITY_H
#define MMFSUBTITLEUTILITY_H

#include <w32std.h>
#include <mmf/common/mmfvideosubtitlecustomcommands.h>

// Utility functions for subtitle support
NONSHARABLE_CLASS( CMMFSubtitleUtility ): public CBase
	{
private:
	friend class CTestStepUnitMMFVidClient;

	// class for storing CRP related data
	class TCrpData
		{
	public:
		TCrpData(TInt aWindowId) : iWindowId(aWindowId), iCrpId(0), iCrpRect() {}

	public:
		TInt iWindowId;
		TWsGraphicId iCrpId;
		TRect iCrpRect;
		};

public:
	static CMMFSubtitleUtility* NewL(RMMFController& aController, RWsSession &aWs);
	~CMMFSubtitleUtility();
	static TBool SubtitlesAvailable(RMMFController& aController);
	TInt AddSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig);
	TInt RemoveSubtitleConfig(TInt aWindowId);
	TInt UpdateSubtitleConfig(const TMMFSubtitleWindowConfig& aConfig);
	TInt EnableSubtitles();
	void DisableSubtitles();
	TArray<TLanguage> SupportedSubtitleLanguagesL();
	void HandleCrpReady(RWindow& aWindow);
	void RedrawSubtitle(RWindow& aWindow, const TRect& aRedrawRect);
	TInt SetSubtitleLanguage(TLanguage aSubtitleLanguage);
	TLanguage SubtitleLanguage();

private:
	CMMFSubtitleUtility(RMMFController& aController);
	void ConstructL(RWsSession &aWs);
	void DrawCrp(RWindow& aWindow, TInt aCrpIdx, TBool aCallBeginRedraw);
	TInt FindCrpArrayIndex(TInt aWindowId);

private:
	RMMFVideoPlaySubtitleSupportCustomCommands iSubtitleSupportCustomCommands;
	CWsScreenDevice *iDevice;
	CWindowGc* iSubtitleGc;
	RArray<TLanguage> iSubtitleLanguages;
	RArray<TCrpData> iCrpDataArray;
	};
	
#endif // MMFSUBTITLEUTILITY_H
