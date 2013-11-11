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

#ifndef CANON_H
#define CANON_H

#include <pdrstore.h>

/**
@internalComponent
*/
const TInt KCanonNumScanLinesPerBand = 24;
const TInt KCanonBytesPerDotColumn = 3;

class CBandedDevice;

/**
@internalComponent
*/
IMPORT_C CPrinterDevice* NewPrinterDeviceL();

/**
@internalComponent
*/
enum TCanonFlags
	{
	ECanonLandscapeNotAvailable = 1
	};

class CCanonDevice : public CFbsDrvDevice
/**
@internalComponent
*/
	{
public:
	CCanonDevice();
	~CCanonDevice();
	TInt CreateContext(CGraphicsContext*& aGC);
	void CreateControlL(CPrinterPort* aPrinterPort);
	};

class CCanonControl : public CFbsDrvControl
/**
@internalComponent
*/
	{
public:
	static CCanonControl* NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId);
	~CCanonControl();
	// print control functions
protected:
	CCanonControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort);
 	void ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId);

	void OutputBandL();
	void MoveToL(const TPoint& aPoint);
	void MoveByL(const TPoint& aPoint);
	TBool TransformBuffer();

protected:
	TBuf8<KCanonNumScanLinesPerBand >> 3> iScanLine;
	};

#endif
