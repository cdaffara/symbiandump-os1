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

#ifndef PCL5_H
#define PCL5_H

#include <pdrstore.h>

class CBandedDevice;

/**
@internalComponent
*/
enum TPcl5PaperSize
	{
	EPcl5Executive = 1,
	EPcl5Letter = 2,
	EPcl5Legal = 3,
	EPcl5Ledger = 6,
	EPcl5A4 = 26,
	EPcl5A3 = 27,
	EPcl5Monarch = 80,
	EPcl5Com_10 = 81,
	EPcl5DL = 90,
	EPcl5C5 = 91
	};

/**
@internalComponent
*/
enum TPcl5Encoding
	{
	EPcl5None = 0,
	EPcl5RunLength = 1,
	EPcl5TIFF = 2,
	EPcl5DeltaRow = 3,
	EPcl5Reserved = 4,
	EPcl5Adaptive = 5
	};

/**
@internalComponent
*/
enum TPcl5Flags
	{
	EPcl5DeskjetPrinter = 1,
	EPcl5LandscapeNotAvailable = 2,
	EPcl5LandscapeBandingRightToLeft = 4
	};

/**
@internalComponent
*/
enum TPcl5RasterGraphicsMode
	{
	EPcl5LogicalPageOrientation=0,
	EPcl5PhysicalPageOrientation=3
	};

/**
@internalComponent
*/
IMPORT_C CPrinterDevice* NewPrinterDeviceL();

class CPcl5Device : public CFbsDrvDevice
/**
@internalComponent
*/
	{
public:
	CPcl5Device();
	~CPcl5Device();
	TInt CreateContext(CGraphicsContext*& aGC);
	void CreateControlL(CPrinterPort* aPrinterPort);
	};

class CPcl5Control : public CFbsDrvControl
/**
@internalComponent
*/
	{
public:
	static CPcl5Control* NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId);
	~CPcl5Control();
	TMoreOnPage QueueGetBand(TRequestStatus& aStatus, TBandAttributes& aBand);
	// print control functions
protected:
	CPcl5Control(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort);
	void ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId);

	void OutputBandL();

	void SetPageSizeL(); 
	void SetPageOrientationL();
	void SetTextColorL(const TRgb& aColor);

protected:
	TBool iLandscapeMode;
	};

#endif
