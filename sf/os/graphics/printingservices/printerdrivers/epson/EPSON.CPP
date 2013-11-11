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


#include "EPSNSTD.H"

#include <banddev.h>
#include <fbs.h>
#include "EPSON.H"
#include "pdrtext.h"



EXPORT_C CPrinterDevice* NewPrinterDeviceL()
	{
	CEpsonDevice* device = new(ELeave) CEpsonDevice;
	return device;
	}

CEpsonDevice::CEpsonDevice():
	CFbsDrvDevice()
	{
	__DECLARE_NAME(_S("CEpsonDevice"));
	}

CEpsonDevice::~CEpsonDevice()
	{
	}

void CEpsonDevice::CreateControlL(CPrinterPort* aPrinterPort)
	{
	__ASSERT_ALWAYS(aPrinterPort, Panic(EEpsonRequiresPrinterPort));
	__ASSERT_ALWAYS(!iControl, Panic(EEpsonControlAlreadyExists));
	__ASSERT_DEBUG(iCurrentPageSpecInTwips.iPortraitPageSize.iWidth && iCurrentPageSpecInTwips.iPortraitPageSize.iHeight, Panic(EEpsonPageSpecNotSet));
	iControl = CEpsonControl::NewL(this, aPrinterPort, *iStore, iModelInfo->iResourcesStreamId);
	}

CEpsonControl* CEpsonControl::NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	CEpsonControl* control=new(ELeave) CEpsonControl(aPdrDevice, aPrinterPort);
	CleanupStack::PushL(control);
	control->ConstructL(aStore, aResourcesStreamId);
	CleanupStack::Pop();
	return control;
	}

CEpsonControl::~CEpsonControl()
	{
	}

void CEpsonControl::ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	if((iPdrDevice->CurrentPageSpecInTwips().iOrientation == TPageSpec::ELandscape)
		&& (iPdrDevice->Flags() & EEpsonLandscapeNotAvailable))
		User::Leave(KErrNotSupported);
	CFbsDrvControl::ConstructL(aStore, aResourcesStreamId);
	TRect rect = iPdrDevice->PrintablePageInPixels();
	TSize size;
	size.iWidth = iPdrDevice->HorizontalPixelsToTwips(1000);
	size.iHeight = iPdrDevice->VerticalPixelsToTwips(1000);
	iBandedDevice = CBandedDevice::NewL(rect, size, iPdrDevice->DisplayMode(), EBandingTopToBottom, KEpsonNumScanLinesPerBand);
	iPageText = CPageText::NewL();
	}

void CEpsonControl::OutputBandL() 	
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
				des.Format(iResources->ResourceString(EPdrBitmapStart), size.iWidth - i);
				iPageBuffer->AddBytesL(des);
				datainband = ETrue;
				}
			if(datainband)
				iPageBuffer->AddBytesL(iScanLine);
			}
		if(datainband)
			{
			iPageBuffer->AddBytesL(iResources->ResourceString(EPdrBitmapEnd));
			iPosition.iX = iPdrDevice->OffsetInPixels().iX;
			}
		
		TInt numentries = iPageText->NumEntries();
		if(numentries)
			{
			CPageTextEntry* entry;
			for(TInt y = bandrect.iTl.iY; y <= bandrect.iBr.iY; y++)
				{
				for(TInt index = 0; (index < numentries); index++)
					{
					entry = (*iPageText)[index];
					TPoint drawPos = entry->iDrawPos - TPoint(0, 20);	// bodge to align pdr fonts with true types
					if(drawPos.iY == y)
						OutputTextL(drawPos, entry->iTextWidthInPixels, *(entry->iTextFormat), *(entry->iText));										 //!!
					}
				}
			}
		}
	}

void CEpsonControl::MoveToL(const TPoint& aPoint) 
	{
	TPoint vector = aPoint - iPosition;
	for(; vector.iY > 255; )
		{
		MoveByL(TPoint(0, 255));
		vector = aPoint - iPosition;
		}
	MoveByL(vector);		
	}

TBool CEpsonControl::TransformBuffer()
	{
	TUint8* pStart = (TUint8*)iScanLine.Ptr();
	TUint8* pEnd = pStart + (KEpsonNumScanLinesPerBand >> 3);
	TUint8* p;
	for(p = pStart; (p < pEnd) && (*p == 0xFF); p++)
		{
		}
	TBool datainscanline = (p < pEnd);
	for(p = pStart; p<pEnd; p++)
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

CEpsonControl::CEpsonControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort):
	CFbsDrvControl(aPdrDevice, aPrinterPort),
	iScanLine()
	{
	__DECLARE_NAME(_S("CEpsonControl"));
	}

