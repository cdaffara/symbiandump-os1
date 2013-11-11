// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "cliwin.h"
#include "server.h"
#include "windowelementset.h"
#include "regionextend.h"
#include <graphics/wsscene.h>
#include <graphics/wsgraphicdrawer.h>
#include "panics.h"
#include "wstop.h"

#include "EVENT.H"

#ifndef KCacheListGranularity
	enum	{KCacheListGranularity = 8};
#endif
#ifndef KCacheExtensionReserve
	enum	{KCacheExtensionReserve = 1};
#endif
	


/** Element to gc rotation function
*/
LOCAL_C inline CFbsBitGc::TGraphicsOrientation ElementToGcRotation(MWsElement::TElementRotation aElementRotation)
	{
	CFbsBitGc::TGraphicsOrientation gcRotation = CFbsBitGc::EGraphicsOrientationNormal;

	switch (aElementRotation)
		{
		case MWsElement::EElementAntiClockwise90:
			gcRotation = CFbsBitGc::EGraphicsOrientationRotated90;
			break;
		case MWsElement::EElementAntiClockwise180:
			gcRotation = CFbsBitGc::EGraphicsOrientationRotated180;
			break;
		case MWsElement::EElementAntiClockwise270:
			gcRotation = CFbsBitGc::EGraphicsOrientationRotated270;
			break;
		default:
			break;
		}
	return gcRotation;
	}

//
// CWindowElement
//

CWindowElement::CWindowElement(const CWsClientWindow& aWindow) :
	iWindow(aWindow), iCache(KCacheListGranularity), iHighestReusedIndex(-1)
	{}


/** Finds all elements within the window that overlap with the region and 
marks them as unassigned.
If any part of these unassigned elements are outside the region, they are also 
marked overlapping and locked config.

@param aRegion The region that is being redrawn.
@return KErrNone on success or KErrNoMemory if critical calculations could not be made.

@see CWindowElementSet::UnassignPlacedElements
**/
TInt CWindowElement::UnassignPlacedElements(const TRegion& aRegion,TInt aGcDrawingCount)
	{
	iFlags = 0;	//reset flags
	iStartGcDrawingCount = aGcDrawingCount;	//Detect any drawing between calls
	iBackgroundElement.SetDrawnOverLast(iBackgroundElement.DrawnOver());
	iNonRedrawVisibleRegion.Clear();
	iNonRedrawVisibleRegion.Copy(const_cast<CWsClientWindow&>(iWindow).VisibleRegion());
	iNonRedrawVisibleRegion.SubRegion(aRegion);
	if (iNonRedrawVisibleRegion.CheckError())
		{	//memory error. Still needs to cope if whole window is being redrawn
		iNonRedrawVisibleRegion.Clear();
		TInt fullWindowRedraw = TRegionExtend::Cast(aRegion).TestDifference(const_cast<CWsClientWindow&>(iWindow).VisibleRegion());
		if (fullWindowRedraw != 0)
			{	//not a full window redraw, return immediately
			return KErrNoMemory;
			}
		}	
	TInt placedElements = iPlacedElements.Count();
	TInt unassignedCount = 0;
	TInt ii;
	TBool cacheError = EFalse;

	STACK_REGION tempSurfaceBaseArea;
	if (iCache.Reserve(placedElements + KCacheExtensionReserve) < KErrNone)	//quite often there may need to be room for more
		{	//failed to reserve space
		cacheError = ETrue;
		}
	TRect tempExtent;
	TRect tempIntersect;
	
	for (ii=0; ii<placedElements; ii++)
		{
		if (!cacheError)
			{	//cache is ok up to this point but does not contain cache for this element yet
			RRegion newRegion;
			if (iCache.Append(newRegion) < KErrNone)
				{
				cacheError = ETrue;
				}
			newRegion.Close();
			}

		iPlacedElements[ii].iElement->GetDestinationRectangle(tempExtent);
		TInt regionReturn = TRegionExtend::Cast(aRegion).TestDifference(tempExtent);
		iPlacedElements[ii].SetDrawnOverLast(iPlacedElements[ii].DrawnOver());	//allows detection of change of state
		if (!(regionReturn&TRegionExtend::ENoIntersect))
			{	//redraw region intersects with element
			iPlacedElements[ii].SetUnassigned();
			iPlacedElements[ii].SetDrawnOver(EFalse);
			unassignedCount++;
			
			if (!(regionReturn&TRegionExtend::EAdd))
				{	//surface is entirely within the redraw region
				iPlacedElements[ii].SetOverlapping(EFalse);
				iPlacedElements[ii].SetLockedConfig(EFalse);
				if(!cacheError)
					{
					TRect destinationRect;
					tempSurfaceBaseArea.Clear();
                    iPlacedElements[ii].iElement->GetDestinationRectangle(destinationRect);
					tempSurfaceBaseArea.AddRect(destinationRect);
					if (!tempSurfaceBaseArea.CheckError())
						{
						tempSurfaceBaseArea.Offset(tempExtent.iTl);	//cache needs to be in absolute coords
						iCache[ii].Copy(tempSurfaceBaseArea);
						}
					if (tempSurfaceBaseArea.CheckError() || iCache[ii].CheckError())
						{	//cache is no good from this point onwards
						iCache[ii].Close();
						iCache.Remove(ii);
						cacheError = ETrue;
						}
					}
				}
			else
				{
                TRect destinationRect;
                tempSurfaceBaseArea.Clear();
                iPlacedElements[ii].iElement->GetDestinationRectangle(destinationRect);
                tempSurfaceBaseArea.AddRect(destinationRect);
				if (tempSurfaceBaseArea.CheckError())
					{	//critical failure, clear cache and return error
					for (ii=iCache.Count(); ii>0; ii--)
						{
						iCache[ii-1].Close();
						iCache.Remove(ii-1);
						}
					iCache.Reset();
					iNonRedrawVisibleRegion.Close();
					tempSurfaceBaseArea.Close();
					return KErrNoMemory;
					}
				else
					{
					tempSurfaceBaseArea.Offset(tempExtent.iTl);	//change to absolute coordinates
					regionReturn = TRegionExtend::Cast(iNonRedrawVisibleRegion ).TestDifference(tempSurfaceBaseArea);
					if (regionReturn&TRegionExtend::ENoIntersect)
						{	//element base area entirely inside redraw region
						iPlacedElements[ii].SetOverlapping(EFalse);
						iPlacedElements[ii].SetLockedConfig(EFalse);
						}
					else
						{	//element base area is at least partly within non redraw visible region
						iPlacedElements[ii].SetOverlapping(ETrue);
						iPlacedElements[ii].SetLockedConfig(ETrue);
						}
					if(!cacheError)
						{
						iCache[ii].Copy(tempSurfaceBaseArea);
						if (iCache[ii].CheckError())
							{	//cache is no good from this point onwards
							iCache[ii].Close();
							iCache.Remove(ii);
							cacheError = ETrue;
							}
						}
					}
				}
			}
		else
			{	//element does not intersect with redraw region
			iPlacedElements[ii].SetUnassigned(EFalse);
			}
		}
	tempSurfaceBaseArea.Close();
	
	if (iRemovedSurfaces.Reserve(unassignedCount) == KErrNone)
		{
		iRemovedSurfacesValid = ETrue;
		}
	else
		{
		iRemovedSurfacesValid = EFalse;
		}
	return KErrNone;
	}

/** Marks all elements that have been assigned since the unassign as drawn over.

@see CWindowElementSet::FlagAssignedElementsDrawnOver
**/
void CWindowElement::FlagAssignedElementsDrawnOver(TInt aGcDrawingCount)
	{
	iStartGcDrawingCount = aGcDrawingCount;
	if (iBackgroundElement.iElement)
		{
		iBackgroundElement.SetDrawnOver();
		}
	TInt placedElementCount = iPlacedElements.Count();
	for (TInt ii = 0; ii < placedElementCount; ii++)
		{
		if(!iPlacedElements[ii].Unassigned() && iPlacedElements[ii].LockedConfig())
			{
			iPlacedElements[ii].SetDrawnOver();
			}
		else if(iPlacedElements[ii].Unassigned() && iPlacedElements[ii].LockedConfig() &&
				!iPlacedElements[ii].ChangedClip() && iPlacedElements[ii].Overlapping())
			{
			iPlacedElements[ii].SetDrawnOver();
			}
		}
	}

/** Checks whether a elements configuration is the same as that of the surface configuration, taking
into account the stateflags (TPlacedAttributes) passed in.
It must match surfaceId and extent.
If it is a locked config it will need to match viewport and orientation as well.

@param aElement Element to compare against.
@param aSurfaceConfiguration Configuration to compare.
@param aStateFlags The TPlacedAttributes flags for the element.
@return ETrue if a match, otherwise EFalse.
*/
TBool CWindowElement::IsElementAMatch(MWsElement& aElement, const TSurfaceConfiguration& aSurfaceConfiguration,
		TInt aStateFlags)
	{
	TBool match = EFalse;
	
	TRect tempElementExtent;
	TRect tempCRPExtent;
	aElement.GetDestinationRectangle(tempElementExtent);
	aSurfaceConfiguration.GetExtent(tempCRPExtent);
	if (tempElementExtent == tempCRPExtent)
		{
		TSurfaceId tempElementSurfaceId;
		TSurfaceId tempCRPSurfaceId;
		aSurfaceConfiguration.GetSurfaceId(tempElementSurfaceId);
		tempCRPSurfaceId = aElement.ConnectedSurface();
		if (tempElementSurfaceId == tempCRPSurfaceId)
			{
			match = ETrue;
			if (aStateFlags&TPlacedAttributes::ELockedConfig)
				{
				match = EFalse;
				TBool orientationOK = EFalse;
				TRect tempCRPViewport;
				aSurfaceConfiguration.GetViewport(tempCRPViewport);
				//if explicit viewport flagged
				TRect tempElementViewport;
				aElement.GetSourceRectangle(tempElementViewport);
				if (tempElementViewport == tempCRPViewport)
					{
					CFbsBitGc::TGraphicsOrientation tempElementOrientation;
					MWsElement::TElementRotation tempCRPRotation;
					CFbsBitGc::TGraphicsOrientation tempCRPOrientation;
					tempElementOrientation = aSurfaceConfiguration.Orientation();
					tempCRPRotation = aElement.SourceRotation();
					tempCRPOrientation = ElementToGcRotation(tempCRPRotation);	//convert to common type
					if (tempElementOrientation == tempCRPOrientation)
						{
						orientationOK = ETrue;
						}
					}
				if(orientationOK)
				    {
				    TBool tempElementFlip = aSurfaceConfiguration.Flip();
				    TBool tempCRPFlip = aElement.SourceFlipping();
				    if(tempElementFlip == tempCRPFlip)
				        {
				        match = ETrue;
				        }
				    }
				}
			}
		}
	return match;
	}

/** Sets the surfaceconfiguration to the element.

@param aElement The element to set.
@param aSurfaceConfiguration The configuration to set to the element.
@param aLimitedSet Set all values if EFalse, otherwise only set viewport and orientation.
@return KErrNone on success, error from compositor if it could not set the surface id.
*/
TInt CWindowElement::SetElement(MWsElement& aElement, const TSurfaceConfiguration& aSurfaceConfiguration, TBool aLimitedSet)
	{	//aLimitedSet - EFalse = set all . ETrue = set viewport and orientation only
	if (!aLimitedSet)
		{
		//set surface id
		TSurfaceId newSurfaceId;
		aSurfaceConfiguration.GetSurfaceId(newSurfaceId);
		TInt error = aElement.ConnectSurface(newSurfaceId);
		if (error != KErrNone)
			{
			return error;
			}
		
		//set extent
		TRect newExtent;
		aSurfaceConfiguration.GetExtent(newExtent);
		aElement.SetDestinationRectangle(newExtent);
		}
	
	//set viewport
	TRect newViewport;
	aSurfaceConfiguration.GetViewport(newViewport);
	if (newViewport.iTl.iX < 0 || newViewport.iTl.iY < 0)
		{
		return KErrArgument;
		}
	
	if (!newViewport.IsEmpty())
		{	//need to check if viewport is valid size
		TInt error = aElement.SetSourceRectangle(newViewport);
        if (error != KErrNone)
            {
            return error;
            }
		}

	//set orientation
	//Note for compatibility with Oghma:
	//This method does not properly error check or return KErrArgument,
	//as that would propagate a client panic in SetBackgroundSurface which did not previously occur.
	aElement.SetSourceRotation(GcToElementRotation(aSurfaceConfiguration.Orientation()));
	// Set or clear flip if the element flags are changing
	if ( (!aSurfaceConfiguration.Flip()) != (!aElement.SourceFlipping()) ) 
	    {
        aElement.SetSourceFlipping(!aElement.SourceFlipping());
	    }
	return KErrNone;
	}

/** Attempts to insert a region cache in to the the windows iCache.
If it fails, it will remove all cache beyond its index as it is now out of step.

@param aIndexToInsert The index of where to insert the cache.
@return ETrue on success, else EFalse.
*/

TBool CWindowElement::InsertCache(TInt aIndexToInsert)
	{
	RRegion newRegion;
	TInt cacheInsertError = iCache.Insert(newRegion, aIndexToInsert);
	if (cacheInsertError == KErrNone)
		{
		newRegion.Close(); 
		return ETrue;	//inserted cache
		}
	if (iCache.Count() > aIndexToInsert)
		{	//if insert failed and there is more cache above where its trying to insert
		iCache.Remove(iCache.Count()-1);
		cacheInsertError = iCache.Insert(newRegion, aIndexToInsert);
		
		if (cacheInsertError == KErrNone)
			{
			newRegion.Close();
			return ETrue;	//inserted cache
			}
		}
	
	//cant insert this cache, there may be invalid cache to destroy
	TInt count = iCache.Count();
	while(count > aIndexToInsert)
		{	//if there is cache where we wished to insert, it and everything above is now invalid
		iCache[count-1].Close();
		iCache.Remove(count-1);
		count--;
		};
	newRegion.Close();
	return EFalse;	//failed to insert
	}

/** Will send a notification that a surface being unreferenced if it is not
found in any WindowElementSet

@pre Make sure the element being removed has already had its surfaceid set to NULL.
@param aSurfaceId The surface being removed
*/
void CWindowElement::NotifyReleasingSurface(TSurfaceId aSurfaceId)
	{
	if (CWsTop::SearchDuplicateSurfaceId(aSurfaceId) == EFalse)
		{	//NOT found other instance of surface id, send notification now
		TWservCrEvent crEvent(TWservCrEvent::ESurfaceUnreferenced, sizeof(aSurfaceId), &aSurfaceId);
		TWindowServerEvent::NotifyDrawer(crEvent);
		}
	}

/** Called to place a element within the window.
It will either create a new element, recycle a element, or extend a element .

@pre UnassignPlacedElements has already been called.
@param aPlacedAttributes Returns the attributes of the surface placed.
@param aSurfaceConfiguration The surface configuration for the surface to place.
@param aUserDefinedRegion The user defined clipping of the window.
@param aScene Access to the scene.
@return Serious error, otherwise flags to describe what extra work needs doing.

@see CWindowElementSet::AssignPlacedElement
*/
TInt CWindowElement::AssignPlacedElement(
		TPlacedAttributes*& aPlacedAttributes,
		const TSurfaceConfiguration& aSurfaceConfiguration,
		const TRegion& aUserDefinedRegion,
		MWsScene& aScene,
		TInt	aGcDrawingCount
		)
	{	
	if (iStartGcDrawingCount!=aGcDrawingCount)
		{
		FlagAssignedElementsDrawnOver(aGcDrawingCount);
		}

	if (iNonRedrawVisibleRegion.CheckError())
		{
		//CANNOT DO ANYTHING, LEAVE
		return KErrNotReady;	//error
		}

	TRect surfaceExtent;
	aSurfaceConfiguration.GetExtent(surfaceExtent);
	if (!aUserDefinedRegion.Intersects(surfaceExtent))
		{
		//clippedRegion.Close();
		return KErrCancel;	//no intersection, so nothing to do
		}

	TInt matchFound = -1;
	
	//check 'current' (last placed element) to see if it is a match
	if (iHighestReusedIndex != -1)
		{
		if (IsElementAMatch(*(iPlacedElements[iHighestReusedIndex].iElement),aSurfaceConfiguration,iPlacedElements[iHighestReusedIndex].Flags()) )
			{
			matchFound = iHighestReusedIndex;
			}
		}

	if (matchFound == -1)
		{
		//check current unassigned elements to see if there is a match
		for (TInt ii = (iHighestReusedIndex == -1)?0:iHighestReusedIndex+1; ii < iPlacedElements.Count(); ii++)
			{
			if (IsElementAMatch(*(iPlacedElements[ii].iElement),aSurfaceConfiguration,iPlacedElements[ii].Flags()))
				{
				matchFound = ii;
				break;
				}
			}
		}
	TInt returnValue = 0;
	if (matchFound == -1)
		{	//not found a element to extend
		//iResort = ETrue;
		matchFound = 0;
		while(1)
			{
			if (iPlacedElements.Count() == 0 || iHighestReusedIndex == -1 || matchFound == iHighestReusedIndex)	//reached last re assigned element, so cannot reuse one
				{
				matchFound = -1;
				break;
				}
			if (iPlacedElements[matchFound].Unassigned() && !iPlacedElements[matchFound].LockedConfig())	//found a element to reuse
				{
				break;
				}
			matchFound++;
			};

		if (matchFound != -1)
			{	//found a element to recycle
			returnValue = AssignRecycleElement(matchFound,aSurfaceConfiguration,aUserDefinedRegion,aScene);
			}
		else
			{	//need to create a new element
			returnValue = AssignCreateElement(aSurfaceConfiguration,aUserDefinedRegion,aScene);
			}
		}
	else
		{	//found a candidate to extend
		returnValue = AssignExtendElement(matchFound,aSurfaceConfiguration,aUserDefinedRegion,aScene);
		}
	if (returnValue < KErrNone)
		{	//failed with error
			return returnValue;
		}
	aPlacedAttributes = &iPlacedElements[iHighestReusedIndex];
	iFlags|=returnValue;	//update window wide flags
	return returnValue;
	}

/** Recycle a element assigned to the window.

@param aIndex The index of the placed element to recycle.
@param aSurfaceConfiguration The surface configuration for the surface to place.
@param aUserDefinedRegion The user defined clipping of the window.
@param aScene Access to the scene.
@return Serious error, otherwise flags to describe what extra work needs doing.

@see CWindowElement::AssignPlacedElement
*/
TInt CWindowElement::AssignRecycleElement(const TInt aIndex, const TSurfaceConfiguration& aSurfaceConfiguration, const TRegion& aUserDefinedRegion, MWsScene& aScene)
	{
	TInt returnValue = 0;
	TInt error = KErrNone;
	STACK_REGION clippedRegion;
	TRect surfaceExtent;
	aSurfaceConfiguration.GetExtent(surfaceExtent);
	clippedRegion.AddRect(surfaceExtent);
	clippedRegion.Intersect(aUserDefinedRegion);
	if (clippedRegion.CheckError())
		{	//failed crucial calculation, leave while nothing has been changed!
		clippedRegion.Close();
		return KErrNoMemory;
		}
	WS_ASSERT_DEBUG(!clippedRegion.IsEmpty(),EWsPanicRegion);
	if (clippedRegion.IsEmpty())
		{
		return KErrArgument;
		}
	
	//keep note of surface being removed
	TSurfaceId oldSurfaceId = iPlacedElements[aIndex].iElement->ConnectedSurface();
	
	//set element with new attributes
	error = SetElement(*(iPlacedElements[aIndex].iElement),aSurfaceConfiguration,EFalse);
	
	if (error < KErrNone)
		{	//must be a bad surface id or bad viewport, leave while nothing has been changed!
		clippedRegion.Close();
		return error;
		}
	
	returnValue|=EFastPath;
	returnValue|=EPauseComposition;
	//element has been altered, must make sure old surface unreferenced notify will be called
	if (!iRemovedSurfacesValid)
		{	//didnt reserve space for surface ids, do search immediately
		NotifyReleasingSurface(oldSurfaceId);
		}
	else
		{	//keep memory of surface id - search will be done during cleanup. Space already been reserved
		iRemovedSurfaces.Append(oldSurfaceId);
		}

	//order placed element correctly in iPlacedElements
	TPlacedAttributes tempAttributes = iPlacedElements[aIndex];
	iPlacedElements.Remove(aIndex);
	//keep cache up to date
	if (iCache.Count() > aIndex)
		{
		iCache[aIndex].Close();
		iCache.Remove(aIndex);
		}

	WS_ASSERT_DEBUG(iHighestReusedIndex != -1,EWsPanicRecycleElement);
	if (iHighestReusedIndex == -1)
		{
		return KErrArgument;
		}
	
	error = iPlacedElements.Insert(tempAttributes, iHighestReusedIndex);
	if (error < KErrNone)
		{	//must destroy the element and leave.  Old elements surface unregistration already dealt with
		aScene.DestroySceneElement(tempAttributes.iElement);
		clippedRegion.Close();
		return KErrNoMemory;
		}
	returnValue|=EResort;	//element has been moved, will need a resort
	//set placed element attributes
	iPlacedElements[iHighestReusedIndex].SetUnassigned(EFalse);
	iPlacedElements[iHighestReusedIndex].SetLockedConfig(ETrue);
	iPlacedElements[iHighestReusedIndex].SetChangedClip(ETrue);
	
	TRect tempViewport;
	aSurfaceConfiguration.GetViewport(tempViewport);

	TBool cacheSaved = EFalse;
	if (iCache.Count() >= iHighestReusedIndex)
		{	//need to update cache with visible region of element
		TBool insertSuccess = InsertCache(iHighestReusedIndex);
		if (insertSuccess != EFalse)
			{
			iCache[iHighestReusedIndex].Copy(clippedRegion);
			if (!iCache[iHighestReusedIndex].CheckError())
				{
				cacheSaved = ETrue;
				}
			}
		}
	if (cacheSaved == EFalse)
		{	//need to set clipping to element immediately
		TInt count = iCache.Count();
		while(count > iHighestReusedIndex)
			{	//if there is cache where we wished to insert, it and everything above is now invalid
			iCache[count-1].Close();
			iCache.Remove(count-1);
			count--;
			};
		}
	clippedRegion.Close();
	return returnValue;
	}

/** Create a new element for the placed surface.

@param aSurfaceConfiguration The surface configuration for the surface to place.
@param aUserDefinedRegion The user defined clipping of the window.
@param aScene Access to the scene.
@return Serious error, otherwise flags to describe what extra work needs doing.

@see CWindowElement::AssignPlacedElement
*/
TInt CWindowElement::AssignCreateElement(const TSurfaceConfiguration& aSurfaceConfiguration, const TRegion& aUserDefinedRegion, MWsScene& aScene)
	{
	TInt returnValue = 0;
	TInt error = KErrNone;
	STACK_REGION clippedRegion;
	TRect surfaceExtent;
	aSurfaceConfiguration.GetExtent(surfaceExtent);
	//calculate region
	clippedRegion.AddRect(surfaceExtent);
	clippedRegion.Intersect(aUserDefinedRegion);
	if (clippedRegion.CheckError())
		{	//failed, return KErrNoMemory
		clippedRegion.Close();
		return KErrNoMemory;
		}
	
	TPlacedAttributes newElement;
	if (iHighestReusedIndex == -1)
		{
		error = iPlacedElements.Insert(newElement,0);
		}
	else
		{	//insert above current highest reused element
		error = iPlacedElements.Insert(newElement,iHighestReusedIndex+1);
		}
	if (error >= KErrNone)
		{
		iHighestReusedIndex++;
		}
	else
		{
		//FAILED! LEAVE FUNCTION. nothing has been changed
		clippedRegion.Close();
		return KErrNoMemory;
		}

	//create new element with new configuration
	MWsElement* element = NULL;
	TRAP(error,element = aScene.CreateSceneElementL());
	if (error != KErrNone)
		{
		//FAILED! LEAVE FUNCTION. nothing has been changed
		clippedRegion.Close();
		return error;
		}

	element->SetGlobalAlpha( 0xFF );

	iPlacedElements[iHighestReusedIndex].iElement = element;
	error = SetElement(*element,aSurfaceConfiguration,EFalse);
	if (error < KErrNone)
		{
		//must be a bad surface id or viewport. Leave now.
		aScene.DestroySceneElement(element);
		clippedRegion.Close();
		iPlacedElements.Remove(iHighestReusedIndex);
		iHighestReusedIndex--;
		return error;
		}
	
	//Now certain that these must be flagged
	returnValue|=EFastPath;
	returnValue|=EPauseComposition;
	returnValue|=EResort;

	//set placed element attributes
	iPlacedElements[iHighestReusedIndex].SetUnassigned(EFalse);
	iPlacedElements[iHighestReusedIndex].SetChangedClip(ETrue);
	iPlacedElements[iHighestReusedIndex].SetLockedConfig(ETrue);
	
	TRect tempViewport;
	aSurfaceConfiguration.GetViewport(tempViewport);

	TBool cacheSaved = EFalse;
	if (iCache.Count() >= iHighestReusedIndex)
		{	//should try to add new elements region to cache
		TBool insertSuccess = InsertCache(iHighestReusedIndex);
		if (insertSuccess != EFalse)
			{
			iCache[iHighestReusedIndex].Copy(clippedRegion);
			if (!iCache[iHighestReusedIndex].CheckError())
				{
				cacheSaved = ETrue;
				}
			}
		}
	if (cacheSaved == EFalse)
		{	//need to clear cache from this point onward, and set clipping to element immediately
		TInt count = iCache.Count();
		while(count > iHighestReusedIndex)
			{	//if there is cache where we wished to insert, it and everything above is now invalid
			iCache[count-1].Close();
			iCache.Remove(count-1);
			count--;
			};
		}
	//add the new element to the scene
    error = aScene.InsertSceneElement(element, NULL);    //place at the back. will get ordered correctly later
	WS_ASSERT_DEBUG(error == KErrNone,EWsPanicSceneErrorIgnored);
	clippedRegion.Close();
	return returnValue;
	}

/** Extend an existing placed surface element.

@param The index of the element to extend.
@param aSurfaceConfiguration The surface configuration for the surface to place.
@param aUserDefinedRegion The user defined clipping of the window.
@param aScene Access to the scene.
@return Serious error, otherwise flags to describe what extra work needs doing.

@see CWindowElement::AssignPlacedElement
*/
TInt CWindowElement::AssignExtendElement(const TInt aIndex, const TSurfaceConfiguration& aSurfaceConfiguration, const TRegion& aUserDefinedRegion, MWsScene& /*aScene*/)
	{
	TInt returnValue = 0;
	TInt error = KErrNone;
	STACK_REGION backupRegionAgainstFail;
	STACK_REGION clippedRegion;
	TRect surfaceExtent;
	aSurfaceConfiguration.GetExtent(surfaceExtent);
	TRect surfaceViewport;
	aSurfaceConfiguration.GetViewport(surfaceViewport);
	
	if (!iPlacedElements[aIndex].LockedConfig())
		{	//set element now. If viewport is invalid its ok to fail now
		error = SetElement(*iPlacedElements[aIndex].iElement,aSurfaceConfiguration,ETrue);
		if (error < KErrNone)
			{
			backupRegionAgainstFail.Close();
			clippedRegion.Close();
			return error;
			}
		}

	if(iPlacedElements[aIndex].Unassigned() && !iPlacedElements[aIndex].Overlapping())
		{
		//backup stays clear
		clippedRegion.Copy(aUserDefinedRegion);
		}
	else
		{
		clippedRegion.Copy(iNonRedrawVisibleRegion);
		if (iCache.Count() > aIndex)	//if cache is ok
			{
			clippedRegion.Intersect(iCache[aIndex]);
			clippedRegion.Union(iCache[aIndex]);	//previously added regions should be kept
			}
		else
			{
			STACK_REGION tempSurfaceBaseArea;
            TRect destinationRect;
            iPlacedElements[aIndex].iElement->GetDestinationRectangle(destinationRect);
            tempSurfaceBaseArea.AddRect(destinationRect);
			tempSurfaceBaseArea.Offset(surfaceExtent.iTl);
			clippedRegion.Intersect(tempSurfaceBaseArea);
			clippedRegion.Union(tempSurfaceBaseArea);
			tempSurfaceBaseArea.Close();
			}
		backupRegionAgainstFail.Copy(clippedRegion);
		//clipped region should contain something
		clippedRegion.Union(aUserDefinedRegion);
		}
	clippedRegion.ClipRect(surfaceExtent);
	
	if (backupRegionAgainstFail.CheckError() || clippedRegion.CheckError())
		{	//failed critical calculations, leave now before anything has been changed
		clippedRegion.Close();
		backupRegionAgainstFail.Close();
		return KErrNoMemory;
		}

	TBool setCache = EFalse;
	if (iCache.Count() > aIndex)	//if Cache is ok
		{
		//compare
		TInt compareReturn;
		compareReturn = TRegionExtend::Cast(iCache[aIndex]).TestDifference(clippedRegion);

		if (compareReturn&TRegionExtend::EDiffers)	//element has changed
			{
			iPlacedElements[aIndex].SetChangedClip(ETrue);
			if (compareReturn&TRegionExtend::EAdd)	//element has become revealed on the screen
				{
				returnValue|=EPauseComposition;	//need to pause before the end of assign placed element
				}
			else
				{
				returnValue|=EFastPath;	//fastpath and pause will be called during cleanup
				}
			}
		
		//copy clipped region to cache
		iCache[aIndex].Copy(clippedRegion);	//can fail
		setCache = ETrue;
		if (iCache[aIndex].CheckError())
			{	//copy failed, remove cache from this element onwards
			TInt count = iCache.Count();
			while(count > aIndex)
				{	//if there is cache where we wished to insert, it and everything above is now invalid
				iCache[count-1].Close();
				iCache.Remove(count-1);
				count--;
				};
			setCache = EFalse;
			}
		}
	if (setCache == EFalse)
		{	//need to pause composition and update element immediately
		returnValue|=EPauseComposition;
		}
	iPlacedElements[aIndex].SetUnassigned(EFalse);
	iPlacedElements[aIndex].SetLockedConfig(ETrue);
	iHighestReusedIndex = aIndex;
	backupRegionAgainstFail.Close();
	clippedRegion.Close();
	return returnValue;
	}

/** Called at the end of a redraw to set cached changed to elements, and
removed elements that are still unassigned.

@param aScene Access to the scene.
@return Flags to describe what extra work needs doing.
@see CWindowElementSet::CleanUpPlacedElements
*/
TInt CWindowElement::CleanUpPlacedElements(MWsScene& aScene, TInt aGcDrawingCount)
	{
	if (iStartGcDrawingCount!=aGcDrawingCount)
		{
		FlagAssignedElementsDrawnOver(aGcDrawingCount);
		}
	TInt returnFlags = iFlags;
	if (iFlags > 0)
		{	//copy flags to return, and ensure pause is flagged
		returnFlags|=EPauseComposition;
		}
	TInt ii;
	TBool removeElements = EFalse;
	//destroy placed element reference
	for (ii=0; ii<iPlacedElements.Count(); ii++)
		{
		if (iPlacedElements[ii].Unassigned() && !iPlacedElements[ii].Overlapping())
			{
			returnFlags|=EFastPath;	//need to invoke 1108
			if (iRemovedSurfacesValid)
				{
				iRemovedSurfaces.Append(iPlacedElements[ii].iElement->ConnectedSurface());
				iPlacedElements[ii].iElement->ConnectSurface(TSurfaceId::CreateNullId());
				}
			else
				{
				TSurfaceId tempId = iPlacedElements[ii].iElement->ConnectedSurface();
				iPlacedElements[ii].iElement->ConnectSurface(TSurfaceId::CreateNullId());
				NotifyReleasingSurface(tempId);
				}
			removeElements = ETrue;
			}
		}

	if (removeElements)
		{	//there will be elements to remove
		for (ii=0; ii<iPlacedElements.Count(); ii++)
			{
			if (iPlacedElements[ii].iElement->ConnectedSurface().IsNull())
				{
				aScene.DestroySceneElement(iPlacedElements[ii].iElement);
				iPlacedElements.Remove(ii);
				if (iCache.Count() > ii)
					{	//there is cache for this element, remove it
					iCache[ii].Close();
					iCache.Remove(ii);
					}
				ii--;
				}
			}
		}

	if (iRemovedSurfaces.Count() > 1)
		{	//remove duplicates of surface id from removal list
		for (ii = 0; ii < iRemovedSurfaces.Count() - 1; ii++)
			{
			for (TInt jj = ii+1; jj < iRemovedSurfaces.Count(); jj++)
				{
				if (iRemovedSurfaces[ii] == iRemovedSurfaces[jj])
					{
					iRemovedSurfaces.Remove(jj);
					jj--;
					}
				}
			}
		}
	
	//do global search for surface ids
	while (iRemovedSurfaces.Count()>0)
		{
		TInt tempRemoval = iRemovedSurfaces.Count() - 1;
		NotifyReleasingSurface(iRemovedSurfaces[tempRemoval]);
		iRemovedSurfaces.Remove(tempRemoval);
		};

	iRemovedSurfaces.Reset();

	//clip unassigned overlapping entries, mark as changedclip if region changed
	for (ii=0; ii<iPlacedElements.Count(); ii++)
		{
		if (iPlacedElements[ii].Unassigned())	//non overlapping unassigned have already been destroyed
			{	//these must overlap
			TBool failureOccured = EFalse;
			if (iCache.Count() > ii)
				{
				//if cache region is entirely inside non redraw vis region, nothing to update!
				TInt changed = 
						TRegionExtend::Cast(iCache[ii]).TestDifference(iNonRedrawVisibleRegion);
				if (changed&TRegionExtend::ESub)
					{	//the clipping will change
					iCache[ii].Intersect(iNonRedrawVisibleRegion);
					if (iCache[ii].CheckError())
						{
						failureOccured = ETrue;
						iCache[ii].Close();
						iCache.Remove(ii);
						}
					else
						{
						iPlacedElements[ii].SetChangedClip(ETrue);	//changed clipping
						}
					}
				}
			else
				{	//attempt getting element region to perform calculation and update element directly
				STACK_REGION elementRegion;
	            TRect destinationRect;
	            iPlacedElements[ii].iElement->GetDestinationRectangle(destinationRect);
	            elementRegion.AddRect(destinationRect);
				if (elementRegion.CheckError())
					{
					failureOccured = ETrue;
					}
				else
					{
					TRect elementExtent;
					iPlacedElements[ii].iElement->GetDestinationRectangle(elementExtent);
					elementRegion.Offset(elementExtent.iTl);	//get element region into absolute coordinates
					TInt changed =
						TRegionExtend::Cast(elementRegion).TestDifference(iNonRedrawVisibleRegion);
					if (changed&TRegionExtend::ESub)
						{	//need to clip element back
						elementRegion.Intersect(iNonRedrawVisibleRegion);
						if (!elementRegion.CheckError())
							{
							elementRegion.Offset(-elementExtent.iTl);	//put element region back into relative coords
							returnFlags|=EFastPath;
							}
						else
							{
							failureOccured = ETrue;
							}
						}
					}
				elementRegion.Close();
				}
			if (failureOccured)
				{	//need to release element
				TSurfaceId tempId = iPlacedElements[ii].iElement->ConnectedSurface();
				iPlacedElements[ii].iElement->ConnectSurface(TSurfaceId::CreateNullId());
				NotifyReleasingSurface(tempId);
				aScene.DestroySceneElement(iPlacedElements[ii].iElement);
				returnFlags|=EFailed;
				iPlacedElements.Remove(ii);	//remove placed element entry, cache is already removed
				ii--;
				}
			}
		}

	//update any elements marked as changedclip , clear all cache, clear flags
	for (ii=0; ii<iPlacedElements.Count(); ii++)
		{
		if (iPlacedElements[ii].ChangedClip())
			{
			returnFlags|=EFastPath;	//need to invoke 1108
			}
		else
			if (	!(returnFlags&EFastPath)
				&&	iPlacedElements[ii].LockedConfig() && !iPlacedElements[ii].Unassigned()
				&&	iPlacedElements[ii].DrawnOver()!=iPlacedElements[ii].DrawnOverLast()	)
						{
						returnFlags|=EFastPath;
						}
		iPlacedElements[ii].ClearRedrawFlags();
		if (iCache.Count() > ii)
			{
			iCache[ii].Clear();
			}
		}
	iCache.Reset();
	iNonRedrawVisibleRegion.Clear();
	iHighestReusedIndex = -1;
	return returnFlags;
	}

/** Move all elements in this windowelement to above the supplied element in the scene

@param aScene Access to the scene.
@param aElement The element to set all this windows elements above.
*/

void CWindowElement::MoveToAboveGivenElement(MWsScene& aScene, MWsElement* aElement)
	{
	MWsElement* newAboveElement = iBackgroundElement.iElement;
	MWsElement* previousAboveElement = aElement;
	if (newAboveElement)
		{
		aScene.InsertSceneElement(newAboveElement, previousAboveElement);	//background element is put on first
		previousAboveElement = newAboveElement;		
		}
	TInt placedElements = iPlacedElements.Count();
	if (placedElements != 0)
		{
		for (TInt ii = 0; ii < placedElements; ii++)
			{
			newAboveElement = iPlacedElements[ii].iElement;
			aScene.InsertSceneElement(newAboveElement, previousAboveElement);	//place element above previous above element
			previousAboveElement = newAboveElement;	
			}
		}
	}

/** Updates the elements extent, whether from a window movement or a change in window size

@param aOffset The movement of the window.  If NULL then the window has changed size.
@see CWindowElementSet::UpdateElementExtent
*/
void CWindowElement::UpdateElementExtent(const TPoint* aOffset)
	{
	if (aOffset)	//window moved
		{
		TRect tempExtent;
		MWsElement* element = iBackgroundElement.iElement;
		TBool complete = EFalse;
		TInt placedElementDone = -1;
		while (!complete)
			{
			if (!element && placedElementDone < iPlacedElements.Count()-1)
				{
				placedElementDone++;
				element = iPlacedElements[placedElementDone].iElement;
				}
			if (!element)
				{
				complete = ETrue;
				}
			else
				{
				tempExtent = const_cast<CWsClientWindow&>(iWindow).GetOriginalDestElementRect();
				tempExtent.Move(*aOffset);
				element->SetDestinationRectangle(tempExtent);
				const_cast<CWsClientWindow&>(iWindow).SetOriginalDestElementRect(tempExtent);
				}
			element = NULL;
			}
		}
	else			//window changed size
		{
		if (!iBackgroundElement.ExplicitExtent() && iBackgroundElement.iElement)
			{
			iBackgroundElement.iElement->SetDestinationRectangle(iWindow.FullRect());
			const_cast<CWsClientWindow&>(iWindow).SetOriginalDestElementRect(iWindow.FullRect());
			}
		}
	}

/** Checks the windows placed elements when the windows visibility is changed.
If the placed elements are no longer visible, they are removed.

@param aRegion The new visible region of the window.
@param aScene Access to the scene.
@return ETrue if any elements have been removed, otherwise EFalse.
@see CWindowElementSet::SetVisibleRegion
*/
TBool CWindowElement::SetVisibleRegion(const TRegion& aRegion, MWsScene& aScene)
	{
	TBool retcode=EFalse;
	if (iPlacedElements.Count() == 0)
		{
		return EFalse;	//there is noting to do
		}
	
	STACK_REGION tempRegion;
	for (TInt ii = 0; ii < iPlacedElements.Count(); ii++)
		{
		TRect tempExtent;
        tempRegion.Clear();
        iPlacedElements[ii].iElement->GetDestinationRectangle(tempExtent);
        tempRegion.AddRect(tempExtent);
		if (tempRegion.CheckError())	//if there was error getting region
			{
			tempRegion.Clear();
			tempRegion.AddRect(tempExtent);
			}
		else
			{	//offset basearea of element
			tempRegion.Offset(tempExtent.iTl);
			}
		TInt regionReturn = TRegionExtend::Cast(aRegion).TestDifference(tempRegion);
		if (regionReturn&TRegionExtend::ENoIntersect)
			{	//placed surface needs to be removed
			TSurfaceId tempId = iPlacedElements[ii].iElement->ConnectedSurface();
			iPlacedElements[ii].iElement->ConnectSurface(TSurfaceId::CreateNullId());
			NotifyReleasingSurface(tempId);
			aScene.DestroySceneElement(iPlacedElements[ii].iElement);
			retcode = ETrue;	//a element has been removed
			iPlacedElements.Remove(ii);
			if (iCache.Count() > ii)
				{	//keep cache up to date
				iCache[ii].Close();
				iCache.Remove(ii);
				}
			ii--;
			}
		}
	tempRegion.Close();
	return retcode;
	}

/** Checks if any of the windows element ids match the one in question.

@param aSurfaceId The surface id to match against.
@return ETrue if a match is found, otherwise EFalse.

@see CWindowElementSet::SearchDuplicateSurfaceId
*/
TBool CWindowElement::SearchDuplicateSurfaceId(const TSurfaceId& aSurfaceId)
	{
	if (iBackgroundElement.iElement)
		{
		if (iBackgroundElement.iElement->ConnectedSurface() == aSurfaceId)
			{
			return ETrue;
			}
		}
	if (iPlacedElements.Count() > 0)
		{
		for (TInt ii = 0; ii < iPlacedElements.Count(); ii++)
			{
			if (iPlacedElements[ii].iElement)	//if removed without reserved space for ids, could be a null element
				{
				if (iPlacedElements[ii].iElement->ConnectedSurface() == aSurfaceId)
					{
					return ETrue;
					}
				}
			}
		}
	return EFalse;
	}

/** Sets the opacity for the background surface and if setting to 0 removes placed surfaces.

@param aOpacity The opacity to set.
@param aScene Access to the scene.
@see CWindowElementSet::SetElementOpacity
*/
void CWindowElement::SetElementOpacity(TInt aOpacity, MWsScene& aScene)
	{
	if (iBackgroundElement.iElement)
		{
	    TUint32 flags = 0;
	    iBackgroundElement.iElement->GetTargetRendererFlags(flags);
	    flags |= MWsElement::EElementTransparencyGlobalAlpha;
		iBackgroundElement.iElement->SetTargetRendererFlags(MWsElement::EElementTransparencyGlobalAlpha);
		iBackgroundElement.iElement->SetGlobalAlpha(aOpacity);
		iBackgroundElement.SetConcealed(aOpacity==0);
		}
	if (aOpacity == 0)
		{
		while (iPlacedElements.Count() > 0)
			{	//remove any placed elements
			TInt placedElement = iPlacedElements.Count()-1;
			TSurfaceId tempId = iPlacedElements[placedElement].iElement->ConnectedSurface();
			iPlacedElements[placedElement].iElement->ConnectSurface(TSurfaceId::CreateNullId());
			NotifyReleasingSurface(tempId);
			aScene.DestroySceneElement(iPlacedElements[placedElement].iElement);
			iPlacedElements.Remove(placedElement);
			};
		}
	}

MWsElement* CWindowElement::Element() const
    {
    return iBackgroundElement.iElement;
    }

//
// CWindowElementSet
//

/** Destroys the set of window-element pairs.
*/
CWindowElementSet::~CWindowElementSet()
	{
	ASSERT(iElements.Count() == 0);
	iElements.Close();
	}

/** Basic NewL constructor
@param aScene To allow access to the scene.
@param aComposer To allow access to the composer.
*/
CWindowElementSet* CWindowElementSet::NewL(MWsScene& aScene)
	{
	CWindowElementSet* wls = new (ELeave) CWindowElementSet(aScene);
	return wls;
	}

/** Sets a new background surface on the window supplied.
Will remove any previous background surface.

@param aWindow The window to place a background surface on.
@return A reference to the new background surface attributes.
*/
TBackgroundAttributes& CWindowElementSet::AcquireBackgroundElementL(CWsClientWindow& aWindow)
	{
	// Find the matching element.
	TInt index;
	TInt result = FindEntry(aWindow, index);
	MWsElement* element;

	// If a background element is already associated with the window, then unregister
	// the surface. Create and add a new element to scene.
	// This will ensure that the element has default values.
	if (result != KErrNotFound && iElements[index]->iBackgroundElement.iElement)
	    {
	    element = iElements[index]->iBackgroundElement.iElement;
		TSurfaceId surface = element->ConnectedSurface();
		element->ConnectSurface(TSurfaceId::CreateNullId());
		UnregisterSurface(surface);
		iScene.DestroySceneElement(element);
	    }
	if (index < 0)
		{
		index = 0;
		}

	// Allocate a new element and add it to the set and the scene
	element = iScene.CreateSceneElementL();
	
	TInt returnCode;
	if (result == KErrNotFound)
		{
		CWindowElement* winelement = new CWindowElement(aWindow);
		if (!winelement)
			{
			iScene.DestroySceneElement(element);
			User::Leave(KErrNoMemory);
			}
		returnCode = iElements.Insert(winelement, index);
		if(returnCode != KErrNone)
		    {
		    delete winelement;
		    iScene.DestroySceneElement(element);
		    User::Leave(returnCode);
		    }
		}

	// Use the element below the insertion point to decide where the element
	// goes in the scene
	returnCode = KErrNone;
	if (index == 0)
		{
		returnCode = iScene.InsertSceneElement(element, NULL);
		}
	else
		{
		//Find highest element in window below
		MWsElement* below;
		TInt placedCount = iElements[index-1]->iPlacedElements.Count();
		if (placedCount > 0)
			{
			below = iElements[index-1]->iPlacedElements[placedCount-1].iElement;
			}
		else
			{	//below = above = background surface
			below = iElements[index-1]->iBackgroundElement.iElement;
			}
		returnCode = iScene.InsertSceneElement(element, below);
		}
	
	__ASSERT_DEBUG(returnCode == KErrNone, Panic(EWsPanicSceneErrorIgnored));

	iElements[index]->iBackgroundElement.iElement = element;
	return iElements[index]->iBackgroundElement;
	}

/** Removes the background element of the specified window.

@param aWindow The window to remove the background element from.
@param aWindowClosing ETrue if aWindow is in between closing state.
@return KErrNone on success or a system-wide error code.
*/
TInt CWindowElementSet::ReleaseBackgroundElement(const CWsClientWindow& aWindow, TBool aWindowClosing /*= EFalse*/)
	{
	TInt index;

	TInt err = FindEntry(aWindow, index, aWindowClosing);
	__ASSERT_DEBUG(err != KErrNotFound, Panic(EWsPanicNoWindowElement));

	if (err==KErrNone)
		{
		CWindowElement* winElement=iElements[index];
		if (winElement->iPlacedElements.Count() == 0)
			{	//destroy the window entry
			DestroyWindowElementEntry(index);
			}
		else
			{	// just destroy the background
			if (winElement->iBackgroundElement.iElement)
				{
				winElement->iBackgroundElement.Destroy(iScene, ETrue);
				}
			}
		}
	return err;
	}

/** Destroys the element associated with this set of attributes

@param aScene To allow access to the scene.
@param aUnregister Whether to call unregister on the surface id.
@return ETrue if a element existed to be destroyed, otherwise EFalse.
*/
TBool TAttributes::Destroy(MWsScene& aScene, TBool aUnregister)
	{
	MWsElement* element = iElement;
	if (element)
		{
		TSurfaceId surface = element->ConnectedSurface();
		element->ConnectSurface(TSurfaceId::CreateNullId());
        if (aUnregister)
            {
            aScene.UnregisterSurface(surface);
            }
		aScene.DestroySceneElement(element);
		iElement = NULL;
		}
	return element!=NULL;
	}

/** Destroys all elements associated with the window and removes the window from the element set

@param aWinElementIndex The index of the window in the element set
*/
void CWindowElementSet::DestroyWindowElementEntry(const TInt aWinElementIndex)
	{
	CWindowElement* winElement=iElements[aWinElementIndex];
	const CWsClientWindow& window = winElement->iWindow;
	winElement->iBackgroundElement.Destroy(iScene, ETrue);
	for (TInt placedIndex=0,maxindex=winElement->iPlacedElements.Count();placedIndex<maxindex;placedIndex++)
		{
		winElement->iPlacedElements[placedIndex].Destroy(iScene, EFalse);
		}
	winElement->iPlacedElements.Close();
	delete winElement;
	iElements.Remove(aWinElementIndex);
	window.Redraw()->SetHasElement(EFalse);
	window.Screen()->ElementRemoved();
	}

/** Removes all elements in the associated window.

@param aWindow The window to remove the elements for.
@param aWindowClosing ETrue if aWindow is in between closing state.
@return KErrNone on success, KErrNotFound if there was no windowelement entry.
*/
TInt CWindowElementSet::ReleaseAllElements(const CWsClientWindow& aWindow, TBool aWindowClosing /*= EFalse*/)
	{
	TInt index;

	TInt err = FindEntry(aWindow, index, aWindowClosing);
	__ASSERT_DEBUG(err != KErrNotFound, Panic(EWsPanicNoWindowElement));
	if (err==KErrNone)
		{
		DestroyWindowElementEntry(index);
		}
	return err;
	}

/** For every window element, checks if any of the windows element ids match the one in question.

@param aSurfaceId The surface id to match against.
@return ETrue if a match is found, otherwise EFalse.

@see CWindowElement::SearchDuplicateSurfaceId
*/
TBool CWindowElementSet::SearchDuplicateSurfaceId(const TSurfaceId& aSurfaceId)
	{
	TInt windowElements = iElements.Count();
	for (TInt ii = 0; ii < windowElements; ii++)
		{
		if (iElements[ii]->SearchDuplicateSurfaceId(aSurfaceId))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

/** For a window, sets the opacity for the background surface and if setting to 
0 removes placed surfaces.

@param aWindow The window to perform the function on.
@param aOpacity The opacity to set.
@see CWindowElement::SetElementOpacity
*/
void CWindowElementSet::SetElementOpacity(CWsClientWindow& aWindow, TInt aOpacity)
	{
	WS_ASSERT_DEBUG(aOpacity == 0 || aOpacity == 255,EWsPanicUnexpectedOpacity);
	return FindElement(aWindow)->SetElementOpacity(aOpacity, iScene);
	}

/** Finds all elements within the window that overlap with the region and 
marks them as unassigned.
If any part of these unassigned elements are outside the region, they are also 
marked overlapping and locked config.

@param aRedrawRegion The region in which elements will be unassigned
@param aWindow The window to perform the function on.
@return KErrNone on success or KErrNoMemory if critical calculations could not be made.
@see CWindowElement::UnassignPlacedElements
*/
TInt CWindowElementSet::UnassignPlacedElements(const TRegion& aRedrawRegion, const CWsClientWindow& aWindow, TInt aGcDrawingCount)
	{
	return FindElement(aWindow)->UnassignPlacedElements(aRedrawRegion, aGcDrawingCount);
	}

/** Called to place a element within the window.
It will either create a new element, recycle a element, or extend a element .

@param aPlacedAttributes Returns the attributes of the surface placed.
@param aSurfaceConfiguration The surface configuration for the surface to place.
@param aUserDefinedRegion The user defined clipping of the window.
@param aWindow The window to perform the function on.
@return KErrNone on success or a system-wide error code.
@see CWindowElement::AssignPlacedElement
*/
TInt CWindowElementSet::AssignPlacedElement(
		TPlacedAttributes*& aPlacedAttributes,
		const TSurfaceConfiguration& aSurfaceConfiguration,
		const TRegion& aUserDefinedRegion,
		const CWsClientWindow& aWindow,
		TInt aGcDrawingCount	)
	{
	TInt index;
	TInt error = KErrNone;
	TInt result = FindEntry(aWindow, index);
	TBool insertedElement = EFalse;
	if (result == KErrNotFound)
		{
		CWindowElement* winelement = new CWindowElement(aWindow);
		if (!winelement)
			{
			return KErrNoMemory;		//memory error
			}
		error = iElements.Insert(winelement, index);
		if (error == KErrNone)
			{
			insertedElement = ETrue;
			aWindow.Redraw()->SetHasElement(ETrue);
			STACK_REGION windowRegion = aWindow.Abs();
			if (!windowRegion.CheckError())
				{
				error = iElements[index]->UnassignPlacedElements(windowRegion, aGcDrawingCount);
				}
			else
				{
				error = KErrNoMemory;
				}
			windowRegion.Close();
			}
		else
			{
			delete winelement;
			}
		}
	if (error == KErrNone)
		{
		
		TInt assignReturn = iElements[index]->AssignPlacedElement(aPlacedAttributes, aSurfaceConfiguration,
				aUserDefinedRegion, iScene, aGcDrawingCount);
		error = assignReturn;	//return assign flags
		}
	if (error < KErrNone && insertedElement)
		{	//remove this element that has just been created
		aWindow.Redraw()->SetHasElement(EFalse);
		iElements.Remove(index);
		}
	return error;
	}

/** Marks all elements that have been assigned since the unassign as drawn over.

@param aWindow The window to perform the function on.
@see CWindowElement::FlagAssignedElementsDrawnOver
**/
//void CWindowElementSet::FlagAssignedElementsDrawnOver(const CWsClientWindow& aWindow)
//	{
//	FindElement(aWindow)->FlagAssignedElementsDrawnOver();
//	}

/** Called at the end of a redraw to set cached changed to elements, and
removed elements that are still unassigned.

@param aWindow The window to perform the function on.
@return Flags to describe what extra work needed doing.
@see CWindowElement::CleanUpPlacedElements
*/
TInt CWindowElementSet::CleanUpPlacedElements(const CWsClientWindow& aWindow, TInt aGcDrawingCount)
	{
	TInt index;
	TInt error = FindEntry(aWindow,index);
	if (error < 0)
		{
		WS_ASSERT_DEBUG(0,EWsPanicNoWindowElement);
		return error;
		}
	TInt returnFlags = iElements[index]->CleanUpPlacedElements(iScene, aGcDrawingCount);

	if (!iElements[index]->iBackgroundElement.iElement && iElements[index]->iPlacedElements.Count() == 0)
		{	//remove window entry
		DestroyWindowElementEntry(index);
		}
	else
		{
		if (returnFlags&CWindowElement::EResort)
			{	//need to sort elements
			//find front most visible element in window behind, invoke sort by z order
			if (index == 0)
				{	//this element is backmost window
				iElements[index]->MoveToAboveGivenElement(iScene,NULL);
				}
			else
				{	//place infront of highest element behind
				if (iElements[index-1]->iPlacedElements.Count() == 0)
					{	//top element of element behind must be the background element
					iElements[index]->MoveToAboveGivenElement(iScene,iElements[index-1]->iBackgroundElement.iElement);				
					}
				else
					{	//top element of element behind must be highest placed element
					TInt placedCount = iElements[index-1]->iPlacedElements.Count();
					iElements[index]->MoveToAboveGivenElement(iScene,iElements[index-1]->iPlacedElements[placedCount-1].iElement);
					}
				}
			}
		}
	if (returnFlags&CWindowElement::EFailed)
		{	//visible elements may have been deleted, error to suggest a full window redraw
		return KErrGeneral;
		}
	return returnFlags;
	}

/** Counts how many elements there are in the set.

@return The number of elements in the set.
*/
TInt CWindowElementSet::Count() const
	{
	return iElements.Count();
	}

/** Returns the background attributes for the specified window

@param aWindow The window to perform the function on.
@return The background surface attributes.  If the window has no elementset entry, returns NULL.
*/
TBackgroundAttributes* CWindowElementSet::FindBackgroundElement(const CWsClientWindow& aWindow)
	{
	TInt index;
	TInt err = FindEntry(aWindow, index);

	if (err != KErrNotFound)
		{
		return &(iElements[index]->iBackgroundElement);
		}
	return NULL;
	}

/**	Returns the contents of the element data associated with the input window.
If this method is successful, then neither pointer will be NULL.

@return standard symbian error code.
@param	aWindow	window to find
@param	aBackAttr	backgroud surface attributes associated with the window
@param	aPlacedAttr	array of placed surface attributes associated with the window.
*/
TInt CWindowElementSet::FindElements(	CWsClientWindow const &aWindow, 
									TBackgroundAttributes const * & aBackAttr, 
									RArray<class TPlacedAttributes> const * &	aPlacedAttr	)
	{
	TInt index;

	TInt err = FindEntry(aWindow, index);
//	__ASSERT_DEBUG(err != KErrNotFound, Panic(EWsPanicNoWindowElement));

//	return *iElements[index].iElement;
	if (err >= KErrNone)
		{
		aBackAttr=&iElements[index]->iBackgroundElement;
		aPlacedAttr=&iElements[index]->iPlacedElements;
		}
	else
		{
		aBackAttr=NULL;
		aPlacedAttr=NULL;
		}
	return err;
	
	}

/** Registers the surface with the compositor.
@param aSurface The surface id to register.
@return  KErrNone if successful, KErrNoMemory if registration fails due to low
memory, KErrNotSupported if the surface is not compatible with
this compositor or KErrBadHandle if the given surface ID does not
represent a valid surface. KErrArgument is returned if the
surface does not have both dimensions less than 32 767 pixels.
*/
TInt CWindowElementSet::RegisterSurface(const TSurfaceId& aSurface)
    {
    
    return iScene.RegisterSurface(aSurface);
    }

/** Unregisters the surface with the compositor.

@param aSurface The surface id to register.
@return  KErrNone if successful. KErrInUse if the surface is
used by a layer or layers. KErrBadHandle if the surface
is not currently registered. KErrArgument if
the surface ID is a NULL ID.
*/

void CWindowElementSet::UnregisterSurface(const TSurfaceId& aSurface)
    {
    
    TInt returnCode = iScene.UnregisterSurface(aSurface);
    __ASSERT_DEBUG((returnCode==KErrNone || returnCode==KErrInUse || returnCode==KErrBadHandle), Panic(EWsPanicSceneErrorIgnored));
    }

/** Sorts the array elements into the same order as the windows are in the
hierarchy. Use after window hierarchy has been modified, to update the scene
order to match.
@return EFalse if element order unchanged, ETrue if order may have changed.
*/
TBool CWindowElementSet::SortByZOrder()
	{
	if (iElements.Count() < 2)
		{
		// Early out for the very common cases where there are zero or one
		// elements, which cannot therefore be out of order.
		return EFalse;
		}

	// The approach being used is to first just sort the array, then update the 
	// scene order afterwards. This is simple to code and since there are not
	// expected to be many elements, it should be perfectly good enough.

	TLinearOrder<CWindowElement> order(WindowOrder);
	iElements.Sort(order);

	TBool orderChanged = EFalse;
	MWsElement* below;
	TInt elementCount = iElements.Count();
	for (TInt index = 0; index < elementCount; index++)
		{
		if (index == 0)
			{
			below = NULL;
			}
		else
			{
			//Find highest element in window below
			TInt placedCount = iElements[index-1]->iPlacedElements.Count();
			if (placedCount > 0)
				{
				below = iElements[index-1]->iPlacedElements[placedCount-1].iElement;
				}
			else
				{	//below = above = background surface
				below = iElements[index-1]->iBackgroundElement.iElement;
				}
			}
		//Find lowest element in above window element
		MWsElement* above;
		if (iElements[index]->iBackgroundElement.iElement != NULL)
			{	//use background element
			above = iElements[index]->iBackgroundElement.iElement;	//background element will be bottom
			}
		else	//use bottom element of placed surfaces
			{
			//above = iElements[index]->iPlacedElements[iElements[index]->iPlacedElements.Count()-1].iElement;	//first or last placed element is bottom?
			above = iElements[index]->iPlacedElements[0].iElement;
			}
		if (above->ElementBelow() != below)
			{
			//CALL below window element function to move all elements above 'below'
			iElements[index]->MoveToAboveGivenElement(iScene, below);
	        orderChanged = ETrue;
			}
		}
	return orderChanged;
	}

/** Processes the specified windows placed elements, for when windows visibility is changed.
If the placed elements are no longer visible, they are removed.

@param aWindow The window to call the function on.
@return Positive if any elements have been removed, zero if not, or errorcode. 
@see CWindowElement::SetVisibleRegion
*/
TInt CWindowElementSet::SetVisibleRegion(CWsClientWindow& aWindow)
	{
	TInt index;
	TInt find = FindEntry(aWindow,index);
	WS_ASSERT_DEBUG(find>=KErrNone,EWsPanicNoWindowElement);
	TBool ret = iElements[index]->SetVisibleRegion(aWindow.VisibleRegion(), iScene);

	if (!iElements[index]->iBackgroundElement.iElement && iElements[index]->iPlacedElements.Count() == 0)
		{
		DestroyWindowElementEntry(index);
		}
	return ret;
	}

/** Updates the specified windows elements extent, either from a window movement or a change in window size

@param aWindow The window to call the function on.
@param aOffset The movement of the window.  If NULL then the window has changed size.
@see CWindowElement::UpdateElementExtent
*/
void CWindowElementSet::UpdateElementExtent(const CWsClientWindow& aWindow, const TPoint* aOffset)
	{
	FindElement(aWindow)->UpdateElementExtent(aOffset);
	}

/** Method to fill in a TSurfaceConfiguration from a scene element.

@param aConfiguration Surface configuration to fill in.
@param aElement Element to get information from.
@return Once multiple versions of TSurfaceConfiguration are available, KErrNotSupported if configuration supplied is too small.
*/
TInt CWindowElementSet::GetConfiguration(TSurfaceConfiguration& aConfiguration,MWsElement& aElement)
	{
	TSurfaceId tempSurfaceId = aElement.ConnectedSurface();
	aConfiguration.SetSurfaceId(tempSurfaceId);
	
	//Convert and copy orientation
	aConfiguration.SetOrientation(ElementToGcRotation(aElement.SourceRotation()));
	
	//Convert and copy flip
	TBool flip = aElement.SourceFlipping();
	aConfiguration.SetFlip(flip);

	//Convert and copy viewport
	TRect tempViewport;
	aElement.GetSourceRectangle(tempViewport);
	aConfiguration.SetViewport(tempViewport);
	TRect tempExtent;
	aElement.GetDestinationRectangle(tempExtent);
	aConfiguration.SetExtent(tempExtent);
	return KErrNone;	//Could fail if there are multiple versions of TSurfaceConfiguration
	}

/** Returns the window element entry for the specified window.

@param aWindow The window to call the function on.
*/
CWindowElement* CWindowElementSet::FindElement(const CWsClientWindow& aWindow) const
	{
	TInt index;
	TInt error = FindEntry(aWindow,index);
	if (error == KErrNone)
		return iElements[index];
	WS_ASSERT_DEBUG(EFalse,EWsPanicNoWindowElement);
	return NULL;
	}

/** Creates a set of window-element pairs for this scene.

@param aScene To allow access to the scene.
@param aComposer To allow access to the composer.
*/
CWindowElementSet::CWindowElementSet(MWsScene& aScene) :
	iScene(aScene)
	{}

/** Searches for the entry in iElements with the given window

@param aWindow The window to find the entry of / where it should be inserted.
@param aIndex aIndex is set to entry found or the insertion point, respectively.
@param aLinearSearch ETrue if a linear search of the window element set is required.
@return KErrNone if found or KErrNotFound.
*/
TInt CWindowElementSet::FindEntry(const CWsClientWindow& aWindow, TInt& aIndex, TBool aLinearSearch /*= EFalse*/) const
	{
	CWindowElement winelement(aWindow);
	
	// The array order makes use of the parent pointer, which gets reset during
	// window shutdown, so if it is clear fall back to a linear search.
	if (!aWindow.BaseParent() || aLinearSearch)
		{
		TIdentityRelation<CWindowElement> match(WindowMatch);
		
		aIndex = iElements.Find(&winelement, match);
		return (aIndex == KErrNotFound) ? KErrNotFound : KErrNone;
		}
	else
		{
		TLinearOrder<CWindowElement> order(WindowOrder);
		return iElements.FindInOrder(&winelement, aIndex, order);
		}
	}

/** Used to find an entry in the set when order cannot be used. 

@param aFirst First windowelement to compare.
@param aSecond Second windowelement to compare.
@return ETrue if the entries are the same, and EFalse if they are not.
*/
TBool CWindowElementSet::WindowMatch(const CWindowElement& aFirst, const CWindowElement& aSecond)
	{
	return (&aFirst.iWindow == &aSecond.iWindow);
	}


/** Used to determine the order of entries in the set.

@param aFirst First windowelement to compare.
@param aSecond Second windowelement to compare.
@return zero if the entries are the same, a negative value if 
aFirst is behind aSecond or a positive value if aFirst is in front of aSecond.
*/
TInt CWindowElementSet::WindowOrder(const CWindowElement& aFirst, const CWindowElement& aSecond)
	{
	TInt result = 0;

	if (&aFirst.iWindow != &aSecond.iWindow)
		{
		result = aFirst.iWindow.IsInfrontOf(&aSecond.iWindow) ? 1 : -1;
		}

	return result;
	}


/**	Re-sends the extents for all the elements (in this window) to the scene  	
 * 	This allows the renderstage to re-scale those element extents
 * 
 * 
 **/
void CWindowElement::ResubmitAllElementExtents()
	{
	if (MWsElement* element=iBackgroundElement.iElement)
		{
		TRect extent(TRect::EUninitialized);
		element->GetDestinationRectangle(extent);
		element->SetDestinationRectangle(extent);
		}
	TInt elementCount = iPlacedElements.Count();
	for (TInt index = 0; index < elementCount; index++)
		{
		if (MWsElement* element=iPlacedElements[index].iElement)
			{
			TRect extent(TRect::EUninitialized);
			element->GetDestinationRectangle(extent);
			element->SetDestinationRectangle(extent);
			}
		}
	}

/**	Re-sends the extents for all the elements (in all the windows) to the scene  	
 * 	This allows the renderstage to re-scale those element extents
 * 
 * 
 **/
void CWindowElementSet::ResubmitAllElementExtents()
	{
	TInt elementCount = iElements.Count();
	for (TInt index = 0; index < elementCount; index++)
		{
		iElements[index]->ResubmitAllElementExtents();
		}
	}

//
// Debug functions
//

/** Returns background attributes for the specified window index.
For use with debug client interface.

@param aWin Window index to get the surface attributes from.
@return Background attributes for the specified window.  NULL if the window index is invalid.
*/
const TBackgroundAttributes* CWindowElementSet::DebugBackgroundAt(TUint aWin)const
	{
	if (aWin>=Count())
		return NULL;
	return &iElements[aWin]->iBackgroundElement;
	}

/** Returns the client window for the specified window index.
For use with debug client interface.

@param aWin Window index to get the client window from.
@return Client window for the specified window.  NULL if the window index is invalid.
*/
const CWsClientWindow* CWindowElementSet::DebugWindowAt(TUint aWin)const
	{
	if (aWin>=Count())
		return NULL;
	return &iElements[aWin]->iWindow;
	}

/** Returns how many placed surfaces are on the specified window
For use with debug client interface.

@param aWin Window index to get the client window from.
@return Amount of placed surfaces for the specified window index
*/
TInt CWindowElementSet::DebugPlacedCountAt(TUint aWin)const
	{
	if (aWin>=Count())
		return -1;
	return iElements[aWin]->iPlacedElements.Count();
	}

/** Returns the placed attributes for the specified placed surface index in the
specified window index.
For use with debug client interface.

@param aWin Window index to get the client window from.
@param aPlace Placed surface index to get the placed surface attributes
@return Placed surface attributes.  NULL if either index was invalid.
*/
const TPlacedAttributes* CWindowElementSet::DebugPlacedAt(TUint aWin,TUint aPlace)const
	{
	if (aWin>=Count())
		return NULL;
	if (aPlace>=iElements[aWin]->iPlacedElements.Count())
		return NULL;
	return &iElements[aWin]->iPlacedElements[aPlace];
	}

MWsElement* CWindowElementSet::GetElementFromWindow(const CWsClientWindow& aWindow) const
    {
    CWindowElement* windowElement = FindElement(aWindow);
    if (windowElement)
        return windowElement->Element();
    else 
        return NULL; 
    }
