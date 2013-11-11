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
// Constants for the AVCTP protocol
// 
//

/**
 @internalComponent
 @file
*/

#ifndef __AVCTPCONSTANTS_H__
#define __AVCTPCONSTANTS_H__

#include <e32std.h>
#include <bluetoothav.h>

/*
@internalTechnology
*/
namespace SymbianAvctp
{

//
// AVCTP Bluetooth Security Defaults
//

const static TBluetoothMitmProtection KInboundAuthenticationDefault = EMitmDesired;
const static TBool KInboundAuthoristationDefault = ETrue;
const static TBool KInboundEncryptionDefault     = ETrue;

const static TBluetoothMitmProtection KOutboundAuthenticationDefault = KInboundAuthenticationDefault;
const static TBool KOutboundAuthoristationDefault = KInboundAuthoristationDefault;
const static TBool KOutboundEncryptionDefault     = KInboundEncryptionDefault;

const static TBluetoothMitmProtection KSecondaryChannelAuthenticationDefault = EMitmNotRequired;
const static TBool KSecondaryChannelAuthoristationDefault = EFalse;
//
// AVCTP protocol constants
//

const static TInt KTransportIdleTimeout		= 5 * 1000000; // i.e. 5 seconds
const static TInt KProtocolIdleTimeout	= 5 * 1000000; // i.e. 5 seconds
/** Total outstanding packets before a sap flows off */
const static TInt KSapOutboundQHighMark		= 10;
/** Point where it will flow back on again */
const static TInt KSapOutboundQLowMark		= 8;
const static TInt KProtocolListeningQueueSize = 5;
const static TUid KAvctpServiceUid = { 0x100095FD };
const static TUint KMaximumIpidResponsesAllowed = 10;

} // end of namespace SymbianAvctp

#endif // __AVCTPCONSTANTS_H__

