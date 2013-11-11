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

#include "directgdigcwrapper.h"
#include <s32mem.h>
#include <graphics/lookuptable.h>
#include <graphics/directgdidriver.h>
#include <graphics/directgdidrawablesource.h>
#include <graphics/sgresourceinternal.h>
#include "mwsgraphicscontexttodirectgdimappings.h"
#include "panic.h"

CDirectGdiGcWrapper* CDirectGdiGcWrapper::NewL()
	{
	CDirectGdiGcWrapper* self = new(ELeave) CDirectGdiGcWrapper;
	CleanupStack::PushL(self);
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	User::LeaveIfNull(driver);
	self->iContext = CDirectGdiContext::NewL(*driver);
	self->iErrorCode = KErrNone;
	self->iGcBuf = CBufSeg::NewL(512);
	//MWsFader
	//Default in BitGdi was 128 for the blackMap and 255 for the whiteMap
	//SetFadingParameters shows how the fade color is computed
	self->iFadeColor.SetInternal(0x80FFFFFF);
	
	self->iLut = PtrTo16BitNormalisationTable();
	CleanupStack::Pop(self);
	return self;
	}

CDirectGdiGcWrapper::~CDirectGdiGcWrapper()
	{
	delete iContext;
	delete iGcBuf;
	for (TInt i = 0; i < iDrawableSources.Count(); ++i)
		{
		iDrawableSources[i]->Close();
		}
	iDrawableSources.ResetAndDestroy();
	iClippingRegion.Close();
	}

TAny* CDirectGdiGcWrapper::ResolveObjectInterface(TUint aTypeId)
	{
	switch(aTypeId)
		{
	case MWsGraphicsContext::EWsObjectInterfaceId:
		return static_cast<MWsGraphicsContext*>(this);
	case MWsFader::EWsObjectInterfaceId:
		return static_cast<MWsFader*>(this);
	case MWsDrawableSourceProvider::EWsObjectInterfaceId:
		return static_cast<MWsDrawableSourceProvider*>(this);
	case MWsTextCursor::EWsObjectInterfaceId:
		return static_cast<MWsTextCursor*>(this);
		}
	return NULL;
	}

void CDirectGdiGcWrapper::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap)
	{
	iContext->BitBlt(aDestPos, aSourceBitmap);
	}

void CDirectGdiGcWrapper::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	iContext->BitBlt(aDestPos, aSourceBitmap, aSourceRect);
	}

void CDirectGdiGcWrapper::BitBltMasked(const TPoint& aDestPos,	const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask)
	{
	iContext->BitBltMasked(aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask);
	}

void CDirectGdiGcWrapper::BitBltMasked(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, const TPoint& aMaskPos)
	{
	iContext->BitBltMasked(aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, aMaskPos);
	}

void CDirectGdiGcWrapper::ResetClippingRegion()
	{
	iContext->ResetClippingRegion();
	}

void CDirectGdiGcWrapper::Clear()
	{
	iContext->Clear();
	}

void CDirectGdiGcWrapper::Clear(const TRect& aRect)
	{
	iContext->Clear(aRect);
	}

void CDirectGdiGcWrapper::ResetBrushPattern()
	{
	iContext->ResetBrushPattern();
	}

void CDirectGdiGcWrapper::ResetFont()
	{
	iContext->ResetFont();
	}

void CDirectGdiGcWrapper::DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	iContext->DrawArc(aRect, aStart, aEnd);
	}

void CDirectGdiGcWrapper::DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	iContext->DrawPie(aRect, aStart, aEnd);
	}

void CDirectGdiGcWrapper::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap)
	{
	iContext->DrawBitmap(aDestRect, aSourceBitmap);
	}

void CDirectGdiGcWrapper::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	iContext->DrawBitmap(aDestRect,	aSourceBitmap, aSourceRect);
	}

void CDirectGdiGcWrapper::DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect, const CFbsBitmap& aMaskBitmap, TBool aInvertMask)
	{
	iContext->DrawBitmapMasked(aDestRect, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask);
	}

void CDirectGdiGcWrapper::DrawRoundRect(const TRect& aRect, const TSize& aEllipse)
	{
	iContext->DrawRoundRect(aRect, aEllipse);
	}

void CDirectGdiGcWrapper::DrawPolyLine(const TArray<TPoint>& aPointList)
	{
	iContext->DrawPolyLine(aPointList);
	}

void CDirectGdiGcWrapper::DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
	{
	iContext->DrawPolyLineNoEndPoint(aPointList);
	}

void CDirectGdiGcWrapper::DrawPolygon(const TArray<TPoint>& aPointList, TFillRule aFillRule)
	{
	iContext->DrawPolygon(aPointList, MWsGraphicsContextToDirectGdiMappings::Convert(aFillRule));
	}

void CDirectGdiGcWrapper::DrawEllipse(const TRect& aRect)
	{
	iContext->DrawEllipse(aRect);
	}

void CDirectGdiGcWrapper::DrawLine(const TPoint& aStart, const TPoint& aEnd)
	{
	iContext->DrawLine(aStart, aEnd);
	}

void CDirectGdiGcWrapper::DrawLineTo(const TPoint& aPoint)
	{
	iContext->DrawLineTo(aPoint);
	}

void CDirectGdiGcWrapper::DrawLineBy(const TPoint& aVector)
	{
	iContext->DrawLineBy(aVector);
	}

void CDirectGdiGcWrapper::DrawRect(const TRect& aRect)
	{
	iContext->DrawRect(aRect);
	}

void CDirectGdiGcWrapper::DrawText(const TDesC& aText, const TTextParameters* aParam)
	{
	iContext->DrawText(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam));
	}

void CDirectGdiGcWrapper::DrawText(const TDesC& aText, const TTextParameters* aParam, const TPoint& aPosition)
	{
	iContext->DrawText(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aPosition);
	}

void CDirectGdiGcWrapper::DrawText(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect)
	{
	iContext->DrawText(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aClipRect);
	}

void CDirectGdiGcWrapper::DrawText(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset, TTextAlign aHrz, TInt aMargin)
	{
	iContext->DrawText(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aClipFillRect, aBaselineOffset, MWsGraphicsContextToDirectGdiMappings::Convert(aHrz), aMargin);
	}

void CDirectGdiGcWrapper::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, TBool aUp)
	{
	iContext->DrawTextVertical(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aUp);
	}

void CDirectGdiGcWrapper::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TPoint& aPosition, TBool aUp)
	{
	iContext->DrawTextVertical(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aPosition, aUp);
	}

void CDirectGdiGcWrapper::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TBool aUp)
	{
	iContext->DrawTextVertical(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aClipRect, aUp);
	}

void CDirectGdiGcWrapper::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, TBool aUp, TTextAlign aVert, TInt aMargin)
	{
	iContext->DrawTextVertical(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aClipRect, aBaselineOffset, aUp, MWsGraphicsContextToDirectGdiMappings::Convert(aVert), aMargin);
	}

void CDirectGdiGcWrapper::DrawTextVertical(const TDesC& aText, const TTextParameters* aParam, const TRect& aClipRect, TInt aBaselineOffset, TInt aTextWidth, TBool aUp, TTextAlign aVert, TInt aMargin)
	{
	iContext->DrawTextVertical(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aClipRect, aBaselineOffset, aTextWidth, aUp, MWsGraphicsContextToDirectGdiMappings::Convert(aVert), aMargin);
	}

void CDirectGdiGcWrapper::MoveTo(const TPoint& aPoint)
	{
	iContext->MoveTo(aPoint);
	}

void CDirectGdiGcWrapper::MoveBy(const TPoint& aVector)
	{
	iContext->MoveBy(aVector);
	}

void CDirectGdiGcWrapper::Plot(const TPoint& aPoint)
	{
	iContext->Plot(aPoint);
	}

void CDirectGdiGcWrapper::Reset()
	{
	iContext->Reset();
	}

void CDirectGdiGcWrapper::SetBrushColor(const TRgb& aColor)
	{
	iContext->SetBrushColor(aColor);
	}

void CDirectGdiGcWrapper::SetBrushOrigin(const TPoint& aOrigin)
	{
	iContext->SetBrushOrigin(aOrigin);
	}

void CDirectGdiGcWrapper::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	iContext->SetBrushStyle(MWsGraphicsContextToDirectGdiMappings::Convert(aBrushStyle));
	}

void CDirectGdiGcWrapper::SetClippingRegion(const TRegion& aRegion)
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	driver->GetError(); //make sure that an error has been received 
	iContext->SetClippingRegion(aRegion);
	TInt err = driver->GetError();
	SetError(err);
	if(err == KErrNone)
		{
		iClippingRegion.Copy(aRegion);
		}
	}

void CDirectGdiGcWrapper::SetDrawMode(TDrawMode aDrawMode)
	{
	iContext->SetDrawMode(MWsGraphicsContextToDirectGdiMappings::LossyConvert(aDrawMode));
	}

void CDirectGdiGcWrapper::SetOrigin(const TPoint& aPoint)
	{
	iContext->SetOrigin(aPoint);
	iOrigin = aPoint;
	}

void CDirectGdiGcWrapper::SetPenColor(const TRgb& aColor)
	{
	iContext->SetPenColor(aColor);
	}

void CDirectGdiGcWrapper::SetPenStyle(TPenStyle aPenStyle)
	{
	iContext->SetPenStyle(MWsGraphicsContextToDirectGdiMappings::Convert(aPenStyle));
	}

void CDirectGdiGcWrapper::SetPenSize(const TSize& aSize)
	{
	iContext->SetPenSize(aSize);
	}

void CDirectGdiGcWrapper::SetTextShadowColor(const TRgb& aColor)
	{
	iContext->SetTextShadowColor(aColor);
	}

void CDirectGdiGcWrapper::SetCharJustification(TInt aExcessWidth, TInt aNumChars)
	{
	iContext->SetCharJustification(aExcessWidth, aNumChars);
	}

void CDirectGdiGcWrapper::SetWordJustification(TInt aExcessWidth, TInt aNumGaps)
	{
	iContext->SetWordJustification(aExcessWidth, aNumGaps);
	}

void CDirectGdiGcWrapper::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	iContext->SetUnderlineStyle(MWsGraphicsContextToDirectGdiMappings::Convert(aUnderlineStyle));
	}

void CDirectGdiGcWrapper::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	iContext->SetStrikethroughStyle(MWsGraphicsContextToDirectGdiMappings::Convert(aStrikethroughStyle));
	}

void CDirectGdiGcWrapper::SetBrushPattern(const CFbsBitmap& aBitmap)
	{
	iContext->SetBrushPattern(aBitmap);
	}

void CDirectGdiGcWrapper::SetBrushPattern(TInt aFbsBitmapHandle)
	{
	iContext->SetBrushPattern(aFbsBitmapHandle);
	}

void CDirectGdiGcWrapper::SetFont(const CFont* aFont)
	{
	iContext->SetFont(aFont);
	}

void CDirectGdiGcWrapper::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{
	iContext->CopyRect(aOffset, aRect);
	}

void CDirectGdiGcWrapper::UpdateJustification(const TDesC& aText, const TTextParameters* aParam)
	{
	iContext->UpdateJustification(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam));
	}

void CDirectGdiGcWrapper::UpdateJustificationVertical(const TDesC& aText, const TTextParameters* aParam, TBool aUp)
	{
	iContext->UpdateJustificationVertical(aText, MWsGraphicsContextToDirectGdiMappings::Convert(aParam), aUp);
	}

void CDirectGdiGcWrapper::SetFontNoDuplicate(const CFont* aFont)
	{
	iContext->SetFontNoDuplicate(static_cast<const CDirectGdiFont*>(aFont));
	}

TBool CDirectGdiGcWrapper::HasBrushPattern() const
	{
	return iContext->HasBrushPattern();
	}

TBool CDirectGdiGcWrapper::HasFont() const
	{
	return iContext->HasFont();
	}

TRgb CDirectGdiGcWrapper::BrushColor() const
	{
	return iContext->BrushColor();
	}

TRgb CDirectGdiGcWrapper::PenColor() const
	{
	return iContext->PenColor();
	}

TRgb CDirectGdiGcWrapper::TextShadowColor() const
	{
	return iContext->TextShadowColor();
	}

TInt CDirectGdiGcWrapper::CreateDrawableSource(const TSgDrawableId& aDrawableId, TAny*& aSource)
	{
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	if (!driver)
		{
		return KErrNotReady;
		}
	RDirectGdiDrawableSource* drawableSource = new RDirectGdiDrawableSource(*driver);
	if (!drawableSource)
		{
		return KErrNoMemory;
		}

	//check usage flags if the drawable is an RSgImage
	RSgImage image;
	TInt res = image.Open(aDrawableId);
	if (res == KErrNone)
		{
		TSgImageInfo info;
		res = image.GetInfo(info);
		image.Close();
		if (res == KErrNone && !(info.iUsage & ESgUsageWindowGcSource))
			{
			res = KErrNotSupported;
			}

		if (res != KErrNone)
			{
			delete drawableSource;
			return res;
			}
		}

	RSgDrawable drawable;
	res = drawable.Open(aDrawableId, ESgDoNotRestrictUsage);
	if (res != KErrNone)
		{
		delete drawableSource;
		return res;
		}
	res = drawableSource->Create(drawable);
	drawable.Close();
	if (res != KErrNone)
		{
		delete drawableSource;
		return res;
		}
	res = iDrawableSources.InsertInAddressOrder(drawableSource);
	if (res != KErrNone)
		{
		drawableSource->Close();
		delete drawableSource;
		return res;
		}
	aSource = drawableSource;
	return KErrNone;
	}

void CDirectGdiGcWrapper::CloseDrawableSource(TAny* aSource)
	{
	RDirectGdiDrawableSource* drawableSource = static_cast<RDirectGdiDrawableSource*>(aSource);
	TInt index = iDrawableSources.FindInAddressOrder(drawableSource);
	if (index != KErrNotFound)
		{
		drawableSource->Close();
		delete drawableSource;
		iDrawableSources.Remove(index);
		}
	}

void CDirectGdiGcWrapper::DrawResource(const TAny* aSource, const TPoint& aPos, CWindowGc::TGraphicsRotation aRotation)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	TInt index = iDrawableSources.FindInAddressOrder(drawableSource);
	if (index == KErrNotFound)
		{
		STD_ASSERT_DEBUG(0, EPluginPanicInvalidDrawableSource);
		return;
		}
	iContext->DrawResource(aPos, *drawableSource, (DirectGdi::TGraphicsRotation)aRotation);
	}

void CDirectGdiGcWrapper::DrawResource(const TAny* aSource, const TRect& aRect, CWindowGc::TGraphicsRotation aRotation)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	TInt index = iDrawableSources.FindInAddressOrder(drawableSource);
	if (index == KErrNotFound)
		{
		STD_ASSERT_DEBUG(0, EPluginPanicInvalidDrawableSource);
		return;
		}
	iContext->DrawResource(aRect, *drawableSource, (DirectGdi::TGraphicsRotation)aRotation);
	}

void CDirectGdiGcWrapper::DrawResource(const TAny* aSource, const TRect& aRectDest, const TRect& aRectSrc, CWindowGc::TGraphicsRotation aRotation)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	TInt index = iDrawableSources.FindInAddressOrder(drawableSource);
	if (index == KErrNotFound)
		{
		STD_ASSERT_DEBUG(0, EPluginPanicInvalidDrawableSource);
		return;
		}
	iContext->DrawResource(aRectDest, *drawableSource, aRectSrc, (DirectGdi::TGraphicsRotation)aRotation);
	}

void CDirectGdiGcWrapper::DrawResource(const TAny* aSource, const TRect& aRect, const TDesC8& aDes)
	{
	const RDirectGdiDrawableSource* drawableSource = static_cast<const RDirectGdiDrawableSource*>(aSource);
	TInt index = iDrawableSources.FindInAddressOrder(drawableSource);
	if (index == KErrNotFound)
		{
		STD_ASSERT_DEBUG(0, EPluginPanicInvalidDrawableSource);
		return;
		}
	iContext->DrawResource(aRect, *drawableSource, aDes);
	}

/**
Sets the error code. If the error code is already set to a value other
than KErrNone, the error code will not be modified.

@param  aErr The error code to set.

@post 	The error code has been set.
*/
void CDirectGdiGcWrapper::SetError(TInt aError)
	{
	if (aError != KErrNone && iErrorCode == KErrNone)
		{
		iErrorCode = aError;
		}
	}

/**
Returns the first error code (set as the result of calling some CDirectGdiGcWrapper API), if any,
since the last call to this function or, if it has not previously been called, since
the CDirectGdiGcWrapper was constructed. Calling this function clears the error code.

@post 	The error code has been reset after being read.

@return The first error code, if any, since the last call to this function or, 
		if it has not previously been called, since the CDirectGdiGcWrapper was constructed. 
		KErrNone will indicate that no such error has occurred.
*/
TInt CDirectGdiGcWrapper::GetError()
	{
	TInt err = iErrorCode;
	iErrorCode = KErrNone;
	return err;
	}

TPoint CDirectGdiGcWrapper::Origin() const
	{
	return iOrigin;
	}

const TRegion& CDirectGdiGcWrapper::ClippingRegion()
	{
	return iClippingRegion;
	}

TInt CDirectGdiGcWrapper::Push()
	{
	// the buf format is len+data where data is written by the GC's ExternalizeL()
	iGcBuf->Reset();
	CBufBase& buf = *iGcBuf;
	const TInt start = buf.Size();
	RBufWriteStream out(buf,start);
	TRAPD(err,out.WriteInt32L(0));
	if(!err)
		{
		TRAP(err,iContext->ExternalizeL(out));
		}
	if(err) //rollback addition
		{
		buf.Delete(start,buf.Size()-start);
		}
	else //fixup len
		{
		TRAP_IGNORE(out.CommitL();) // can't see this failing
		TPckgBuf<TInt32> pckg(buf.Size()-sizeof(TInt32)-start);
		buf.Write(start,pckg);
		}
	return err;
	}

void CDirectGdiGcWrapper::Pop()
	{
	CBufBase& buf = *iGcBuf;
	TInt ofs = 0;
	FOREVER
		{
		TInt chunk = 0;
		RBufReadStream in(buf,ofs);
		TRAPD(err,chunk = in.ReadInt32L());
		if(err)
			{
			STD_ASSERT_DEBUG(err != 0, EPluginPanicPopGcSettings);
			return;
			}
		if(ofs+sizeof(TInt32)+chunk >= buf.Size()) // the last chunk?
			{
			TRAP_IGNORE(iContext->InternalizeL(in));
			buf.Delete(ofs,buf.Size()-ofs);
			return;
			}
		ofs += chunk + sizeof(TInt32);
		}
	}

//Default method of fading simply uses bitgdi to perform fading
void CDirectGdiGcWrapper::FadeArea(const TRegion& aRegion)
	{
	if (!&aRegion || aRegion.CheckError())
		return;

	iContext->Reset();
	iContext->SetClippingRegion(aRegion);
	iContext->SetPenStyle(DirectGdi::ENullPen);
	iContext->SetBrushStyle(DirectGdi::ESolidBrush);
	iContext->SetBrushColor(iFadeColor);
	iContext->DrawRect(aRegion.BoundingRect());
	}
	
//Default method of fading expects two TUint8's describing the black/white map 
//as possible fading parameters
void CDirectGdiGcWrapper::SetFadingParameters(const TDesC8& aData)
  	{
	TPckgBuf<TFadingParams> buf;
	buf.Copy(aData);
	TFadingParams parameters = buf();

	//Situations where blackMap > whiteMap are NOT supported
	if (parameters.blackMap > parameters.whiteMap)
		{
		TUint8 oldMap = parameters.blackMap;
		parameters.blackMap = parameters.whiteMap;
		parameters.whiteMap = oldMap;
		}
	
	//CFbsBitGc::FadeArea() does the following per color component:
	//   dst = dst * (whiteMap - blackMap) + blackMap;

	//To achieve the same effect using MWsGraphicsContext we draw a rectangle
	//with specific intensity and alpha values:
	//   dst = dst * (1 - alpha) + intensity * alpha;
	//Thus:
	//   alpha = 1 - whiteMap + blackMap;
	//   intensity = blackMap / alpha;

	// alpha = 1 - whiteMap + blackMap;
	TInt alpha = 255 - parameters.whiteMap + parameters.blackMap;
	// intensity = blackMap / alpha;
	TInt i = (parameters.blackMap * iLut[alpha]) >> 8;

	iFadeColor.SetInternal(i << 16 | i << 8 | i | alpha << 24);
  	}

void CDirectGdiGcWrapper::DrawTextCursor(const TTextCursorInfo& aTextCursorInfo)
	{
	/*
	 * This function is written with the following assumption:
	 * The UI Toolkit uses text entry windows with a white background
	 * and black text, but always requests a white text cursor.
	 * 
	 * We therefore ignore the KRgbWhite text cursor cursor supplied
	 * and use a Black overprinting strategy instead.
	 */
	STD_ASSERT_ALWAYS(
		aTextCursorInfo.iTextCursorType == TTextCursor::ETypeRectangle ||
		aTextCursorInfo.iTextCursorType == TTextCursor::ETypeHollowRectangle,
		EPluginPanicInvalidCursorType
	);

	TRegionFix<1> fullWindowRegion;
	const TRegion* clippingRegion = &aTextCursorInfo.iRegion;
	if (aTextCursorInfo.iRegion.CheckError())
		{
		fullWindowRegion.AddRect(aTextCursorInfo.iWindow->AbsRect());
		clippingRegion = &fullWindowRegion;
		}

	if (clippingRegion->IsEmpty())
		{
		return;
		}

	iContext->SetDrawMode(DirectGdi::EDrawModePEN);
	switch (aTextCursorInfo.iTextCursorType)
		{
	case TTextCursor::ETypeRectangle:
		{
		iContext->SetBrushStyle(DirectGdi::ESolidBrush);
		iContext->SetPenStyle(DirectGdi::ENullPen);
		iContext->SetBrushColor(KRgbBlack);
		}
		break;
	case TTextCursor::ETypeHollowRectangle:
		{
		iContext->SetBrushStyle(DirectGdi::ENullBrush);
		iContext->SetPenStyle(DirectGdi::ESolidPen);
		iContext->SetPenColor(KRgbBlack);
		}
		break;
		}
	iContext->SetClippingRegion(*clippingRegion);
	/*
	 * During Sprite drawing, the GC gets reset.  Possibly other code could
	 * have done this also.  So make sure we setup the origin so that window-relative
	 * co-ordinates work as expected; iCursorRect is in window co-ordinates.
	 */
	iContext->SetOrigin(aTextCursorInfo.iWindow->Origin());
	iContext->DrawRect(aTextCursorInfo.iCursorRect);
	}
