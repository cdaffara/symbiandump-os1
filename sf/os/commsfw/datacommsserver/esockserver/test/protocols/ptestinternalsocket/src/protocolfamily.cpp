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

#include <es_prot.h>
#include <e32test.h>
#include "es_ptestinternalsocket.h"
#include <comms-infras/nifif.h>
#include <comms-infras/eintsock.h>
#include <comms-infras/ss_roles.h>
#include <nifman_internal.h>
#include <es_mbuf.h>
#include <ss_std.h>
#include <ss_pman.h>
#include <ss_glob.h>
#include <e32math.h>
#include <dns_qry.h>
#include <agenterrors.h>
#include <e32property.h>

#include "proxyprotocol.h"

#include "protocolfamily.h"

extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallProxy(void);	// Force export 

	EXPORT_C CProtocolFamilyBase * InstallProxy(void)
	//
	// Create a new protocol family
	//
		{
		return CTestIntSocketProtocolFamily::NewL();
		}
	}

CTestIntSocketProtocolFamily* CTestIntSocketProtocolFamily::NewL()
//
//
//
	{
	CTestIntSocketProtocolFamily* pf=new(ELeave) CTestIntSocketProtocolFamily();
	CleanupStack::PushL(pf);
	pf->ConstructL();
	CleanupStack::Pop();
	return pf;
	}

void CTestIntSocketProtocolFamily::ConstructL()
//
//
//
	{
	// Force a fail on Memory tests
	char* ptr=new(ELeave) char;
	delete ptr;

	}

CTestIntSocketProtocolFamily::CTestIntSocketProtocolFamily() : CProtocolFamilyBase()
	{
	iProtocolDescs[0].iAddrFamily=KProxyAddrFamily;
	iProtocolDescs[0].iSockType=KSockDatagram; // not important whether datagram or stream for proxy
	iProtocolDescs[0].iProtocol=KProxy;
	iProtocolDescs[0].iVersion=TVersion(KPTestIntSocketMajor,KPTestIntSocketMinor,KPTestIntSocketBuild);
	iProtocolDescs[0].iByteOrder=EBigEndian;
	iProtocolDescs[0].iServiceInfo=kDDatagramServiceInfo|KSIPeekData;
	iProtocolDescs[0].iSecurity=KSocketNoSecurity;
	iProtocolDescs[0].iMessageSize=0x300;
	iProtocolDescs[0].iName=KProxyName;
	iProtocolDescs[0].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[0].iNamingServices=KNSNameResolution|KNSServiceResolution|KNSInfoDatabase;
	iProtocolDescs[0].iNumSockets=100;
/*
	iProtocolDescs[1].iAddrFamily=KProxyAddrFamily;
	iProtocolDescs[1].iSockType=KSockDatagram;
	iProtocolDescs[1].iProtocol=KTargetDatagram;
	iProtocolDescs[1].iVersion=TVersion(KPTestIntSocketMajor,KPTestIntSocketMinor,KPTestIntSocketBuild);
	iProtocolDescs[1].iByteOrder=EBigEndian;
	iProtocolDescs[1].iServiceInfo=kDDatagramServiceInfo|KSIPeekData;
	iProtocolDescs[1].iSecurity=KSocketNoSecurity;
	iProtocolDescs[1].iMessageSize=0x300;
	iProtocolDescs[1].iName=KTargetDatagramName;
	iProtocolDescs[1].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[1].iNamingServices=KNSNameResolution|KNSServiceResolution|KNSInfoDatabase;
	iProtocolDescs[1].iNumSockets=100;

	iProtocolDescs[2].iAddrFamily=KProxyAddrFamily;
	iProtocolDescs[2].iSockType=KSockStream;
	iProtocolDescs[2].iProtocol=KTargetStream;
	iProtocolDescs[2].iVersion=TVersion(KPTestIntSocketMajor,KPTestIntSocketMinor,KPTestIntSocketBuild);
	iProtocolDescs[2].iByteOrder=EBigEndian;
	iProtocolDescs[2].iServiceInfo=KDStreamServiceInfo;
	iProtocolDescs[2].iSecurity=KSocketNoSecurity;
	iProtocolDescs[2].iMessageSize=KSocketMessageSizeIsStream;
	iProtocolDescs[2].iName=KTargetStreamName;
	iProtocolDescs[2].iServiceTypeInfo=ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[2].iNamingServices=0;
	iProtocolDescs[2].iNumSockets=100;
*/
	__DECLARE_NAME(_S("CTestIntSocketProtocolFamily"));
	}

#pragma warning( disable : 4100 )

TInt CTestIntSocketProtocolFamily::Install()
	{
	// Force a fail on Memory tests
	char* ptr=new char;
	if (!ptr)
		return KErrNoMemory;
	delete ptr;
	return KErrNone;
	}

TInt CTestIntSocketProtocolFamily::Remove()
	{
	return KErrNone;
	}

TUint CTestIntSocketProtocolFamily::ProtocolList(TServerProtocolDesc *& aProtocolDescPointer)
	{
	// Make sure we don't register the proxy protocol twice when testing two worker threads.
	// We only need one instance and if we try to register a second, ESockSvr will panic the
	// second worker thread with a misconfiguration error.
	if(SockManGlobals::Get()->SelfWorker()->WorkerId() == KProxyWorkerTwoId)
		{
		aProtocolDescPointer = NULL;
		
		return 0;
		}
 
  	aProtocolDescPointer=new TServerProtocolDesc[KPTestIntSocketNumProtocols];
  	if (!aProtocolDescPointer)
  		return 0;

	Mem::Copy(aProtocolDescPointer, iProtocolDescs, sizeof(TServerProtocolDesc)*KPTestIntSocketNumProtocols);

	TRAP_IGNORE(Nif::CheckInstalledMBufManagerL();)

	return KPTestIntSocketNumProtocols;
	};

CProtocolBase * CTestIntSocketProtocolFamily::NewProtocolL(TUint /* aSockType */,TUint aProtocol)
	{

	CProtocolBase* p=CProxyProtocol::NewL(aProtocol, &iProtocolDescs[aProtocol-1]);	
	return p;
	}



