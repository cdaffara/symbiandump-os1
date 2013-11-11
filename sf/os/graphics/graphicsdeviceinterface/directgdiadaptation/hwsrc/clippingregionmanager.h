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
//


#ifndef CLIPPINGREGIONMANAGER_H_
#define CLIPPINGREGIONMANAGER_H_

#include <VG/openvg.h>
#include <e32std.h>

/** 
Defines a rectangle of VGint coordinates for use when passing clipping rectangles to OpenVG.
*/
struct TVgClippingRect
	{
	VGint iX;
	VGint iY;
	VGint iWidth;
	VGint iHeight;
	};

/**
Helper for managing clipping (OpenVG: scissoring) regions within CVGEngine.

OpenVG supports only a fixed number of clipping rects (actual number is defined by 
the OpenVG implementation, though guaranteed to be at least 32), whereas 
Symbian OS stipulates no upper bound on the number of clipping rects in the
clipping region.   

This class keeps the entire clipping region in OpenVG coordinates, and may
be used to iterate over the clipping region, passing a set number of
clipping rectangles to OpenVG on each iteration.

A typical use case would be as follows:
@code
	for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
		vgDrawImage(openVgImage);
@endcode

@internalComponent
*/
NONSHARABLE_CLASS(RClippingRegionManager)
	{
	public:
		RClippingRegionManager ();
		void  Close ();
		void  Reset ();
		void  Initialize (TUint aItemsPerStep, TSize aVgEngineTargetSize);
		TInt  SetClippingRegion(const TRegion& aRegion);
		const RRegion& ClippingRegion () const;
		void  ClipTo(const TRect& aClipRect);
		void  Begin();
		TBool Next();
		TBool ApplyClipRegion();
		TBool Intersects(const TRect& aClipRect) const;
		
	private:
		void ResetIterator ();
		TUint RemainingItems() const;
		TUint ItemsInThisIteration() const;
		TInt  Append(const TPoint& aVgPosition, VGint width, VGint height);
		const TPoint ConvertToVgCoords(const TRect& aRect) const;
		TInt ConvertClippingRegion(const TRegion& aRegion);
		
	private:
		RArray<TVgClippingRect> iClippingRects; 	/**< Array of clipping rectangles stored in OpenVG coordinates.*/
		RRegion iClippingRegion;					/**< The full clipping region.*/
		TSize iVgEngineTargetSize;
		TUint iItemsPerStep;
		TUint iCurrentRect;
		TUint iBegun;
	};

#endif
