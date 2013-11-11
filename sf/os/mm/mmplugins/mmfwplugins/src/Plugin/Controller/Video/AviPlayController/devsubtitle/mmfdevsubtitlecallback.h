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

#ifndef MMFDEVSUBTITLECALLBACK_H
#define MMFDEVSUBTITLECALLBACK_H

NONSHARABLE_CLASS(MMMFDevSubtitleCallback)
	{
public:
	virtual void RotationComplete(TInt aResult)=0;
	virtual void SubtitleTimerExpired()=0;
	};
	
#endif // MMFDEVSUBTITLECALLBACK_H
