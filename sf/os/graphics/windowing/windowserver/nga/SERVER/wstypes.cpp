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
// Basic window server types
// 
//

#include "wstypes.h"

#if defined(_DEBUG)
RWsRegion::RWsRegion() :
	RRegion()
	{}
RWsRegion::RWsRegion(TInt aGran) :
	RRegion (aGran)
	{}
RWsRegion::RWsRegion(const RRegion& aRegion) :
	RRegion(aRegion)
	{}
RWsRegion::RWsRegion(const TRect& aRect,TInt aGran) :
	RRegion(aRect, aGran)
	{}
RWsRegion::RWsRegion(TInt aCount,TRect* aRectangleList,TInt aGran) :
	RRegion(aCount, aRectangleList, aGran)
	{}

TBool RWsRegion::Contains(const RWsRegion& aRegion) const
	{
	if(IsEmpty() || aRegion.IsEmpty())
		return EFalse;
	
	if(iError && aRegion.CheckError())
		return ETrue;
	
	RWsRegion region(aRegion);
	region.SubRegion(*this);
	const TBool result = region.IsEmpty();
	region.Close();
	return result;
	}

#endif

RWsRegion nullRegion;
