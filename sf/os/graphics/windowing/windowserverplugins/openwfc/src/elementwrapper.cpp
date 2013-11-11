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
//

#include "openwfcpanics.h"
#include "elementwrapper.h"
#include <WF/wfc.h>
#include <graphics/surfacetypes.h>
#include <hal.h>

CElementWrapper* CElementWrapper::NewL(COpenWfcWrapper& aCOpenWfcWrapper, CElementWrapper*& aCleanupPrevRef)
    {
    CElementWrapper* wrapper = new (ELeave) CElementWrapper(aCOpenWfcWrapper);
    CleanupStack::PushL(wrapper);
    wrapper->ConstructL(aCleanupPrevRef);
    CleanupStack::Pop(wrapper);
    return wrapper;
    }

CElementWrapper::CElementWrapper(COpenWfcWrapper& aOpenWfcWrapper):
    iUpdateFlags(EUpdate_ALL_ATTRIBS),iOpenWfcWrapper(aOpenWfcWrapper),iGlobalAlpha(0xff),
    iFlipped(EFalse),iRotation(MWsElement::EElementAntiClockwise0)
    {
    }
CElementWrapper::~CElementWrapper()
    {
    wfcDestroyElement(iOpenWfcWrapper.Device(),iElement);

    UnsetSurface(iSurfaceId);
    iUpdateFlags = EUpdate_None;

    if (iCleanupNext)
        {
        iCleanupNext->iCleanupPrevRef=iCleanupPrevRef;
        }
    *iCleanupPrevRef=iCleanupNext;
    }

void CElementWrapper::ConstructL(CElementWrapper*& aCleanupPrevRef)
    {
    if (aCleanupPrevRef)
        {
        iCleanupNext=aCleanupPrevRef;
        iCleanupNext->iCleanupPrevRef=&iCleanupNext;
        }
    aCleanupPrevRef=this;
    iCleanupPrevRef=&aCleanupPrevRef;

    iElement=wfcCreateElement(iOpenWfcWrapper.Device(),iOpenWfcWrapper.OnScreenContext(),NULL);
    if (iElement==WFC_INVALID_HANDLE)
        {   //if context and device are good then only explanation is memory
        #ifdef _DEBUG
            WFCErrorCode wfcErr = 
        #endif
        wfcGetError(iOpenWfcWrapper.Device());
        OPENWFC_ASSERT_DEBUG(wfcErr==WFC_ERROR_OUT_OF_MEMORY,EPanicUnexpectedErrorElementConstruct);
        User::Leave(KErrNoMemory);
        }
    }

TInt CElementWrapper::SetSourceRectangle(const TRect& aSrc)
    {
    if(iSurfaceId.IsNull())
        return KErrGeneral;

    //Check aSrc rect is within the bounds of the source image
    TInt32 width,height;
    HelpStreamGetHeader(&width,&height);
    if(aSrc.iTl.iX < 0 || aSrc.iTl.iY < 0 || aSrc.iBr.iX > width || aSrc.iBr.iY > height)
        {
        return KErrArgument;
        }

    iSourcRect = aSrc;
    iUpdateFlags |= EUpdate_SettingValid|EUpdate_SOURCE_RECTANGLE;
    iOpenWfcWrapper.SetCompositionModified();
    
    return KErrNone;
    }

TInt CElementWrapper::GetSourceRectangle(TRect& aSrc)
    {
    if(iSurfaceId.IsNull())
        return KErrGeneral;

    if (iSourcRect.IsEmpty())
        {        
        //get true source rectangle from surface
        TInt32 width,height;
        HelpStreamGetHeader(&width,&height);
        aSrc.iTl.iX = 0;
        aSrc.iTl.iY = 0;
        aSrc.iBr.iX = width;
        aSrc.iBr.iY = height;
        }
    else
        {
        aSrc=iSourcRect;
        }
    
    return KErrNone;
    }

TInt CElementWrapper::SetSourceFlipping(const TBool aFlip)
    {
    if(iSurfaceId.IsNull())
        return KErrGeneral;

    iFlipped = aFlip;
    iUpdateFlags |= EUpdate_SettingValid|EUpdate_SOURCE_FLIP;
    
    return KErrNone;
    }

TBool CElementWrapper::SourceFlipping() const
    {
    if(iSurfaceId.IsNull())
        return EFalse; //default is not flipped
    
    return iFlipped;
    }

TInt CElementWrapper::SetGlobalAlpha(const TInt aAlpha)
    {
    if (aAlpha!=iGlobalAlpha)
        {
        if ((aAlpha>=0xff)!=(iGlobalAlpha==0xff))
            {
            iUpdateFlags |= EUpdate_SettingValid|EUpdate_TRANSPARENCY_TYPES;
            }
        if (aAlpha < 0)
            {
            iGlobalAlpha = 0;
            }
        else
            {
            iGlobalAlpha = aAlpha > 0xff ? 0xff : aAlpha;
            }
        iUpdateFlags |= EUpdate_SettingValid|EUpdate_GLOBAL_ALPHA;
        }
    iOpenWfcWrapper.SetCompositionModified();
    
    return KErrNone;
    }

void CElementWrapper::GlobalAlpha(TInt& aAlpha) const
    {
    aAlpha = iGlobalAlpha;
    }

TInt CElementWrapper::SetDestinationRectangle(const TRect& aDest)
    {
    if(iSurfaceId.IsNull())
        return KErrGeneral;

    //Check aDest is not out of bounds
    if(aDest.iBr.iX < aDest.iTl.iX || aDest.iBr.iY < aDest.iTl.iY)
        {
        return KErrArgument;
        }

    TRect compositionDestRect;
    TRect lastCompositionDestRect;
    GetDestinationRectangle(lastCompositionDestRect);
    TInt err = KErrNone;
    //always do the translate
    err = iOpenWfcWrapper.DisplayPolicy()->MapCoordinates(EApplicationSpace, aDest, ECompositionSpace, compositionDestRect);

    if(compositionDestRect != lastCompositionDestRect)
        {
        iDestinationRect=aDest;
        iUpdateFlags |= EUpdate_SettingValid|EUpdate_DESTINATION_RECTANGLE;

        iOpenWfcWrapper.SetCompositionModified();
        }
    
    return err;
    }

TInt CElementWrapper::GetDestinationRectangle(TRect& aDest) const
    {
    if(iSurfaceId.IsNull())
        return KErrGeneral;

    aDest=iDestinationRect;
    
    return KErrNone;
    }

TInt CElementWrapper::SetDestinationClippingRect(const TRect& aDestClipRect)
	{
	if(iSurfaceId.IsNull())
        return KErrGeneral;

    TRect compositionDestClipRect;
    TRect lastCompositionDestClipRect;
    GetDestinationClippingRect(lastCompositionDestClipRect);
    TInt err = KErrNone;
    //always do the translate
    err = iOpenWfcWrapper.DisplayPolicy()->MapCoordinates(EApplicationSpace, aDestClipRect, ECompositionSpace, compositionDestClipRect);
    
    if(compositionDestClipRect != lastCompositionDestClipRect)
    	{
    	TRect destClipRect(aDestClipRect);
		//Check if the destination clip rectangle is not empty
		if (!destClipRect.IsEmpty())
			{
			//Calculate the intersection of the destination clipped rectangle
			destClipRect.Intersection(iDestinationRect);
			if (!destClipRect.IsEmpty())
				{
				// Check if the clipped rectangle is valid,it doesn't have coordinates
				if (destClipRect.iTl.iX < 0 || destClipRect.iTl.iY < 0)
					{
					return KErrArgument;
					}
				}
			}
		if (iDestinationClippingRect != aDestClipRect)
			{
			iDestinationClippingRect = aDestClipRect;
			iUpdateFlags |= EUpdate_SettingValid | EUpdate_DESTINATION_CLIP_RECTANGLE;

			iOpenWfcWrapper.SetCompositionModified();
			}
		err = KErrNone;
		}
	return err;
	}

TInt CElementWrapper::GetDestinationClippingRect(TRect& aDestClipRect) const
    {
	if(iSurfaceId.IsNull())
        return KErrGeneral;

    aDestClipRect = iDestinationClippingRect;

    return KErrNone;
    }
    

/*
Fast swap of two 32-bit TInt type integers.
@param aA Argument one. 
@param aB Argument two.
*/
inline void SwapInt(TInt& aA, TInt& aB)
   {
   aA ^= aB;
   aB ^= aA;
   aA ^= aB;
   }

/*
 Rectangle corner point enumeration.
 Consider TopLeft     point as 0
 Consider TopRight    point as 1
 Consider BottomRight point as 2
 Consider BottomLeft  point as 3
*/
enum TRectangleCornerPoint
    {
    ETopLeft,
    ETopRight,
    EBottomRight,
    EBottomLeft
    };

/**
 Get Bottom right corner of the rectangle. 
 @param aRect input rectangle.
 @return Bottom right corner point coordinates of rectangle.
 */
inline TPoint BottomRight(const TRect& aRect)
    {
    return aRect.iBr;
    }

/**
 Get Top left corner of the rectangle. 
 @param aRect input rectangle.
 @return Top left corner point coordinates of rectangle.
 */
inline TPoint TopLeft(const TRect& aRect)
    {
    return aRect.iTl;
    }

/**
 Get Top right corner of the rectangle. 
 @param aRect input rectangle.
 @return Top right corner point coordinates of rectangle.
 */
inline TPoint TopRight(const TRect& aRect)
    {
    return TPoint(aRect.iBr.iX, aRect.iTl.iY);
    }

/**
 Get Bottom left corner of the rectangle. 
 @param aRect input rectangle.
 @return Bottom left corner point coordinates of rectangle.
 */
inline TPoint BottomLeft(const TRect& aRect)
    {
    return TPoint(aRect.iTl.iX, aRect.iBr.iY);
    }

/*
 Make horizontal flip of the corner point.
 */
inline TRectangleCornerPoint FlipRectangleCorner(TRectangleCornerPoint aSourceRectCorner)
    {
    return static_cast<TRectangleCornerPoint> (static_cast<TInt> (aSourceRectCorner ^ 0x00000003));
    }

/*
 Make clockwise rotation of corner point. 
 */
inline TRectangleCornerPoint RotateRectangleCorner(TRectangleCornerPoint aSourceRectCorner,const MWsElement::TElementRotation& aElementRotation)
    {
    return static_cast<TRectangleCornerPoint> ((static_cast<TInt> (aSourceRectCorner) + static_cast<TInt> (aElementRotation)) & 0x00000003);
    }

/*
 Check if there is an inverted aspect ratio. 
 */
inline TBool CheckInvertedAspectRatio(const MWsElement::TElementRotation& aElementRotation)
    {
    return  (aElementRotation & 0x00000001);
    }
/**
 Compare the dedicated rectangle corner coordinates of two rectangles.
 @param aCorner rectangle corner point.
 @param aRect1 the first rectangle to compare.
 @param aRect2 the second rectangle to compare.
 @return ETrue if the checked corner coordinates are the same. 
 */
TBool CheckTwoRectangleCorners(TRectangleCornerPoint aCorner,const TRect& aRect1,const TRect& aRect2)
    {
    aCorner = static_cast<TRectangleCornerPoint> (static_cast<TInt> (aCorner) & 0x00000003); // Normalize rectangle corner value
    switch (aCorner)
        {
        case ETopLeft:
            return (TopLeft(aRect1) == TopLeft(aRect2));
        case ETopRight:
            return (TopRight(aRect1) == TopRight(aRect2));
        case EBottomRight:
            return (BottomRight(aRect1) == BottomRight(aRect2));
        case EBottomLeft:
            return (BottomLeft(aRect1) == BottomLeft(aRect2));
        default:
            return EFalse;
        }
    }

/**
 Calculate the new source rectangle extent when a window is clipped by it parent window. 
 Destination clipped rectangle should match to the source rectangle with taking into account 
 flip, scaling and rotation.
 @param aDestElementRect destination element rectangle without parent window clipping   
 @param aDestElementClippedRect destination element rectangle clipped by the parent window.
 @param aSrcOriginalElementRect  source element rectangle without any clipping changes.
 @param aElementFlip horizontal flip flag.
 @param aElementRotation target anti clockwise rotation factor.
 @return clipped source rectangle.
 */  
TRect CElementWrapper::SetSourceDestRect(const TRect& aDestElementRect,const TRect& aDestElementClippedRect,const TRect& aSrcOriginalElementRect,const TBool& aElementFlip,const MWsElement::TElementRotation& aElementRotation) const
    {
	if ((aSrcOriginalElementRect.Width() == 0) || (aSrcOriginalElementRect.Height() == 0))
		{
		// There are either width or height equal zero.
		return TRect();
		}

	TRectangleCornerPoint sourceRectangleCorner = ETopLeft;
	TRect destElementMovedClippedRect(aDestElementClippedRect);
	destElementMovedClippedRect.Intersection(aDestElementRect);
	TBool fullFlipRotationScale = EFalse;
	// Compare four corner points of Destination moved and Destination moved clipped rectangles.
	// Find the first equal corner coordinates for Destination moved and Destination moved clipped rectangles.
	// It is the Fixed or non changeable point/corner of the source rectangle.
	if (destElementMovedClippedRect.IsEmpty())
		{
		return TRect();
		}
	for (sourceRectangleCorner = ETopLeft; ((TInt) sourceRectangleCorner) <= ((TInt) EBottomLeft);)
		{
		if (CheckTwoRectangleCorners(sourceRectangleCorner, aDestElementRect, destElementMovedClippedRect))
			{
			break;
			}
		sourceRectangleCorner = static_cast<TRectangleCornerPoint> ((TInt) sourceRectangleCorner + 1);
		}
	// There are no equals coordinates, hence, it must be calculated with different  by different way
	if ((TInt) sourceRectangleCorner > (TInt) EBottomLeft)
		{
		sourceRectangleCorner = ETopLeft;
		fullFlipRotationScale = ETrue;
		}

	// Clockwise rotate of the detected Fixed point/corner.  
	sourceRectangleCorner = RotateRectangleCorner(sourceRectangleCorner, aElementRotation);

	// If flip flag set, then make horizontal flip of the Fixed point.
	if (aElementFlip)
		{
		sourceRectangleCorner = FlipRectangleCorner(sourceRectangleCorner);
		}

	// Make scale calculation for the source clipping
	TInt dX = 0;
	TInt dY = 0;
	if (fullFlipRotationScale)
		{
		dX = ((aDestElementRect.iTl.iX - aDestElementClippedRect.iTl.iX) * aSrcOriginalElementRect.Width()) / aDestElementRect.Width();
		dY = ((aDestElementRect.iTl.iY - aDestElementClippedRect.iTl.iY) * aSrcOriginalElementRect.Height()) / aDestElementRect.Height();
		}
	TInt clipSrcWidth = (destElementMovedClippedRect.Width() * aSrcOriginalElementRect.Width()) / aDestElementRect.Width();
	TInt clipSrcHeight = (destElementMovedClippedRect.Height() * aSrcOriginalElementRect.Height()) / aDestElementRect.Height();
	TRect resultSourceRect = aSrcOriginalElementRect;

	// Swap the clipping width and height if the target was rotated on 90 or 270 degrees.
	if (CheckInvertedAspectRatio(aElementRotation))
		{
		SwapInt(clipSrcWidth, clipSrcHeight);
		SwapInt(dX, dY);
		}

	// Assuming that coordinates of the source Fixed corner/point was not changed due to clipping,
	// calculate coordinates of an opposite point to the Fixed corner/point.
	// This point must be differ from original source  for providing proper clipping visual result.
	switch (sourceRectangleCorner)
		{
		case ETopLeft:
			{
			// If TopRight of aDestElementRect is not the same as TopRight of aDestElementClippedRect 
			if (!CheckTwoRectangleCorners(ETopRight, aDestElementRect, destElementMovedClippedRect))
				{// calculate point ETopRight
				resultSourceRect.iBr.iX = resultSourceRect.iTl.iX + clipSrcWidth;
				}
			// If BottomLeft of aDestElementRect is not the same as BottomLeft of aDestElementClippedRect
			if (!CheckTwoRectangleCorners(EBottomLeft, aDestElementRect, destElementMovedClippedRect))
				{// calculate point EBottomLeft
				resultSourceRect.iBr.iY = resultSourceRect.iTl.iY + clipSrcHeight;
				}
			if (fullFlipRotationScale)
				{
				resultSourceRect.iTl.iX += dX;
				resultSourceRect.iTl.iY += dY;
				}
			}
			break;
		case ETopRight:
			{
			// if TopLeft of aDestElementRect is not the same as TopLeft of aDestElementClippedRect
			if (!CheckTwoRectangleCorners(ETopLeft, aDestElementRect, destElementMovedClippedRect))
				{// calculate point ETopLeft
				resultSourceRect.iTl.iX = resultSourceRect.iBr.iX - clipSrcWidth;
				}
			// if BottomRight of aDestElementRect is not the same as BottomRight of aDestElementClippedRect
			if (!CheckTwoRectangleCorners(EBottomRight, aDestElementRect, destElementMovedClippedRect))
				{// calculate point EBottomRight
				resultSourceRect.iBr.iY = resultSourceRect.iTl.iY + clipSrcHeight;
				}
			if (fullFlipRotationScale)
				{
				resultSourceRect.iBr.iX -= dX;
				resultSourceRect.iTl.iY += dY;
				}
			}
			break;
		case EBottomRight:
			{
			// if BottomLeft of aDestElementRect is not the same as BottomLeft of aDestElementClippedRect
			if (!CheckTwoRectangleCorners(EBottomLeft, aDestElementRect, destElementMovedClippedRect))
				{// calculate point EBottomLeft
				resultSourceRect.iTl.iX = resultSourceRect.iBr.iX - clipSrcWidth;
				}
			// if TopRight of aDestElementRect is not the same as TopRight of aDestElementClippedRect
			if (!CheckTwoRectangleCorners(ETopRight, aDestElementRect, destElementMovedClippedRect))
				{// calculate point ETopRight
				resultSourceRect.iTl.iY = resultSourceRect.iBr.iY - clipSrcHeight;
				}
			if (fullFlipRotationScale)
				{
				resultSourceRect.iBr.iX -= dX;
				resultSourceRect.iBr.iY -= dY;
				}
			}
			break;
		case EBottomLeft:
			{
			// if BottomRight of aDestElementRect is not the same as BottomRight of aDestElementClippedRect don't change width
			if (CheckTwoRectangleCorners(EBottomRight, aDestElementRect, destElementMovedClippedRect))
				{// calculate point EBottomRight
				resultSourceRect.iBr.iX = resultSourceRect.iTl.iX + clipSrcWidth;
				}
			// if TopLeft of aDestElementRect is not the same as TopLeft of aDestElementClippedRect
			if (CheckTwoRectangleCorners(ETopLeft, aDestElementRect, destElementMovedClippedRect))
				{// calculate point ETopLeft
				resultSourceRect.iTl.iY = resultSourceRect.iBr.iY - clipSrcHeight;
				}
			if (fullFlipRotationScale)
				{
				resultSourceRect.iTl.iX += dX;
				resultSourceRect.iBr.iY -= dY;
				}
			}
			break;
		}
	return resultSourceRect;
	}



TInt CElementWrapper::SetSourceRotation(const TElementRotation aElementRotation)
    {
    if(iSurfaceId.IsNull())
        return KErrGeneral;
    
    if(aElementRotation < 0 || aElementRotation > 3)
        return KErrArgument;
    
    iRotation = aElementRotation;
    iUpdateFlags |= EUpdate_SettingValid|EUpdate_SOURCE_ROTATION;

    iOpenWfcWrapper.SetCompositionModified();
    
    return KErrNone;
    }

MWsElement::TElementRotation CElementWrapper::SourceRotation() const
    {
    if(iSurfaceId.IsNull())
        return EElementAntiClockwise0; //default rotation

    return iRotation;
    }

TInt CElementWrapper::SetTargetRendererFlags(const TUint32& aTargetRendererFlags)
    {
    iTargetRendererFlags = aTargetRendererFlags;
    if (iTargetRendererFlags & EElementTransparencyGlobalAlpha)
        {
        iUpdateFlags |= EUpdate_SettingValid|EUpdate_GLOBAL_ALPHA;
        }
    
    if (iTargetRendererFlags & EElementTransparencySource)
        {
        iUpdateFlags |= EUpdate_SettingValid|EUpdate_TRANSPARENCY_TYPES;
        }
    
    return KErrNone;
    }

TInt CElementWrapper::SetRenderStageFlags(const TUint32& aRenderStageFlags)
    {
    iRenderStageFlags = aRenderStageFlags;
    
    return KErrNone;
    }

void CElementWrapper::GetTargetRendererFlags(TUint32& aTargetRendererFlags) const
    {
    aTargetRendererFlags = iTargetRendererFlags;
    }

void CElementWrapper::GetRenderStageFlags(TUint32& aRenderStageFlags) const
    {
    aRenderStageFlags = iRenderStageFlags;
    }

MWsElement* CElementWrapper::ElementAbove()
    {
    //Check this element has not already been reequsted to be removed
    if (iUpdateFlags&EUpdate_Remove)
        {
        return NULL;
        }
    else
        {
        return iElementAbove;
        }
    }

MWsElement* CElementWrapper::ElementBelow()
    {
    //Check this element has not already been reequsted to be removed
    if (iUpdateFlags&EUpdate_Remove)
        {
        return NULL;
        }
    else
        {
        return iElementBelow;
        }
    }

TInt CElementWrapper::ConnectSurface(const TSurfaceId& aSurface)
    {
    TInt err = KErrNone;
     if (aSurface!=iSurfaceId)
        {
        if (!aSurface.IsNull())
            {
            if(!iOpenWfcWrapper.IncEltRefRegSource(aSurface))
                {
                return KErrNotFound;
                }
            else
                {
                RSurfaceManager::TInfoBuf buff;
                err = err = SurfaceInfo(aSurface, buff);
                if (err != KErrNone)
                    {
                    return iOpenWfcWrapper.DecEltRefRegSource(aSurface);
                    }
                else
                    {
                    iSurfaceSize=buff().iSize;
                    }
                }
            }
        else
            {
            iSurfaceSize.SetSize(0,0);
            }
        UnsetSurface(iSurfaceId);
        iSurfaceId=aSurface;
        iUpdateFlags |= EUpdate_SettingValid|EUpdate_SOURCE;
        if (iSourcRect.IsEmpty())
            {
            iUpdateFlags |= EUpdate_SettingValid|EUpdate_SOURCE_RECTANGLE;
            }
        }
    else
        {   //Note this code is simply here to return the error!
        if (!iSurfaceId.IsNull())
            {
            if (iOpenWfcWrapper.IncEltRefRegSource(iSurfaceId))
                {
                iOpenWfcWrapper.DecEltRefRegSource(iSurfaceId);
                }
            else
                {
                return KErrNotFound;
                }
            }
        }
    
    iOpenWfcWrapper.SetCompositionModified();
    
    return err;
    }

const TSurfaceId& CElementWrapper::ConnectedSurface() const
    {
    return iSurfaceId;
    }

/** 
 * Sets the element source to match the surface ID and optionally locks the reference count until next composition.
 **/
void CElementWrapper::SetEltSourceComposed()
    {
    iUpdateFlags |= EUpdate_SceneCommited;
    }

/** 
 * Sets the element source to match the surface ID and optionally locks the reference count until next composition.
 **/
void CElementWrapper::SetEltSource(WFCElement aElement)const
    {
    WFCSource source = WFC_INVALID_HANDLE;
    if (!iSurfaceId.IsNull())
        {
        source=iOpenWfcWrapper.IncEltRefRegSource(iSurfaceId);
        if(source)
            {
            TInt remains=iOpenWfcWrapper.DecEltRefRegSource(iSurfaceId);
            OPENWFC_ASSERT_DEBUG(remains>0,EPanicWfcNoStreamSurface);
            }
        }
    wfcSetElementAttribi(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_SOURCE,source);
    }

/** 
 * Internal method releases the previous registered surface, and clears the caller's member var.
 * @return true if any resources released
 **/
TBool CElementWrapper::UnsetSurface(TSurfaceId& aSurface)
    {
    if (!aSurface.IsNull())
        {
        TInt err = iOpenWfcWrapper.DecEltRefRegSource(aSurface);
        OPENWFC_ASSERT_DEBUG(err >= 0,EPanicWfcSourceLookupOnElementRemoval);
        aSurface = aSurface.CreateNullId();
        return (err == KErrNone); //>0 indicates object count non-zero
        }
    return EFalse;
    }

void CElementWrapper::SetEltTransparencyTypes(WFCElement aElement)const
    {
    TInt transType = WFC_TRANSPARENCY_NONE;
    if (iTargetRendererFlags & EElementTransparencySource)
        {
        transType |= WFC_TRANSPARENCY_SOURCE;
        }
    if (iTargetRendererFlags & EElementTransparencyGlobalAlpha)
        {
        transType |= WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA;
        }
    wfcSetElementAttribi(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_TRANSPARENCY_TYPES,transType);
    }

void CElementWrapper::HelpStreamGetHeader(khronos_int32_t* aWidth, khronos_int32_t* aHeight)const
    {
    if (aWidth)
        {
        *aWidth=iSurfaceSize.iWidth;
        }
    if (aHeight)
        {
        *aHeight=iSurfaceSize.iHeight;
        }
    }

/**
 *  Maps the source rectangle to fp values matching the region and destination rectangle data over a element object.
 *  This is when the source rectangle changes.
 *  Only repositions existing elements
 **/
void CElementWrapper::SetEltSourceRectangle(WFCElement aElement)
    {
    TRect srcRect;
    GetSourceRectangle(srcRect);
    if (!iDestinationClippingRect.IsEmpty())
    	{
    	 TRect destinationRect=iDestinationRect;
    	 destinationRect.Intersection(iDestinationClippingRect);
    	 if(!destinationRect.IsEmpty())
    		 {
    		 srcRect = SetSourceDestRect(iDestinationRect,destinationRect,srcRect,iFlipped,iRotation);
    		 }
    	}
    float rect[4]={srcRect.iTl.iX,srcRect.iTl.iY,srcRect.Width(),srcRect.Height()};
    wfcSetElementAttribfv(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_SOURCE_RECTANGLE,4,rect);
    }

void CElementWrapper::SetEltSourceFlip(WFCElement aElement)const
    {
    wfcSetElementAttribi(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_SOURCE_FLIP, iFlipped);
    }

/**
 *  Maps the region and destination rectangle data into a element object
 *  taking into account the source rectangle mapping and region data
 *  @return KErrNone or KErrMemory
 **/
void CElementWrapper::SetEltDestinationRectangle(WFCElement aElement)const
    {
    TRect destinationRect=iDestinationRect;
    if (!iDestinationClippingRect.IsEmpty())
    	{
    	destinationRect.Intersection(iDestinationClippingRect);
    	}
    float destRect[4]={destinationRect.iTl.iX,destinationRect.iTl.iY,destinationRect.Width()<0?0:destinationRect.Width(),destinationRect.Height()<0?0:destinationRect.Height()};
    wfcSetElementAttribfv(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_DESTINATION_RECTANGLE,4,destRect);

    LILOG(("### EXIT SetEltDestinationRectangle()"));
    }

/**
 *  Maps the region and destination rectangle data into a element object.
 *  taking in to account the source rectangle mapping and region data
 *  @param aOffset  an offset ADDED to every destination rectangle
 *  @return KErrNone or KErrMemory
 **/
void CElementWrapper::SetEltDestinationRectangle(WFCElement aElement, const TPoint& aOffset)const
    {
    TRect destinationRect=iDestinationRect;
    if (!iDestinationClippingRect.IsEmpty())
        {
        destinationRect.Intersection(iDestinationClippingRect);
        }
    float rect[4]={destinationRect.iTl.iX+aOffset.iX,destinationRect.iTl.iY+aOffset.iY,destinationRect.Width(),destinationRect.Height()};
    wfcSetElementAttribfv(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_DESTINATION_RECTANGLE,4,rect);
    }

void CElementWrapper::RemoveAfter(CElementWrapper** apElementBelow)
    {
    iUpdateFlags&=~(EUpdate_Insert|EUpdate_Remove);
    if(apElementBelow)
        {
        *apElementBelow = iElementAbove;
        if (iElementAbove)
            {
            iElementAbove->iElementBelow = iElementBelow;
            }
        }
    iElementBelow = NULL;
    iElementAbove = NULL;
    }

void CElementWrapper::InsertAfter(CElementWrapper*& aRefElementBelow, CElementWrapper* aElementBelow, TOwfUpdates aUpdateFlag)
    {
    OPENWFC_ASSERT_DEBUG(aUpdateFlag&(EUpdate_Insert|aUpdateFlag&EUpdate_Remove|aUpdateFlag&EUpdate_Destroy), EPanicUnexpectedErrorElementScene);
    iUpdateFlags |= EUpdate_SettingValid|aUpdateFlag;
    iElementAbove = aRefElementBelow;
    iElementBelow = aElementBelow;
    aRefElementBelow = this;
    if (iElementAbove)
        {
        iElementAbove->iElementBelow = this;
        }
    }

/* Returns a pointer referencing this object from below.
 * @return the pointer or null if there is no "below" object
 */
CElementWrapper** CElementWrapper::RefsMyElementBelow()
    {
    if (iElementBelow)
        {
        return &iElementBelow->iElementAbove;
        }

    return NULL;
    }

CElementWrapper*& CElementWrapper::MyElementAbove()
    {
    return iElementAbove;
    }

TInt CElementWrapper::WfcRotation(MWsElement::TElementRotation aRotation)
    {
    switch (aRotation)
        {
    case EElementAntiClockwise0:
        return WFC_ROTATION_0;
    case EElementAntiClockwise90:
        return WFC_ROTATION_270;
    case EElementAntiClockwise180:
        return WFC_ROTATION_180;
    default: // EElementAntiClockwise270
        return WFC_ROTATION_90;
        }
    }

TBool CElementWrapper::IsNewElementLegal()const
    {
    //OWF SI will fail if certain illegal element attributes are passed to it.
    //Various members must be non-zero:
        //destination rectangle
        //source
        //global alpha
    if (iDestinationRect.IsEmpty() || iSurfaceId.IsNull()||iGlobalAlpha==0)
        {
        return EFalse;
        }
    //If the source rectangle is set it must be inside the surface size
    if (!iSourcRect.IsEmpty())
        {
        if (iSourcRect.iTl.iX<0 || iSourcRect.iTl.iY<0)
            {
            return EFalse;
            }
        TInt32 width;
        TInt32 height;
        HelpStreamGetHeader(&width,&height);
        if (iSourcRect.iBr.iX>width || iSourcRect.iBr.iY>height)
            {
            return EFalse;
            }
        }
    //If mask is defined and enabled, Mask size must match destination rectangle size
        //masks are currently not implemented
    //If mask is enabled then source alpha must not be enabled
        //masks are currently not implemented
    return ETrue;
    }
/** 
 * Flushes any flagged scene or attribute changes into the element objects
 * @return  EFalse if no flushing occurred - for any reason
 **/
TBool CElementWrapper::FlushSceneChanges()
    {
    TInt updateFlags=iUpdateFlags;
    if  ((updateFlags&EUpdate_AttributesIllegal) && !(updateFlags&EUpdate_SettingValid))
        {
        return EFalse;
        }
    
    if (updateFlags==EUpdate_None)
        {
        return EFalse;
        }

    if (updateFlags&(EUpdate_Remove|EUpdate_Destroy) && (updateFlags&EUpdate_SceneCommited))
        {
        wfcRemoveElement(iOpenWfcWrapper.Device(),iElement);
        }
    
    if (updateFlags&EUpdate_Destroy)
        {
        return EFalse;
        }
    
    if (updateFlags&(EUpdate_Remove|EUpdate_Destroy))
        {
        iUpdateFlags&=~(EUpdate_Insert|EUpdate_Remove|EUpdate_SceneCommited);
        return EFalse;
        }
    
    TBool isElementLegal=IsNewElementLegal();
    if (!isElementLegal)
        {   //If the element would not be legal then it must not be inserted
            //It may even need to be removed.
        if (updateFlags&EUpdate_SceneCommited)
            {   //it is in the scene, remove it
            wfcRemoveElement(iOpenWfcWrapper.Device(),iElement);
            updateFlags &= ~EUpdate_SceneCommited;
            }

        //Re-flag the update but also set a flag
        //so that we don't bother to check legality again until another attribute change.
        updateFlags&=~EUpdate_SettingValid;
        updateFlags|=EUpdate_AttributesIllegal;
        iUpdateFlags=updateFlags;

        LILOG(("### EXIT CElementWrapper::FlushSceneChanges() FALSE"));
        return EFalse;
        }

    WFCElement elementBelow=WFC_INVALID_HANDLE;
    if ((updateFlags&(EUpdate_Insert|EUpdate_AttributesIllegal)))
        {
        if (CElementWrapper* elementWrapperBelow=iElementBelow)
            {
            while (elementWrapperBelow && elementWrapperBelow->iUpdateFlags&EUpdate_AttributesIllegal)
                {
                elementWrapperBelow=elementWrapperBelow->iElementBelow;
                }
            if (elementWrapperBelow)
                {
                elementBelow=elementWrapperBelow->iElement;
                }
            }
        }
    
    iUpdateFlags = EUpdate_SceneCommited;
    
    if ((updateFlags&~EUpdate_Remove))
        {
        FlushAttributeChanges(iElement, updateFlags);
        if (updateFlags&(EUpdate_Insert|EUpdate_AttributesIllegal))
            {
            wfcInsertElement(iOpenWfcWrapper.Device(),iElement,elementBelow);
            }
        }

    LILOG(("### EXIT CElementWrapper::FlushSceneChanges() FALSE"));
    return ETrue;
    }

void CElementWrapper::FlushAttributeChanges(WFCElement aElement, TInt aUpdateFlags)
    {
    LILOG(("### ENTER FlushAttributeChanges() aStart(%d), aPastMax(%d), uFl(0x%x)", aStart, aPastMax, aUpdateFlags));

    //At present all attribute changes are flushed immediately... assert that behaviour.
    if (aUpdateFlags&EUpdate_SOURCE_GEOMETRY_SET)
        {
        SetEltSourceRectangle(aElement);
        }
    if (aUpdateFlags&(EUpdate_SOURCE|EUpdate_SettingValid))
        {
        SetEltSource(aElement);
        }
    if (aUpdateFlags&EUpdate_SOURCE_FLIP)
        {
        SetEltSourceFlip(aElement);
        }
    if (aUpdateFlags&EUpdate_SOURCE_ROTATION)
        {
        TInt wfcRot=WfcRotation(iRotation);
        wfcSetElementAttribi(iOpenWfcWrapper.Device(),iElement,WFC_ELEMENT_SOURCE_ROTATION,wfcRot);
        }
    if (aUpdateFlags&EUpdate_TRANSPARENCY_TYPES)
        {
        SetEltTransparencyTypes(aElement);
        }
    if (aUpdateFlags&EUpdate_GLOBAL_ALPHA)
        {
        wfcSetElementAttribi(iOpenWfcWrapper.Device(),aElement,WFC_ELEMENT_GLOBAL_ALPHA,iGlobalAlpha);
        }
    if (aUpdateFlags&EUpdate_DESTINATION_RECTANGLE)
        {
        SetEltDestinationRectangle(aElement);
        }
    LILOG(("### EXIT FlushAttributeChanges()"));
    }

TInt CElementWrapper::ReplicateElements(WFCContext aContext, WFCElement& aPreviousTargetElement,const TPoint& aOffset)
    {
    if (aContext == WFC_INVALID_HANDLE)
        {
        return KErrArgument;
        }
    
    if (!IsNewElementLegal())
        {
        return KErrNone;
        }

    WFCDevice device = iOpenWfcWrapper.Device();
    WFCContext onScreenContext = iOpenWfcWrapper.OnScreenContext();
    // clean the existing system errors and look for new ones
    wfcGetError(device);
    WFCSource targetImageSource = wfcCreateSourceFromStream(device, aContext, reinterpret_cast<WFCNativeStreamType>(&iSurfaceId), NULL);
    if (targetImageSource == WFC_INVALID_HANDLE)
        {
        return COpenWfcWrapper::TranslateOpenWfcError(wfcGetError(device));
        }
     
    WFCElement targetElement = WFC_INVALID_HANDLE;

    // the elements are created but we don't get hold of them for longer than we need
    // all elements created now are to be destroyed when composition is completed
    targetElement = wfcCreateElement(device, aContext, NULL);
    if (targetElement == WFC_INVALID_HANDLE)
        {
        return COpenWfcWrapper::TranslateOpenWfcError(wfcGetError(device));
        }

    // we can just flush the attributes to the new elements because the off-screen context attributes
    // share the same device with the on-screen context. If this changes make device a parameter!
    wfcSetElementAttribi(device,targetElement,WFC_ELEMENT_SOURCE,targetImageSource);
    FlushAttributeChanges(targetElement, (EUpdate_ALL_ATTRIBS&~(EUpdate_SOURCE|EUpdate_DESTINATION_RECTANGLE)));
    SetEltDestinationRectangle(targetElement, aOffset);
    wfcInsertElement(iOpenWfcWrapper.Device(),targetElement,aPreviousTargetElement);
    aPreviousTargetElement = targetElement;

    return COpenWfcWrapper::TranslateOpenWfcError(wfcGetError(device));
    }

TInt CElementWrapper::SurfaceInfo(const TSurfaceId& aSurface, RSurfaceManager::TInfoBuf& aInfo)
    {
    TInt err = KErrNone;
    switch(aSurface.Type())
        {
        case TSurfaceTypes::ESurfaceManagerSurface:
            {
            err = Owner().SurfaceManager().SurfaceInfo(aSurface, aInfo);                   
            break;
            }
        case TSurfaceId::EScreenSurface:      
            {
            TInt screenId = aSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField];
            TInt width = 0;
            err = HAL::Get(screenId, HALData::EDisplayXPixels, width);
            if (err != KErrNone)
                {
                iOpenWfcWrapper.DecEltRefRegSource(aSurface);
                return err;
                }
            
            TInt height = 0;
            err = HAL::Get(screenId, HALData::EDisplayYPixels, height);
            if (err != KErrNone)
                {
                iOpenWfcWrapper.DecEltRefRegSource(aSurface);
                return err;
                }
            
            aInfo().iSize = TSize(width, height);      
            
#ifdef  SYMBIAN_ROTATION_MODE_CHANGES
            TInt displayMode = aSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField];
#else
            TInt displayMode = 0;
            err = HAL::Get(screenId, HALData::EDisplayMode, displayMode);
            if (err != KErrNone)
                {
                iOpenWfcWrapper.DecEltRefRegSource(aSurface);
                return err;
                }                       
#endif                      

#if defined(SYMBIAN_ROTATION_MODE_CHANGES)
            if (displayMode & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) // 90 | 270 degree rotation
                {
                // Swap dimensions and recalculate stride. Assume no padding for now.
                aInfo().iSize.iWidth = height;
                aInfo().iSize.iHeight = width;
                //"vertical" stride has already been fetched
                }
#elif defined(SYMBIAN_ROTATION_CHANGES)
            if (aSurface.iInternal[1] & (2 | 8))    // 90 | 270 degree rotation
                {
                // Swap dimensions and recalculate stride. Assume no padding for now.
                aInfo().iSize.iWidth = height;
                aInfo().iSize.iHeight = width;
                }
#endif                          
            
            break;
            }
        default:
            err = KErrNotSupported;
        }    
    
    return err;
    }
