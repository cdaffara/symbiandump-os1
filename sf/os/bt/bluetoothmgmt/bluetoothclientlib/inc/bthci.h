// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// HCI Direct Access Types
// 
//

#ifndef BTHCI_H
#define BTHCI_H

#include <btsocketoptionlevels.h>
#include <es_sock.h>

NONSHARABLE_CLASS(RBluetoothPowerSwitch)
/** This is intended for controlling whether the Bluetooth hardware is switched on or not.

@publishedPartner
@deprecated
@see HCI_v2 documentation
*/
	{
	public:

	IMPORT_C RBluetoothPowerSwitch();
	IMPORT_C void RequestSwitchOn();
	IMPORT_C void RequestSwitchOff();
	};


class RHCIServerSession;
class RSocketBasedHciDirectAccess;

NONSHARABLE_CLASS(RHCIDirectAccess)
/**
API used for licensee-specific direct HCI access

This class allows vendor-specific messages to be passed through to the HCI for
customised (licensee-specific) HCI functionality.

Note: No use of this class should be required by default. It is provided to
assist with hardware workarounds, or link policy not implemented by the
Bluetooth stack.

Do not use unless entirely familar with this API and the specific HCI in use!!!!

@publishedPartner
@released
*/
	{
public:
	IMPORT_C RHCIDirectAccess();
	//API used for licensee-specific direct HCI access
	IMPORT_C TInt Open();
	IMPORT_C TInt Open(RSocketServ& aSocketServ);
	IMPORT_C void Close();

	IMPORT_C void Ioctl(TUint aCommand, TRequestStatus &aStatus, TDes8* aDesc=NULL,TUint aLevel = KSolBtHCI); 
	IMPORT_C void CancelIoctl();

	IMPORT_C void AsyncMessage(TUint aCommand, TRequestStatus &aStatus, TDes8* aDesc); 
	IMPORT_C void CancelAsyncMessage();

	IMPORT_C TInt SubSessionHandle();
private:
	RHCIServerSession* iHCIServerSession;
	RSocketBasedHciDirectAccess* iSocketAccess;
	TUint32 iReserved; // Padding for possible future "per-copy" state.
	};

#endif // BTHCI_H