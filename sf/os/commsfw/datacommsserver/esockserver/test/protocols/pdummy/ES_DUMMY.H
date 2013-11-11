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

#if !defined (__ES_DUMMY_H__)
#define __ES_DUMMY_H__

#include <e32def.h>
#include <es_sock.h>

const TUint KDummyIoctlCheckStarted=0x000000010;
const TUint KDummyIoctlCheckBound=0x00000011;
const TUint KDummyIoctlIgnore=0x00000111;
const TUint KDummyIoctlCancelled=0x00001111;

//RProperty Handles for Dummy Protocol
const TUid KDummyUid                		= {0x1020316E};
const TInt KDummyTerminationProperty		= 1; 
//This property needs to be set to 0 for normal operation 
//and 1 to request termination


//Socket server test ioclts
const TUint KDummyIoctlPanicProtocolModule	= 0x00003001;
const TUint KDummyIocltNonCompleting 		= 0x00003002;
const TUint KDummyIoctlPanicDealer			= 0x00003003;
const TUint KDummyIoctBlockHardOnClose		= 0x00003004;
const TUint KDummyIoctlSlowIoctlReturn		= 0x00003005;
const TUint KDummyIoctlHangIoctlReturn		= 0x00003006;
const TUint KDummyIocltJustComplete			= 0x00003007;
const TUint KDummyIocltRemoteDisconnect		= 0x00003008;
const TUint KDummyIoctlCompleteWithData		= 0x00003009;

const TUint KIoctlDummyLevel=KSOLSocket+1;
const TUint KDummyOptionSetConnectComplete	=	0x00001234;
const TUint KDummyOptionSetErrorNextListen	=	0x00000001;
const TUint KDummyOptionSetBlockConnect		=	0x00000010;
const TUint KDummyOptionSetIocltComplete	=   0x00000020;
const TUint KDummyOptionSetErrorNextWrite	=	0x00000030;
const TUint KDummyOptionSetErrorNextShutdown=	0x00000040;
const TUint KDummyOptionSetErrorNextConnect	=	0x00000050;
const TUint KDummyOptionSetFlowOffWrite  	=   0x00000060;
const TUint KDummyOptionSetTimedFlowOffWrite =  0x00000070;
const TUint KDummyOptionSetGobbleMBufs 		 =  0x00000380;
const TUint KDummyOptionSetFreeMBufs         =  0x00000390;
const TUint KDummyOptionSetFreeSomeMBufs     =  0x000003a0;
const TUint KDummyOptionGetMBufFreeSpace     =  0x000003b0;
const TUint KDummyOptionSetBlockConnectData  =  0x000003c0;
const TUint KDummyOptionIssueNoBearer        =  0x000003d0;

// set option used for testing CProtocolBase implemenations
const TUint KDummyOptionSetProtocolBaseBind					=	0x00000110;
const TUint KDummyOptionSetProtocolBaseBindTo				=	0x00000120;
const TUint KDummyOptionSetProtocolBaseError				=	0x00000130;
const TUint KDummyOptionSetProtocolBaseGetOption			=   0x00000140;
const TUint KDummyOptionSetProtocolBaseNewHostResolver		=	0x00000150;
const TUint KDummyOptionSetProtocolBaseNewNetDatabase		=	0x00000160;
const TUint KDummyOptionSetProtocolBaseNewSAP				=	0x00000170;
const TUint KDummyOptionSetProtocolBaseNewServiceResolver	=   0x00000180;
const TUint KDummyOptionProtocolBaseProcess					=   0x00000190;
const TUint KDummyOptionSetProtocolBaseSend 		    	=   0x000001a0;
const TUint KDummyOptionSetProtocolBaseSetOption        	=   0x000001b0;

const TUint KDummyOptionLeakMemory			=	0x00000080;
//Socket Protocols and families

const TUint KPDummyNumProtocols=5;
const TUint KDummyAddrFamily=0xFFFF;	// was 0x10000; SIP inadvertently took that
const TUint KDummyOne=1;
const TUint KDummyTwo=2;
const TUint KDummyThree=3;
const TUint KDummyFour=4;
const TUint KDummyFive=5;

const TUint KDummyMajor=0;
const TUint KDummyMinor=1;
const TUint KDummyBuild=0;

const TInt KDummyWriteFlowOffTimeout = 1000;

const TUint KDStreamServiceInfo = KSIReliable | KSIInOrder | KSIStreamBased | KSIUrgentData | KSIGracefulClose;
const TUint kDDatagramServiceInfo = KSIConnectionLess | KSIMessageBased | KSIBroadcast| KSIGracefulClose;

// Ioctl data
_LIT8(KIoctlData, "Ioctl Data");

// Connect complete data
_LIT8(KConnectCompleteData, "Connect Complete Data");

// Dummy protocol descriptive names
_LIT(KDummyOneName, "Dummy Protocol 1");
_LIT(KDummyTwoName, "Dummy Protocol 2");
_LIT(KDummyThreeName, "Dummy Protocol 3");
_LIT(KDummyFourName, "Dummy Protocol 4");
_LIT(KDummyFiveName, "Dummy Protocol 5");

// Dummy interface names
_LIT(KDummySglIfName, "testsglif");
_LIT(KDummyMulIfName, "testmulif");


#endif
