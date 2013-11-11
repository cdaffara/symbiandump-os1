// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef HCTLBCSPCONSTS_H
#define HCTLBCSPCONSTS_H


/**BCSP Channel allocation*/
const TUint8 KBcspAckChnl=0x00;				// channel 0
const TUint8 KBcspLinkChnl=0x01;			// channel 1 Link Establishment
const TUint8 KBcspBcCmdChnl=0x02;			// channel 2
const TUint8 KBcspCommEvntChnl=0x05;		// channel 5
const TUint8 KBcspACLDataChnl=0x06;			// channel 6
const TUint8 KBcspSynchronousDataChnl=0x07;	// channel 7

//FlagByte Constants
const TUint8 KBcspCrcPresentMask = 0x40;
const TUint8 KBcspProtocolTypeMask = 0x80;

const TUint8 KBcspSeqMask = 0x07;
const TUint8 KBcspAckFieldOffset = 3;
const TUint8 KBcspAckMask = 0x38;

const TUint8 KBcspPayloadLen1stBitsMask=0xf0;
const TUint8 KBcspProtocolIdMask=0x0f;	

/**HCTL Header and trailer consts*/
const TUint8 KBcspHeaderFlagsByteIndex = 0;
const TUint8 KBcspHeaderByte2Index = 1;
const TUint8 KBcspHeaderByte3Index = 2;
const TUint8 KBcspHeaderChecksumByteIndex = 3;

const TUint8 KBcspHeaderBytes = 4;
const TUint8 KBcspCrcBytes = 2;

/**Defined max payload sizes of various packet types*/
const TUint16 KMaxSCOData = 64;
const TUint16 KMaxHCICommand = 256;
const TUint16 KMaxACLData = 1021;

const TUint16 KMaxReliablePayloadSize=KMaxACLData+5;
const TUint16 KMaxUnreliablePayloadSize=KMaxSCOData+4;

/**Defined max windows size i.e. no. packets that can be transmitted without acknowledgement */
const TUint8  KBcspReliableWindowSize = 4;
const TUint8  KBcspUnreliableQueueSize = 4;
const TUint8  KBcspWindowSize=4;      // Maximum Window Size for BCSP in Casiras by default
const TUint8  KMaxBcspWindowSize=8;   // Window Size for BCSP in Casiras by default
const TUint16 KHCIHeaderSize=4;


/** Flags field masks */

const TInt KBcspUnreliableDatagramQueue=0;
const TInt KBcspReliableSequenceQueue=1;

/**Sequence layer constants */
const TUint8 KWindowSize=4;				// Define according to efficiency and performance
const TUint8 KMaxWindowSize=8;
const TInt KRxAckTimeout=250000;			// Some number to tune/adjust for performance
const TInt KTxAckTimeout=125000;			// Some number to tune/adjust for performance
const TUint8 KTxRetryLimit=20;				// Some number to tune/adjust for performance

/**Link Establishment layer constants */
_LIT8(KBcspLinkMsg_Sync,     "\xda\xdc\xed\xed");
_LIT8(KBcspLinkMsg_SyncResp, "\xac\xaf\xef\xee");
_LIT8(KBcspLinkMsg_Conf,     "\xad\xef\xac\xed");
_LIT8(KBcspLinkMsg_ConfResp, "\xde\xad\xd0\xd0");
const TInt KBcspConfCntMax=50;
const TInt KTShyTimeout = 250000;
const TInt KTConfTimeout = 250000;


/**SLIP Encoding Bytes */
const TUint8 KSlipWrapperByte = 0xc0;	
const TUint8 KSlipByteDB = 0xdb;	
const TUint8 KSlipByteDC = 0xdc;	
const TUint8 KSlipByteDD = 0xdd;	

/**BCSP Framing errors*/

const TInt KErrSlipCorrupted=-6100;
const TInt KErrBcspHeaderCorrupt=-6101;
const TInt KErrBcspCorruptedHCIPayload=-6102;
const TInt KErrBcspCRCCheckFailed=-6103;
const TInt KErrBcspUnRecognizableHCIData=-6104;
const TInt KErrBcspPacketTypeUnrecognized=-6105;
const TInt KErrBcspInvalidAckFlagValue=-6106;
const TInt KErrBcspInvalidSeqFlagValue=-6107;
const TInt KErrBcspWriteACLDataFailed=-6108;
const TInt KErrBcspWriteCommandDataFailed=-6109;
const TInt KErrBcspNothingToSend = -6110;
const TInt KErrBcspMaxRetries = -6111;
const TInt KErrBcspWriteBcCmdDataFailed=-6112;

const TUint KHCTLRecvBufSize=2048;

#endif // HCTLBCSPCONSTS_H
