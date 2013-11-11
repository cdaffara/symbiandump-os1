// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _LINKCONSTS_H_
#define _LINKCONSTS_H_

#include <bt_sock.h>
#include <bluetooth/hci/basebandpacketconsts.h>

static const TInt KLinkMgrIncomingBufferCountPerLink = 8;

// Timers
//Timeout for idle timer - used for interval between protocol being ready to close, 
//and it actually closing
const TInt KLinkMgrProtocolIdleTimer = 2*1000000;
//Minimum interval allowed between successive AFHHostChannelClassification commands
const TInt KAFHHostChannelClassificationIntervalTimer = 1000000;
//Timeout to prevent further AFHHostChannelClassification commands 
//being blocked forever if no command complete is received.
const TInt KAFHHostChannelClassificationCommandCompleteTimer = 30000000;

// The number of buffers which we'd like to have for buffering data coming 
// from the L2CAP to HCI for flow control.
const TUint KHCIPreferedNumberOfHCIACLDataBuffers=8;

// In the HCI we have to account for all the packets pending on each connection
// so that we can update our data packet credits accordingly when disconnection
// or flush occures. The reason why this is 7 is because we do not need more for
// point to point Bluetooth (and we need the memory :-).
#ifdef PROXY_COMMUNICATES
const TUint KHCIMaxNumberOfACLConnectionRecords=8;	// one for broadcast
#else
const TUint KHCIMaxNumberOfACLConnectionRecords=7;
#endif

// Maximum number of connection records that the HCI will keep in order to check
// on disconnection whether an ACO or ACL connection was dropped.
// The maximum number of record is equal to the max number of ACL record x 3 
// since the standard allows for three SCO channels per baseband link
const TUint KHCIMaxNumberOfConnectionRecords=KHCIMaxNumberOfACLConnectionRecords*3;

// For a UART HCTL implementation of the HCI, we need to be able to buffer
// HCI Data buffers of at least 255+1+4 bytes, since the HC must guarantee to  
// be able to accept/buffer data packets of 255 bytes (EXCLUDING the headers).

// CSR ignore this bit of the spec, so we may just bin these anyway.

const TUint16 KHCIHCTLUartACLHeaderSize=1;
const TUint16 KHCIACLPacketHeaderSize=4;
const TUint16 KHCIACLMinDataBufferSize=255;
const TUint16 KHCIACLMinPacketBufferSize=
	KHCIHCTLUartACLHeaderSize+KHCIACLPacketHeaderSize+KHCIACLMinDataBufferSize;
const TUint KBluetoothMinHCBufs = 5;


//
// Physical Link Config
//

const TUint16 KStackACLBuffersSize = 1021; // size of 3DH-5 (largest packet size)
const TUint16 KStackACLBuffersNum = 16;
const TUint16 KStackACLBuffersTideMarkNum = 8;

#ifdef STACK_SCO_DATA
const TUint8 KStackSCOBuffersSize = 255;
const TUint16 KStackSCOBuffersNum = 1;
#else
const TUint8 KStackSCOBuffersSize = 0;
const TUint16 KStackSCOBuffersNum = 0;
#endif

const TUint16 KLinkMgrIncomingBufferHeaderSize = 1; // header is just the ACL flags
const TUint16 KLinkMgrIncomingBufferSize = KStackACLBuffersSize + KLinkMgrIncomingBufferHeaderSize;

static const THCIScanEnable KHCIDefaultScanMode =  EInquiryAndPageScan;

static const TUint16 KHCIDefaultAcceptTimeout = 0x1FA0;  // time=n*0.625ms
static const TUint16 KHCIDefaultPageTimeout = 0x8000;
static const TUint16 KHCIMaxSlotsOnePacketType = KDM1Packet | 
												 KDH1Packet;
static const TUint16 KHCIMaxSlotsThreePacketType = KDM1Packet |
												   KDH1Packet |
												   KDM3Packet |
												   KDH3Packet;
static const TUint16 KHCIMaxSlotsFivePacketType = KDM1Packet |
												  KDH1Packet |
												  KDM3Packet |
												  KDH3Packet |
												  KDM5Packet |
												  KDH5Packet;
static const TUint16 KHCIDefaultPacketType = KHCIMaxSlotsFivePacketType;
static const TUint16 KHCIDefaultSCOPacketType = KHV1Packet |
												KHV2Packet |
												KHV3Packet;
static const TInt KPhysicalLinkIdleTimeout = 5;
static const TInt KMaxPhysicalLinkIdleTimeout = 60;
#endif
