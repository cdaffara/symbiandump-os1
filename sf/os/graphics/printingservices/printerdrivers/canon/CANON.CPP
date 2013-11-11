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

#include "CANONSTD.H"

#include <banddev.h>
#include <fbs.h>
#include "CANON.H"
#include "pdrtext.h"


EXPORT_C CPrinterDevice* NewPrinterDeviceL()
	{
	CCanonDevice* device = new(ELeave) CCanonDevice;
	return device;
	}

CCanonDevice::CCanonDevice():
	CFbsDrvDevice()
	{
	__DECLARE_NAME(_S("CCanonDevice"));
	}

CCanonDevice::~CCanonDevice()
	{
	}

TInt CCanonDevice::CreateContext(CGraphicsContext*& aGC)
	{
	__ASSERT_DEBUG(iControl, Panic(ECanonControlDoesNotExist));
	CPdrControl* control = (CPdrControl*)iControl;
	return control->CreateContext(aGC);
	}

void CCanonDevice::CreateControlL(CPrinterPort* aPrinterPort)
	{
	__ASSERT_ALWAYS(aPrinterPort, Panic(ECanonRequiresPrinterPort));
	__ASSERT_ALWAYS(!iControl, Panic(ECanonControlAlreadyExists));
	__ASSERT_DEBUG(iCurrentPageSpecInTwips.iPortraitPageSize.iWidth && iCurrentPageSpecInTwips.iPortraitPageSize.iHeight, Panic(ECanonPageSpecNotSet));
	iControl = CCanonControl::NewL(this, aPrinterPort, *iStore, iModelInfo->iResourcesStreamId);
	}

CCanonControl* CCanonControl::NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	CCanonControl* control = new(ELeave) CCanonControl(aPdrDevice, aPrinterPort);
	CleanupStack::PushL(control);
	control->ConstructL(aStore, aResourcesStreamId);
	CleanupStack::Pop();
	return control;
	}

CCanonControl::~CCanonControl()
	{
	}

void CCanonControl::ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	if((iPdrDevice->CurrentPageSpecInTwips().iOrientation == TPageSpec::ELandscape)
		&& (iPdrDevice->Flags() & ECanonLandscapeNotAvailable))
		User::Leave(KErrNotSupported);
	CFbsDrvControl::ConstructL(aStore, aResourcesStreamId);

	TRect rect = iPdrDevice->PrintablePageInPixels();
	TSize size;
	size.iWidth = iPdrDevice->HorizontalPixelsToTwips(1000);
	size.iHeight = iPdrDevice->VerticalPixelsToTwips(1000);
	iBandedDevice = CBandedDevice::NewL(rect, size, iPdrDevice->DisplayMode(), EBandingTopToBottom, KCanonNumScanLinesPerBand);
	TInt len = CFbsBitmap::ScanLineLength(iBandedDevice->BandBitmap()->SizeInPixels().iWidth, iPdrDevice->DisplayMode());
	CFbsDrvControl::iScanLine = HBufC8::NewL(len);
	CFbsDrvControl::iCompressedScanLine = HBufC8::NewL(len);
	iPageText = CPageText::NewL();
	}

void CCanonControl::OutputBandL() 	
	{
	if(IsGraphicsBand())
		{
		TRect bandrect = iBandedDevice->BandRect();
		TSize size = bandrect.Size();
		TCommandString des;
		TBool datainband = EFalse;
		for(TInt i = 0; i < size.iWidth; i++)
			{
			iBandedDevice->BandBitmap()->GetVerticalScanLine(iScanLine, i, iPdrDevice->DisplayMode());
			if(TransformBuffer() && !datainband)
				{
				MoveToL(bandrect.iTl + TPoint(i, 0));
				des.Format(iResources->ResourceString(EPdrBitmapStart), KCanonBytesPerDotColumn * (size.iWidth - i));
				iPageBuffer->AddBytesL(des);
				datainband = ETrue;
				}
			if(datainband)
				iPageBuffer->AddBytesL(iScanLine);
			}
		iPageBuffer->AddBytesL(iResources->ResourceString(EPdrBitmapEnd));

		iPosition.iX = iPdrDevice->OffsetInPixels().iX;
		TInt numentries = iPageText->NumEntries();
		if(numentries)
			{
			CPageTextEntry* entry;
			for(TInt y = bandrect.iTl.iY; y <= bandrect.iBr.iY; y++)
				{
				for(TInt index = 0; (index < numentries); index++)
					{
					entry = (*iPageText)[index];
					TPoint drawPos = entry->iDrawPos - TPoint(0, 16);	// bodge to align pdr fonts with true types
					if(drawPos.iY == y)
						OutputTextL(drawPos, entry->iTextWidthInPixels, *(entry->iTextFormat), *(entry->iText));
					}
				}
			}
		}
	}

void CCanonControl::MoveToL(const TPoint& aPoint) 
	{
	TPoint vector = aPoint - iPosition;
	TInt maxNoOfYIncrements = 256 * 5 / 6; // Ensures no more than 256
	for(; vector.iY >= maxNoOfYIncrements; )
		{
		MoveByL(TPoint(0, maxNoOfYIncrements));
		vector = aPoint - iPosition;
		}
	MoveByL(vector);		
	iPosition = aPoint;
	}

void CCanonControl::MoveByL(const TPoint& aPoint)
	{
	TPoint vector = aPoint;
	if(vector.iX < 0)
		{
		CommandL(EPdrCarriageReturn);
		vector.iX += iPosition.iX - iPdrDevice->OffsetInPixels().iX;
		}
	vector.iX = vector.iX * 2 / 3; // X increments are in 120ths of an inch
	vector.iY = vector.iY * 6 / 5; // Y increments are in 216ths of an inch
	TPoint oldPosition = iPosition;
	CFbsDrvControl::MoveByL(vector);
	iPosition = oldPosition + aPoint; // Correct the position
	}

TBool CCanonControl::TransformBuffer()
	{
	TUint8* pStart = (TUint8*)iScanLine.Ptr();
	TUint8* pEnd = pStart + (KCanonNumScanLinesPerBand >> 3);
	TUint8* p;
	for(p = pStart; (p < pEnd) && (*p == 0xFF); p++)
		{
		}
	TBool datainscanline = (p < pEnd);
	for(p = pStart; p < pEnd; p++)
		{
		TInt byte1 = *p;
		TInt byte2 = 0;
		for(TInt j = 0; j < 8; j++)
			{
			byte2 = byte2 << 1;
			byte2 = byte2 | (byte1 & 1);
			byte1 = byte1 >> 1;
			}
		*p = (TUint8)~byte2;
		}
	return datainscanline;	// returns ETrue if there are non-blank bytes in scanline
  }

CCanonControl::CCanonControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort):
	CFbsDrvControl(aPdrDevice, aPrinterPort)
	{
	__DECLARE_NAME(_S("CCanonControl"));
	}

