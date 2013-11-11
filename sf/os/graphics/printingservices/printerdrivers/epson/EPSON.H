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

#ifndef EPSON_H
#define EPSON_H

#include <pdrstore.h>

/**
@internalComponent
*/
const TInt KEpsonNumScanLinesPerBand = 24;

/**
@internalComponent
*/
IMPORT_C CPrinterDevice* NewPrinterDeviceL();

/**
@internalComponent
*/
enum TEpsonFlags
	{
	EEpsonLandscapeNotAvailable = 1
	};

class CEpsonDevice : public CFbsDrvDevice
/**
@internalComponent
*/
	{
public:
	CEpsonDevice();
	~CEpsonDevice();
	void CreateControlL(CPrinterPort* aPrinterPort);
	};

class CEpsonControl : public CFbsDrvControl
/**
@internalComponent
*/
	{
public:
	static CEpsonControl* NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId);
	~CEpsonControl();

	// print control functions

protected:
	CEpsonControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort);
	void ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId);

	void OutputBandL();
	void MoveToL(const TPoint& aPoint); 
	TBool TransformBuffer();

protected:
	TBuf8<KEpsonNumScanLinesPerBand >> 3> iScanLine;
	};

#endif
