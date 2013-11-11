// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
*/

#ifndef BLUETOOTH_DUT_MODE_H
#define BLUETOOTH_DUT_MODE_H

#include <e32base.h>

class RHCIServerSession;

_LIT(KBluetoothDutModePanic, "DutModeClient");

NONSHARABLE_CLASS(RBluetoothDutMode)
/**
API used to control the Bluetooth Device Under Test Mode

@publishedPartner
@released
*/
	{
public:
	enum TBluetoothDutModePanic
		{
		EApiUsedWhenHandleIsNotOpen = 0,
		EHandleIsAlreadyOpen = 1,
		};

	enum TBluetoothDutModeFunctionIds
		{
		EHCIActivateDutMode = 0,
		EHCIDeactivateDutMode = 1,
		};

public:
	IMPORT_C RBluetoothDutMode();

	IMPORT_C TInt Open();
	IMPORT_C void Close();

	// Device Under Test methods
	IMPORT_C TInt ActivateDutMode() const;
	IMPORT_C TInt DeactivateDutMode() const;

private:
	// private copy c'tor to prevent shallow copy of iHCIServerSession
	RBluetoothDutMode(const RBluetoothDutMode&);
		
private:
	RHCIServerSession* iHCIServerSession;

	TUint32 iReserved1; // Padding for possible future "per-copy" state.
	};

#endif // BLUETOOTH_DUT_MODE_H
