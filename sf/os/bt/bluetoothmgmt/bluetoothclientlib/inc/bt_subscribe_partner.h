// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef BT_SUBSCRIBE_PARTNER_H
#define BT_SUBSCRIBE_PARTNER_H

#include <bt_subscribe_keybase.h>

/**
KPropertyKeyBluetoothSetAFHChannelAssessmentMode
The key to send the Bluetooth AFH Channel Assessment Mode to the local Bluetooth hardware.
If AFH Channel Assessment Mode is on, then the local hardware will keep checking what 
channels are busy and so should be avoided in a connection using AFH.
The P&S value will contain a boolean: ETrue if AFH Channel Assessment Mode is required, 
EFalse if not.
The hardware is likely to support this either never, in master role only, 
or in both master and slave roles.
The default setting of AFH Channel Assessment Mode in the hardware is 'on' 
whenever the hardware can support it.
@publishedPartner
@released
@capability LocalServices	Needed for both read and write access to this property
@capability NetworkControl	Needed for both read and write access to this property
*/
const TUint KPropertyKeyBluetoothSetAFHChannelAssessmentMode = (KUidBluetoothPubSubKeyBase + 10);

/**
KPropertyKeyBluetoothAFHChannelAssessmentMode
This is a deprecated constant, maintained for source compatibility with non-secured 
Symbian OS platforms.

@publishedPartner
@deprecated
*/
const TUint KPropertyKeyBluetoothAFHChannelAssessmentMode = KPropertyKeyBluetoothSetAFHChannelAssessmentMode;

/**
KPropertyKeyBluetoothSetSimplePairingDebugMode
The key is intended to be used by a platform to set the stack into simple pairing debug mode.
Once enabled (by setting to ETrue) then it cannot be disabled, the mode will remain set until
the Bluetooth stack has been restarted.
When the Symbian simple pairing debug mode is enabled then the stack will instruct the hardware
to generate debug link keys for use with air traffic capture.
When the Symbian simple pairing debug mode is disabled a remote device attempting to make a 
connection with a debug link key will be rejected for secure reasons. 
@publishedPartner
@released
@capability LocalServices	Needed for both read and write access to this property
@capability CommDD			Needed for write access to this property
*/
const TUint KPropertyKeyBluetoothSetSimplePairingDebugMode = (KUidBluetoothPubSubKeyBase + 21);

/** +-   
KPropertyKeyBluetoothSetPageScanParameters     
The key to control the parameters of the page scan Bluetooth hardware is currently performing.     
The P&S value will contain an integer from TPageScanParameterSettings enumeration.     
The values are mapped to actual parameters in the Bluetooth stack.     
@see TPageScanParameterSettings     
@publishedPartner     
@released     
@capability LocalServices   Needed for both read and write access to this property     
@capability NetworkControl  Needed for both read and write access to this property     
*/     
const TUint KPropertyKeyBluetoothSetPageScanParameters = (KUidBluetoothPubSubKeyBase + 23);     
      
/**     
KPropertyKeyBluetoothGetPageScanParameters     
The key is intended to be used by the Bluetooth stack to provide an indication to the client     
which has requested a page scan parameters change that the parameters have been enabled.     
The value returned is of the TPageScanParameterSettings type.     
The enumeration may gain additional members in future.     
@see TPageScanParameterSettings     
@publishedPartner     
@released     
@capability LocalServices   Needed for both read and write access to this property     
@capability NetworkControl  Needed for both read and write access to this property     
*/     
const TUint KPropertyKeyBluetoothGetPageScanParameters = (KUidBluetoothPubSubKeyBase + 24);     
  

#endif //BT_SUBSCRIBE_PARTNER_H