// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "PDRVSTD.H"

#include <fbs.h>
#include <banddev.h>
#include "PRINTDRV.H"
#include "pdrtext.h"


EXPORT_C CPrinterDevice* NewPrinterDeviceL()
	{
	CPrintDrvDevice* device = new(ELeave) CPrintDrvDevice;
	return device;
	}

CPrintDrvDevice::CPrintDrvDevice()
	{
	__DECLARE_NAME( _S( "CPrintDrvDevice" ) );
	}

CPrintDrvDevice::~CPrintDrvDevice()
	{
	}

TInt CPrintDrvDevice::CreateContext( CGraphicsContext*& aGc )
	{
	__ASSERT_DEBUG( iControl, Panic( EPrintDrvControlDoesNotExist ) );
	CPdrControl* control = (CPdrControl*) iControl;
	return control->CreateContext( aGc );
	}

void CPrintDrvDevice::CreateControlL(CPrinterPort* aPrinterPort)
	{
	__ASSERT_ALWAYS( aPrinterPort, Panic( EPrintDrvRequiresPrinterPort ) );
	__ASSERT_ALWAYS( !iControl, Panic( EPrintDrvControlAlreadyExists ) );
	__ASSERT_DEBUG( iCurrentPageSpecInTwips.iPortraitPageSize.iWidth && iCurrentPageSpecInTwips.iPortraitPageSize.iHeight, Panic( EPrintDrvPageSpecNotSet ) );
	iControl = CPrintDrvControl::NewL( this, aPrinterPort, *iStore, iModelInfo->iResourcesStreamId );
	}

CPrintDrvControl* CPrintDrvControl::NewL( CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId )
	{
	CPrintDrvControl* control = new(ELeave) CPrintDrvControl( aPdrDevice, aPrinterPort );
	CleanupStack::PushL( control );
	control->ConstructL( aStore, aResourcesStreamId );
	CleanupStack::Pop();
	return control;
	}

CPrintDrvControl::~CPrintDrvControl()
	{
	delete iScanLine;
	}

CPrintDrvControl::CPrintDrvControl( CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort ):
	CPdrControl( aPdrDevice, aPrinterPort )
	{
	}

void CPrintDrvControl::ConstructL( CStreamStore& aStore, TStreamId aResourcesStreamId )
	{
	CPdrControl::ConstructL( aStore, aResourcesStreamId );
	TRect rect = iPdrDevice->PrintablePageInPixels();
	TSize size;
	size.iWidth = iPdrDevice->HorizontalPixelsToTwips( 1000 );
	size.iHeight = iPdrDevice->VerticalPixelsToTwips( 1000 );
	iBandedDevice = CBandedDevice::NewL( rect, size, iPdrDevice->DisplayMode(), EBandingTopToBottom );
	iScanLine = HBufC8::NewL( CFbsBitmap::ScanLineLength( iBandedDevice->BandBitmap()->SizeInPixels().iWidth, iPdrDevice->DisplayMode() ) );
	}

void CPrintDrvControl::OutputTextL( const TPoint& aPoint, TInt aWidthInPixels, const TTextFormat& aTextFormat, const TDesC8& aString )
	{
	CPdrControl::OutputTextL( aPoint, aWidthInPixels, aTextFormat, aString ); 
	CommandL( EPdrResource1 );
	}

void CPrintDrvControl::OutputBandL()
	{
	TRect bandrect = iBandedDevice->BandRect();
	TSize size = bandrect.Size();
	CommandL( EPdrBitmapStart );
	for ( TInt i = 0; i<size.iHeight; i++ )
		{
		CommandL( EPdrScanLine );
//		TPtr8 ptr = iScanLine->Des();
//		iBandedDevice->BandBitmap()->GetScanLine( ptr, TPoint( 0, i ), size.iWidth, iPdrDevice->DisplayMode() );
//		iPageBuffer->AddBytesL( ptr );
		CommandL( EPdrEndScanLine );
		}
	CommandL( EPdrBitmapEnd );
	}

