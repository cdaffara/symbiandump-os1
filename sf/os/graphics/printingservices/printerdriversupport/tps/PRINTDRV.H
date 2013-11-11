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

#ifndef PRINTDRV_H
#define PRINTDRV_H

#include <pdrstore.h>
#include <pdrport.h>

IMPORT_C CPrinterDevice* NewPrinterDeviceL();

class CPrintDrvDevice : public CPdrDevice
	{
public:
	CPrintDrvDevice();
	~CPrintDrvDevice();
	TInt CreateContext(CGraphicsContext*& aGc);
	void CreateControlL(CPrinterPort* aPrinterPort);
	};

class CPrintDrvControl : public CPdrControl
	{
public:
	static CPrintDrvControl* NewL(CPdrDevice* aPdrDevice,CPrinterPort* aPrinterPort,CStreamStore& aStore,TStreamId aResourcesStreamId);
	~CPrintDrvControl();

protected:
 	CPrintDrvControl(CPdrDevice* aPdrDevice,CPrinterPort* aPrinterPort);
 	void ConstructL(CStreamStore& aStore,TStreamId aResourcesStreamId);

	void OutputTextL(const TPoint& aPoint,TInt aWidthInPixels,const TTextFormat& aTextFormat,const TDesC8& aString); 
	void OutputBandL();

private:
	HBufC8* iScanLine;
	}; 

#endif
