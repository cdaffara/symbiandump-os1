// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BITBASE_H__
#define __BITBASE_H__

#include <gdi.h>

/**
@publishedAll
*/
class TSpriteBase
	{
public:
	virtual void Hide(const TRect& aRect,const TRegion* aRegion)=0;
	virtual void Reveal(const TRect& aRect,const TRegion* aRegion)=0;
	};

#endif

