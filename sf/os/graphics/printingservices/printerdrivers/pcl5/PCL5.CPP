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

#include "PCL5STD.H"

#include <fbs.h>
#include <banddev.h>
#include "PCL5.H"
#include "pdrtext.h"


EXPORT_C CPrinterDevice* NewPrinterDeviceL()
	{
	CPcl5Device* device = new(ELeave) CPcl5Device;
	return device;
	}

CPcl5Device::CPcl5Device()
:	CFbsDrvDevice()
	{
	__DECLARE_NAME(_S("CPcl5Device"));
	}

CPcl5Device::~CPcl5Device()
	{
	}

TInt CPcl5Device::CreateContext(CGraphicsContext*& aGC)
	{
	__ASSERT_DEBUG(iControl,Panic(EPcl5ControlDoesNotExist));
	CPdrControl* control=(CPdrControl*) iControl;
	return control->CreateContext(aGC);
	}

void CPcl5Device::CreateControlL(CPrinterPort* aPrinterPort)
	{
	__ASSERT_ALWAYS(aPrinterPort, Panic(EPcl5RequiresPrinterPort));
	__ASSERT_ALWAYS(!iControl, Panic(EPcl5ControlAlreadyExists));
	__ASSERT_DEBUG(iCurrentPageSpecInTwips.iPortraitPageSize.iWidth && iCurrentPageSpecInTwips.iPortraitPageSize.iHeight, Panic(EPcl5PageSpecNotSet));
	iControl = CPcl5Control::NewL(this, aPrinterPort, *iStore, iModelInfo->iResourcesStreamId);
	}

CPcl5Control* CPcl5Control::NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	CPcl5Control* control = new(ELeave) CPcl5Control(aPdrDevice, aPrinterPort);
	CleanupStack::PushL(control);
	control->ConstructL(aStore, aResourcesStreamId);
	CleanupStack::Pop();
	return control;
	}

CPcl5Control::~CPcl5Control()
	{
	}

CPcl5Control::CPcl5Control(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort)
:	CFbsDrvControl(aPdrDevice, aPrinterPort)
	{
	__DECLARE_NAME(_S("CPcl5Control"));
	}

CPrinterControl::TMoreOnPage CPcl5Control::QueueGetBand(TRequestStatus& aStatus, TBandAttributes& aBand)
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
			{
			moreonpage = ENoMoreOnPage;
			if (!(iPdrDevice->Flags() & EPcl5DeskjetPrinter))
				{
				CPageTextEntry* entry; //!!
				TInt numentries = iPageText->NumEntries();
				for (; (iEntryIndex < numentries); iEntryIndex++) //!!
					{	
					entry = (*iPageText)[iEntryIndex];
					OutputTextL(entry->iDrawPos, entry->iTextWidthInPixels, *(entry->iTextFormat), *(entry->iText)); //!!
					}
				}
			}
		else
			moreonpage = EMoreOnPage;
		iPageBuffer->StartFlush(aStatus);
		}
	return moreonpage;
	}

void CPcl5Control::ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	if ((iPdrDevice->CurrentPageSpecInTwips().iOrientation == TPageSpec::ELandscape)
		&& (iPdrDevice->Flags() & EPcl5LandscapeNotAvailable))
		User::Leave(KErrNotSupported);
	CFbsDrvControl::ConstructL(aStore, aResourcesStreamId);

	TRect rect = iPdrDevice->PrintablePageInPixels();
	iLandscapeMode = ((iPdrDevice->CurrentPageSpecInTwips().iOrientation == TPageSpec::ELandscape)
		&& !(iPdrDevice->Flags() & EPcl5LandscapeNotAvailable));
	TBandingDirection bandorientation =
		{
		(iLandscapeMode && (iPdrDevice->Flags() & EPcl5LandscapeBandingRightToLeft)) ?
		EBandingRightToLeft :
		EBandingTopToBottom
		};	
	TSize size;
	size.iWidth = iPdrDevice->HorizontalPixelsToTwips(1000);
	size.iHeight = iPdrDevice->VerticalPixelsToTwips(1000);
	iBandedDevice = CBandedDevice::NewL(rect, size, iPdrDevice->DisplayMode(), bandorientation);
	TInt len =
		{
		(bandorientation == EBandingTopToBottom) ?
		CFbsBitmap::ScanLineLength(iBandedDevice->BandBitmap()->SizeInPixels().iWidth, iPdrDevice->DisplayMode()) :
		CFbsBitmap::ScanLineLength(iBandedDevice->BandBitmap()->SizeInPixels().iHeight, iPdrDevice->DisplayMode())
		};
	iScanLine = HBufC8::NewL(len);
	iCompressedScanLine = HBufC8::NewL(len);
	if (!iLandscapeMode || !(iPdrDevice->Flags() & EPcl5DeskjetPrinter))
		iPageText = CPageText::NewL();
	}

void CPcl5Control::OutputBandL()
	{
	if (IsGraphicsBand())
		{
		TRect bandrect = iBandedDevice->BandRect();
		TSize size = bandrect.Size();
		TCommandString des;
		TBool datainband = EFalse;
		TInt numscanlines = size.iHeight;
		if (iBandedDevice->BandingDirection() == EBandingRightToLeft)
			numscanlines = size.iWidth;
		for (TInt i = 0; i < numscanlines; i++)
			{
			TInt x = bandrect.iTl.iX;
			TInt y = bandrect.iTl.iY + i;
			TPtr8 ptr = iScanLine->Des();
			if (iBandedDevice->BandingDirection() == EBandingTopToBottom)
				iBandedDevice->BandBitmap()->GetScanLine(ptr, TPoint(0, i), size.iWidth, iPdrDevice->DisplayMode());
			else
				{
				iBandedDevice->BandBitmap()->GetVerticalScanLine(ptr, numscanlines - (i + 1), iPdrDevice->DisplayMode());
				x = bandrect.iBr.iX - i;
				y = bandrect.iTl.iY;
				}
			if ((iPdrDevice->Flags() & EPcl5DeskjetPrinter) && !iLandscapeMode)
				{
				CPageTextEntry* entry;
				TBool textinscanline=EFalse;
				TInt numentries=iPageText->NumEntries();
				for (; (iEntryIndex < numentries) && ((entry = (*iPageText)[iEntryIndex])->iDrawPos.iY <= (y + iPageText->MaxFontHeightInPixels())); iEntryIndex++)
					{
					OutputTextL(entry->iDrawPos, entry->iTextWidthInPixels, *(entry->iTextFormat), *(entry->iText));
					textinscanline = ETrue;
					}
				if (textinscanline && datainband)
					MoveToL(TPoint(x, y));
				}
			if (TransformBuffer() && !datainband)
				{
				MoveToL(TPoint(x, y));
				if (iBandedDevice->BandingDirection() == EBandingLeftToRight)
					des.Format(iResources->ResourceString(EPdrBitmapStart), EPcl5PhysicalPageOrientation);
				else
					des.Format(iResources->ResourceString(EPdrBitmapStart), EPcl5LogicalPageOrientation);
				iPageBuffer->AddBytesL(des);
				datainband = ETrue;
				}
			if (datainband)
				{
				TCommandString buf = iResources->ResourceString(EPdrScanLine);
				if (CompressBuffer())
					{
					des.Format(buf, EPcl5RunLength, iCompressedScanLine->Des().Length());
					iPageBuffer->AddBytesL(des);
					iPageBuffer->AddBytesL(iCompressedScanLine->Des());
					}
				else
					{
					des.Format(buf, EPcl5None, iScanLine->Des().Length());
					iPageBuffer->AddBytesL(des);
					iPageBuffer->AddBytesL(iScanLine->Des());
					}
				}
			}
		if (datainband)
			iPageBuffer->AddBytesL(iResources->ResourceString(EPdrBitmapEnd));
		}
	}

void CPcl5Control::SetPageSizeL()
	{
	TCommandString des;  //  Need more paper sizes
	TSize size = iPdrDevice->CurrentPageSpecInTwips().iPortraitPageSize;
	if (size == KA4PaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5A4);
	else if (size == KLegalPaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5Legal);
	else if (size == KExecutivePaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5Executive);
	else if (size == KLetterPaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5Letter);
	else if (size == KCom_10PaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5Com_10);
	else if (size == KMonarchPaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5Monarch);
	else if (size == KDLPaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5DL);
	else if (size == KC5PaperSizeInTwips)
		des.Format(iResources->ResourceString(EPdrSetPageSize), EPcl5C5);
	iPageBuffer->AddBytesL(des);
	}

void CPcl5Control::SetPageOrientationL()
	{
	if (iLandscapeMode)
		CommandL(EPdrLandscape);
	else
		CommandL(EPdrPortrait);
	}

void CPcl5Control::SetTextColorL(const TRgb& aColor)
	{
	TCommandString des;
	TInt color = aColor.Gray2();
	if (iPdrDevice->Flags() & EPcl5DeskjetPrinter)
		color = !color;
	des.Format(iResources->ResourceString(EPdrSetTextColor), color);
	iPageBuffer->AddBytesL(des);
	}
