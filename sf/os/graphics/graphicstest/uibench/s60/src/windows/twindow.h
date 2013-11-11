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

#ifndef TWINDOW_H
#define TWINDOW_H


#include <w32std.h>


class CSurfaceUtility;
class CEglEnvironment;
class CEGLRendering;

/**
 * Baseclass for all test windows. Is responsible for creation of destruction of the actual
 * WServ window. Also computes dirty regions for the window.
 * 
 */
class CTWindow : public CBase
{
public:
	virtual ~CTWindow();
	virtual void LoadL(CSurfaceUtility* aUtility, TPtrC aFileName);
	virtual void RenderL();

	void Move(TInt aX, TInt aY);
	TPoint CurrentPosition();
	void SetPosition(TInt aX, TInt aY);
	const RRegion& DirtyRegion() const;
	void SetVisible(TBool aIsVisible);
	TInt SetBackgroundSurface(const TSurfaceId& aSurface);
	TSize Size();
	
	static TSize GetDisplaySizeInPixels();
	static CFbsBitmap* CreateBitmapFromFileL(const TDesC& aFileName);
	
protected:
    CTWindow(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);

protected:
    RWindow iWindow;
    TBool iLoaded;
    
private:	
	RRegion iDirtyRegion;
	TSize iSize;
	TPoint iPosition;
	TSize iScreenSize;	
};

// function pointer is used by the controller to create windows
typedef CTWindow* (*pTWindowCreatorFunction)(RWsSession &aWs,
			const RWindowTreeNode &aParent, 
			const TPoint& aStartingPoint,
			const TSize& aWindowSize);

#endif /*TWINDOW_H*/
