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

#ifndef MMFSUBTITLEGRAPHIC_H
#define MMFSUBTITLEGRAPHIC_H

#include <e32std.h>
#include <w32std.h>

/** 
 * Client representation of a window-server-side subtitle drawer
*/
NONSHARABLE_CLASS(CMMFSubtitleGraphic): public CWsGraphic
    {
public:
    IMPORT_C static CMMFSubtitleGraphic* NewL();
    IMPORT_C virtual ~CMMFSubtitleGraphic();
	IMPORT_C void Clear();
	IMPORT_C TInt DrawFrame(TInt aFrameHandle, const TRect& aDirtyRegion, const TTimeIntervalMicroSeconds& aDisplayDuration);
	IMPORT_C TInt Initialize(TInt aBuffer1, TInt aBuffer2);
	IMPORT_C void Initialize();
	IMPORT_C TInt SwapFrame(TUint aExpectedBuffer, const TRect& aDirtyRegion, const TTimeIntervalMicroSeconds& aDisplayDuration);

private:
	CMMFSubtitleGraphic();
	void ConstructL();
	
	// Utility functions
	TInt CheckBitmapHandle(TInt aHandle);
	
	// From CWsGraphic
	void HandleMessage(const TDesC8 &aData);
	void OnReplace();

private:
	enum TSubtitleGraphicState
		{
		ESubtitleGraphicStateWaiting 		= 1,
		ESubtitleGraphicStateInit			= 2,
		ESubtitleGraphicStateInitSimple		= 3
		};	
		
	TSubtitleGraphicState iState;
		
	// Used to check bitmap handles.  Constructed as part of the object to avoid
	// duplicate allocation.
	CFbsBitmap* iTempBitmap;
	};

#endif
