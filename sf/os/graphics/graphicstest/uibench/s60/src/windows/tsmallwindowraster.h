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

#ifndef TSMALLWINDOWRASTER_H
#define TSMALLWINDOWRASTER_H


#include "twindow.h"


/**
 *  CTSmallWindowRaster
 * 
 */
class CTSmallWindowRaster : public CTWindow
	{
public:
	virtual ~CTSmallWindowRaster();

	static CTWindow* NewL(RWsSession &aWs,
			const RWindowTreeNode &aParent, 
			const TPoint& aStartingPoint,
			const TSize& aWindowSize);

	virtual void LoadL(CSurfaceUtility* aUtility, TPtrC aFileName);
	virtual void RenderL();
	
protected:
	CTSmallWindowRaster(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);
	void DrawBitmap(TRect& aDestRect);
	
protected:
    TRect            iSourceRect;
    
private:
	CWsScreenDevice* iScreenDev;
	CWindowGc*       iGc;
	
    TRect            iDestRect;
    CFbsBitmap*      iSourceBitmap;
	};

#endif // TSMALLWINDOWRASTER_H
