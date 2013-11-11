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

#ifndef SUBTITLEROTATION_H
#define SUBTITLEROTATION_H

#include <mmf/common/mmfvideo.h>
#include <bitmaptransforms.h>

#include "mmfdevsubtitlecallback.h"

NONSHARABLE_CLASS(CSubtitleRotation) : public CActive
	{
public:
	static CSubtitleRotation* NewL(MMMFDevSubtitleCallback& aCallback);
	static CSubtitleRotation* NewLC(MMMFDevSubtitleCallback& aCallback);
	
	~CSubtitleRotation();
	
	void StartRotation(CFbsBitmap& aSrcBmp, CFbsBitmap& aTrgBmp, TVideoRotation aDisplayRotation);
	
private:
	void ConstructL();
	CSubtitleRotation(MMMFDevSubtitleCallback& aCallback);
	void DoCancel();
	
	void RunL();
	
private:
	MMMFDevSubtitleCallback& iMMFDevSubtitleCallback;
	CBitmapRotator* iRotator;
	};
	
#endif // SUBTITLEROTATION_H
