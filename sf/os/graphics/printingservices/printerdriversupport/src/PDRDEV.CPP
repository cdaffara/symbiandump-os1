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

#include <banddev.h>
#include <pdrstore.h>
#include <bitdev.h>
#include <bitstd.h>
#include "PDRBODY.H"
#include "PDRSTD.H"
#include "pdrtext.h"

EXPORT_C CPdrDevice::CPdrDevice():
	CPrinterDevice(),
	iStore(NULL),
	iModel(),
	iModelInfo(NULL),
	iTypefaceStore(NULL)
	{
	__DECLARE_NAME(_S("CPdrDevice"));
	}

EXPORT_C CPdrDevice::~CPdrDevice()
	{
	DeleteControl();
	delete iModelInfo;
	delete iTypefaceStore;
	}

EXPORT_C void CPdrDevice::SelectPageSpecInTwips(const TPageSpec& aPageSpec)
	{
	CPrinterDevice::SelectPageSpecInTwips(aPageSpec);
	iTypefaceStore->SetPageOrientation(aPageSpec.iOrientation);
	}

EXPORT_C TDisplayMode CPdrDevice::DisplayMode() const
	{
	return iModelInfo->iDisplayMode;
	}

EXPORT_C TSize CPdrDevice::SizeInPixels() const
	{
	TSize size;
	size.iWidth = HorizontalTwipsToPixels(SizeInTwips().iWidth);
	size.iHeight = VerticalTwipsToPixels(SizeInTwips().iHeight);
	return size;
	}

EXPORT_C TSize CPdrDevice::SizeInTwips() const
	{
	TSize size;
	if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::EPortrait)
		{
		size = iCurrentPageSpecInTwips.iPortraitPageSize;
		}
	else if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::ELandscape)
		{
		size.iWidth = iCurrentPageSpecInTwips.iPortraitPageSize.iHeight;
		size.iHeight = iCurrentPageSpecInTwips.iPortraitPageSize.iWidth;
		}
	return size;
	}

EXPORT_C TRect CPdrDevice::PrintablePageInPixels() const
	{
	TRect rect;
	if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::EPortrait)
		{
		rect.iTl.iX = iModelInfo->iPortraitOffsetInPixels.iX;
		rect.iTl.iY = iModelInfo->iMinMarginsInPixels.iTop;
		rect.iBr.iX = SizeInPixels().iWidth-iModelInfo->iPortraitOffsetInPixels.iX;
		rect.iBr.iY = SizeInPixels().iHeight-iModelInfo->iMinMarginsInPixels.iBottom;
		}
	else if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::ELandscape)
		{
		rect.iTl.iX = iModelInfo->iLandscapeOffsetInPixels.iX;
		rect.iTl.iY = iModelInfo->iMinMarginsInPixels.iLeft;
		rect.iBr.iX = SizeInPixels().iWidth-iModelInfo->iLandscapeOffsetInPixels.iX;
		rect.iBr.iY = SizeInPixels().iHeight-iModelInfo->iMinMarginsInPixels.iRight;
		}
	return rect;
	}

EXPORT_C TInt CPdrDevice::HorizontalTwipsToPixels(TInt aTwips) const
	{
	return ((1000 * aTwips) + (KPixelSizeInTwips().iWidth / 2)) / KPixelSizeInTwips().iWidth;
	}

EXPORT_C TInt CPdrDevice::VerticalTwipsToPixels(TInt aTwips) const
	{
	return ((1000 * aTwips) + (KPixelSizeInTwips().iHeight / 2)) / KPixelSizeInTwips().iHeight;
	}

EXPORT_C TInt CPdrDevice::HorizontalPixelsToTwips(TInt aPixels) const
	{
	return ((aPixels * KPixelSizeInTwips().iWidth) + 500) / 1000;
	}

EXPORT_C TInt CPdrDevice::VerticalPixelsToTwips(TInt aPixels) const
	{
	return ((aPixels * KPixelSizeInTwips().iHeight) + 500) / 1000;
	}

/** Creates a font from those available in the printer device's 
typeface store that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function is replaced by GetNearestFontToDesignHeightInTwips()

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes.
@deprecated */
EXPORT_C TInt CPdrDevice::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Creates a font from those available in the printer device's 
typeface store that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function replaces GetNearestFontInTwips()

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes. */
EXPORT_C TInt CPdrDevice::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return iTypefaceStore->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** This call is defined because it had to be - it appeared as an abstract virtual in
the base class. But it should never actually get called for this class. */
EXPORT_C TInt CPdrDevice::GetNearestFontToMaxHeightInTwips(CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/, TInt /*aMaxHeight*/)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CPdrDevice::NumTypefaces() const
	{
	return iTypefaceStore->NumTypefaces();
	}

EXPORT_C void CPdrDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport, TInt aTypefaceIndex) const
	{
	iTypefaceStore->TypefaceSupport(aTypefaceSupport, aTypefaceIndex);
	}

EXPORT_C TInt CPdrDevice::FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const
	{
	return iTypefaceStore->FontHeightInTwips(aTypefaceIndex, aHeightIndex);
	}

EXPORT_C void CPdrDevice::PaletteAttributes(TBool& aModifiable, TInt& aNumEntries) const
	{
	aModifiable = EFalse;
	aNumEntries = 0;
	}

EXPORT_C void CPdrDevice::SetPalette(CPalette* /*aPalette*/)
	{
	}
	
EXPORT_C TInt CPdrDevice::GetPalette(CPalette*& /*aPalette*/) const
	{
	return KErrNotSupported;
	}

EXPORT_C TPrinterModelEntry CPdrDevice::Model() const
	{
	return iModel.iEntry;
	}

EXPORT_C TInt CPdrDevice::Flags() const
	{
	return iModelInfo->iFlags;
	}

EXPORT_C TInt CPdrDevice::SetModel(const TPrinterModelHeader& aModel, CStreamStore& aStore)
	{
	iModel = aModel;
	iStore = &aStore;
	TRAPD(ret, DoSetModelL());
	return ret;
	}

EXPORT_C void CPdrDevice::ReleaseFont(CFont* aFont)
	{
	iTypefaceStore->ReleaseFont(aFont);
	}

EXPORT_C TPoint CPdrDevice::OffsetInPixels()
	{
	TPoint offset;
	if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::EPortrait)
		offset = iModelInfo->iPortraitOffsetInPixels;
	else if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::ELandscape)
		offset = iModelInfo->iLandscapeOffsetInPixels;
	return offset;
	}

EXPORT_C TSize CPdrDevice::KPixelSizeInTwips() const
	{
	TSize size; 
	if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::EPortrait)
		size = TSize(iModelInfo->iKPixelWidthInTwips, iModelInfo->iKPixelHeightInTwips);
	else if (iCurrentPageSpecInTwips.iOrientation == TPageSpec::ELandscape)
		size = TSize(iModelInfo->iKPixelHeightInTwips, iModelInfo->iKPixelWidthInTwips);
	return size;
	}

void CPdrDevice::DoSetModelL()
	{
	RStoreReadStream stream;
	stream.OpenLC(*iStore, iModel.iModelDataStreamId);
	iModelInfo = new(ELeave) CPdrModelInfo();
	iModelInfo->InternalizeL(stream);
	CleanupStack::PopAndDestroy();
	iTypefaceStore = CPdrTypefaceStore::NewL(*iStore, iModelInfo->iNumTypefaceFonts, iModelInfo->iTypefaceFontsEntryList, iCurrentPageSpecInTwips.iOrientation, iModelInfo->iKPixelHeightInTwips, this);
	}

EXPORT_C CPdrControl::CPdrControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort):
	CPrinterControl(aPrinterPort),
	iPdrDevice(aPdrDevice),
	iPageBuffer(NULL),
	iResources(NULL),
	iBandedDevice(NULL),
	iPageText(NULL),
	iBandIndex(0),
	iEntryIndex(0),
	iPosition(iPdrDevice->OffsetInPixels()),
	iTextFormat()
	{
	__DECLARE_NAME(_S("CPdrControl"));
	}

EXPORT_C CPdrControl::~CPdrControl()
	{
	delete iPageBuffer;
	delete iResources;
	delete iBandedDevice;
	delete iPageText;
	}

EXPORT_C TInt CPdrControl::CreateContext(CGraphicsContext*& aGC)
	{
	__ASSERT_DEBUG(iState == EPrinting, Panic(EPdrNotPrinting));
	TRAPD(ret, aGC = CPdrGc::NewL(iPdrDevice));
	return ret;
	}

EXPORT_C TInt CPdrControl::BandsPerPage()
	{
	TInt numbands = {iBandedDevice ? iBandedDevice->NumBands() + 1 : 1};
	return numbands;
	}

/**
 @return EMoreOnPage if more to print on the current page,
 otherwise ENoMoreOnPage.
*/
EXPORT_C CPrinterControl::TMoreOnPage CPdrControl::QueueGetBand(TRequestStatus& aStatus, TBandAttributes& aBand)
	{
	TMoreOnPage moreonpage = ENoMoreOnPage;

	TRAPD(ret, DoQueueGetBandL());
	if (ret != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, ret);
		}
	else
		{
		if (IsGraphicsBand())
			{
			aBand.iRect = iBandedDevice->BandRect();
			aBand.iTextIsIgnored = ETrue;
			aBand.iGraphicsIsIgnored = EFalse;
			aBand.iFirstBandOnPage = EFalse;
			}
		else
			{
			aBand.iRect = iPdrDevice->PrintablePageInPixels();
			aBand.iTextIsIgnored = EFalse;
			aBand.iGraphicsIsIgnored = ETrue;
			aBand.iFirstBandOnPage = ETrue;
			}
		if (iBandIndex == (BandsPerPage() - 1))
			moreonpage = ENoMoreOnPage;
		else
			moreonpage = EMoreOnPage;
		iPageBuffer->StartFlush(aStatus);
		}
	return moreonpage;
	}

EXPORT_C void CPdrControl::QueueEndPrint(TRequestStatus& aStatus)
	{
	TRAPD(ret, DoQueueEndPrintL());
	iState = ENotPrinting;
	if (ret != KErrNone)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, ret);
		}
	else
		iPageBuffer->StartFlush(aStatus);
	}

/**
 Tidy up synchronously in a short time.
*/
EXPORT_C void CPdrControl::AbortPrint() // tidy up synchronously in a short time, return success code
	{
	iPageBuffer->Cancel(); // dont call DoCancel()
	iState = ENotPrinting;
	}

EXPORT_C void CPdrControl::DrawTextL(const TPoint& aPoint, const TFontUnderline aUnderlineStyle, const TFontStrikethrough aStrikethroughStyle, const TRgb& aColor, const CInfoFont* aFont, const TDesC& aString)
	{
	__ASSERT_DEBUG(iState == EPrinting,Panic(EPdrNotPrinting));
	if (iPageText)
		iPageText->AddEntryL(aPoint, aUnderlineStyle, aStrikethroughStyle, aColor, aFont, aString);
	else
		{
		HBufC8* string = aFont->TranslateStringL(aString);
		CleanupStack::PushL(string);
		TTextFormat textformat(aUnderlineStyle, aStrikethroughStyle, aColor, aFont->CommandString(), aFont->FontSpecInTwips().iFontStyle);
		OutputTextL(aPoint + TPoint(0, aFont->BaselineOffsetInPixels()), aFont->MeasureText(aString), textformat,string->Des());
		CleanupStack::PopAndDestroy();
		}
	}

EXPORT_C TBool CPdrControl::IsGraphicsBand() const
	{
	return iBandIndex;
	}

EXPORT_C void CPdrControl::MoveToL(const TPoint& aPoint) 
	{
	// PCL, at least, treats move command strings with an explicit
	// sign as being relative, so if the offset is negative then
	// set the absolute position to zero first.

	TPoint offset = iPdrDevice->OffsetInPixels();
	TCommandString des;
	if (aPoint.iX - iPosition.iX)
		{
		if (aPoint.iX - iPosition.iX < 0)
			{
			des.Format(iResources->ResourceString(EPdrSetXPos), 0);
			iPageBuffer->AddBytesL(des);
			}

		des.Format(iResources->ResourceString(EPdrSetXPos), aPoint.iX - offset.iX);
		iPageBuffer->AddBytesL(des);
		}
	if (aPoint.iY - iPosition.iY)
		{
		if (aPoint.iY - iPosition.iY < 0)
			{
			des.Format(iResources->ResourceString(EPdrSetYPos), 0);
			iPageBuffer->AddBytesL(des);
			}

		des.Format(iResources->ResourceString(EPdrSetYPos), aPoint.iY - offset.iY);
		iPageBuffer->AddBytesL(des);
		}
	iPosition = aPoint;
	}

EXPORT_C void CPdrControl::MoveByL(const TPoint& aVector) 
	{
	TCommandString resource;
	TCommandString des;
	TPoint vector = aVector;
	if (vector.iX)
		{
		if (aVector.iX < 0)
			{
			CommandL(EPdrCarriageReturn);
			vector.iX += iPosition.iX - iPdrDevice->OffsetInPixels().iX;
			}
		resource.Copy(iResources->ResourceString(EPdrIncrementXPos));
		__ASSERT_DEBUG(resource.Length() >= 2, Panic(EPdrBadResourceString));
		if (resource[0] == '*')
			{
			for (; vector.iX > 0; vector.iX--)
				iPageBuffer->AddBytesL(resource.Mid(1));
			}
		else
			{
			des.Format(resource, vector.iX);
			iPageBuffer->AddBytesL(des);
			}
		}
	resource.Copy(iResources->ResourceString(EPdrIncrementYPos));
	if (vector.iY)
		{
		__ASSERT_DEBUG(resource.Length() >= 2, Panic(EPdrBadResourceString));
		if (resource[0] == '*')
			{
			for (; vector.iY > 0; vector.iY--)
				iPageBuffer->AddBytesL(resource.Mid(1));
			}
		else
			{
			des.Format(resource, vector.iY);
			iPageBuffer->AddBytesL(des);
			}
		}
	iPosition += aVector;
	}

EXPORT_C void CPdrControl::OutputTextL(const TPoint& aPoint, TInt aWidthInPixels, const TTextFormat& aTextFormat, const TDesC8& aString)
	{
	__ASSERT_DEBUG(iState == EPrinting,Panic(EPdrNotPrinting));

	if (iTextFormat.iFontString.Compare(aTextFormat.iFontString))
		iPageBuffer->AddBytesL(aTextFormat.iFontString);

	if (iTextFormat.iFontStyle.Posture() != aTextFormat.iFontStyle.Posture())
		SetFontPostureL(aTextFormat.iFontStyle.Posture());

	if (iTextFormat.iFontStyle.StrokeWeight() != aTextFormat.iFontStyle.StrokeWeight())
		SetFontStrokeWeightL(aTextFormat.iFontStyle.StrokeWeight());

	if (iTextFormat.iColor != aTextFormat.iColor)
		SetTextColorL(aTextFormat.iColor);

	iTextFormat = aTextFormat;

	MoveToL(aPoint);

	if (aTextFormat.iUnderlineStyle == EUnderlineOn)
		CommandL(EPdrUnderlineOn);

	if (aTextFormat.iStrikethroughStyle == EStrikethroughOn)
		CommandL(EPdrStrikethroughOn);

	iPageBuffer->AddBytesL(aString);

	iPosition.iX += aWidthInPixels;

	if (aTextFormat.iUnderlineStyle == EUnderlineOn)
		CommandL(EPdrUnderlineOff);

	if (aTextFormat.iStrikethroughStyle == EStrikethroughOn)
		CommandL(EPdrStrikethroughOff);
	}

EXPORT_C void CPdrControl::DoQueueGetBandL()
	{
	if (iState == ENotPrinting)
		{
		iState = EPrinting;
		iBandIndex = 0;
		if (iBandedDevice)
			iBandedDevice->Reset();
		CommandL(EPdrReset);
		SetPageSizeL();
		CommandL(EPdrPreAmble);
		SetPageOrientationL();
		SetTextColorL(KRgbBlack);	
		}
 	else 
		{
		OutputBandL();
		if (iBandIndex == (BandsPerPage() - 1))
			{
			iBandIndex = 0;
			iEntryIndex = 0;
			if (iPageText)
				iPageText->Reset(); 
			CommandL(EPdrNewPage);
			iPosition = iPdrDevice->OffsetInPixels();
			}
		else
			iBandIndex = iBandedDevice->NextBand() + 1;
		}
	}

EXPORT_C void CPdrControl::DoQueueEndPrintL()
	{
	OutputBandL();
	CommandL(EPdrPostAmble);
	}

EXPORT_C void CPdrControl::ConstructL(CStreamStore& aStore, TStreamId aStreamId)
	{
	__ASSERT_ALWAYS(iPrinterPort, Panic(EPdrRequiresPrinterPort));
	iPageBuffer = CPageBuffer::NewL(iPrinterPort);
	iResources = new(ELeave) CPdrResources();
	iResources->RestoreL(aStore, aStreamId);
	}

EXPORT_C void CPdrControl::SetPageSizeL()
	{
	}
	 
EXPORT_C void CPdrControl::SetPageOrientationL()
	{
	TPageSpec::TPageOrientation orientation = iPdrDevice->CurrentPageSpecInTwips().iOrientation;
	if (orientation == TPageSpec::EPortrait)
		CommandL(EPdrPortrait);
	else
		CommandL(EPdrLandscape);
	}

EXPORT_C void CPdrControl::SetFontStrokeWeightL(const TFontStrokeWeight aStrokeWeight)
	{
	if (aStrokeWeight == EStrokeWeightNormal)
		CommandL(EPdrBoldOff);
	else 
		CommandL(EPdrBoldOn);
	}

EXPORT_C void CPdrControl::SetFontPostureL(const TFontPosture aPosture)
	{
	if (aPosture == EPostureUpright)
		CommandL(EPdrItalicOff);
	else 
		CommandL(EPdrItalicOn);
	}

EXPORT_C void CPdrControl::SetTextColorL(const TRgb& /*aColor*/) 
	{
	}

EXPORT_C void CPdrControl::CommandL(const TInt anId) 
	{
	iPageBuffer->AddBytesL(iResources->ResourceString(anId));
	}

void CPdrGc::ConstructL()
	{
	if (PdrControl()->BandedDevice())
		{
		User::LeaveIfError(PdrControl()->BandedDevice()->CreateContext((CGraphicsContext*&)iBandedGc));
		TFontSpec spec;
		User::LeaveIfError(PdrControl()->BandedDevice()->GetNearestFontToDesignHeightInTwips((CFont*&)iFbsFont, spec));
		iBandedGc->UseFont(iFbsFont);
		}
	}

CPdrGc::CPdrGc(CPdrDevice* aDevice):
	iPdrDevice(aDevice),
	iBandedGc(NULL),
	iFbsFont(NULL),
	iBandedFont(NULL),
	iFont(NULL),
	iOrigin(0, 0),
	iPosition(0, 0),
	iUnderlineStyle(EUnderlineOff),
	iStrikethroughStyle(EStrikethroughOff),
	iClippingRect(iPdrDevice->PrintablePageInPixels()),
	iWordExcessWidthInPixels(0),
	iNumGaps(0),
	iCharExcessWidthInPixels(0),
	iNumChars(0),
	iPenColor(KRgbBlack)
	{
	}

CPdrGc* CPdrGc::NewL(CPdrDevice* aDevice)
	{
	CPdrGc* gc = new(ELeave) CPdrGc(aDevice);
	CleanupStack::PushL(gc);
	gc->ConstructL();
	CleanupStack::Pop();
	return gc;
	}

EXPORT_C CPdrGc::~CPdrGc()
	{
	if (iBandedGc)
		{
		if (iBandedGc->IsFontUsed())
			{
			iBandedGc->DiscardFont();
			iBandedFont = NULL;
			}
		delete iBandedGc;
		PdrControl()->BandedDevice()->ReleaseFont(iFbsFont);
		}
	}

EXPORT_C CGraphicsDevice* CPdrGc::Device() const
	{
	return iPdrDevice;
	}

EXPORT_C void CPdrGc::SetOrigin(const TPoint& aPos)
	{
	if (!PdrControl()->IsGraphicsBand())
		iOrigin = aPos;
	TPoint pos = PdrControl()->BandedDevice()->FullOriginToBandOrigin(aPos);
	iBandedGc->SetOrigin(pos);
	}

EXPORT_C void CPdrGc::SetDrawMode(TDrawMode aDrawingMode)
	{
	iBandedGc->SetDrawMode(aDrawingMode);
	}

EXPORT_C void CPdrGc::SetClippingRect(const TRect& aRect)
	{
	if (!PdrControl()->IsGraphicsBand())
		{
		iClippingRect = aRect;
		iClippingRect.Move(TPoint(0, 0) + iOrigin);	//  Recorded in original coordinates
		}
	iBandedGc->SetClippingRect(aRect);
	}

EXPORT_C void CPdrGc::CancelClippingRect()
	{
	if (!PdrControl()->IsGraphicsBand())
		iClippingRect = iPdrDevice->PrintablePageInPixels();
	iBandedGc->CancelClippingRect();
	}

EXPORT_C void CPdrGc::Reset()
	{
	if (iBandedGc->IsFontUsed())
		{
		iBandedGc->DiscardFont();
		iBandedFont = NULL;
		}
	iBandedGc->Reset();
	iBandedGc->UseFont(iFbsFont);
	SetOrigin(TPoint(0, 0));
	CancelClippingRect();
	}

EXPORT_C void CPdrGc::UseFont(const CFont* aFont)
	{
	if (aFont->TypeUid() == TUid::Uid(KCInfoFontUidVal))
		{
		iFont = (CInfoFont*)aFont;
		iPrintTextUsingBitmaps = EFalse;
		// We may have to use bitmaps - so set the font up in the bitmap gc as well
		if (iBandedGc->IsFontUsed())
			iBandedGc->DiscardFont();
		iBandedFont = iFont->RealFont();
		iBandedGc->UseFont(iBandedFont);
		}
	else 
		{
		if (iBandedGc->IsFontUsed())
			{
			iBandedGc->DiscardFont();
			iBandedFont = NULL;
			}
		iBandedGc->UseFont(aFont);
		iPrintTextUsingBitmaps = ETrue;
		}
	}

EXPORT_C void CPdrGc::DiscardFont()
	{
	iFont = NULL;
	if (iBandedFont)
		{
		iBandedGc->DiscardFont();
		iBandedFont = NULL;
		iBandedGc->UseFont(iFbsFont);
		}
	}

EXPORT_C void CPdrGc::SetPenColor(const TRgb& aColor)
	{
	if (!PdrControl()->IsGraphicsBand())
		iPenColor = aColor;
	iBandedGc->SetPenColor(aColor);
	}

EXPORT_C void CPdrGc::SetPenStyle(TPenStyle aPenStyle)
	{
	iBandedGc->SetPenStyle(aPenStyle);
	}

EXPORT_C void CPdrGc::SetPenSize(const TSize& aSize)
	{
	iBandedGc->SetPenSize(aSize);
	}

EXPORT_C void CPdrGc::SetBrushColor(const TRgb& aColor)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->SetBrushColor(aColor);
	}

EXPORT_C void CPdrGc::SetBrushStyle(TBrushStyle aBrushStyle)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->SetBrushStyle(aBrushStyle);
	}

EXPORT_C void CPdrGc::SetBrushOrigin(const TPoint& aOrigin)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->SetBrushOrigin(aOrigin);
	}

EXPORT_C void CPdrGc::UseBrushPattern(const CFbsBitmap* aBitmap)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->UseBrushPattern(aBitmap);
	}

EXPORT_C void CPdrGc::DiscardBrushPattern()
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DiscardBrushPattern();
	}

EXPORT_C void CPdrGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
	{
	if (!PdrControl()->IsGraphicsBand())
		iUnderlineStyle = aUnderlineStyle;
	iBandedGc->SetUnderlineStyle(aUnderlineStyle);
	}

EXPORT_C void CPdrGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
	{
	if (!PdrControl()->IsGraphicsBand())
		iStrikethroughStyle = aStrikethroughStyle;
	iBandedGc->SetStrikethroughStyle(aStrikethroughStyle);
	}

EXPORT_C void CPdrGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
	{
	if (!PdrControl()->IsGraphicsBand())
		{
		iWordExcessWidthInPixels = aExcessWidth;
		iNumGaps = aNumGaps;
		}
	iBandedGc->SetWordJustification(aExcessWidth, aNumGaps);
	}

EXPORT_C void CPdrGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
	{
	if (!PdrControl()->IsGraphicsBand())
		{
		iCharExcessWidthInPixels = aExcessWidth;
		iNumChars = aNumChars;
		}
	iBandedGc->SetCharJustification(aExcessWidth, aNumChars);
	}

EXPORT_C void CPdrGc::MoveTo(const TPoint& aPoint)
	{
	if (!PdrControl()->IsGraphicsBand())
		iPosition = aPoint;
	iBandedGc->MoveTo(aPoint);
	}

EXPORT_C void CPdrGc::MoveBy(const TPoint& aVector)
	{
	if (!PdrControl()->IsGraphicsBand())
		iPosition += aVector;
	iBandedGc->MoveBy(aVector);
	}

EXPORT_C void CPdrGc::Plot(const TPoint& aPoint)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->Plot(aPoint);
	}

EXPORT_C void CPdrGc::DrawArc(const TRect& aRect,const TPoint& aStart,const TPoint& aEnd)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawArc(aRect, aStart, aEnd);
	}

EXPORT_C void CPdrGc::DrawLine(const TPoint& aPoint1,const TPoint& aPoint2)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawLine(aPoint1, aPoint2);
	}

EXPORT_C void CPdrGc::DrawLineTo(const TPoint& aPoint)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawLineTo(aPoint);
	}

EXPORT_C void CPdrGc::DrawLineBy(const TPoint& aVector)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawLineBy(aVector);
	}

EXPORT_C void CPdrGc::DrawPolyLine(const CArrayFix<TPoint>* aPointList)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawPolyLine(aPointList);
	}

EXPORT_C void CPdrGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawPolyLine(aPointList, aNumPoints);
	}

EXPORT_C void CPdrGc::DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawPie(aRect, aStart, aEnd);
	}

EXPORT_C void CPdrGc::DrawEllipse(const TRect& aRect)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawEllipse(aRect);
	}

EXPORT_C void CPdrGc::DrawRect(const TRect& aRect)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawRect(aRect);
	}

EXPORT_C void CPdrGc::DrawRoundRect(const TRect& aRect,const TSize& aCornerSize)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawRoundRect(aRect, aCornerSize);
	}

EXPORT_C TInt CPdrGc::DrawPolygon(const CArrayFix<TPoint>* aPointList, TFillRule aFillRule)
	{
	TInt ret = KErrNone;
	if (PdrControl()->IsGraphicsBand())
		ret = iBandedGc->DrawPolygon(aPointList, aFillRule);
	return ret;
	}

EXPORT_C TInt CPdrGc::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	TInt ret = KErrNone;
	if (PdrControl()->IsGraphicsBand())
		ret = iBandedGc->DrawPolygon(aPointList, aNumPoints, aFillRule);
	return ret;
	}

EXPORT_C void CPdrGc::DrawBitmap(const TPoint& aTopLeft, const CFbsBitmap* aSource)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawBitmap(aTopLeft, aSource);
	}

EXPORT_C void CPdrGc::DrawBitmap(const TRect& aDestRect, const CFbsBitmap* aSource)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawBitmap(aDestRect, aSource);
	}

EXPORT_C void CPdrGc::DrawBitmap(const TRect& aDestRect, const CFbsBitmap* aSource, const TRect& aSourceRect)
	{
	if (PdrControl()->IsGraphicsBand())
		iBandedGc->DrawBitmap(aDestRect, aSource, aSourceRect);
	}

EXPORT_C void CPdrGc::DrawBitmapMasked(const TRect& /*aDestRect*/,const CFbsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CFbsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{}

EXPORT_C void CPdrGc::DrawBitmapMasked(const TRect& /*aDestRect*/,const CWsBitmap* /*aBitmap*/,const TRect& /*aSourceRect*/,const CWsBitmap* /*aMaskBitmap*/,TBool /*aInvertMask*/)
	{}
		
EXPORT_C void CPdrGc::MapColors(const TRect& /*aRect*/,const TRgb* /*aColors*/,TInt /*aNumPairs*/,TBool /*aMapForwards*/)
	{
	}

EXPORT_C TInt CPdrGc::SetClippingRegion(const TRegion &/*aRegion*/)
	{	
		return KErrNone;
	}

EXPORT_C void CPdrGc::CancelClippingRegion()
	{}

EXPORT_C void CPdrGc::DrawTextVertical(const TDesC& /*aText*/,const TPoint& /*aPos*/,TBool /*aUp*/)
	{}
	
EXPORT_C void CPdrGc::DrawTextVertical(const TDesC& /*aText*/,const TRect& /*aBox*/,TInt /*aBaselineOffset*/,TBool /*aUp*/,TTextAlign /*aVert*/,TInt /*aMargin*/)
	{}

EXPORT_C TInt CPdrGc::AlphaBlendBitmaps(const TPoint& /*aDestPt*/, const CFbsBitmap* /*aSrcBmp*/, const TRect& /*aSrcRect*/, const CFbsBitmap* /*aAlphaBmp*/, const TPoint& /*aAlphaPt*/) 
	{
		return KErrNone;
	}
	
EXPORT_C TInt CPdrGc::AlphaBlendBitmaps(const TPoint& /*aDestPt*/, const CWsBitmap* /*aSrcBmp*/,  const TRect& /*aSrcRect*/, const CWsBitmap*  /*aAlphaBmp*/, const TPoint& /*aAlphaPt*/)
	{
		return KErrNone;
	}

CPdrControl* CPdrGc::PdrControl() const 
{
	return ((CPdrControl*) iPdrDevice->iControl);
}

/**
@deprecated Interface is deprecated because it is unsafe as it may leave. It is available for backward compatibility reasons only.
@see CPdrGc::DrawTextL
*/

EXPORT_C void CPdrGc::DrawText(const TDesC& aString, const TPoint& aPosition)
	{
	TRAP_IGNORE(DrawTextL(aString, aPosition));
	}

EXPORT_C void CPdrGc::DrawTextL(const TDesC& aString, const TPoint& aPosition)
	{
	if (!iPrintTextUsingBitmaps)
		{
		TInt firstCharNot = 0;
		TInt length = 0;
		__ASSERT_DEBUG(iFont, Panic(EPdrNoFontInUse));
		if (iFont->AllCharsInFontRepertoire(aString, firstCharNot, length))
			{
			iPosition = aPosition;
			TInt start = 0;
			TPtrC ptr;  // Checks to see iWordExcessWidthInPixels > 0 to avoid panic in JustificationInPixels()
			for (TInt i = 0; (i < aString.Length()) && iNumGaps && iWordExcessWidthInPixels; i++)
				{
				if (aString[i] == ' ')
					{
					ptr.Set(aString.Mid(start, i - start + 1));
					if (!PdrControl()->IsGraphicsBand())
						{
						PdrControl()->DrawTextL(iPosition + iOrigin, iUnderlineStyle, iStrikethroughStyle, iPenColor, iFont, ptr);
						}
					iPosition += TPoint(iFont->MeasureText(ptr) + JustificationInPixels(iWordExcessWidthInPixels, iNumGaps), 0);
					start = i + 1;
					}
				}
			if (start < aString.Length())
				{
				ptr.Set(aString.Mid(start));
				if (!PdrControl()->IsGraphicsBand())
					{
					PdrControl()->DrawTextL(iPosition + iOrigin, iUnderlineStyle, iStrikethroughStyle, iPenColor, iFont, ptr);
					}
				iPosition += TPoint(iFont->MeasureText(ptr), 0);
				}
			}
		else
			{
			if (firstCharNot > 0)
				{	// Take text substring and draw it
				TPtrC ptr;
				ptr.Set(aString.Mid(0, firstCharNot));
				DrawText(ptr, aPosition);
				// Whole string wasn't text so there must be some (bitmap) string left
				// Update drawing position
				TPoint position = iPosition;
				// Take rest of string and try again
				ptr.Set(aString.Mid(firstCharNot));
				DrawText(ptr, position);
				}
			else
				{	// Take bitmap substring and draw it
				TPtrC ptr;
				ptr.Set(aString.Mid(0, length));
				if (PdrControl()->IsGraphicsBand())
					{
					iPrintTextUsingBitmaps = ETrue;
					DrawText(ptr, aPosition);
					iPrintTextUsingBitmaps = EFalse;
					}
				if (length < aString.Length())
					{
					// There must be some (text) string left
					// Update drawing position
					iPosition = aPosition;
					TPtrC ptr2;
					TInt start = 0;
					// Checks to see iWordExcessWidthInPixels > 0 to avoid panic in JustificationInPixels()
					for (TInt i = 0; (i < ptr.Length()) && iNumGaps && iWordExcessWidthInPixels; i++)
						{
						if (ptr[i] == ' ')
							{
							ptr2.Set(ptr.Mid(start, i - start + 1));
							iPosition += TPoint(iFont->MeasureText(ptr2) + JustificationInPixels(iWordExcessWidthInPixels, iNumGaps), 0);
							start = i + 1;
							}
						}
					if (start < ptr.Length())
						{
						ptr2.Set(ptr.Mid(start));
						iPosition += TPoint(iFont->MeasureText(ptr2), 0);
						}
					TPoint position = iPosition;
					// Take rest of string and try again
					ptr.Set(aString.Mid(length));
					DrawText(ptr, position);
					}
				}
			}
		}
	else
		{
		if (PdrControl()->IsGraphicsBand())
			iBandedGc->DrawText(aString, aPosition);
		}
	}

EXPORT_C void CPdrGc::DrawText(const TDesC& aString, const TRect& aBox, TInt aBaselineOffset, TTextAlign aHoriz, TInt aLeftMrg)
	{
	if (!iPrintTextUsingBitmaps)
		{
		__ASSERT_DEBUG(iFont, Panic(EPdrNoFontInUse));
		if (!PdrControl()->IsGraphicsBand())
			{
			TInt width = iFont->MeasureText(aString);
			TPoint pos;
			pos.iY = aBox.iTl.iY + aBaselineOffset;
			if (aHoriz == ELeft)
				pos.iX = aBox.iTl.iX + aLeftMrg;
			else if (aHoriz == ERight)
				pos.iX = aBox.iBr.iX - aLeftMrg - width;
			else if (aHoriz == ECenter)
				pos.iX = (aBox.iTl.iX + aBox.iBr.iX - width) / 2;
			DrawText(aString, pos);
			}
		else
			iBandedGc->DrawText(KNullDesC, aBox, aBaselineOffset, aHoriz, aLeftMrg);
		}
	else
		{
		if (PdrControl()->IsGraphicsBand())
			iBandedGc->DrawText(aString, aBox, aBaselineOffset, aHoriz, aLeftMrg);
		}
	}
