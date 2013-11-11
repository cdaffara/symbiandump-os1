// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Publish and Subscribe keys and categories.
// Allows clients to observe various events published by the Bluetooth subsystem,
// and for clients to influence some parameters in the Bluetooth subsystem
// 
//


#ifndef BT_SUBSCRIBE_H
#define BT_SUBSCRIBE_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bt_subscribe_partner.h>
#endif

#include <bt_subscribe_keybase.h>

#include <e32property.h>

// P&S Categories
 
/**
KPropertyUidBluetoothCategory
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.

@publishedAll
@deprecated
*/
const TUid KPropertyUidBluetoothCategory = {KUidSystemCategoryValue};

/**
KPropertyUidBluetoothControlCategory
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.

@publishedAll
@deprecated
*/
const TUid KPropertyUidBluetoothControlCategory = {KUidSystemCategoryValue};

// P&S Keys

//-------------

/**
KPropertyKeyBluetoothGetLocalDeviceAddress
The key to observe the device address of the local Bluetooth hardware
The P&S value will contain a descriptor of the device address
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetLocalDeviceAddress = (KUidBluetoothPubSubKeyBase + 0);

/**
KPropertyKeyBluetoothLocalDeviceAddress
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.
@publishedAll
@deprecated
*/
const TUint KPropertyKeyBluetoothLocalDeviceAddress = KPropertyKeyBluetoothGetLocalDeviceAddress;

//-------------

/**
KPropertyKeyBluetoothGetPHYCount
The key to observe the the number of Bluetooth physical links attached to the local device
The P&S value will contain the number of physical links
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetPHYCount = (KUidBluetoothPubSubKeyBase + 1);

/**
KPropertyKeyBluetoothPHYCount
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.
@publishedAll
@deprecated
*/
const TUint KPropertyKeyBluetoothPHYCount = KPropertyKeyBluetoothGetPHYCount;

//-------------

/**
KPropertyKeyBluetoothGetConnectingStatus
The key to observe whether the local device is paging another device (i.e. is connecting a physical link)
The P&S value will contain a boolean: ETrue if connecting, otherwise EFalse
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetConnectingStatus = (KUidBluetoothPubSubKeyBase + 2);

/**
KPropertyKeyBluetoothConnecting
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.
@publishedAll
@deprecated
*/
const TUint KPropertyKeyBluetoothConnecting = KPropertyKeyBluetoothGetConnectingStatus;

//-------------

/**
KPropertyKeyBluetoothGetScanningStatus
The key to observe what scans the Bluetooth hardware is currently performing
The P&S value contains an integer with the scan value as published in the Bluetooth Core Specification

The use of this key is equivalent in a secure Symbian OS platform to the use of the key
KPropertyKeyBluetoothScanning with the category KPropertyUidBluetoothCategory in a non-secure
Symbian OS platform.

@see Bluetooth Core Specification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetScanningStatus = (KUidBluetoothPubSubKeyBase + 3);

/**
KPropertyKeyBluetoothSetScanningStatus
The key to control what scans the Bluetooth hardware is currently performing
The P&S value contains an integer with the scan value as published in the Bluetooth Core Specification

The use of this key is equivalent in a secure Symbian OS platform to the use of the key
KPropertyKeyBluetoothScanning with the category KPropertyUidBluetoothControlCategory in a non-secure
Symbian OS platform.

@see Bluetooth Core Specification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/
const TUint KPropertyKeyBluetoothSetScanningStatus = (KUidBluetoothPubSubKeyBase + 4);

//-------------

/**
KPropertyKeyBluetoothGetLimitedDiscoverableStatus
The key to observe whether the local device is in Limited Discoverable mode
The P&S value will contain a boolean: ETrue if in limited discoverable mode, otherwise EFalse

The use of this key is equivalent in a secure Symbian OS platform to the use of the key
KPropertyKeyBluetoothLimitedDiscoverable with the category KPropertyUidBluetoothCategory in a non-secure
Symbian OS platform.

@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetLimitedDiscoverableStatus = (KUidBluetoothPubSubKeyBase + 5);

/**
KPropertyKeyBluetoothSetLimitedDiscoverableStatus
The key to control whether the local device is in Limited Discoverable mode
The P&S value will contain a boolean: ETrue if in limited discoverable mode, otherwise EFalse

The use of this key is equivalent in a secure Symbian OS platform to the use of the key
KPropertyKeyBluetoothLimitedDiscoverable with the category KPropertyUidBluetoothControlCategory in a non-secure
Symbian OS platform.

@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/

const TUint KPropertyKeyBluetoothSetLimitedDiscoverableStatus = (KUidBluetoothPubSubKeyBase + 6);


//-------------

/**
KPropertyKeyBluetoothGetDeviceClass
The key to observe the class of device of the local device
The P&S value will contain an integer of the local device class.

The use of this key is equivalent in a secure Symbian OS platform to the use of the key
KPropertyKeyBluetoothDeviceClass with the category KPropertyUidBluetoothCategory in a non-secure
Symbian OS platform.

@see Bluetooth Core Specification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetDeviceClass = (KUidBluetoothPubSubKeyBase + 7);

/**
KPropertyKeyBluetoothSetDeviceClass
The key to set the class of device of the local device
The P&S value will contain an integer of the local device class.

The use of this key is equivalent in a secure Symbian OS platform to the use of the key
KPropertyKeyBluetoothDeviceClass with the category KPropertyUidBluetoothControlCategory in a non-secure
Symbian OS platform.

@see Bluetooth Core Specification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/
const TUint KPropertyKeyBluetoothSetDeviceClass = (KUidBluetoothPubSubKeyBase + 8);

//-------------

/**
KPropertyKeyBluetoothSetAFHHostChannelClassification
The key to send an AFH Host Channel Classification to the local Bluetooth hardware
The P&S value will contain a descriptor containing the AFH Host Channel Classification 
bit set. A bit which is set to zero instructs the local Bluetooth hardware 
not to hop to the frequency represented by that bit in a connection in which 
it is master and which is using AFH. Also, if the local Bluetooth hardware is slave in a 
connection, which is using AFH, an AFH Host Channel Classification may be used to advise
the remote master what frequencies to avoid.

NB. THIS IS SUPPORTED ONLY IN VERSION 9.0 AND THEREAFTER. The same functionality can be
accessed in 8.1 also through the use of the KPropertyKeyBluetoothAFHHostChannelClassification
key in conjunction with category KPropertyUidBluetoothControlCategory.

@see TBTAFHHostChannelClassification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/
const TUint KPropertyKeyBluetoothSetAFHHostChannelClassification = (KUidBluetoothPubSubKeyBase + 9);

/**
KPropertyKeyBluetoothAFHHostChannelClassification
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.

@publishedAll
@deprecated
*/
const TUint KPropertyKeyBluetoothAFHHostChannelClassification = KPropertyKeyBluetoothSetAFHHostChannelClassification;

//-------------

/**
KPropertyKeyBluetoothGetRegistryTableChange
The key to observe changes in the Bluetooth Registry
The P&S value will contain an integer describing which of the tables in the Bluetooth Registry changed
see further in this header file.
This property is written to only by the Symbian OS Bluetooth sub-system.
@publishedAll
@released
@capability LocalServices	Needed for read access to this property
*/
const TUint KPropertyKeyBluetoothGetRegistryTableChange = (KUidBluetoothPubSubKeyBase + 11);

/**
KPropertyKeyBluetoothRegistryTableChange
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.

@publishedAll
@deprecated
*/
const TUint KPropertyKeyBluetoothRegistryTableChange = KPropertyKeyBluetoothGetRegistryTableChange;

//-------------

// Some keys have values that are "wellknown" - these are enumerated here
// RegistryTableChanges

//-------------

/**
KRegistryChangeRemoteTable
@see KPropertyKeyBluetoothGetRegistryTableChange
The value in the KPropertyKeyBluetoothGetRegistryTableChange key if the Remote device table changed
@publishedAll
@released
*/
const TUint KRegistryChangeRemoteTable = (KUidBluetoothPubSubKeyBase + 12);

//-------------

/**
KRegistryChangeLocalTable
@see KPropertyKeyBluetoothGetRegistryTableChange
The value in the KPropertyKeyBluetoothGetRegistryTableChange key if the Local device table changed
@publishedAll
@released
*/
const TUint KRegistryChangeLocalTable = (KUidBluetoothPubSubKeyBase + 13);

//-------------

/**
KRegistryChangeCSYTable
@see KPropertyKeyBluetoothGetRegistryTableChange
The value in the KPropertyKeyBluetoothGetRegistryTableChange key if the CSY-settings table changed
@publishedAll
@released
*/
const TUint KRegistryChangeCSYTable = (KUidBluetoothPubSubKeyBase + 14);

//-------------

/**
KPropertyKeyBluetoothGetDeviceName
The key to observe the name of device of the local device
The P&S value will be of type EText and contain the local device name in Unicode.
At all times the name must have a length <= KHCILocalDeviceNameMaxLength (defined in hciconsts.h).
@see Bluetooth Core Specification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetDeviceName = (KUidBluetoothPubSubKeyBase + 15);

/**
KPropertyKeyBluetoothSetDeviceName
The key to set the name of device of the local device
The P&S value will be of type EText and contain the local device name in Unicode.
At all times the name must have a length <= KHCILocalDeviceNameMaxLength (defined in hciconsts.h).
@see Bluetooth Core Specification
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/
const TUint KPropertyKeyBluetoothSetDeviceName = (KUidBluetoothPubSubKeyBase + 16);

//-------------

/**
KPropertyKeyBluetoothGetCorruptRegistryResetIndication
This key is intended to be used by the UI to be informed when a corrupt 
Bluetooth registry has been detected so that the UI can inform the user
that all of the pairings they have made have been lost.

The P&S value will either be 0 or give the tick count, as given by 
User::TickCount(), at the time which the corrupt Bluetooth Registry has 
been restored or replaced. If the key is 0 this means that the Bluetooth 
registry has not been found to be corrupt since the device was turned on.
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetCorruptRegistryResetIndication = (KUidBluetoothPubSubKeyBase + 17);

/**
KPropertyKeyBluetoothCorruptRegistryReset
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.

@publishedAll
@deprecated
*/
const TUint KPropertyKeyBluetoothCorruptRegistryReset = KPropertyKeyBluetoothGetCorruptRegistryResetIndication;
 
//-------------

/**
KPropertyKeyBluetoothGetAcceptPairedOnlyMode
This key is intended to be used by the UI to discover whether a device will accept all connections or only paired ones.
The P&S value will contain a boolean: ETrue - the stack will only to accept connection requests from paired devices.
EFalse - the stack will accept connection requests from both paired and unpaired devices.
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetAcceptPairedOnlyMode = (KUidBluetoothPubSubKeyBase + 18);

/**
KPropertyKeyBluetoothSetAcceptPairedOnlyMode
The key to set the mode for handling connection requests
The P&S value will contain a boolean: ETrue - the user wishes ONLY to accept connection requests from paired devices.
EFalse - the user wishes to accept connection requests from both paired and unpaired devices.
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/
const TUint KPropertyKeyBluetoothSetAcceptPairedOnlyMode = (KUidBluetoothPubSubKeyBase + 19);

/**
KPropertyKeyBluetoothHostResolverActive
The key is intended to be used by the UI to discover whether the device is attempting to do a discovery
If the device is attempting a discovery activities such as streaming of AV data will be affected
The P&S value will contain a boolean: ETrue - the stack is attempting a discovery/discovery+rnr
EFalse - the stack is curently not attempting a discovery
@publishedAll
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothHostResolverActive = (KUidBluetoothPubSubKeyBase + 20); 


/**
KPropertyKeyBluetoothGetSimplePairingDebugMode
The key is intended to be used by a UI to provide an indication to the user of the device that the
Symbian simple pairing debug mode has been enabled.
As this mode should only be being used by engineers, and provides a potential security risk for
end users the UI should make obvious to an end-user that they have entered this mode and what it
means for them.
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothGetSimplePairingDebugMode = (KUidBluetoothPubSubKeyBase + 22);


#endif //BT_SUBSCRIBE_H
