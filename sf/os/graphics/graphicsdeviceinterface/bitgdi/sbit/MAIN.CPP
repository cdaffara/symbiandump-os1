// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bitstd.h>
#include <bitdev.h>
#include <bitdraw.h>
#include <bitdrawscaling.h>
#include <bitdrawinterfaceid.h>
#include "BITPANIC.H"
#include <s32mem.h>
#include <shapeinfo.h>
#include "bitgcextradata.h"

#define KDefaultShadowColor KRgbGray

//CFbsBitGc streams - version numbers.
//Add new version numbers here. A reason of adding new version numbers may be adding new 
//CFbsBitGc data members, which may have to be externalized/internalized. When that happens:
//1.Put a new enum item (like EFbsBitGc_Ver01) with a version number, which is greater than
//  the last version number, which was used. 
//2.Comment the new enum item.
//3.Update KFbsBitGc_VerNo value to be the last enum item value.
//4.Update InternalizeL/ExternalizeL methods after adding the new version number.
//  For example: If a new member is added to the class - TInt iSmth, when InternalizeL
//  is called to operate on older archive, iSmth member won't be in the archive!
//  So, in InternalizeL, there should be a check, something like:
//	TUint16 archiveVerNo = 0;
//	aReadStream >> archiveVerNo;
//  .......................
//  if(archiveVerNo < EFbsBitGc_Ver03)   //EFbsBitGc_Ver03 has been added, when iSmth has been added
//		{
//      //Do nothing - iSmth is not in the archive
//      //Initialize it with some default value
//      iSmth = KDefVal;
//		}
//	else
//		{
//		aReadStream >> iSmth;
//		}
enum
	{
	EFbsBitGc_Ver01 = 1, //Base version number, when InternalizeL/ExternalizeL were added
	EFbsBitGc_Ver02 = 2, //Supports InternalizeL/ExternalizeL for ShadowColor. 
	};
LOCAL_D const TUint16 KFbsBitGc_VerNo = EFbsBitGc_Ver02;

// Global panic functions

void Panic(TBitgdiPanic aPanicCode)
	{
	_LIT(KBitgdiPanicCategory,"BITGDI");
	User::Panic(KBitgdiPanicCategory,aPanicCode);
	}

_LIT(KBITGDIPanicDesc1,"BitGdi internal Panic %S, in file %S @ line %i");
_LIT(KBITGDIPanicDesc2,"Assert condition = \"%S\"");
_LIT(KBITGDIPanicDesc3, "BitGdi internal Invariant, in file %S @ line %i");

void PanicWithInfo(TBitgdiPanic aPanic, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KBITGDIPanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);
	Panic(aPanic);
	}

void PanicWithCondAndInfo(TBitgdiPanic aPanic, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KBITGDIPanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);

	buf.Format(KBITGDIPanicDesc2, &aCondition);
	RDebug::Print(buf);
	Panic(aPanic);
	}
	
void InvariantWithCondAndInfo(const TDesC& aCondition, const TDesC& aFileName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KBITGDIPanicDesc3, &aFileName, aLine);
	RDebug::Print(buf);
	
	buf.Format(KBITGDIPanicDesc2, &aCondition);
	RDebug::Print(buf);
	User::Invariant();
	}


//
// CFbsBitGc
//


/** Factory function for creating a CFbsBitGc object
The object is then ready for a call to Activate().
@return A pointer to the newly created object. */
EXPORT_C CFbsBitGc* CFbsBitGc::NewL()
	{
	CFbsBitGc* bitGc = new(ELeave) CFbsBitGc;
	CleanupStack::PushL(bitGc);
	bitGc->iFbsBitGcExtraData = new(ELeave) CFbsBitGcExtraData;
	CleanupStack::Pop(bitGc);
	return bitGc;
	}

CFbsBitGc::CFbsBitGc():
	CBitmapContext(),
	iBrushBitmap(),
	iBrushUsed(EFalse),
	iBrushColor(KRgbWhite),
	iBrushOrigin(TPoint(0,0)),
	iBrushStyle(ENullBrush),
	iClipRect(TRect(0,0,0,0)),
	iDefaultRegion(),
	iDefaultRegionPtr(NULL),
	iUserClipRect(TRect(0,0,0,0)),
	iDevice(NULL),
	iDitherOrigin(TPoint(0,0)),
	iDotLength(0),
	iDotMask(0),
	iDotParam(0),
	iDotDirection(1),
	iDrawMode(EDrawModePEN),
	iFont(),
	iCharJustExcess(0),
	iCharJustNum(0),
	iWordJustExcess(0),
	iWordJustNum(0),
	iLastPrintPosition(TPoint(0,0)),
	iLinePosition(TPoint(0,0)),
	iOrigin(TPoint(0,0)),
	iPenColor(KRgbBlack),
	iPenStyle(ESolidPen),
	iPenSize(TSize(1,1)),
	iShadowMode(CFbsDrawDevice::ENoShadow),
	iAutoUpdateJustification(ETrue),
	iFadeBlackMap(128),
	iFadeWhiteMap(255),
	iStrikethrough(EStrikethroughOff),
	iUnderline(EUnderlineOff),
	iUserDisplayMode(ENone)
	{}


/** Frees all resources owned by the object. */
EXPORT_C CFbsBitGc::~CFbsBitGc()
	{
	Reset();
	iBrushBitmap.Reset();
	iClippingRegion.Close();
	delete iFbsBitGcExtraData;
	}

/** Sets the object to draw to a particular device
@param aDevice The target device. */
EXPORT_C void CFbsBitGc::Activate(CFbsDevice* aDevice)
	{
	if (!aDevice)
		return;
	iDevice = aDevice;
	TRect deviceRect;
	iDevice->iDrawDevice->GetDrawRect(deviceRect);
	iDefaultRegion.Clear();
	iDefaultRegion.AddRect(deviceRect);
	iDefaultRegionPtr = &iDefaultRegion;
	iUserClipRect = deviceRect;
	iDrawnTo.SetWidth(0);
	iDrawnTo.SetHeight(0);
	iAutoUpdateJustification = ETrue;
	}

/** Sets the object to draw to a particular device but doesn't 'use up' 
justification settings when drawing text.
This is similar to Activate().
@param aDevice The target device. */
EXPORT_C void CFbsBitGc::ActivateNoJustAutoUpdate(CFbsDevice* aDevice)
	{
	Activate(aDevice);
	iAutoUpdateJustification = EFalse;
	}

void CFbsBitGc::SetupDevice() const
	{
	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;
	drawDevice->SetDitherOrigin(iDitherOrigin);
	drawDevice->SetShadowMode(CFbsDrawDevice::TShadowMode(iShadowMode));
	drawDevice->SetFadingParameters(iFadeBlackMap,iFadeWhiteMap);
	drawDevice->SetUserDisplayMode(iUserDisplayMode);
	}

void CFbsBitGc::CheckDevice() const
	{
	BG_ASSERT_ALWAYS(iDevice,EBitgdiPanicNoDevicePresent);
	}

TBool CFbsBitGc::CheckDevice(const TRect& aRect) const
	{
	BG_ASSERT_ALWAYS(iDevice,EBitgdiPanicNoDevicePresent);

	return aRect.IsEmpty();
	}

/** Externalizes an object of this class to a write stream.
It is important that the brush bitmap of the GC is maintained between calls
to externalize and internalize.  The brush bitmap handle is externalized,
not the bitmap data.  This is done for performance.

@param aWriteStream Stream to which the object should be externalized.
@internalAll */
EXPORT_C void CFbsBitGc::ExternalizeL(RWriteStream& aWriteStream)
	{
	aWriteStream << KFbsBitGc_VerNo;
	TInt bitmapHandle=iBrushBitmap.Handle();
	aWriteStream.WriteInt32L(bitmapHandle);
	iBrushColor.ExternalizeL(aWriteStream);
	aWriteStream.WriteInt8L(iBrushUsed);
	aWriteStream << iBrushOrigin;
	aWriteStream.WriteUint8L(iBrushStyle);
	aWriteStream << iDitherOrigin;
	aWriteStream << iUserClipRect;
	aWriteStream << iDrawnTo;
	aWriteStream.WriteUint32L(iDotLength);
	aWriteStream.WriteUint32L(iDotMask);
	aWriteStream.WriteUint32L(iDotParam);
	aWriteStream.WriteUint32L(iDotDirection); 
	aWriteStream.WriteUint8L(iDrawMode); 
	aWriteStream.WriteUint32L(iCharJustExcess);
	aWriteStream.WriteUint32L(iCharJustNum);
	aWriteStream.WriteUint32L(iWordJustExcess);
	aWriteStream.WriteUint32L(iWordJustNum);
	aWriteStream << iLastPrintPosition;
	aWriteStream << iLinePosition;
	aWriteStream << iOrigin; 
	iPenColor.ExternalizeL(aWriteStream); 
	aWriteStream.WriteUint8L(iPenStyle);   
	aWriteStream << iPenSize;
	aWriteStream << iShadowMode;
	aWriteStream << iAutoUpdateJustification;
	aWriteStream << iFadeBlackMap;
	aWriteStream << iFadeWhiteMap;
	aWriteStream.WriteUint8L(iStrikethrough); 
	aWriteStream.WriteUint8L(iUnderline);

	const TFontSpec fontSpec=iFont.FontSpecInTwips();
	fontSpec.ExternalizeL(aWriteStream);

	aWriteStream.WriteUint8L(iUserDisplayMode);
	iFbsBitGcExtraData->ShadowColor().ExternalizeL(aWriteStream);
	}

/** Internalizes an object of this class from a read stream.
It is important that the brush bitmap of the GC is maintained between calls
to externalize and internalize.  The brush bitmap handle is internalized,
not the bitmap data.

@param aReadStream Stream from which the object is to be internalized.
@internalAll */
EXPORT_C void CFbsBitGc::InternalizeL(RReadStream& aReadStream)
	{
	TUint16 archiveVerNo = 0;
	aReadStream >> archiveVerNo;
	TInt bitmapHandle=aReadStream.ReadInt32L();
	if (bitmapHandle)
		{
		UseBrushPattern(bitmapHandle);
		}
	iBrushColor.InternalizeL(aReadStream);
	iBrushUsed=aReadStream.ReadInt8L();
	aReadStream >> iBrushOrigin;
	iBrushStyle=(TBrushStyle)aReadStream.ReadUint8L();
	aReadStream >> iDitherOrigin;
	aReadStream >> iUserClipRect;
	aReadStream >> iDrawnTo;
	iDotLength=aReadStream.ReadUint32L();
	iDotMask=aReadStream.ReadUint32L();
	iDotParam=aReadStream.ReadUint32L();
	iDotDirection=aReadStream.ReadUint32L(); 
	iDrawMode=(TDrawMode)aReadStream.ReadUint8L(); 
	iCharJustExcess=aReadStream.ReadUint32L();
	iCharJustNum=aReadStream.ReadUint32L();
	iWordJustExcess=aReadStream.ReadUint32L();
	iWordJustNum=aReadStream.ReadUint32L();
	aReadStream >> iLastPrintPosition;
	aReadStream >> iLinePosition;
	aReadStream >> iOrigin; 
	iPenColor.InternalizeL(aReadStream); 
	iPenStyle=(TPenStyle)aReadStream.ReadUint8L();   
	aReadStream >> iPenSize;
	SetPenSize(iPenSize);
	aReadStream >> iShadowMode;
	aReadStream >> iAutoUpdateJustification;
	aReadStream >> iFadeBlackMap;
	aReadStream >> iFadeWhiteMap;
	iStrikethrough=(TFontStrikethrough)aReadStream.ReadUint8L(); 
	iUnderline=(TFontUnderline)aReadStream.ReadUint8L();
	TFontSpec fontSpec;
	fontSpec.InternalizeL(aReadStream);
	if (fontSpec.iHeight != 0)
		{
		CFbsFont* font = NULL;
		User::LeaveIfError(iDevice->GetNearestFontToDesignHeightInTwips(font,fontSpec));
		TInt err = iFont.Duplicate(font->Handle());
		iDevice->ReleaseFont(font);
		User::LeaveIfError(err);
		UseFont(iFont.Handle());
		}
	iUserDisplayMode=(TDisplayMode)aReadStream.ReadUint8L();
	if(archiveVerNo < EFbsBitGc_Ver02)  //Without shadowcolor
		{
		//Initialize it with the default value
		iFbsBitGcExtraData->SetShadowColor(KDefaultShadowColor);
		}
	else  //With shadowcolor
		{
		TRgb shadowColor;
		shadowColor.InternalizeL(aReadStream);
		iFbsBitGcExtraData->SetShadowColor(shadowColor);	
		}
	}

/** Copies all settings from the specified bitmap graphics context.
@param aGc The bitmap graphics context whose settings are to be copied. */
EXPORT_C void CFbsBitGc::CopySettings(const CFbsBitGc& aGc)
	{
	iBrushColor = aGc.iBrushColor;
	iBrushUsed = aGc.iBrushUsed;
	iBrushOrigin = aGc.iBrushOrigin;
	iBrushStyle = aGc.iBrushStyle;
	iDefaultRegionPtr = aGc.iDefaultRegionPtr;
	iDitherOrigin = aGc.iDitherOrigin;
	iDotLength = aGc.iDotLength;
	iDotMask = aGc.iDotMask;
	iDotParam = aGc.iDotParam;
	iDotDirection = aGc.iDotDirection;
	iDrawMode = aGc.iDrawMode;
	iCharJustExcess = aGc.iCharJustExcess;
	iCharJustNum = aGc.iCharJustNum;
	iWordJustExcess = aGc.iWordJustExcess;
	iWordJustNum = aGc.iWordJustNum;
	iLastPrintPosition = aGc.iLastPrintPosition;
	iLinePosition = aGc.iLinePosition;
	iOrigin = aGc.iOrigin;
	iPenColor = aGc.iPenColor;
	iPenStyle = aGc.iPenStyle;
	SetPenSize(aGc.iPenSize);
	iShadowMode = aGc.iShadowMode;
	iStrikethrough = aGc.iStrikethrough;
	iUnderline = aGc.iUnderline;
	iUserDisplayMode = aGc.iUserDisplayMode;
	if(aGc.iFont.Handle())
		UseFont(aGc.iFont.Handle());
	if(aGc.iBrushBitmap.Handle())
		UseBrushPattern(aGc.iBrushBitmap.Handle());
	iFbsBitGcExtraData->SetShadowColor(aGc.iFbsBitGcExtraData->ShadowColor());
	}

/** Gets a pointer to the graphics device for the graphics context.
The graphics device is the device currently being drawn to. 
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::Device(). The function behaviour is the same as documented 
in that class. 
@see CGraphicsContext::Device() */
EXPORT_C CGraphicsDevice* CFbsBitGc::Device() const
	{
	return iDevice;
	}

/** Discards a non-built-in brush pattern.
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::DiscardBrushPattern(). The function behaviour is the same 
as documented in that class. 
@see CGraphicsContext::DiscardBrushPattern() */
EXPORT_C void CFbsBitGc::DiscardBrushPattern()
	{
	iBrushBitmap.Reset();
	iBrushUsed = EFalse;
	if (iBrushStyle == EPatternedBrush)
		iBrushStyle = ENullBrush;
	}

/** Discards a selected device font.
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::DiscardFont(). The function behaviour is the same as documented 
in that class. 
@see CGraphicsContext::DiscardFont() */
EXPORT_C void CFbsBitGc::DiscardFont()
	{
	iFont.Reset();
	}

/** Maps pixels in the specified rectangle.
The function tries to match the colour of a pixel with one of the RGB values 
in an array of RGB pairs. If there is a match, the colour is changed to the 
value specified in the other RGB in the RGB pair.
@param aRect The rectangle in which pixels are to be mapped.
@param aColors A pointer to a set of RGB pairs. 
@param aNumPairs The number of pairs 
@param aMapForwards ETrue, mapping is done from the first RGB to the second 
RGB in the pair; EFalse, mapping is done from the second RGB to the first 
RGB in the pair. */
EXPORT_C void CFbsBitGc::MapColors(const TRect& aRect,
								   const TRgb* aColors,
								   TInt aNumPairs,
								   TBool aMapForwards)
	{
	if (CheckDevice(aRect) || !aColors)
		return;

	TRect rcpy(aRect);
	rcpy.Move(iOrigin);
	AddRect(rcpy);
	if (UserClipRect(rcpy))
		return;

	SetupDevice();
	iDevice->DrawingBegin();

	CFbsDrawDevice* drawDevice = iDevice->iDrawDevice;

#if defined(_DEBUG)
	TRect deviceRect;
	drawDevice->GetDrawRect(deviceRect);
#endif

	const TInt limit = iDefaultRegionPtr->Count();
	for (TInt count = 0; count < limit; count++)
		{
		iClipRect = (*iDefaultRegionPtr)[count];
		if (!iClipRect.Intersects(rcpy))
			continue;

		iClipRect.Intersection(rcpy);

		BG_ASSERT_DEBUG(iClipRect.iTl.iX >= deviceRect.iTl.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iTl.iY >= deviceRect.iTl.iY, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iX <= deviceRect.iBr.iX, EBitgdiPanicOutOfBounds);
		BG_ASSERT_DEBUG(iClipRect.iBr.iY <= deviceRect.iBr.iY, EBitgdiPanicOutOfBounds);

		drawDevice->MapColors(iClipRect,aColors,aNumPairs,aMapForwards);
		drawDevice->UpdateRegion(iClipRect);
		}

	iDevice->DrawingEnd();
	}

/** Sets the internal drawing position relative to the co-ordinate origin.
A subsequent call to DrawLineTo() or DrawLineBy() uses the new drawing point
as the start point for the line drawn.The function provides a concrete
implementation of the pure virtual function CGraphicsContext::MoveTo(). 
The function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::MoveTo(const TPoint& aPoint)
    {
	iLinePosition = aPoint;
	}

/** Sets the drawing point relative to the current co-ordinates.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::MoveBy(). The function behaviour 
is the same as documented in that class. */
EXPORT_C void CFbsBitGc::MoveBy(const TPoint& aVector)
   {
	iLinePosition += aVector;
	}

/** Resets the graphics context to its default settings.
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::Reset(). The function behaviour is the same as documented 
in that class. 
@see CGraphicsContext::Reset() */
EXPORT_C void CFbsBitGc::Reset()
	{
	iDefaultRegionPtr = &iDefaultRegion;
	if (iDefaultRegion.Count() == 0)
		iUserClipRect = TRect(0,0,0,0);
	else
		iUserClipRect = iDefaultRegion[0];
	iLastPrintPosition.SetXY(0,0);
	if(iFbsBitGcExtraData)
		{
		iFbsBitGcExtraData->Reset();	
		}
	iPenColor = KRgbBlack;
	iPenStyle = ESolidPen;
	iPenSize.SetSize(1,1);
	iDrawMode = EDrawModePEN;
	iFont.Reset();
	iLinePosition.SetXY(0,0);
	iOrigin.SetXY(0,0);
	iBrushUsed = EFalse;
	iBrushColor = KRgbWhite;
	iBrushOrigin.SetXY(0,0);
	iBrushStyle = ENullBrush;
	iCharJustExcess = 0;
	iCharJustNum = 0;
	iWordJustExcess = 0;
	iWordJustNum = 0;
	iDitherOrigin.SetXY(0,0);
	iDotLength = 0;
	iDotMask = 0;
	iDotParam = 0;
	iDotDirection = 1;
	iShadowMode = CFbsDrawDevice::ENoShadow;
	iStrikethrough = EStrikethroughOff;
	iUnderline = EUnderlineOff;
	iUserDisplayMode = ENone;
	}

/** Needs to be called if the device is resized.
This only applies to devices of type CFbsBitmapDevice. */
EXPORT_C void CFbsBitGc::Resized()
	{
	CheckDevice();

	TRect deviceRect;
	iDevice->iDrawDevice->GetDrawRect(deviceRect);
	iUserClipRect = deviceRect;
	iDefaultRegion.Clear();
	iDefaultRegion.AddRect(deviceRect);
	iDefaultRegionPtr = &iDefaultRegion;

	if (deviceRect.Contains(iLastPrintPosition))
		iLastPrintPosition.SetXY(0,0);
	if (deviceRect.Contains(iLinePosition))
		iLinePosition.SetXY(0,0);
	if (deviceRect.Contains(iOrigin))
		iOrigin.SetXY(0,0);
	if (deviceRect.Contains(iBrushOrigin))
		iBrushOrigin.SetXY(0,0);
	}

/**Returns current setting of brush color.
@internalAll*/ 
EXPORT_C TRgb CFbsBitGc::BrushColor()
	{
	return iBrushColor; 
	}

/** Sets the brush pattern origin.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetBrushOrigin(). The function
behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetBrushOrigin(const TPoint& origin)
    {
	iBrushOrigin = origin;
	}

/**Sets the brush colour.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetBrushColor(). The function
behaviour is the same as documented in that class. */ 
EXPORT_C void CFbsBitGc::SetBrushColor(const TRgb& aColor)
    {
	iBrushColor = aColor;
	}

/** Sets the brush style.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetBrushStyle(). The function
behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetBrushStyle(TBrushStyle aStyle)
{
	iBrushStyle = aStyle;
	}


/** Sets the dither origin.
This is only useful for modes that do dithering. If the display is scrolled 
an odd number of pixels then the (2x2) dither pattern will not match up for 
new drawing unless this is called.
@param aPoint The dither origin. */
EXPORT_C void CFbsBitGc::SetDitherOrigin(const TPoint& aPoint)
	{
	iDitherOrigin = iOrigin + aPoint;
	}

/** Sets the character justification.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetCharJustification(). The
function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
   {
	if (aExcessWidth == 0 || aNumChars <= 0)
		{
		iCharJustExcess = 0;
		iCharJustNum = 0;
		}
	else
		{
		iCharJustExcess = aExcessWidth;
		iCharJustNum = aNumChars;
		}
	}

/** Sets the clipping rectangle. 
This function provides a concrete implementation of the pure virtual function
CGraphicsContext::SetClippingRect(). The function behaviour is the same as 
documented in that class.  
@see CGraphicsContext::SetClippingRect() */
EXPORT_C void CFbsBitGc::SetClippingRect(const TRect& aRect)
    {
	CheckDevice();

	TRect deviceRect;
	iDevice->iDrawDevice->GetDrawRect(deviceRect);
	iUserClipRect = deviceRect;
	iUserClipRect.Intersection(TRect(aRect.iTl + iOrigin,aRect.iBr + iOrigin));
	}

/** Cancels clipping rectangle and region. */
EXPORT_C void CFbsBitGc::CancelClipping()
	{
	CancelClippingRect();
	CancelClippingRegion();
	}

/** Cancels any clipping rectangle.
Clipping reverts to the full device area, the default.
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::CancelClippingRect(). The function behaviour is the same 
as documented in that class. 
@see CGraphicsContext::CancelClippingRect() */
EXPORT_C void CFbsBitGc::CancelClippingRect()
	{
	if (iDefaultRegion.Count() == 0)
		iUserClipRect = TRect(0,0,0,0);
	else
		iUserClipRect = iDefaultRegion[0];
	}

/** Cancels the clipping region. */
EXPORT_C void CFbsBitGc::CancelClippingRegion()
	{
	iDefaultRegionPtr = &iDefaultRegion;
	}

/** Sets the word justification.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetWordJustification(). The
function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
   {
	if (aExcessWidth <= 0 || aNumGaps <= 0)
		{
		iWordJustExcess = 0;
		iWordJustNum = 0;
		}
	else
		{
		iWordJustExcess = aExcessWidth;
		iWordJustNum = aNumGaps;
		}
	}

/** Sets a clipping region by storing a pointer to the TRegion parameter.
This function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetClippingRegion(). The function behaviour is the 
same as documented in that class.
@panic BITGDI 10 if aRegion is invalid or if aRegion is not contained by the 
device area. */
EXPORT_C void CFbsBitGc::SetClippingRegion(const TRegion* aRegion)
	{
	if (!aRegion)
		{
		iDefaultRegionPtr = &iDefaultRegion;
		return;
		}

	BG_ASSERT_ALWAYS(!aRegion->CheckError(),EBitgdiPanicInvalidRegion);
	BG_ASSERT_ALWAYS(aRegion->IsContainedBy(iDefaultRegion.BoundingRect()),EBitgdiPanicInvalidRegion);
	
	iDefaultRegionPtr = aRegion;
	}

/** Sets a clipping region by storing a copy of the TRegion parameter.
@param aRegion The clipping region to be stored and used. aRegion must be valid and bounded withing the iDefaultRegion.
@return KErrNone if successful; KErrArgument if aRegion is invalid, KErrMemory if the region could not be allocated.
@see CGraphicsContext::SetClippingRegion().
*/
EXPORT_C TInt CFbsBitGc::SetClippingRegion(const TRegion& aRegion)
	{
		if (aRegion.CheckError() || !aRegion.IsContainedBy(iDefaultRegion.BoundingRect()))
			return KErrArgument;
					
		iClippingRegion.Copy(aRegion);
		if (iClippingRegion.CheckError())		
			return KErrNoMemory;
		
		iDefaultRegionPtr = &iClippingRegion;
	
		return KErrNone;
	}

/** Sets the drawing mode.
This affects the colour that is actually drawn, because it defines the way 
that the current screen colour logically combines with the current pen colour 
and brush colour.
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::SetDrawMode(). The function behaviour is the same as documented 
in that class. 
@see CGraphicsContext::SetDrawMode() */
EXPORT_C void CFbsBitGc::SetDrawMode(CGraphicsContext::TDrawMode aDrawMode)
	{
	iDrawMode = aDrawMode;
	}

/**Returns current setting of pen color.
@internalAll*/ 
EXPORT_C TRgb CFbsBitGc::PenColor()
	{
	return iPenColor; 
	}

/** Sets the position of the co-ordinate origin.
All subsequent drawing operations are then done relative to this
origin.The function provides a concrete implementation of the pure
virtual function CGraphicsContext::SetOrigin(). The
function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetOrigin(const TPoint& aPoint)
{
	iDitherOrigin -= iOrigin;
	iOrigin = aPoint;
	iDitherOrigin += iOrigin;
	}

/** Sets the pen colour.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetPenColor(). The function
behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetPenColor(const TRgb& aColor)
    {
	iPenColor = aColor;
	}


/** Sets the line drawing style for the pen. 
The function provides a concrete implementation of the pure virtual function 
CGraphicsContext::SetPenStyle(). The function behaviour is the same as documented 
in that class. 
@see CGraphicsContext::SetPenStyle() */
EXPORT_C void CFbsBitGc::SetPenStyle(TPenStyle aStyle)
	{
	iPenStyle = aStyle;

	switch(iPenStyle)
		{
	case ENullPen:
		iDotMask=0;
		iDotLength=0;
		break;
	case EDottedPen:
		iDotMask=1;
		iDotLength=4;
		break;
	case EDashedPen:
		iDotMask=7;
		iDotLength=6;
		break;
	case EDotDashPen:
		iDotMask=113;
		iDotLength=10;
		break;
	case EDotDotDashPen:
		iDotMask=1809;
		iDotLength=14;
		break;
	default:
		iDotMask=1;
		iDotLength=1;
		break;
		};

	iDotParam=0;
	}

/** Sets the line drawing size for the pen.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetPenSize(). The function
behaviour is the same as documented in that class. */ 
EXPORT_C void CFbsBitGc::SetPenSize(const TSize& aSize)
    {
	iPenSize.SetSize(Abs(aSize.iWidth),Abs(aSize.iHeight));

	const TSize maxsize = iDevice->iDrawDevice->SizeInPixels();

	iPenSize.iWidth = Min(iPenSize.iWidth,maxsize.iWidth << 1);
	iPenSize.iHeight = Min(iPenSize.iHeight,maxsize.iHeight << 1);

	if (iPenSize.iWidth || iPenSize.iHeight)
		PenAllocate();
	else
		{
		iFbsBitGcExtraData->ResetPenArray();
		}

	iDotParam = 0;
	}

/** Simulates another graphics mode.
Some devices running in some modes can simulate other modes (EGray16 will 
do EGray4 and EGray2, EGray4 will do EGray2).
@param aDisplayMode The display mode to be set. */
EXPORT_C void CFbsBitGc::SetUserDisplayMode(TDisplayMode aDisplayMode)
	{
	if(aDisplayMode==iDevice->DisplayMode())
		aDisplayMode = ENone;
	iUserDisplayMode = aDisplayMode;
	}

/** Sets the underline style for all subsequently drawn text.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetUnderlineStyle(). The
function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
    {
	iUnderline = aUnderlineStyle;
	}

/** Sets the strikethrough style for all subsequently drawn text.
The function provides a concrete implementation of the pure virtual
function CGraphicsContext::SetStrikethroughStyle(). The
function behaviour is the same as documented in that class. */
EXPORT_C void CFbsBitGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
    {
	iStrikethrough = aStrikethroughStyle;
	}

/** Sets the shadow mode on or off.
@param aShadowMode ETrue, shadow mode is on; EFalse, shadow mode is off. EFalse 
is the default. */
EXPORT_C void CFbsBitGc::SetShadowMode(TBool aShadowMode)
	{
	if (aShadowMode)
		iShadowMode |= CFbsDrawDevice::EShadow;
	else
		iShadowMode &= ~CFbsDrawDevice::EShadow;
	}

/** Sets whether the graphics context is faded.
The function provides a concrete implementation of the pure virtual
function CBitmapContext::SetFaded(). The function
behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::SetFaded(TBool aFadeMode)
    {
	if (aFadeMode)
		iShadowMode |= CFbsDrawDevice::EFade;
	else
		iShadowMode &= ~CFbsDrawDevice::EFade;
	}

/** Set fading parameters.
The function provides a concrete implementation of the pure virtual
function CBitmapContext::SetFadingParameters(). The
function behaviour is the same as documented in that class.
*/
EXPORT_C void CFbsBitGc::SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)
    {
	iFadeBlackMap = aBlackMap;
	iFadeWhiteMap = aWhiteMap;
	}

/** Gets the orientations supported.
@param aOrientation[4] An array of four TBool values. Each entry in this 
array corresponds to successive values of the TGraphicsOrientation enum.
If the first array entry has an EFrue value, then the normal orientation
is supported; if the second entry array has an ETrue value, 
then a 90 degree orientation is supported etc. */	
EXPORT_C void CFbsBitGc::OrientationsAvailable(TBool aOrientation[4])
    {
	if (iDevice)
		{
		iDevice->iDrawDevice->OrientationsAvailable(aOrientation);
		}
	else
		{
		aOrientation[EGraphicsOrientationNormal] = EFalse;
		aOrientation[EGraphicsOrientationRotated90] = EFalse;
		aOrientation[EGraphicsOrientationRotated180] = EFalse;
		aOrientation[EGraphicsOrientationRotated270] = EFalse;
		}
	}

/** Sets the orientation.
@param aOrientation The required orientation
@return ETrue, if the requested orientation is supported; EFalse, otherwise. */
EXPORT_C TBool CFbsBitGc::SetOrientation(TGraphicsOrientation aOrientation)
	{
	if (!iDevice)
		return EFalse;

	TBool ret = iDevice->SetOrientation(aOrientation);
	if(ret)
		{
		TRect deviceRect;
		iDevice->iDrawDevice->GetDrawRect(deviceRect);
		iDefaultRegion.Clear();
		iDefaultRegion.AddRect(deviceRect);
		iUserClipRect = deviceRect;
		iDrawnTo.SetRect(0,0,0,0);
		}

	return ret;
	}

/** Sets the specified bitmap to be used as the brush pattern.
@param aBitmap The bitmap.*/
EXPORT_C void CFbsBitGc::UseBrushPattern(const CFbsBitmap* aBitmap)
	{
	BG_ASSERT_ALWAYS(aBitmap != NULL,EBitgdiPanicInvalidBitmap);
	
	TInt brushResult = UseBrushPattern(((CFbsBitmap*)aBitmap)->Handle());
	BG_ASSERT_ALWAYS(brushResult == KErrNone,EBitgdiPanicInvalidBitmap);
	}

/** Sets the specified bitmap to be used as the brush pattern.
@param aFbsBitmapHandle The handle number of the bitmap.
@return KErrNone, if successful; otherwise another of the other system wide 
error codes. */
EXPORT_C TInt CFbsBitGc::UseBrushPattern(TInt aFbsBitmapHandle)
	{
	BG_ASSERT_ALWAYS(aFbsBitmapHandle,EBitgdiPanicInvalidBitmap);

	iBrushUsed = ETrue;
	if (iBrushBitmap.Handle() == aFbsBitmapHandle)
		return KErrNone;

	TInt ret = iBrushBitmap.Duplicate(aFbsBitmapHandle);
	if (ret != KErrNone)
		iBrushUsed = EFalse;

	return ret;
	}

/** Selects the device font, identified by handle number, to be used for text drawing.
Notes:
When the font is no longer required, use DiscardFont() to free up the memory 
used. If UseFont() is used again without using DiscardFont() then the previous 
font is discarded automatically.
If no font has been selected, and an attempt is made to draw text with DrawText(), 
then a panic occurs.
@param aFontHandle A handle number for a device font. 
@return The duplicate handle number for the device font.
@see CFont */
EXPORT_C void CFbsBitGc::UseFont(const CFont* aFont)
	{
	BG_ASSERT_ALWAYS(aFont != NULL,EBitgdiPanicInvalidFont);
	BG_ASSERT_ALWAYS(aFont->TypeUid() == KCFbsFontUid,EBitgdiPanicInvalidFont);

	TInt fontResult = UseFont(((CFbsBitGcFont*)aFont)->Handle());
	BG_ASSERT_ALWAYS(fontResult == KErrNone,EBitgdiPanicInvalidFont);
	}

/** Selects the device font, identified by handle, to be used for
text drawing.
Notes:When the font is no longer required, use DiscardFont() to free up the memory used.
If UseFont() is used again without using DiscardFont() then the previous font is 
discarded automatically.If no font has been selected, and an attempt is made to
draw text with DrawText(), then a panic occurs.
@param aFbsFontHandle A handle for a device font.
@return  The result of CFbsFont::Duplicate(). */
EXPORT_C TInt CFbsBitGc::UseFont(TInt aFbsFontHandle)
   {
	BG_ASSERT_ALWAYS(aFbsFontHandle,EBitgdiPanicInvalidFont);

	if (iFont.Handle() == aFbsFontHandle)
		return KErrNone;

	return iFont.Duplicate(aFbsFontHandle);
	}

/** Selects a device font for text drawing but does not take a copy. 
The original must not be destroyed until UseFont(), UseFontNoDuplicate(), 
DiscardFont() or the destructor is called.
@param aFont A pointer to the font to be used. */
EXPORT_C void CFbsBitGc::UseFontNoDuplicate(const CFbsBitGcFont* aFont)
	{
	DiscardFont();

	BG_ASSERT_ALWAYS(aFont->TypeUid() == KCFbsFontUid,EBitgdiPanicInvalidFont);

	if (aFont->Handle() == NULL)
		return;

	iFont = *aFont;
	}

/** Tests whether a brush pattern is being used.
@return ETrue, if a brush pattern is being used; EFalse, otherwise. */
EXPORT_C TBool CFbsBitGc::IsBrushPatternUsed() const
	{
	return iBrushUsed;
	}

/** Tests whether a font is used.
@return ETrue, if a font is being used; EFalse, otherwise. */
EXPORT_C TBool CFbsBitGc::IsFontUsed() const
	{
	if (iFont.Handle())
		return ETrue;

	return EFalse;
	}

TBool CFbsBitGc::UserClipRect(TRect& aRect)
	{
	if (!aRect.Intersects(iUserClipRect))
		return ETrue;

	aRect.Intersection(iUserClipRect);

	return EFalse;
	}

/** Fetches the bounding rectangle of all drawing done since this function was last 
called.
@param aRect The bounding rectangle. */
EXPORT_C void CFbsBitGc::RectDrawnTo(TRect& aRect)
	{
	aRect = iDrawnTo;
	iDrawnTo.SetRect(0,0,0,0);
	}

/** Updates the justification settings.
This function assumes that ActivateNoJustAutoUpdate() has been used.
@param aText The text for which justification is to be adjusted. */
EXPORT_C void CFbsBitGc::UpdateJustification(const TDesC& aText)
	{
	TTextParameters* param = NULL;
	UpdateJustification(aText,param);
	}

/** Updates the justification for vertical text.
@param aText The text for which justification is to be adjusted.
@param aUp ETrue, if text is to be justified upwards;EFalse, if text is to 
be justified downwards. */
EXPORT_C void CFbsBitGc::UpdateJustificationVertical(const TDesC& aText, TBool aUp)
	{
	TTextParameters* param = NULL;
	UpdateJustificationVertical(aText,param,aUp);
	}

/** Updates the justification settings.
This function assumes that ActivateNoJustAutoUpdate() has been used.
@param aText The text for which justification is to be adjusted. */
EXPORT_C void CFbsBitGc::UpdateJustification(const TDesC& aText,const TTextParameters* aParam)
	{
	if ((iCharJustNum < 1 || iCharJustExcess == 0) && (iWordJustNum < 1 || iWordJustExcess < 1))
		return;

	TInt length = aText.Length();
	CFont::TPositionParam param;
	param.iText.Set(aText);		// Set the start of the string
	if (aParam)
		{
		length = aParam->iEnd;
		param.iPosInText = aParam->iStart;
		}
	TInt excess = 0;
	TInt glyphs = 0;
	RShapeInfo shapeInfo;
	for (TInt count = 0; count < length; count++)
		{
		if (iCharJustNum > 0 && iCharJustExcess != 0)
			excess += CGraphicsContext::JustificationInPixels(iCharJustExcess, iCharJustNum);
		if (iWordJustNum > 0 && iWordJustExcess > 0 && aText[count] == ' ')
			excess += CGraphicsContext::JustificationInPixels(iWordJustExcess, iWordJustNum);

		if (iCharJustNum < glyphs + length - count)	// there's at least 1 combined glyph to come
			{
			// otherwise we can skip this slow bit and just increment
			if (iFont.GetCharacterPosition2(param, shapeInfo))
				count = param.iPosInText - 1;	// -1 'cos it gets incremented anyway
			}
		glyphs++;
		}
	if(shapeInfo.IsOpen())
			shapeInfo.Close();

	iLastPrintPosition.iX += excess;
	}

/** Updates the justification for vertical text.
@param aText The text for which justification is to be adjusted.
@param aUp ETrue, if text is to be justified upwards;EFalse, if text is to 
be justified downwards. */
EXPORT_C void CFbsBitGc::UpdateJustificationVertical(const TDesC& aText,const TTextParameters* aParam,TBool aUp)
	{
	if ((iCharJustNum < 1 || iCharJustExcess == 0) && (iWordJustNum < 1 || iWordJustExcess < 1))
		return;

	TInt length = aText.Length();
	CFont::TPositionParam param;
	param.iText.Set(aText);		// Set the start of the string
	if (aParam)
		{
		length = aParam->iEnd;
		param.iPosInText = aParam->iStart;
		}
	TInt excess=0;
	TInt glyphs = 0;
	RShapeInfo shapeInfo;
	for (TInt count = 0; count < length; count++)
		{
		if (iCharJustNum > 0 && iCharJustExcess != 0)
			excess += CGraphicsContext::JustificationInPixels(iCharJustExcess, iCharJustNum);
		if (iWordJustNum > 0 && iWordJustExcess > 0 && aText[count] == ' ')
			excess += CGraphicsContext::JustificationInPixels(iWordJustExcess, iWordJustNum);
		if (iCharJustNum < glyphs + length - count)	// there's at least 1 combined glyph to come
			{
			// otherwise we can skip this slow bit and just increment
			if (iFont.GetCharacterPosition2(param, shapeInfo))
				count = param.iPosInText - 1;	// -1 'cos it gets incremented anyway
			}
		glyphs++;
		}
	if (shapeInfo.IsOpen())
		shapeInfo.Close();

	if (aUp)
		iLastPrintPosition.iY -= excess;
	else
		iLastPrintPosition.iY += excess;
	}
void CFbsBitGc::AddRect(const TRect& aRect)
	{
	if (iDrawnTo.IsEmpty())
		iDrawnTo = aRect;
	else if (!aRect.IsEmpty())
		iDrawnTo.BoundingRect(aRect);
	}

CFbsBitGcExtraData::CFbsBitGcExtraData()
	{
	iShadowColor = KDefaultShadowColor;	//default shadow color
	}

CFbsBitGcExtraData::~CFbsBitGcExtraData()
	{
	delete[] iPenArray;
	}

void CFbsBitGcExtraData::Reset()
	{
	SetShadowColor(KDefaultShadowColor);
	ResetPenArray();
	}

#define ORIENTATION_TO_CAP(orientation)	(1 << (TInt)orientation)

CGraphicsAccelerator* CFbsBitGc::GraphicsAccelerator()
	{
	CGraphicsAccelerator* ga = iDevice->GraphicsAccelerator();
	if(!ga)
		return NULL;

	const TGraphicsAcceleratorCaps* caps = ga->Capabilities();
 	//iReserved[0] specifies the supported rendering orientations. 
 	//Existing variable name is used to avoid SC break since it is an array.
 	const TUint orientationCaps = caps->iReserved[0]; 	
 	const TGraphicsOrientation orientation = iDevice->Orientation();
 	if(iDevice->iScreenDevice 
 	&& ((orientationCaps == 0 && orientation !=EGraphicsOrientationNormal)
 	|| ((ORIENTATION_TO_CAP(orientation) & orientationCaps) ==0)))
		return NULL;

	if(iUserDisplayMode!=ENone)
		return NULL;

	return ga;
	}

/** Replace target device but keep the current state

@param aDevice The target device
@internalComponent
*/
EXPORT_C void CFbsBitGc::ChangeDevice(CFbsDevice* aDevice)
	{
	iDevice = aDevice;
	}
