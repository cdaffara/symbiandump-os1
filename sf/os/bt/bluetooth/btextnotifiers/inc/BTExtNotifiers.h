// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTEXTNOTIFIERS_H
#define BTEXTNOTIFIERS_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <btextnotifierspartner.h>
#endif

#include <btextnotifiersconsts.h>

#include <e32std.h>
#include <bttypes.h>
#include <btdevice.h>

#define NOTIFIERS_SUPPORT_PASSKEY_MIN_LENGTH

/**
@file
@publishedAll
@released
**/

NONSHARABLE_CLASS(TBTDeviceSelectionParams)
/**
@publishedAll
@released

Class to allow parameters to be sent to the device selection dialog via the RNotifier API.
**/
	{
public:
	IMPORT_C TBTDeviceSelectionParams();
	IMPORT_C void SetUUID(const TUUID& aUUID);
	IMPORT_C void SetDeviceClass(TBTDeviceClass aClass);
	IMPORT_C const TUUID& UUID();
	IMPORT_C TBTDeviceClass DeviceClass();
	IMPORT_C TBool IsValidDeviceClass();
	IMPORT_C TBool IsValidUUID();
private:
	TBTDeviceClass iDeviceClass; /*!< The device class */
	TUUID iSdpUuid; /*!< The sdp uuid */
	TBool iValidDeviceClass; /*!< ETrue if iDeviceClass has been set, EFalse if not */
	TBool iValidUuid; /*!< ETrue if iSdpUuid has be set, EFalse if not */

	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

NONSHARABLE_CLASS(TBTDeviceResponseParams)
/**
@publishedAll
@released

Class to allow parameters to be received from the device selection dialog via the RNotifier API.
**/
	{
public:
	IMPORT_C TBTDeviceResponseParams();
	IMPORT_C void SetDeviceAddress(const TBTDevAddr& aBDAddr);	//< Set the address of the device
	IMPORT_C void SetDeviceName(const TDesC& aName);	//< Set the name of the device
	IMPORT_C void SetDeviceClass(TBTDeviceClass aClass);
	IMPORT_C const TBTDevAddr& BDAddr() const;				//< Returns the address of the device
	IMPORT_C const TDesC& DeviceName() const;		//< Returns the name of the device
	IMPORT_C TBTDeviceClass DeviceClass();
	IMPORT_C TBool IsValidBDAddr() const;			//< Has the address of the device been set?
	IMPORT_C TBool IsValidDeviceName() const;		//< Has the name of the device been set?
	IMPORT_C TBool IsValidDeviceClass();
private:
	TBTDevAddr iBDAddr; /*!< The BT address of the remote device */
	TBuf<KMaxBluetoothNameLen> iDeviceName; /*!< The name given by the remote device */
	TBTDeviceClass iDeviceClass; /*!< The device class */
	TBool iValidBDAddr; /*!< ETrue if iBDAddr has been set, EFalse if not */
	TBool iValidDeviceName; /*!< ETrue if iDeviceName has been set, EFalse if not */
	TBool iValidDeviceClass; /*!< ETrue if iDeviceClass has been set, EFalse if not */
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used	
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

/**
@publishedAll
@released

Typedef'ed pckgbuf to send paramaters to the device selection dialog via the notifier framework.
**/
typedef TPckgBuf<TBTDeviceSelectionParams> TBTDeviceSelectionParamsPckg;

/**
@publishedAll
@released

Typedef'ed pckgbuf to retrieve the response from the device selection dialog via the notifier framework.
**/
typedef TPckgBuf<TBTDeviceResponseParams> TBTDeviceResponseParamsPckg;

/**
A remote device address, and a boolean indicating if the remote device has access
to the uplink
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
@publishedPartner
@released
*/
NONSHARABLE_CLASS(TPanConnection)
	{
public:
	IMPORT_C TPanConnection(TBTDevAddr& aRemoteDeviceAddress,
				   			TBool aUplinkAccessAllowed);
				   
	IMPORT_C const TBTDevAddr& RemoteDeviceAddress() const;
	IMPORT_C TBool UplinkAccessAllowed() const;
	IMPORT_C TBool IsValid() const;
	
	// Allows creation of default NULL instance
	IMPORT_C TPanConnection();
	
private:
	TBTDevAddr iRemoteDeviceAddress;
	TBool iUplinkAccessAllowed;

	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32 iPadding1; 
	TUint32 iPadding2; 	
	};

/**
A list of TPanConnection instances
This is not intended for external use by application developers.

@see TPanConnection
@publishedAll
@released
*/
NONSHARABLE_CLASS(TPanConnectionList)
	{
public:
	IMPORT_C TPanConnectionList();
	
	IMPORT_C void AddRemoteConnectionL(const TPanConnection& aRemoteConnection);
	IMPORT_C const TPanConnection* GetFirstRemoteConnection();
	IMPORT_C const TPanConnection* GetNextRemoteConnection();

	IMPORT_C void SetRequestedConnectionAddr(const TBTDevAddr& aRequestedConnectionAddr);
	IMPORT_C const TBTDevAddr* RequestedConnectionAddr();

	IMPORT_C const TUUID& CurrentLocalRole() const;
	IMPORT_C void SetCurrentLocalRole(const TUUID& aLocalRole);
		
	IMPORT_C TUint8 MaxNumberOfRemoteConnections();
	
private:
	const static TUint8 KMaxRemoteConnections = 7;

private:
	TUint8 iPosition;	// the position within the list for stateful reading
	TFixedArray<TPanConnection, KMaxRemoteConnections> iRemoteConnections;

	TBTDevAddr iRequestedConnectionAddr;

	TUUID iCurrentLocalRole;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32 iPadding1; 
	TUint32 iPadding2; 	
	};

/**
@publishedAll
@released

Typedef'ed pckgbuf for TPanConnectionList.
This is not intended for external use by application developers.
**/
typedef TPckgBuf<TPanConnectionList> TPanConnectionListPckg;


/**
@publishedAll
@released

The response from the incoming PAN NAP connection selection dialog.
This is intended for internal sub-system use only.
**/
enum TNapConnectionResult
	{
	EDisallowNewNapConnection,
	EAcceptNapConnectionAllowUplinkAccess,
	EAcceptNapConnectionDisallowUplinkAccess,
	};

/**
@publishedAll
@released

Typedef'ed pckgbuf to retrieve the response from the incoming PAN NAP connection 
selection dialog via the notifier framework.
This is intended for internal sub-system use only.
**/
typedef TPckgBuf<TNapConnectionResult> TNapConnectionResultPckg;		

#endif // BTEXTNOTIFIERS_H
