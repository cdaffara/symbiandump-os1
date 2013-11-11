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
// Constants for L2CAP
// 
//

#ifndef L2CONSTANTS_H
#define L2CONSTANTS_H

#include <e32def.h>
#include <e32base.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/hciconsts.h>

const TInt KL2CAPIncomingConnQueueSize=1;
const TUid KUidServiceL2CAP={0x100095FB};

//
// L2CAP level constants
//
GLREF_C const TInt KHCIIncomingBufferCount;	// in linkconsts.h
	
const static TUint16 KL2CapSignallingCID 		= 0x0001;
const static TUint16 KL2CapConnectionlessCID 	= 0x0002;
const static TUint16 KL2CapDynamicCIDStart 		= 0x0040;
const static TUint16 KL2CapDynamicCIDEnd 		= 0xffff;

const static TUint16 KL2CapDynamicPSMStart 		= KMinUserPSM;

const static TInt KL2ConfigRequestDelayTimout 	= 100000; // In MicroSeconds
const static TInt KL2ConfigWatchdogTimeout 		= 120; // In Seconds

const static TInt KL2ProtocolIdleTimeout 		= 2; 	// Seconds before the whole protocol dies
const static TInt KL2CAPSockets 				= 100;

const static TInt KL2ProtocolSecondTimerMultiplier = 1000000;
const static TInt KL2MuxerIdleTimeout   		= 20;	// Seconds before Mux disconnects

const static TUint16 KInfiniteFlush 			= 0xffff;
const static TUint16 KL2CapMaxMTUSize			= 0xffff;
const static TUint16 KL2CapDefaultMTUSize		= 672;
const static TUint16 KL2CapPreferredMTUSize		= 5120;

const static TUint16 KConfigOptionContinuationMask	= 0x0001; 	// c-bit

const static TUint8 KOptionTypeIsHintMask  			= 0x80; 	// hint bit

const static TInt KL2CapTxSeqValues = 64;	// the number of values that Tx Seq can take (valid values: 0-63)

// The feature bits are (NS - not supported in our implementation):
// 	Flow Control Mode	0x0001
// 	Retransmission Mode	0x0002
//	Bi-di QoS			0x0004 (NS)
// 	Enhanced RTX Mode	0x0008
// 	Streaming Mode		0x0010
//  FCS Option			0x0020 (NS)
static const TUint8 KL2CAPExtendedFeaturesSupported = 0x01 | 0x02 | 0x08 | 0x10;

static const TUint8 KL2CAPExtendedHeaderSizeIncrease = 0x04;

static const TUint16 KBBPacketSize[] = {17,27,54,83,121,183,224,339,367,552,679,1021};
static const TUint16 KBaseBandPacketCount = sizeof(KBBPacketSize)/sizeof(KBBPacketSize[0]);

#endif
