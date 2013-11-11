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

#ifndef TREGIONEXTEND_H_
#define TREGIONEXTEND_H_

#include <e32std.h>

class TRegionExtend : public TRegion
	{
	//Note that this class is generally only good for regions with dimensions less than 45000x45000
	//as integers are used to hold the final total area, and would overflow.
	//In debug build, panic TRegionExtend x003 is raised, where x indicates a code-point
	// when any rectangle in the region exceeds 32k
	
private:
	//Do not construct instances of this class, use the Cast() methods to convert existing region instances
	TRegionExtend();
	TRegionExtend(const TRegionExtend&);
public:
	//Please use TRegionExtend::Cast() calls to extend existing TRegion objects
	static TRegionExtend& 		Cast(TRegion& aSrc)			{	return (TRegionExtend&) aSrc;	}
	static TRegionExtend* 		Cast(TRegion* aSrc)			{	return (TRegionExtend*) aSrc;	}
	static const TRegionExtend& Cast(const TRegion& aSrc)	{	return (const TRegionExtend&) aSrc;	}
	static const TRegionExtend* Cast(const TRegion* aSrc)	{	return (const TRegionExtend*) aSrc;	}
	
public:
	using	TRegion::RectangleListW;
	enum TOverlapFlags
		{
		EExact=			0x0000,
		ESub=			0x0001,	
		EAdd=			0x0002,
		EDiffers=		0x0003,	//Added and/or subtracted
		ENoIntersect=	0x0004,
		EErrorRegion=	0x0008	//This code is returned if one of the input regions is flagged as invalid
		};

	/** Avoids the use of a temp region by performing area calc on the fly.
	 * If both regions are empty then EOverlapNoIntersect only is returned.
	 * Limitation: this method is only good for regions with less than 2 billion units of area!
	 * @param aTarget	target region for difference
	 **/
	TOverlapFlags	TestDifference(const TRegion& aTarget)const;
	
	/** Further avoids the use of a temp region by performing area calc on the fly.
	 * If both regions are empty then EOverlapNoIntersect only is returned.
	 * Limitation: this method is only good for regions with less than 2 billion units of area!
	 * @param aTarget	target region for difference
	 * @param aOffset	origin offset transformation from this to target region
	 **/
	TOverlapFlags	TestDifference(const TRegion& aTarget,TPoint aOffset )const;
	
	/** Avoids the use of a temp region by performing area calc on the fly.
	 * If region and rect both empty then EOverlapNoIntersect only is returned.
	 * Limitation: this method is only good for regions and rects with less than 2 billion units of area!
	 **/
	TOverlapFlags	TestDifference(const TRect&)const;
	
	/** Intended as an internal method to test for the overlap of two rectangles.
	 * 
	 **/
	static TOverlapFlags TestDifference(const TRect&,const TRect&);
	
	/** Returns total area of the region
	 * Limitation: this method is only good for regions with less than 2 billion units of area!
	 **/
	TUint	Area()const;
	

protected:
	};

#endif /*TREGIONEXTEND_H_*/
