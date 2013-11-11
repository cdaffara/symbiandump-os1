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

#ifndef SUBTITLETIMER_H
#define SUBTITLETIMER_H

#include <e32std.h>
#include <e32base.h>

#include "mmfdevsubtitlecallback.h"

/**
Provides timing facilities to devsubtitle.  Used to determine when a subtitle frame is due
*/
NONSHARABLE_CLASS(CSubtitleTimer) : public CTimer
	{
public:
	static CSubtitleTimer* NewL(MMMFDevSubtitleCallback& aCallback);
	void StartTimer(const TTimeIntervalMicroSeconds32& aTime);
	
private:
	CSubtitleTimer(MMMFDevSubtitleCallback& aCallback);
	void RunL();

private:
	MMMFDevSubtitleCallback& iMMFDevSubtitleCallback;
	};
	
#endif // SUBTITLETIMER_H
