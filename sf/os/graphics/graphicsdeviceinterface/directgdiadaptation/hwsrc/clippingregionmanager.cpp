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

#include "clippingregionmanager.h"

const TInt KVgClippingRectSize = sizeof(TVgClippingRect)/sizeof(VGint);

RClippingRegionManager::RClippingRegionManager () : 
	iItemsPerStep (1)
	{
	}

void RClippingRegionManager::Close ()
	{
	iClippingRegion.Close();
	iClippingRects.Close();
	}

/** 
Initialise the iterator with run-time information.
@param aItemsPerStep		Number of clipping rectangles to set in OpenVG per iteeration.
@param aVgEngineTargetSize	Dimensions of image target. This is used to perform Symbian to OpenVG
							coordinate transformations.
*/
void RClippingRegionManager::Initialize (TUint aItemsPerStep, TSize aVgEngineTargetSize)
	{
	iItemsPerStep = aItemsPerStep;
	iVgEngineTargetSize = aVgEngineTargetSize;
	}

/**
Set the iterator to the beginning of the region.
*/
void RClippingRegionManager::Begin()
	{
	iCurrentRect = 0;
	iBegun = EFalse;
	}

/**
Reset the state associated with the iterator, but not the clipping region.
Note that the iItemsPerStep member is not reset.
*/
void RClippingRegionManager::ResetIterator ()
	{
	iClippingRects.Reset();
	iCurrentRect = 0;
	iBegun = EFalse;
	}

/**
Reset the object state.
@see ResetIterator()
*/
void RClippingRegionManager::Reset ()
	{
	iClippingRegion.Clear();
	ResetIterator();
	}

/**
@return The number of items before the iterator gets to the End() item.
*/
TUint RClippingRegionManager::RemainingItems() const
	{
	return iClippingRects.Count() - iCurrentRect;
	}

/**
@return The number of items to be set in this iteration. This value will be either
		iItemsPerStep or the number of items to the end of the list (whichever
		is smaller)
*/
TUint RClippingRegionManager::ItemsInThisIteration() const
	{
	TUint remainingItems = RemainingItems();
	return (remainingItems > iItemsPerStep) ? iItemsPerStep : remainingItems;
	}

/**
Set the clip region for the current iteration in OpenVG. If no clipping 
rectangles are defined in the clipping region then clipping is turned off, 
and this method returns ETrue on the first invocation after Begin()
(behaviour is as if one big clip rect is defined that matches the extents 
of the target pixmap exactly).

@return ETrue if a region was set, or no clipping rectangles are defined.
*/
TBool RClippingRegionManager::ApplyClipRegion()
	{
	TBool result = EFalse;
	
	if (iClippingRects.Count() == 0)
		{
		if (!iBegun)
			{
			vgSeti(VG_SCISSORING, VG_FALSE);
			result = ETrue;
			}
		else
			{
			result = EFalse;
			}
		}
	else
		{
		TUint itemsInThisIteration = ItemsInThisIteration();
		if (itemsInThisIteration)
			{
			vgSeti(VG_SCISSORING, VG_TRUE);
			vgSetiv(
				VG_SCISSOR_RECTS,
				KVgClippingRectSize*itemsInThisIteration, 
				reinterpret_cast<VGint*>(&iClippingRects[iCurrentRect]));
			}
		result = itemsInThisIteration>0;
		}

	iBegun = ETrue;
	return result;
	}


/**
Moves the iterator forward by iItemsPerStep items. 
@return ETrue if there are items to go after the iteration, otherwise false.
*/
TBool RClippingRegionManager::Next()
	{
	TBool result = EFalse;
	if (RemainingItems() > iItemsPerStep)
		{
		iCurrentRect += iItemsPerStep;
		result = ETrue;
		}
	else
		{
		iCurrentRect = iClippingRects.Count();	
		}
	return result;
	}
	

/**
@see RArray::Append()
*/
TInt RClippingRegionManager::Append(const TPoint& aVgPosition, VGint aWidth, VGint aHeight)
	{
	TVgClippingRect clippingRect = {aVgPosition.iX, aVgPosition.iY, aWidth, aHeight};
	return iClippingRects.Append(clippingRect);
	}

/**
@see MDirectGdiEngine::SetClippingRegion(const TRegion&)
*/
TInt RClippingRegionManager::SetClippingRegion(const TRegion& aRegion)
	{
	TInt result = KErrNone;
	Reset();
	if(aRegion.Count() != 0)
		{
		result = ConvertClippingRegion (aRegion);
		
		if (result != KErrNoMemory)
			{
			iClippingRegion.Copy(aRegion);
			if(iClippingRegion.CheckError())
				{
				result = KErrNoMemory;
				}
			}
		}
	return result;
	}

/**
Convert the specified clipping region to OpenVG coords and store the
converted coords internally.
@param aRegion	Region for conversion.
@return  KErrNoMemory, otherwise KErrNone.
*/
TInt RClippingRegionManager::ConvertClippingRegion(const TRegion& aRegion)
	{
	TInt result = KErrNone;
	ResetIterator();
	const TInt count = aRegion.Count();
	if(count != 0)
		{
		for(TInt ii = 0; ii < count; ++ii)
			{
			const TRect& regionRect = aRegion[ii];
			// Clipping regions in OpenVG are not subject to transformations, 
			// so need to pass in coordinates which are already in OpenVG coordinate system.
			// Get the rectangle's position, specified in OpenVG coordinate system.
			if (Append(
					ConvertToVgCoords(regionRect), 
					regionRect.Width(), 
					regionRect.Height()) == KErrNoMemory)
				{
				result = KErrNoMemory;
				break;
				}
			}
		}
	return result;
	}

/** 
If the clipping region is empty, then set it to the specified clip rect,
otherwise set it to its intersection with the specified clip rect.
@param aClipRect Rectangle to clip to.
*/
void RClippingRegionManager::ClipTo(const TRect& aClipRect)
	{
	if (iClippingRegion.IsEmpty())
		{
		iClippingRegion.AddRect(aClipRect);		
		}
	else
		{
		iClippingRegion.ClipRect(aClipRect);
		}
	
	ConvertClippingRegion(iClippingRegion);
	}

/**
@return The clipping region.
*/
const RRegion& RClippingRegionManager::ClippingRegion() const
	{
	return const_cast<const RRegion&>(iClippingRegion);
	}

/**
@return 'ETrue' if aClipRect intersects with the currently defined clipping region.
		Note that if no clipping region is defined then ETrue is always returned.
*/
TBool RClippingRegionManager::Intersects(const TRect& aClipRect) const
	{
	return (iClippingRegion.IsEmpty() || iClippingRegion.Intersects(aClipRect));
	}

/**
Converts the position of a rectangle from Symbian graphics coordinate system to OpenVG coordinate system.

The Symbian coordinate system's x-axis increases positively from the origin rightwards.
The y-axis increases positively from the origin downwards.
A rectangle's position is specified by the top-left coordinate.

The OpenVG coordinate system's x-axis increases positively from the origin rightwards.
The y-axis increases positively from the origin upwards.
A rectangle's position is specified by the bottom-left coordinate.

A point (X,Y) in Symbian coordinate system would be equivalent to point
(X',Y') in OpenVG coordinate system by the following transformations:
X' = X
Y' = (Height of rendering target) - Y

@param	aRect	A rectangle whose position is to be converted for use in OpenVG.

@return The bottom-left point of the rectangle, specified in OpenVG specific coordinate system.
*/
const TPoint RClippingRegionManager::ConvertToVgCoords(const TRect& aRect) const
	{
	return TPoint(aRect.iTl.iX, iVgEngineTargetSize.iHeight - aRect.iBr.iY);
	}
