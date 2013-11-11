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

#include <es_ver.h>
#include <es_prot.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_prot_internal.h>
#endif
#include "ES_IPC.H"
#include "IPC_MAIN.H"

CIpcProtocolFamily::CIpcProtocolFamily()
//
// CTor for the IPC protocol family
//
	{
	__DECLARE_NAME(_S("CIpcProtocolFamily"));
	}

CIpcProtocolFamily * CIpcProtocolFamily::NewL()
//
// Creator for IPC protocol family
//
	{
	return new (ELeave)CIpcProtocolFamily;
	}

TInt CIpcProtocolFamily::Install()
//
// Nothing to do
//
	{
	return KErrNone;
	}

TInt CIpcProtocolFamily::Remove()
//
// Nothing to do
//
	{
	return KErrNone;
	}

CProtocolBase * CIpcProtocolFamily::NewProtocolL(TUint aSockType,TUint /*aProtocol*/)
//
// return a new CIPCProtocol - this is parameterised on socketType
//
	{
	return CIpcProtocol::NewL(aSockType);
	}

TUint CIpcProtocolFamily::ProtocolList(TServerProtocolDesc *& aProtocolList)
//
//
//
	{

	TRAPD(ret,aProtocolList=new(ELeave) TServerProtocolDesc[2]);
	if(ret!=KErrNone)
		return 0;

	// Stream protocol
	_LIT(ipcStream,"IPC Stream");
	aProtocolList[0].iName=ipcStream;
	aProtocolList[0].iAddrFamily=KIPCAddrFamily;
	aProtocolList[0].iSockType=KSockStream;
	aProtocolList[0].iProtocol=KIPCStreamProtocol;

	aProtocolList[0].iVersion=TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
	aProtocolList[0].iByteOrder=ELittleEndian;
	aProtocolList[0].iServiceInfo=KIPCStreamServiceInfo;
	aProtocolList[0].iNamingServices=0;
	aProtocolList[0].iSecurity=KSocketNoSecurity;
	aProtocolList[0].iMessageSize=KSocketMessageSizeIsStream;
	aProtocolList[0].iServiceTypeInfo=ESocketSupport;
	aProtocolList[0].iNumSockets=KIPSNumberSockets;

	// Datagram protocol
	_LIT(ipcDatagramm,"IPC Datagramm");
	aProtocolList[1].iName=ipcDatagramm;
	aProtocolList[1].iAddrFamily=KIPCAddrFamily;
	aProtocolList[1].iSockType=KSockDatagram;
	aProtocolList[1].iProtocol=KIPCDatagramProtocol;

	aProtocolList[1].iVersion=TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
	aProtocolList[1].iByteOrder=ELittleEndian;
	aProtocolList[1].iServiceInfo=KIPCDatagramServiceInfo;
	aProtocolList[1].iNamingServices=0;
	aProtocolList[1].iSecurity=KSocketNoSecurity;
	aProtocolList[1].iMessageSize=KSocketMessageSizeNoLimit;
	aProtocolList[1].iServiceTypeInfo=ESocketSupport;
	aProtocolList[1].iNumSockets=KIPSNumberSockets;
	
	return 2;
	}

extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallIPC(void);	// Force export 
	
	EXPORT_C CProtocolFamilyBase * InstallIPC()
	//
	// Create a new protocol family
	//
		{
		return CIpcProtocolFamily::NewL();
		}
	}

