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
// Constants for the RFCOMM protocol
// 
//

#ifndef RFCOMMCONSTS_H
#define RFCOMMCONSTS_H

const TInt KRFCOMMPSM = 0x03;  // The PSM on which RFCOMM resides
const TUid KUidServiceRfcomm={0x100095FC};
const TUint KRFCOMMIncomingConnQueueSize=5;

const static TInt KRfcommMuxDisconnectingChannelTimeout = 1*1000000; // microseconds
const static TInt KRfcommIdleTimeout = 5*1000000;  

// This mux idle timeout value needs to take into account authentication
// occurring before the first DLC (not the muxer) is created hence 60 seconds
const static TInt KRfcommMuxIdleTimeoutOpening = 60*1000000;  

// This mux idle timeout value is used when the last DLC (not the muxer) has
// been closed
const static TInt KRfcommMuxIdleTimeoutClosing = 5*1000000;  
const static TInt KRfcommMuxConnectingChannelTimeout = 60*1000000;

const static TInt KMinRfcommDLCI = 2;
const static TInt KMaxRfcommDLCI = 0x3f; //0b111111
const static TInt KMaxRfcommPriority = 0x3f; //0b111111

// Frame layout

//	Used when decoding incoming frames
const static TInt KShortFrameHeaderLength = 3; // addr, ctrl, len
const static TInt KLongFrameHeaderLength = 4; // addr, ctrl , 2*len

//	Used when encoding outgoing frames
const static TInt KMaxFrameHeaderLength = 4; // addr, ctrl , 2*len
const static TInt KMaxCreditFrameHeaderLength = 5; // addr, ctrl , 2*len, credit

//	Used in calculation of RFCOMM MTU
const static TInt KFCSFieldLength = 1;

//	Used when encoding mux commands into a UIH frame
const static TInt KShortMuxCommandHeaderLength = 2;	// type, length
const static TInt KLongMuxCommandHeaderLength = 3;	// type, 2*length

const static TInt KMaxFrameOverhead = 5;  // addr, ctrl, len*2, fcs
const static TInt KMaxFrameCreditOverhead = 6;  // addr, ctrl, len*2, credit, fcs
const static TInt KMinFrameLength = 4;
const static TInt KFrameAddrOffset = 0;
const static TInt KFrameCtrlOffset = 1;
const static TInt KMinAllowedDataSize = 11;  //< Must fit a RPN frame
const static TUint8 KPollFinalBitmask = 0x10;
const static TUint8 KEABitmask = 0x01;
const static TUint8 KCRBitmask = 0x02;
const static TUint8 KDirectionMask = 0x01;
const static TUint8 KMuxDLCI = 0;
// Signalling constants
const static TUint8 KSABMCtrlField = 0x2F;
const static TUint8 KUACtrlField = 0x63;
const static TUint8 KDMCtrlField = 0x0F;
const static TUint8 KDISCCtrlField = 0x43;
const static TUint8 KUIHCtrlField = 0xEF;
//TRY_CBFC
const static TUint8 KUIHCBFCCtrlField = 0xFF; // UIH control field with poll bit set 
const static TUint8 KCBFCCommandFlag = 0xf0;
const static TUint8 KCBFCResponseFlag = 0xe0; // actually 0xe forwards
const static TUint8 KInitialCredit = 3; //given to remote at first PN negotiation
										// - any more floods L2CAP
const static TInt KRfcommSAPCBFCBufferMultiple = 7; //max credits after initial negotiation
													//KRfcommSAPCBFCBufferMultiple*MTU used
													//for buffer size
const static TUint8 KRfcommCreditsLowWaterMark = 3; //donate credits if RxCredit 
													//(remote proxy credit)
													//hits this value
// was 14 but seemed to lock up

// Note all these types have no CR or EA bits encoded
const static TInt KTestType =0x08;
const static TInt KPNType= 0x20;
const static TInt KRPNType= 0x24;
const static TInt KFConType= 0x28;
const static TInt KFCoffType= 0x18;
const static TInt KMSCType= 0x38;
const static TInt KNSCType= 0x04;
const static TInt KRLSType= 0x14;

const static TUint8 KRfcommCtrlFrameLengthByte = 0x01;  //< EA bit set, length 0
const static TInt KRfcommCtrlFrameSize = 4;

const static TInt KFrameTimerT1=60*1000000;  // 60 secs, defined in spec
const static TInt KFrameTimerT1SABM=300*1000000;  // 300 secs, defined in spec
const static TInt KFrameTimerT2=60*1000000;  // 60 secs, defined in spec

const static TInt KCtrlFrameType=1;
const static TInt KDataFrameType=2;
const static TInt KMuxCtrlFrameType=3;
const static TInt KCreditDataFrameType=4; //Shehla - added to ease Blogging

// Setup "constants"
const static TUint8 KInitialModemStatus = 0x46; //< RTC, RTR, DV set

const static TInt KMaxOutboundQLength = 10;
const static TInt KRfcommDefaultFrameSize = 131;
const static TInt KRfcommDefaultMTU = 127;  // actual data size
const static TInt KRfcommMaxMTU = 32768;  	// maximum allowed data size
const static TInt KMaxL2CAPMTUFor1LengthByte = 131;	//	Maximum L2CAP MTU where (for a full
													//	size RFCOMM frame) one length byte 
													//	would suffice.
													
/*	
Maximum L2CAP MTU that RFComm should allow its L2Cap bearer to configure.
This is based on the specified maximum information field size (32768)
together with associated header and FCS lengths.
Note: it might be assumed that we should use the longer length field
in this calculation. However this yields the value 32773 - 
and this has been rejected during interop with a Royaltech GPS device.
*/
const static TInt KMaxL2CAPMTUAllowed = 
		KRfcommMaxMTU+KShortFrameHeaderLength+KFCSFieldLength;

const static TInt KRfcommSAPBufferMultiple = 50;  //< n*MTU for buffer size
const static TInt KRfcommSAPBufferLowMultiple = 20; //< n*MTU is flow on level
const static TInt KRfcommSAPBufferHighMultiple = 40; //< n*MTU is flow off level

// Masks for interpreting signalling commands - moved to bt_sock.h

const static TUint8 KRPNRequestLength = 1;
const static TUint8 KRPNCommandLength = 8;
const static TUint8 KRPNResponseLength = 8;

const static TUint8 KFConCommandLength = 0;
const static TUint8 KFCoffCommandLength = 0;
const static TUint8 KNSCCommandLength = 1;
const static TUint8 KMSCCommandLength = 2;
const static TUint8 KRLSCommandLength = 2;

// Test Ioctl definitions
// NB. Client Ioctls must not have KInternalIoctlBit
// (0x80000000) set.
const static TUint KRFCOMMFConIoctl =	0x7FFFFFFF;
const static TUint KRFCOMMFCoffIoctl =	0x7FFFFFFE;


// Errors are in rfcommerrors.h

#endif
