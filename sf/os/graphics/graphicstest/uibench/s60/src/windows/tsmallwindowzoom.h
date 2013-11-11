// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TSMALLWINDOWZOOM_H
#define TSMALLWINDOWZOOM_H


#include "tsmallwindowraster.h"


class CTSmallWindowZoom : public CTSmallWindowRaster
	{
public:
	virtual ~CTSmallWindowZoom();

	static CTWindow* NewL(RWsSession &aWs,
			const RWindowTreeNode &aParent, 
			const TPoint& aStartingPoint,
			const TSize& aWindowSize);

	virtual void LoadL(CSurfaceUtility* aUtility, TPtrC aFileName);
	virtual void RenderL();
	
protected:
	CTSmallWindowZoom(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);
	
private:
    enum TZoom {EZoomIn, EZoomOut};
    
private:
	TRect            iZoomRect; 
	TInt 			 iDelta;
	TZoom            iZoom;
	};

#endif // CTSMALLWINDOWZOOM_H
