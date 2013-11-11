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
#include "GNRLSTD.H"
#include "GENERAL.H"
#include "pdrtext.h"


EXPORT_C CPrinterDevice* NewPrinterDeviceL()
	{
	CGeneralPrinterDevice* device = new(ELeave) CGeneralPrinterDevice;
	return device;
	}

CGeneralPrinterDevice::CGeneralPrinterDevice():
CPdrDevice()
	{
	__DECLARE_NAME(_S("CGeneralDevice"));
	}

CGeneralPrinterDevice::~CGeneralPrinterDevice()
	{
	}

TInt CGeneralPrinterDevice::CreateContext(CGraphicsContext*& aGC)
	{
	__ASSERT_DEBUG(iControl, Panic(EGeneralPrinterControlDoesNotExist));
	CPdrControl* control = (CPdrControl*)iControl;
	return control->CreateContext(aGC);
	}

TSize CGeneralPrinterDevice::KPixelSizeInTwips() const
	{
	return TSize(iModelInfo->iKPixelWidthInTwips, iModelInfo->iKPixelHeightInTwips);
	}

void CGeneralPrinterDevice::CreateControlL(CPrinterPort* aPrinterPort)
	{
	__ASSERT_ALWAYS(aPrinterPort, Panic(EGeneralPrinterRequiresPrinterPort));
	__ASSERT_ALWAYS(!iControl, Panic(EGeneralPrinterControlAlreadyExists));
	__ASSERT_DEBUG(iCurrentPageSpecInTwips.iPortraitPageSize.iWidth && iCurrentPageSpecInTwips.iPortraitPageSize.iHeight, Panic(EGeneralPrinterPageSpecNotSet));
	iControl = CGeneralPrinterControl::NewL(this, aPrinterPort, *iStore, iModelInfo->iResourcesStreamId);
	}

CGeneralPrinterControl* CGeneralPrinterControl::NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	CGeneralPrinterControl* control = new(ELeave) CGeneralPrinterControl(aPdrDevice, aPrinterPort);
	CleanupStack::PushL(control);
	control->ConstructL(aStore, aResourcesStreamId);
	CleanupStack::Pop();
	return control;
	}

CGeneralPrinterControl::~CGeneralPrinterControl()
	{
	}

CGeneralPrinterControl::CGeneralPrinterControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort):
	CPdrControl(aPdrDevice, aPrinterPort)
	{
	__DECLARE_NAME(_S("CGeneralPrinterControl"));
	}

void CGeneralPrinterControl::ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	if((iPdrDevice->CurrentPageSpecInTwips().iOrientation == TPageSpec::ELandscape)
		&& (iPdrDevice->Flags() & EGeneralLandscapeNotAvailable))
		User::Leave(KErrNotSupported);
	CPdrControl::ConstructL(aStore, aResourcesStreamId);

	const TRect rect = iPdrDevice->PrintablePageInPixels();
	const TSize size(0,0);
	iBandedDevice = CBandedDevice::NewL(rect, size, iPdrDevice->DisplayMode(), EBandingTopToBottom, 0);

	iPageText = CPageText::NewL();
	}

void CGeneralPrinterControl::OutputTextL(const TPoint& aPoint, TInt aWidthInPixels, const TTextFormat& /*aTextFormat*/, const TDesC8& aString)
	{
	MoveByL(aPoint - iPosition);
	iPageBuffer->AddBytesL(aString);
	iPosition.iX += aWidthInPixels;
	}

void CGeneralPrinterControl::OutputBandL() 	
	{
	TInt numentries = iPageText->NumEntries();
	iPosition = iPdrDevice->OffsetInPixels();
	for(TInt i = 0; i < numentries; i++)	 
		{
		CPageTextEntry* entry = (*iPageText)[i];																						 //!!
		OutputTextL(entry->iDrawPos, entry->iTextWidthInPixels, *entry->iTextFormat, entry->iText->Des());
		}																										 //!!
	iPageText->Reset();
	}

