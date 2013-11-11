// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__ES_PTESTINTERNALSOCKET_H__)
#define __ES_PTESTINTERNALSOCKET_H__

#include <e32def.h>
#include <es_sock.h>
#include <test/testexecutestepbase.h>
#include <test/testexecuteclient.h>
#include <test/es_dummy.h>
#include <elements/cftransport.h>


//This property needs to be set to 0 for normal operation 
//and 1 to request termination

//Socket server test ioclts
const TInt KProxyProtocolTestRequest 	= 0x00003001; // reuse ioctl value from pdummy

typedef TVerdict TProxyProtocolTestResult;

// Structure used to pass back result of ioctl (result of test)
struct TProxyProtocolRequest
	{
	TBuf<KMaxTestExecuteNameLength> name;
	TVerdict result;
	};

//Socket Protocols and families
const TInt KPTestIntSocketNumProtocols=1;	// is this the right number?
const TUint KProxyAddrFamily=0x20000;
const TUint KProxy=1;
const TUint KTargetDatagram=2;
const TUint KTargetStream=3;

// version numbers - dll wide?
const TUint KPTestIntSocketMajor=0;
const TUint KPTestIntSocketMinor=1;
const TUint KPTestIntSocketBuild=0;

//const TUint KDStreamServiceInfo = KSIReliable | KSIInOrder | KSIStreamBased | KSIUrgentData | KSIGracefulClose;
//const TUint kDDatagramServiceInfo = KSIConnectionLess | KSIMessageBased | KSIBroadcast| KSIGracefulClose;
  
// Dummy protocol descriptive names
_LIT(KProxyName, "Proxy Protocol");
_LIT(KTargetDatagramName, "Target Datagram Protocol");
_LIT(KTargetStreamName, "Target Stream Protocol");

const CommsFW::TWorkerId KProxyWorkerTwoId = 14;

#endif

