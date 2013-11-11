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

#include <e32def.h>
#include "regionextend.h"

//Only call this if certain neither rectangle is empty.
//Region rectangles are always non-empty
inline /*static*/
TRegionExtend::TOverlapFlags TestDifferenceNotEmpty(const TRect& aThis,const TRect& aThat)
	{
	struct SubAdd
	{
		inline static TRegionExtend::TOverlapFlags Conv(TInt delta)
			{	//returns sub, exact or add for delta of each edge pair
				//neg --> +1	//pos --> +2	//zero ==> 0	
				return TRegionExtend::TOverlapFlags(	
						((delta>>31)&1)	+(((-delta)>>30)&2)		);
			}
	};
	//could use SubAdd for this if...
	if (	aThis.iTl.iX>=aThat.iBr.iX
		||	aThis.iTl.iY>=aThat.iBr.iY
		||					aThat.iTl.iX>=aThis.iBr.iX
		||					aThat.iTl.iY>=aThis.iBr.iY
	)
		return TRegionExtend::TOverlapFlags(TRegionExtend::EDiffers|TRegionExtend::ENoIntersect);

	TInt subAdd=(	SubAdd::Conv(aThis.iTl.iX-aThat.iTl.iX)
					|	SubAdd::Conv(aThis.iTl.iY-aThat.iTl.iY)
					|	SubAdd::Conv(aThat.iBr.iX-aThis.iBr.iX)
					|	SubAdd::Conv(aThat.iBr.iY-aThis.iBr.iY)
				);
	return 
	TRegionExtend::TOverlapFlags(subAdd);
	}

/**	Calc total area of a list of non-intersecting rectangles (ie a region)
 * 	@param	aThisRects	the array of rectangles
 * 	@param	aThisCount	the number in the array
 */
inline TUint RectListArea(const TRect* aThisRects,TInt aThisCount)
	{
	TInt thisArea=0;
	for (TInt stepThis=aThisCount-1;stepThis>=0;stepThis--)
		{
		TSize size=aThisRects[stepThis].Size();
		__ASSERT_DEBUG((size.iWidth|size.iHeight)<32768,User::Panic(_L("TRegionExtend"),4003));
		thisArea+=size.iWidth*size.iHeight;
		}
	return thisArea;
	}
/**	Returns the result code flag based on the calculated intersection of two areas
 * 	The intersection is always less than or equal to this and that.
 * 	@param 	aThisArea		Start area
 * 	@param	aIntersectArea	Intersection
 * 	@param	aThatArea		Final area
 * 	@return
 * 		-	EDiffers 	if both this and that differ from intersect 
 * 		-	EExact		if both this and that are same as intersect
 * 		-	ESub		if only that is same as intersect, so this is bigger
 * 		-	EAdd		if only this is same as intersect, so that is bigger
 * 		
 **/
inline TRegionExtend::TOverlapFlags AreaDiffResults(TUint aThisArea,TUint aIntersectArea,TUint aThatArea)
	{
	if (aThisArea>aIntersectArea)
		if (aThatArea>aIntersectArea)
			return TRegionExtend::EDiffers;
		else
			return TRegionExtend::ESub;
	else
		if (aThatArea>aIntersectArea)
			return TRegionExtend::EAdd;
		else
			return TRegionExtend::EExact;
	}
/**	Calculates the intersection area of one rectangle with an array of non intersecting rectangles
 * 	It is presumed that the caller will loop through all the cells of a target region,
 * 	repeating this call for a source region, so we can add an extra optimisation 
 * 	to avoid revisiting any source rectangles that have been consumed completely in later passes.
 * 	The simplest test is that the source cell is wholy inside the target rect.
 * 	The simplest record is a bit field, but that only works up to 32 elements, then will not optimise further elements.
 * 	@param	aThisCount 			num elements in rect array
 * 	@param	aThisRects			array of rectangles
 * 	@param	aThatRect			intersecting rectangle
 * 	@param	aOptimiseThisBits	record of elements of rect aray that have been fully consumed
 * 	@return	total intersection area
 **/
inline TUint TestDifferenceRegionInnerLoop(TInt aThisCount,const TRect* aThisRects,TRect& aThatRect,TUint& aOptimiseThisBits)
	{
	TUint intersectArea=0;
	for (TInt stepThis=aThisCount-1,bit=1;stepThis>=0;stepThis--,bit<<=1)
		{
		if (!(aOptimiseThisBits&bit))
			{
			const TRect& thisRect=aThisRects[stepThis];
			TRegionExtend::TOverlapFlags flags=TestDifferenceNotEmpty(thisRect,aThatRect);
			if (!(flags&TRegionExtend::ENoIntersect))
				{
				if (!(flags&TRegionExtend::EAdd))
					{	//Skip rest of inner loop if a containing rect is found
					TSize size=aThatRect.Size();
					intersectArea+=size.iWidth*size.iHeight;
					if (!(flags&TRegionExtend::ESub))	//equal rects...
						{	//skip this cell for rest of outer loop if a contains rect is found
						aOptimiseThisBits|=bit;
						}
					break;	//this cell contains the target rect so don't bother checking any more
					}
				else
					if (!(flags&TRegionExtend::ESub))
						{	//skip this cell for rest of outer loop if a contains rect is found
						aOptimiseThisBits|=bit;
						TSize size=thisRect.Size();
						intersectArea+=size.iWidth*size.iHeight;
						}
					else
						{
						TRect intersect=thisRect;
						intersect.Intersection(aThatRect);
						TSize size=intersect.Size();
						intersectArea+=size.iWidth*size.iHeight;
						}
				}
			}
		}
	return intersectArea;
	}
/** Avoids the use of a temp region by performing area calc on the fly.
 * If both regions are empty then EOverlapNoIntersect only is returned.
 * @param	aThat	target region
 * @return	flags from TOverlapFlags enumeration
 * 		-	EExact			=0 if rgions are exactly identical
 * 		-	ESub			Flagged if some rectangles are removed converting current region to that target
 * 		-	EAdd			Flagged if some rectangles are added converting current region to that target
 * 		- 	ENoIntersect	if there is no common region between the current region and that target
 * 		-	EErrorRegion	One of the regions is signalling CheckError()
 **/
TRegionExtend::TOverlapFlags	TRegionExtend::TestDifference(const TRegion& aThat)const
	{
	TInt intersectArea=0;
	TInt thisArea=0;
	TInt thatArea=0;
	const TRect* thisRects=RectangleList();
	const TRect* thatRects=aThat.RectangleList();
	TInt thatCount=aThat.Count();
	TInt thisCount=Count();
	
	if (CheckError()||aThat.CheckError())
		return EErrorRegion;
	if (thisCount==0)
		if (thatCount==0)
			return ENoIntersect;
		else
			return TOverlapFlags(ENoIntersect|EAdd);
	//both regions are populated. How big is the intersection?
	//The following optimisation bit is that 
	//if any rect is fully contained by a rect in the opposite region 
	//then further compares against that rect are skipped. For this, inner loop is skipped immediately
	//Can track the first 32 rects of aThat. The remainder won't benefit from the optimisation
	TUint optimiseThisBits=0;	
	for (TInt stepThat=thatCount-1;stepThat>=0;stepThat--)
		{
		TRect thatRect=thatRects[stepThat];
		intersectArea+=TestDifferenceRegionInnerLoop(thisCount,thisRects,thatRect,optimiseThisBits);
		}
	if (intersectArea==0)
		if (thatCount==0)
			return TOverlapFlags(ENoIntersect|ESub);
		else
			return 	TOverlapFlags(ENoIntersect|EAdd|ESub);
	thatArea=RectListArea(thatRects,thatCount);
	thisArea=RectListArea(thisRects,thisCount);
	return AreaDiffResults( thisArea, intersectArea, thatArea );
	}

/** Avoids the use of a temp region by performing area calc on the fly.
 * This version further optimises the process by avoiding the client having to re-origin either region.
  * If both regions are empty then EOverlapNoIntersect only is returned.
 * @param	aThat	target region
 * @return	flags from TOverlapFlags enumeration
 * 		-	EExact			=0 if rgions are exactly identical
 * 		-	ESub			Flagged if some rectangles are removed converting current region to that target
 * 		-	EAdd			Flagged if some rectangles are added converting current region to that target
 * 		- 	ENoIntersect	if there is no common region between the current region and that target
 * 		-	EErrorRegion	One of the regions is signalling CheckError()
 **/
TRegionExtend::TOverlapFlags	TRegionExtend::TestDifference(const TRegion& aThat,TPoint aOffsetToThat)const
	{
	TInt intersectArea=0;
	TInt thisArea=0;
	TInt thatArea=0;
	const TRect* thisRects=RectangleList();
	const TRect* thatRects=aThat.RectangleList();
	TInt thatCount=aThat.Count();
	TInt thisCount=Count();
	
	if (CheckError()||aThat.CheckError())
		return EErrorRegion;
	if (thisCount==0)
		if (thatCount==0)
			return ENoIntersect;
		else
			return TOverlapFlags(ENoIntersect|EAdd);
	//both regions are populated. How big is the intersection?
	//The following optimisation bit is that 
	//if any rect is fully contained by a rect in the opposite region 
	//then further compares against that rect are skipped. For this, inner loop is skipped immediately
	//Can track the first 32 rects of aThat. The remainder won't benefit from the optimisation
	TUint optimiseThisBits=0;	
	for (TInt stepThat=thatCount-1;stepThat>=0;stepThat--)
		{
		TRect thatRect=thatRects[stepThat];
		thatRect.Move(-aOffsetToThat.iX,-aOffsetToThat.iY);	//this line is the only difference, but the next lump has a lot of parameters...
		intersectArea+=TestDifferenceRegionInnerLoop(thisCount,thisRects,thatRect,optimiseThisBits);
		}
	if (intersectArea==0)
		if (thatCount==0)
			return TOverlapFlags(ENoIntersect|ESub);
		else
			return 	TOverlapFlags(ENoIntersect|EAdd|ESub);
	
	thatArea=RectListArea(thatRects,thatCount);
	thisArea=RectListArea(thisRects,thisCount);
	return AreaDiffResults( thisArea, intersectArea, thatArea );
	}

/** This returns the same comparrison flags between two rectangles.
 *	Note that a zero return means exact intersection... 
 *  Intended as an internal method, but there doesn't seem to be a useful public alternative.
 * 	@param  aThis	source rectangle
 *	@param	aThat	target rectangle
 * 	@return	flags from TOverlapFlags enumeration
 * 		-	EExact			=0 if rgions are exactly identical
 * 		-	ESub			Flagged if some rectangles are removed converting this source rectangle to that target
 * 		-	EAdd			Flagged if some rectangles are added converting this source rectangle to that target
 * 		- 	ENoIntersect	if there is no common region between this source rectangle and that target
 **/
TRegionExtend::TOverlapFlags TRegionExtend::TestDifference(const TRect& aThis,const TRect& aThat)
	{
	if (aThis.IsEmpty())
		if (aThat.IsEmpty())
			return ENoIntersect;
		else
			return TOverlapFlags(EAdd|ENoIntersect);
	else
		if (aThat.IsEmpty())
			return TOverlapFlags(ESub|ENoIntersect);
	return TestDifferenceNotEmpty(aThis,aThat);
	}	


/** Returns total area of the region
 * 	@return total area
 **/
TUint	TRegionExtend::Area()const
{ 
return RectListArea(RectangleList(),Count());
}

/** Avoids the use of a temp region by performing area calc on the fly.
 * If both are empty then EOverlapNoIntersect only is returned.
 * @param	aThat	target rectangle
 * @return	flags from TOverlapFlags enumeration
 * 		-	EExact			=0 if region exactly fills rectangle
 * 		-	ESub			Flagged if some rectangles are removed converting current region to that target
 * 		-	EAdd			Flagged if some rectangles are added converting current region to that target
 * 		- 	ENoIntersect	if there is no common region between the current region and that target
 * 		-	EErrorRegion	One of the region is signalling CheckError()
 **/
TRegionExtend::TOverlapFlags	TRegionExtend::TestDifference(const TRect& aThat)const
	{
	TInt intersectArea=0;
	const TRect* thisRects=RectangleList();
	TInt thisCount=Count();
	
	if (aThat.IsEmpty())
		if (thisCount==0)
			return ENoIntersect;
		else
			return TOverlapFlags(ENoIntersect|ESub);
	if (CheckError())
		return EErrorRegion;
	TInt output=ENoIntersect;
	for (TInt stepThis=thisCount-1,bit=1;stepThis>=0;stepThis--,bit+=bit)
		{
		TOverlapFlags flags=TestDifferenceNotEmpty(thisRects[stepThis],aThat);
		if (!(flags&ENoIntersect))
			{
			if (!(flags&EAdd))		//the target rect does not add anything to this region element
				{	//Skip rest of inner loop if a containing rect is found
				if ((flags&ESub)||thisCount>1)
					return ESub;	//the region element is bigger or there are more region elements
				else
					return EExact;
				}
			else
				{
				TRect intersect=thisRects[stepThis];
				intersect.Intersection(aThat);
				TSize size=intersect.Size();
				__ASSERT_DEBUG((size.iWidth|size.iHeight)<32768,User::Panic(_L("TRegionExtend"),1003));
				intersectArea+=size.iWidth*size.iHeight;
				
				}
			output&=~ENoIntersect;
			}
		output|=(flags&ESub);
		}
	if (intersectArea==0)
		{
		return TOverlapFlags(output|EAdd);
		}
	TSize size=aThat.Size();
	__ASSERT_DEBUG((size.iWidth|size.iHeight)<32768,User::Panic(_L("TRegionExtend"),2003));
	TInt thatArea=size.iWidth*size.iHeight;
	if (thatArea>intersectArea)
		return TOverlapFlags(output|EAdd);
	else
		return TOverlapFlags(output);
	}
	

