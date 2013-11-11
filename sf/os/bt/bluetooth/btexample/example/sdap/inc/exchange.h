// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <bt_sock.h>
#include <btsdp.h>

class CExchanger : public CBase
/**
Abstract base class for protocols that can locate compatible devices.
@internalComponent
*/
{
	public:
		/**
		@internalComponent
		Begins a search for compatible devices.  Until successful
		completion, no other function should be called as the data
		may be invalid.
		@param aStatus
		A TRequestStatus object that is signalled when the search
		completes.  It should be used from within an active object
		*/
		virtual void LookForPeersL(TRequestStatus& aStatus) = 0;
		/**
		@internalComponent
		@return The number of compatible peer devices found
		*/
		virtual TInt Count() = 0;
		/**
		@internalComponent
		Selects the first compatible device that was found
		*/
		virtual void First() = 0;
		/**
		@internalComponent
		Retrieves the selected device, and selects the next device
		that was found.
		@param aPtr
		A reference to a TNameEntry* pointer.  On success, it is filled
		in with a pointer to a device that was found.  
		@return KErrNotFound on failure, KErrNone on success.
		*/
		virtual TInt Next(TNameEntry*& aPtr) = 0;
	protected:
		RSocketServ iSockSession;
	private:
};

class CBluetoothSeeker;
class CBluetoothExchanger : public CExchanger
/**
@internalComponent
Implementation of CExchanger for bluetooth.  Uses inquiry and the SDP 
protocol to retrieve a filtered list of all devices in range that support
the service, as defined by UUID
*/
{
	public:
		IMPORT_C static CBluetoothExchanger* NewL(const TUUID &aUUID);
		IMPORT_C static CBluetoothExchanger* NewLC(const TUUID &aUUID);
		~CBluetoothExchanger();
		//virtual implementations
		IMPORT_C virtual void LookForPeersL(TRequestStatus& aStatus);
		IMPORT_C virtual TInt Count();
		IMPORT_C virtual void First();
		IMPORT_C virtual TInt Next(TNameEntry*& aPtr);
	private:
		void ConstructL(const TUUID &aUUID);
		RSdp iSdpSession;
		RSdpDatabase iSdpDb;
		CBluetoothSeeker *iSeeker;
		TInt iDeviceIndex;
};

#endif //EXCHANGE_H
