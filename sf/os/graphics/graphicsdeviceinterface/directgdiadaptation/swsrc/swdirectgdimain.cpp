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

#include "directgdiadapter.h"
#include "swdirectgdidriverimpl.h"
#include "swdirectgdiimagetargetimpl.h"
#include <graphics/directgdiimagetarget.h>

// Use on every argument if a method is not implemented yet
#define UNUSED(x) {(void)(x); User::Invariant();}

/**
Engine constructor.

@param aDriver The driver implementation.
 */
CSwDirectGdiEngine::CSwDirectGdiEngine(CSwDirectGdiDriverImpl* aDriver):
	iBrushBitmap(),
	iBrushColor(KRgbWhite),
	iBrushOrigin(0,0),
	iBrushStyle(DirectGdi::ENullBrush),
	iDefaultRegion(),
	iDefaultRegionPtr(NULL),
	iDotLength(0),
	iDotMask(0),
	iDotParam(0),
	iDotDirection(1),
	iDrawDevice(NULL),
	iDrawMode(DirectGdi::EDrawModePEN),
	iDriver(aDriver),
	iLinePosition(0,0),
	iOrigin(0,0),
	iPenArray(NULL),
	iPenColor(KRgbBlack),
	iTextShadowColor(KRgbGray),
	iPenSize(1, 1),
	iPenStyle(DirectGdi::ESolidPen)
	{			
	}

CSwDirectGdiEngine::~CSwDirectGdiEngine()
	{
	Deactivate();
	ResetPenArray();
	iClippingRegion.Close();
	iBrushBitmap.Reset();
	}

/**
@see MDirectGdiEngine::Activate()

@return KErrNone if successful, otherwise one of the the system-wide error codes.
@panic DGDIAdapter 32, if the handle of aTarget is not null, but is not associated with a valid target. 
@panic DGDIAdapter 34, if the passed target has a NULL handle (debug only).
@panic DGDIAdapter 53, if the passed target has a NULL draw device (debug only).
*/
TInt CSwDirectGdiEngine::Activate(RDirectGdiImageTarget& aTarget) 
	{
	GRAPHICS_ASSERT_DEBUG(aTarget.Handle(), EDirectGdiPanicActivateWithNullHandle);
	GRAPHICS_ASSERT_ALWAYS(iDriver->ValidImageTarget(aTarget.Handle()), EDirectGdiPanicResourceHandleNotFound);
		
	CSwDirectGdiImageTargetImpl* target = reinterpret_cast<CSwDirectGdiImageTargetImpl*>(aTarget.Handle());
	if(iRenderingTarget == target)
		{
		return KErrNone;
		}

	Deactivate();
	
	iRenderingTarget = target;
	target->Open();	
	iDrawDevice = target->DrawDevice();
	GRAPHICS_ASSERT_DEBUG(iDrawDevice, EDirectGdiPanicActivateWithNullDrawDevice);
	TRect deviceRect;
	iDrawDevice->GetDrawRect(deviceRect);
	iDefaultRegion.Clear();
	iDefaultRegion.AddRect(deviceRect);
	iDefaultRegionPtr = &iDefaultRegion;	
	
	return KErrNone;
	}

/**
 Unbinds the drawing engine.
 */
void CSwDirectGdiEngine::Deactivate()
	{
	if (iRenderingTarget)
		{
		iDriver->Deactivate(iRenderingTarget);
		iDrawDevice = NULL;
		}		
	}

/** 
@see MDirectGdiEngine::ExternalizeL()
*/
void CSwDirectGdiEngine::ExternalizeL(RWriteStream& aWriteStream)
	{	
	aWriteStream.WriteUint32L(iDotLength);
	aWriteStream.WriteUint32L(iDotMask);
	aWriteStream.WriteUint32L(iDotParam);
	aWriteStream.WriteUint32L(iDotDirection); 
	aWriteStream << iLinePosition;
	}

/** 
@see MDirectGdiEngine::InternalizeL()
*/
void CSwDirectGdiEngine::InternalizeL(RReadStream& aReadStream)
	{	
	iDotLength = aReadStream.ReadUint32L();
	iDotMask = aReadStream.ReadUint32L();
	iDotParam = aReadStream.ReadUint32L();
	iDotDirection = aReadStream.ReadUint32L(); 
	aReadStream >> iLinePosition;
	}

/**
@see MDirectGdiEngine::ResetBrushPattern()
*/
void CSwDirectGdiEngine::ResetBrushPattern()
	{
	iBrushBitmap.Reset();
	}

/** 
@see MDirectGdiEngine::MoveTo()
*/
void CSwDirectGdiEngine::MoveTo(const TPoint& aPoint)
	{
	iLinePosition = aPoint;
	}

/**
@see MDirectGdiEngine::MoveBy()
*/
void CSwDirectGdiEngine::MoveBy(const TPoint& aVector)
	{
	iLinePosition += aVector;
	}

/**
@see MDirectGdiEngine::Reset()

@pre Has been called by CDirectGdiContext::Reset()

Most states are set by the context Reset(), only states that can't be set by the context need resetting here.
*/
void CSwDirectGdiEngine::Reset()
	{
	iDotLength = 0;
	iDotMask = 0;
	iDotParam = 0;
	iDotDirection = 1;	
	ResetPenArray();
	}

/**
@see MDirectGdiEngine::SetBrushOrigin()
*/
void CSwDirectGdiEngine::SetBrushOrigin(const TPoint& aOrigin)
	{
	iBrushOrigin = aOrigin;
	}

/** 
@see MDirectGdiEngine::SetBrushColor()
 */
void CSwDirectGdiEngine::SetBrushColor(const TRgb& aColor)
	{
	iBrushColor = aColor;
	}

/**	
@see MDirectGdiEngine::SetBrushStyle()
@panic DGDIAdapter 12, if aStyle is EPatternedBrush but no valid brush bitmap has been set.
*/
void CSwDirectGdiEngine::SetBrushStyle(DirectGdi::TBrushStyle aStyle)
	{
	if (aStyle == DirectGdi::EPatternedBrush)
		{
		GRAPHICS_ASSERT_ALWAYS(iBrushBitmap.Handle() != 0, EDirectGdiPanicPatternedBrushNotSet);
		}
	iBrushStyle = aStyle;
	}

/**
@see MDirectGdiEngine::ResetClippingRegion()
 */
void CSwDirectGdiEngine::ResetClippingRegion()
	{
	iDefaultRegionPtr = &iDefaultRegion;	
	iClippingRegion.Clear();
	}

/**
The error state is set to KErrNoMemory if the required memory could not be allocated.

@see MDirectGdiEngine::SetClippingRegion()
*/
void CSwDirectGdiEngine::SetClippingRegion(const TRegion& aRegion)
	{
	TRect boundingRect=iDefaultRegion.BoundingRect();
	boundingRect.iTl-=iDrawOrigin;
	boundingRect.iBr-=iDrawOrigin;
	if (!aRegion.IsContainedBy(boundingRect))
		{
		iDriver->SetError(KErrArgument);
		return;
		}
	
	iClippingRegion.Copy(aRegion);
	
	if (iClippingRegion.CheckError())		
		{
		iDriver->SetError(KErrNoMemory);
		return;
		}
	iClippingRegion.Offset(iDrawOrigin);
	iDefaultRegionPtr = &iClippingRegion;
	}

/**
@see MDirectGdiEngine::SetDrawMode()
*/
void CSwDirectGdiEngine::SetDrawMode(DirectGdi::TDrawMode aDrawMode)
	{
	iDrawMode = aDrawMode;
	}

/** 
@see MDirectGdiEngine::SetOrigin()
*/
void CSwDirectGdiEngine::SetOrigin(const TPoint& aOrigin)
	{
	iOrigin = aOrigin+iDrawOrigin;
	}

/** 
@see MDrawDeviceOrigin::Set()
*/
TInt CSwDirectGdiEngine::Set(const TPoint& aDrawOrigin)
	{
	TPoint moveOrigin=aDrawOrigin;
	moveOrigin-=iDrawOrigin;
	iOrigin+=moveOrigin;
	iClippingRegion.Offset(moveOrigin);
	iDrawOrigin = aDrawOrigin;
	return KErrNone;
	}

/** 
@see MDrawDeviceOrigin::Get()
*/
void CSwDirectGdiEngine::Get(TPoint& aDrawOrigin)
	{
	aDrawOrigin=iDrawOrigin;
	}

/** 
@see MDirectGdiEngine::SetPenColor()
 */
void CSwDirectGdiEngine::SetPenColor(const TRgb& aColor)
	{
	iPenColor = aColor;
	}

/** 
@see MDirectGdiEngine::SetPenStyle()
*/
void CSwDirectGdiEngine::SetPenStyle(DirectGdi::TPenStyle aStyle)
	{
	iPenStyle = aStyle;	
	switch(iPenStyle)
		{
		case DirectGdi::ENullPen:
			iDotMask=0;
			iDotLength=0;
			break;
		case DirectGdi::EDottedPen:
			iDotMask=1;
			iDotLength=4;
			break;
		case DirectGdi::EDashedPen:
			iDotMask=7;	
			iDotLength=6;
			break;
		case DirectGdi::EDotDashPen:
			iDotMask=113;
			iDotLength=10;
			break;
		case DirectGdi::EDotDotDashPen:
			iDotMask=1809;
			iDotLength=14;
			break;
		case DirectGdi::ESolidPen:
		default:
			iDotMask=1;
			iDotLength=1;
			break;
		};
	iDotParam=0;
	}

/**
The error state is set to KErrNoMemory if the required memory could not be allocated.

@see MDirectGdiEngine::SetPenSize()

@pre aSize is not the same as iPenSize.
*/
void CSwDirectGdiEngine::SetPenSize(const TSize& aSize)
	{
	iPenSize = aSize;

	if ((iPenSize.iWidth >= 1) || (iPenSize.iHeight >= 1))
		{
		TInt err = PenAllocate();
		iDriver->SetError(err);
		}
	else
		{
		ResetPenArray();
		}

	iDotParam = 0;
	}

/** 
@see MDirectGdiEngine::SetBrushPattern()

@pre aPattern is a valid bitmap.
*/
TInt CSwDirectGdiEngine::SetBrushPattern(const CFbsBitmap& aPattern)
	{			
	if (aPattern.ExtendedBitmapType() != KNullUid)
		{
		return KErrNotSupported; // Not supported for extended bitmaps		
		}
	
	return iBrushBitmap.Duplicate(aPattern.Handle());
	}



