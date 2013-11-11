// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#ifndef PAIRING_H
#define PAIRING_H

#include <e32std.h>
#include <bt_sock.h>
#include <bttypespartner.h>

/**
The UID indicating the dedicated bonding service request.
*/
const TUid KBluetoothDedicatedBondingUid = {0x10286491};

/**
Represents a connection to the pairing server inside the Bluetooth
stack.  This provides the conduit for other pairing related actitivies.
*/
NONSHARABLE_CLASS(RBluetoothPairingServer)
	: private RSessionBase
	{
public:
	IMPORT_C RBluetoothPairingServer();
	IMPORT_C TVersion Version() const;
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	
public:
	RSessionBase& Session();
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32		iPadding1;
	TUint32		iPadding2;
	};

/**
This class provides the facilities to initiate a dedicated bonding request.
*/
NONSHARABLE_CLASS(RBluetoothDedicatedBondingInitiator)
	: private RSubSessionBase
	{
public:
	IMPORT_C RBluetoothDedicatedBondingInitiator();
	IMPORT_C void Start(RBluetoothPairingServer& aPairingServer, const TBTDevAddr& aAddr, TRequestStatus& aStatus);
	IMPORT_C void Close();
	
private:
	TPckgBuf<TBTDevAddr>	iDedicatedBondAddress;
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32		iPadding1;
	TUint32		iPadding2;
	};

/**
This class provides facilities related to Bluetooth Out-Of-Band pairing.
It provides functions for retrieving local OOB data that is to be sent
to remote devices, and also facilities for providing remote OOB data that 
has been received.
*/
NONSHARABLE_CLASS(RBluetoothOobData)
	: private RSubSessionBase
	{
public:
	IMPORT_C RBluetoothOobData();
	IMPORT_C TInt Open(RBluetoothPairingServer& aPairingServer);
	IMPORT_C void Close();
	
	// Local OOB Functions
	IMPORT_C void RefreshLocalOobData();
	IMPORT_C void ReadLocalOobData(TBluetoothSimplePairingHash& aHash, TBluetoothSimplePairingRandomizer& aRandomizer, TRequestStatus& aStatus);
	IMPORT_C void CancelReadLocalOobData();
	
	// Remote OOB Functions
	IMPORT_C TInt ProvideRemoteOobData(const TBTDevAddr& aAddr, const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer);
	IMPORT_C TInt ProvideRemoteOobData(const TBTDevAddr& aAddr, const TBluetoothSimplePairingHash& aHash);
	IMPORT_C TInt ProvideRemoteOobData(const TDesC8& aData);
	IMPORT_C void ClearRemoteOobData(const TBTDevAddr& aAddr);

private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32		iPadding1;
	TUint32		iPadding2;
	};

/**
Class used to retrieve results of Bluetooth simple pairing attempts.
The pairing result session is used to obtain results of simple pairing attempts 
using the Numeric Comparison or Passkey Entry association models.
*/
NONSHARABLE_CLASS(RBluetoothPairingResult)
	: private RSubSessionBase
	{
public:
	IMPORT_C RBluetoothPairingResult();
	
	IMPORT_C TInt Open(RBluetoothPairingServer& aPairingServer);
	IMPORT_C void Close();
	
	IMPORT_C void SimplePairingResult(TBTDevAddr& aAddr, TRequestStatus& aStatus);
	IMPORT_C void CancelSimplePairingResult();
	
private:
	TPtr8			iDeviceAddressBuffer;
	TRequestStatus*	iRequest;
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32		iPadding1;
	TUint32		iPadding2;
	};

/**
Class used to retrieve results of local Bluetooth authentication attempts.
The authentication result session is used to obtain results of authentication complete events, 
which are generated for both simple and legacy pairing attempts.
*/
NONSHARABLE_CLASS(RBluetoothAuthenticationResult)
	: private RSubSessionBase
	{
public:
	IMPORT_C RBluetoothAuthenticationResult();

	IMPORT_C TInt Open(RBluetoothPairingServer& aPairingServer);
	IMPORT_C void Close();

	IMPORT_C void AuthenticationResult(TBTDevAddr& aAddr, TRequestStatus& aStatus);
	IMPORT_C void CancelAuthenticationResult();

private:
	TPtr8			iDeviceAddressBuffer;
	TRequestStatus*	iRequest;
	// This data padding has been added to help prevent future binary compatibility breaks
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32		iPadding1;
	TUint32		iPadding2;
	};

/**
Pairing Server client panic category.
*/
_LIT(KPairingServerPanicCat, "PairingSrv");

/**
Pairing Server client panic reasons
*/
enum TPairingServerPanicReason
	{
	EPairingServerBadDescriptor,
	EPairingServerBadSubSessionHandle,
	EPairingServerDedicatedBondAlreadyIssued,
	EPairingServerReadLocalOobDataOutstanding,
	EPairingServerSessionAlreadyConnected,
	EPairingServerSessionNotConnected,
	EPairingServerOobDataSessionAlreadyOpen,
	EPairingServerOobDataSessionNotOpen,
	EPairingServerDedicatedBondAlreadyInProgress,
	EPairingServerSimplePairingResultSessionAlreadyOpen,
	EPairingServerSimplePairingResultRequestOutstanding,
	EPairingServerAuthenticationResultSessionAlreadyOpen,
	EPairingServerAuthenticationResultRequestOutstanding,
	};


#endif // PAIRING_H
