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

#ifndef GENERAL_H
#define GENERAL_H

#include <pdrstore.h>

/**
@internalComponent
*/
IMPORT_C CPrinterDevice* NewPrinterDeviceL();

/**
@internalComponent
*/
enum TGeneralFlags
	{
	EGeneralLandscapeNotAvailable = 1
	};

class CGeneralPrinterDevice : public CPdrDevice
/**
@internalComponent
*/
	{
public:
	CGeneralPrinterDevice();
	~CGeneralPrinterDevice();
    TInt CreateContext(CGraphicsContext*& aGC);
	void CreateControlL(CPrinterPort* aPrinterPort);
protected:
 	TSize KPixelSizeInTwips() const;
	};

class CGeneralPrinterControl : public CPdrControl
/**
@internalComponent
*/
	{
public:
	static CGeneralPrinterControl* NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId);
	~CGeneralPrinterControl();

protected:
	CGeneralPrinterControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort);
 	void ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId);

	void OutputTextL(const TPoint& aPoint, TInt aWidthInPixels, const TTextFormat& aTextFormat, const TDesC8& aString); 
	void OutputBandL();
	}; 

#endif
