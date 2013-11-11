// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ws_mainTraces.h"
#endif

#include "es_ver.h"
#include "es_prot.h"
#include "es_wsms.h"
#include "ws_main.h"
#include <es_prot_internal.h>

extern "C"
IMPORT_C CProtocolFamilyBase* InstallWAPSMS(void);

//
// Constructor
//
CWapSmsProtocolFamily::CWapSmsProtocolFamily()
	{

	__DECLARE_NAME(_S("CWapSmsProtocolFamily"));
	} // CWapSmsProtocolFamily::CWapSmsProtocolFamily


//
// Factory
//
CWapSmsProtocolFamily * CWapSmsProtocolFamily::NewL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOLFAMILY_NEWL_1, "CWapSmsProtocolFamily::NewL()");

	return new (ELeave)CWapSmsProtocolFamily;
	} // CWapSmsProtocolFamily::NewL


//
// Nothing to be done
//
TInt CWapSmsProtocolFamily::Install()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOLFAMILY_INSTALL_1, "CWapSmsProtocolFamily::Install()");

	return KErrNone;
	} // CWapSmsProtocolFamily::Install


//
// Nothing to be done
//
TInt CWapSmsProtocolFamily::Remove()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOLFAMILY_REMOVE_1, "CWapSmsProtocolFamily::Remove()");

	return KErrNone;
	} // CWapSmsProtocolFamily::Remove


//
// Return a new CWapSmsProtocol
//
CProtocolBase * CWapSmsProtocolFamily::NewProtocolL(TUint aSockType,TUint /*aProtocol*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOLFAMILY_NEWPROTOCOLL_1, "CWapSmsProtocolFamily::NewProtocolL()");

	if (aSockType!=KSockDatagram)
		return NULL;

	return CWapSmsProtocol::NewL();
	} // CWapSmsProtocolFamily::NewProtocolL


//
// Create a description of the WapSms protocol
//
TUint CWapSmsProtocolFamily::ProtocolList(TServerProtocolDesc *& aProtocolList)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSMSPROTOCOLFAMILY_PROTOCOLLIST_1, "CWapSmsProtocolFamily::ProtocolList()");

	TRAPD(ret, (aProtocolList=new(ELeave) TServerProtocolDesc[1]) );
	if(ret!=KErrNone)
		return 0;

	// Datagram protocol
	aProtocolList[0].iName=KWAPSMSProtocolId;
	aProtocolList[0].iAddrFamily=KWAPSMSAddrFamily;
	aProtocolList[0].iSockType=KSockDatagram;
	aProtocolList[0].iProtocol=KWAPSMSDatagramProtocol;

	aProtocolList[0].iVersion=TVersion(KWapSmsMajorVersionNumber,KWapSmsMinorVersionNumber,KWapSmsBuildVersionNumber);
	aProtocolList[0].iByteOrder=ELittleEndian;
	aProtocolList[0].iServiceInfo=KWAPSMSDatagramServiceInfo;
	aProtocolList[0].iNamingServices=0;
	aProtocolList[0].iSecurity=KSocketNoSecurity;
	aProtocolList[0].iMessageSize=KWAPSMSMaxDatagramSize;
	aProtocolList[0].iServiceTypeInfo=ESocketSupport;
	aProtocolList[0].iNumSockets=KWAPSMSNumberSockets;
	return 1;
	} // CWapSmsProtocolFamily::ProtocolList


//
// Create a new protocol family
//
EXPORT_C CProtocolFamilyBase * InstallWAPSMS(void)
	{
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, INSTALLWAPSMS_1, "InstallWAPSMS [version %u.%u.%u]",KWapSmsMajorVersionNumber, KWapSmsMinorVersionNumber, KWapSmsBuildVersionNumber);

	CWapSmsProtocolFamily* wapProt=NULL;
	TRAPD(ret, (wapProt=CWapSmsProtocolFamily::NewL()) );
	if (ret==KErrNone)
		return wapProt;
	return NULL;
	} // CWapSmsProtocolFamily::ProtocolList
