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

#ifndef TFLOWINDOW_H
#define TFLOWINDOW_H

#include <twindow.h>

#include <w32std.h>
#include <graphics/sgimagecollection.h>
#include <graphics/surfaceupdateclient.h>

/**
 * This class uses a RSgImage for drawing. The surface Id of the RSgImageCollection is used
 * by the SurfaceUpdateManager to draw the surface to the screen.
 * 
 */
class CTFlowWindow : public CTWindow
{
public:
	virtual ~CTFlowWindow();
	static CTWindow* NewL(RWsSession &aWs, const RWindowTreeNode &aParent, 
			const TPoint& aStartingPoint, const TSize& aWindowSize);

	virtual void LoadL(CSurfaceUtility* aUtility, TPtrC aFileName);
	virtual void RenderL();

protected:
	CTFlowWindow(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);

private:
	RSgImageCollection iImageCollection;
	RSgImage iImage;
	TSgImageInfo iImageInfo;
	RSurfaceUpdateSession iUpdateSession;
};

#endif /*TFLOWINDOW_H*/
