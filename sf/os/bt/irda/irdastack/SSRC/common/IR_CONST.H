// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__IR_CONST_H__)
#define __IR_CONST_H__

#include <es_prot.h>
#include <d32comm.h>
#include "es_prot_internal.h"


// P&S key used to disable sending of discovery responses.
const TUint KIrdaDiscoveryResponseSuppressor = 0x1028279f;

// Enum used for Discovery Response Suppressor key
enum TDiscoveryResponseSuppressorStatus
	{
	EAllowDiscoveryResponses,
	ESuppressDiscoveryResponses,
	};

//#define _MKLOG
//#define __TRACEWIN__  // For tracewin logging window

//#################### PRT CONSTANTS ###########################

const TUint KIrdaMinHeapSize=0x0400;
const TUint KIrdaMaxHeapSize=0x4000;
const TUint KIrdaSenderPriority=ECommTransmitPriority;
const TUint KIrdaReceiverPriority=ECommReceivePriority;

const TUint32 KIrdaAddrNone=0x00000000;
// Version numbers for IrDA
const TUint KIrdaMajor=1;  // For major ROM releases
const TUint KIrdaMinor=0;  // For minor ROM releases
const TUint KIrdaBuild=205; // This is the overall Version number

const TUint KIrdaDatagramServiceInfo=
  KSIReliable|KSIDatagram|KSIGracefulClose|KSIUrgentData|KSIBroadcast|KSIQOS|KSIConnectData;

const TUint8 KDefaultDiscoverySlots=6;
const TUint8 KDefaultTinyTPCredit=4;

const TUint KIrdaDefaultSendBufSize=64;
const TUint KIrdaDefaultRecvBufSize=64;
/* This is the amount of heap space we alloc for irSender and irReceiver
frames WITHOUT allowing for the extra space required for byte stuffing */
/********* Frame sizes - INCLUDES the size of the FCS field ********/
const TUint KDiscoveryCMDFrameSize=16;       /* A+C+1+4+4+1+1+1+FCS */
const TUint KEndOfDiscoveryCMDFrameSize=19;  /* A+C+1+4+4+1+1+1+3+FCS */
const TUint KDiscoveryRSPFrameSize=19;       /* A+C+1+4+4+1+1+1+3+FCS */
const TUint KTESTFrameSize=36;               /* A+C+4+4+24+FCS */
const TUint KSNRMFrameSize=34;               /* A+C+4+4+1+21+FCS */
const TUint KFRMRFrameSize=7;                /* A+C+1+1+1+FCS */
const TUint KDISCFrameSize=4;                /* A+C+FCS */
const TUint KUAfromDISCFrameSize=12;         /* A+C+4+4+FCS */
const TUint KUAfromSNRMFrameSize=33;         /* A+C+4+4+21+FCS */

//
const TUint KDMFrameSize=4;                  /* A+C+FCS & no info field */
const TUint KRRFrameSize=4;                  /* A+C+FCS */
const TUint KREJFrameSize=4;                 /* A+C+FCS */
const TUint KRDFrameSize=4;                  /* A+C+FCS */
const TUint KRNRMFrameSize=4;                /* A+C+FCS */
const TUint KRNRFrameSize=4;                 /* A+C+FCS */
const TUint16 KInitFCS=0xffff;               /* Initial FCS value for CRC */
const TUint16 KGoodFCS=0xf0b8;               /* Good final FCS value for CRC */

/************************** Timeouts ****************************/
const TUint KSlotTimeout=110;                /* 60ms */
const TUint KMediaBusyTimeout=500;           /* 500ms */
const TUint KIrmuxTimeout=2000;              /* 2 secs */
const TUint KIrmuxSAPTimeout=2000;           /* 2 secs */
const TUint KIrmuxMonitorTimeout=2000;       /* 2 secs */
const TUint KMilliSecondFactor=1000;         /* ms to microsec */

/********* Frame destroy defines - immediate or delayed? ********/
const TUint KDataFrame=1;
const TUint KConnectingAsPrimary=1;          /* Trying to change baud up */
const TUint KConnectingAsSecondary=2;
const TUint KDisconnecting=3;                /* Trying to change baud down */
const TUint KDisconnectOnTimeout=4;          /* Timed out */

/***************** IRLAP DEVICE ADDRESS CONSTANTS ****************/
const TUint8 KEOFMask=0xc1;                  /* End of frame char */
const TUint8 KBOFMask=0xc0;                  /* Start of frame char */
const TUint8 KAdditionalBOFMask=0xff;        /* Additional BOF=0xff */
const TUint8 KESCMask=0x7d;                  /* Byte stuff char mask */
const TUint8 KByteStuffMask=0x20;            /* Byte stuff:OR with this */

/*************** IRLAP DEVICE ADDRESS CONSTANTS ********************/
const TUint KBroadcastDevAddrMask=0xffffffff;
const TUint KNullDevAddrMask=0x00000000;
const TUint KDevAddrLength=4;

/*************** IRLAP ADDRESS FIELD CONSTANTS *********************/
const TUint KAddressFieldOffset=0;
const TUint8 KCRMask=0x01;                   /* 00000001 - mask in the C/R bit */
const TUint8 KConnAddrMask=0xfe;             /* 11111110 - mask off C/R bit */
const TUint8 KBroadcastConnAddrMask=0xfe;    /* 11111110 - broadcast connAddr */
const TUint8 KNullConnAddrMask=0x00;         /* 00000000 - null connAddr */
const TUint8 KMaxConnAddrMask=0x7f;          /* 01111111 - need to shl this by 1 */

/*************** IRLAP CONTROL FIELD CONSTANTS *********************/
const TUint8 KControlFieldOffset=1;          /* offset into IrLap frame */
const TUint8 KFrameTypeMask=0x03;            /* 00000011 - only the last two bits */
const TUint8 KInfoFrameControlMask=0x13;     /* 00010011 - just last 2 bits and P/F */
const TUint8 KPFMask=0x10;                   /* 00010000 - to get just the P/F bit */

/* IrLAP UNNUMBERED Field Values */
const TUint8 KUnnumberedFrameTypeMask=0x03;  /* 00000011 */
const TUint8 KUnnumberedFrameMask=0x03;      /* 00000011 - U frame has 11 as last 2 bits */
const TUint8 KUnnumberedPFrameMask=0x13;     /* 00010011 - captures P/F bit too */
const TUint8 KUnnumberedNotPFrameMask=0x03;  /* 00000011 - captures P/F bit too */
const TUint8 KUnnumberedFFrameMask=0x13;     /* 00010011 - captures P/F bit too */
const TUint8 KUnnumberedNotFFrameMask=0x03;  /* 00000011 - captures P/F bit too */

/* Sent by primary as CMD frames to a secondary */
const TUint8 KSNRMPMask=0x93;                /* 10010011 */
const TUint8 KSNRMNotPMask=0x83;             /* 10000011 */
const TUint8 KDISCPMask=0x53;                /* 01010011 */
const TUint8 KDISCNotPMask=0x43;             /* 01000011 */
const TUint8 KUIPMask=0x13;                  /* 00010011 */
const TUint8 KUINotPMask=0x03;               /* 00000011 */
const TUint8 KXIDPMask=0x3f;                 /* 00111111 */
const TUint8 KXIDNotPMask=0x2f;              /* 00101111 */
const TUint8 KTESTPMask=0xf3;                /* 11110011 */

/* Sent by secondary as RSP frames to a primary */
const TUint8 KRNRMFMask=0x93;                /* 10010011 */
const TUint8 KRNRMNotFMask=0x83;             /* 10000011 */
const TUint8 KUAFMask=0x73;                  /* 01110011 */
const TUint8 KUANotFMask=0x63;               /* 01100011 */
const TUint8 KFRMRFMask=0x97;                /* 10010111 */
const TUint8 KFRMRNotFMask=0x87;             /* 10000111 */
const TUint8 KDMFMask=0x1f;                  /* 00011111 */
const TUint8 KDMNotFMask=0x0f;               /* 00001111 */
const TUint8 KRDFMask=0x53;                  /* 01010011 */
const TUint8 KRD_NOTF=0x43;                  /* 01000011 */
const TUint8 KUIFMask=0x13;                  /* 00010011 */
const TUint8 KUINotFMask=0x03;               /* 00000011 */
const TUint8 KXIDFMask=0xbf;                 /* 10111111 */
const TUint8 KXIDNotFMask=0xaf;              /* 10101111 */
const TUint8 KTESTFMask=0xf3;                /* 11110011 */

/* XID frame masks and field offsets */
const TUint8 KFormatFieldOffset=2;
const TUint8 KDiscoveryFormatMask=0x01;      /* 00000001 */
const TUint8 KNegotiationFormatMask=0x02;    /* 00000010 */
/* For format field value 0x01 => DISCOVERY XID */
const TUint8 KMaxSlotsBitsMask=0x03;
const TUint8 KAddressConflictBitMask=0x04;   /* 00000100 in discovery flags byte */
const TUint KSourceDevAddrFieldOffset=3;
const TUint KDestDevAddrFieldOffset=7;
const TUint KDiscoveryFlagsFieldOffset=11;
const TUint KSlotNumberFieldOffset=12;
const TUint8 KIrlapVersionFieldOffset=13;
const TUint8 KIrlapVersion=0x00;
const TUint KDiscoveryDeviceInfoFieldOffset=14;
const TUint KFirstServiceHintByteOffset=14;
const TUint8 KServiceHintsExtensionMask=0x80;
const TUint KSecondServiceHintByteOffset=15;
const TUint KCharacterSetFieldOffset=16;
const TUint8 KASCIIMask=0x00;                /* 8086 ASCII */
const TUint8 KUnicodeMask=0xff;
const TUint KNicknameFieldOffset=17;
const TInt KDiscoveryNicknameLength=20;
const TUint KDiscoveryDeviceInfoLength=27;
const TUint KNDMAdditionalBOFs=10;           
const TUint KNDMMinimumTurnaroundTime=0;    /* 0 ms */

/* Various negotiation constants */
const TUint KNegotiationLength=21;
const TUint KMaxNumberOfNegotiables=7;
const TUint8 KBaudRateTuple=0x01;
const TUint8 KBaudRateLengthTuple=1;
const TUint8 KMaxTATimeTuple=0x82;
const TUint8 KMaxTATimeLengthTuple=1;
const TUint8 KMaxDataSizeTuple=0x83;
const TUint8 KMaxDataSizeLengthTuple=1;
const TUint8 KMaxWindowSizeTuple=0x84;
const TUint8 KMaxWindowSizeLengthTuple=1;
const TUint8 KNumberBofsTuple=0x85;
const TUint8 KNumberBofsLengthTuple=1;
const TUint8 KMinTATimeTuple=0x86;
const TUint8 KMinTATimeLengthTuple=1;
const TUint8 KLinkDisconnTuple=0x08;
const TUint8 KLinkDisconnLengthTuple=1;
/* Default negotiation parameters */
const TBps KDefaultBaudRate=EBps9600;
const TUint KDefaultMaxHostTATime=500;
const TUint KDefaultMaxHostDataSize=64;
const TUint KDefaultReceiveWindow=1;
const TUint KDefaultAdditionalBofs=0;
const TUint KDefaultMinTATime=10;
const TUint KDefaultLinkDisconnTime=3;
const TUint KDefaultSubMsTATime=1; // Submillisecond time defaults to 1ms
const TUint KDefaultSupportedBaudRates=2;
/* Actual host supported negotiation parameters */
const TBps KMaxSupportedBaudRate=EBps115200;
const TUint16 KMaxSupportedBaudRates=0x003F; // All rates supported upto 115200bps
const TUint KMinSupportedMaxTATime=500;
const TUint KMaxSupportedMaxHostDataSize=2048;
const TUint KMaxReceiveWindow=7;
const TUint KMaxAdditionalBofs=48;
const TUint KMinTATime=0;
const TUint KSupportedLinkDisconnTime=40;

/* SNRM frame masks and field offsets */
const TUint KSNRMSourceDevAddrFieldOffset=2;
const TUint KSNRMDestDevAddrFieldOffset=6;
const TUint KSNRMProposedConnAddrFieldOffset=10;
const TUint KSNRMNegotiationFieldOffset=11;

/* UA frame masks and field offsets */
const TUint KUASourceDevAddrFieldOffset=2;
const TUint KUADestDevAddrFieldOffset=6;
const TUint KUANegotiationFieldOffset=10;
const TBool KfromDISC=EFalse;
const TBool KfromSNRM=ETrue;
const TBool KFullSNRM=ETrue;
const TBool KResetSNRM=EFalse;

/* FRMR frame masks and field offsets */
const TUint KFRMRFirstInfoFieldByteOffset=2;   /* Rejected C Field */
const TUint KFRMRSecondInfoFieldByteOffset=3;  /* Frame fields */
const TUint KFRMRThirdInfoFieldByteOffset=4;   /* FRMR reason */

/* TEST frame masks and field offsets */
const TUint KTESTSourceDevAddrFieldOffset=2;
const TUint KTESTDestDevAddrFieldOffset=6;
const TUint KTESTDataFieldOffset=10;
const TUint KTESTConnDataFieldOffset=2;
const TUint KTESTInfoFieldLength=24;
#define  KTESTFrameBanner _S(" Protea IrDA TEST Frame ")

/* IrLAP Supervisory Frame values */
const TUint8 KSupervisoryFrameTypeMask=0x01;            /* 00000001 */
const TUint8 KSupervisoryFrameControlMask=0x1f;         /* 00011111 */
const TUint8 KRRPMask=0x11;                             /* 00010001 */
const TUint8 KRRFMask=0x11;                             /* 00010001 */
const TUint8 KRNRPMask=0x15;                            /* 00010101 */
const TUint8 KRNRFMask=0x15;                            /* 00010101 */
const TUint8 KREJPMask=0x19;                            /* 00011001 */
const TUint8 KREJFMask=0x19;                            /* 00011001 */
const TUint8 KSREJPMask=0x1d;                           /* 00011101 */
const TUint8 KSREJFMask=0x1d;                           /* 00011101 */

/* IrLAP Information Frame values */
const TUint8 KInfo1FrameTypeMask=0x00;                  /* 00000000 */
const TUint8 KInfo2FrameTypeMask=0x02;                  /* 00000010 */
const TUint8 KInfoFrameMask=0x01;                       /* 00000001 */
const TUint8 KInfo1PMask=0x10;                          /* 00010000 */
const TUint8 KInfo1FMask=0x10;
const TUint8 KInfo2PMask=0x12;                          /* 00010010 */
const TUint8 KInfo2FMask=0x12;
const TUint8 KInfo1NotPMask=0x00;                       /* 00000000 */
const TUint8 KInfo1NotFMask=0x00;
const TUint8 KInfo2NotPMask=0x02;                       /* 00000010 */
const TUint8 KInfo2NotFMask=0x02;
const TUint8 KNsMask=0x0e;                              /* 00001110 */
const TUint8 KNrMask=0xe0;                              /* 11100000 */     

const TUint KHighWaterMark=8;
const TUint KLowWaterMark=3;

/******************** IRMUX ADDRESS CONSTANTS **********************/
const TUint KIrmuxConnectMinFrameSize=8;                /* A+C+D+S+op+rsvd+...+FCS */
const TUint KIASConnectFrameSize=8;                     /* A+C+D+S+op+rsvd+FCS */
const TUint KIASQueryFrameSize=66;                      /* A+C+D+S+..60..+FCS */
const TUint KIASGetValueByClassMinFrameSize=7;          /* A+C+D+S+op+ret+...+FCS */
const TUint KIrmuxDisconnectFrameSize=8;                /* A+C+D+S+op+rsvd+FCS */
const TUint KIrmuxAccessModeFrameSize=9;                /* A+C+D+S+op+stat+mode+FCS */

const TUint8 KLSAP_SELMask=0x7f;
const TUint KMaxLSAP = 0x6f;
const TUint KMinLSAP = 0x01;
const TUint8 KIrmuxControlFrameMask=0x80;
const TUint KIrmuxDLSAP_SELFieldOffset=0;
const TUint KIrmuxSLSAP_SELFieldOffset=1;
const TUint KIrmuxOpcodeFieldOffset=2;
const TUint KIrmuxParametersFieldOffset=3;
const TUint8 KIrmuxReservedParametersMask=0x00;
const TUint KIrmuxControlFrameDataFieldOffset=4;
const TUint KIrmuxControlFrameDataFieldLength=60;

const TUint KModeStatusFieldOffset=3;
const TUint8 KModeStatusSuccessMask=0x00;
const TUint8 KModeStatusFailureMask=0x01;
const TUint8 KModeStatusUnsupportedMask=0xff;
const TUint KModeFieldOffset=4;
const TUint8 KMultiplexModeMask=0x00;
const TUint8 KExclusiveModeMask=0x01;

const TUint8 KIrmuxConnectRequestMask=0x01;             /* 00000001 */
const TUint8 KIrmuxConnectConfirmMask=0x81;             /* 10000001 */
const TUint8 KIrmuxDisconnectMask=0x02;                 /* 00000010 */
const TUint8 KIrmuxAccessModeRequestMask=0x03;          /* 00000011 */
const TUint8 KIrmuxAccessModeConfirmMask=0x83;          /* 10000011 */

const TUint KHoldoffMark=5;

const TUint KIASControlFieldOffset=4;
const TUint KIASResultsFieldOffset=5;
const TUint8 KIASLstMask=0x80;
const TUint8 KIASAckMask=0x40;
const TUint8 KIASOpcodeMask=0x3f;

/****************** IR TINY TP ADDRESS CONSTANTS *******************/
const TUint8 KTPParameterMask=0x80;                     /* 10000000 */
const TUint8 KTPDeltaCreditMask=0x7f;                   /* 01111111 */
const TUint8 KTinyTPInitialCreditPosition=0x06;         //This is offset from start of TinyTP Connect Frame
const TUint KIrdaMaxTPBufferSize=8192;                  //This is used to calculate the initial credit value
const TUint8 KTPMaxSduSizePIMask=0x01;

const TUint8 KIrmuxSapFrameOverhead  = 0x02;
const TUint8 KTinyTPSapFrameOverhead = 0x03;

const TUint KMaxIrdaMinTABofSize	   = 115;
const TUint KMaxIrdaRawFrameSize       = 4154;// 0x103A
const TUint KMaxIrdaProcessedFrameSize = 2052;// 0x0804

#endif // __IR_CONST_H__
