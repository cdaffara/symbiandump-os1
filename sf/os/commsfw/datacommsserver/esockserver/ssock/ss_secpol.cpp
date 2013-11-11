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
// Security Policy definitions for Platform security.
//
//

/**
 @file SS_SECPOL.CPP
 @internalComponent
*/

#include <e32base.h>
#include <ss_std.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_partner.h>
#include <es_sock_internal.h>
#endif
#include <comms-infras/sockmes.h>
#include <comms-infras/es_connectionservermessages.h>
#include "ss_secpol.h"

namespace ESock
{

// NOTE: why does the following generate an error in ARMv5 indicating that esocksvr.dll
// has initialised data ?
//		_LIT_VENDOR_ID(KSymbianVID,0x70000001);
//

/** The Symbian Vendor ID. */
const TUint KSymbianVID = 0x70000001;

/** Socket Server Security Policy Definitions */
const TUint SocketServerRangeCount = 5;

//

/** Definition of IPC ranges that each cover the same policy */
const TInt SocketServerRanges[SocketServerRangeCount] = 
	{
	ESSInstallExtension,	// policyNetworkControl
	ECNControl,				// policyCustom
	ESoCreate,				// policyDeferred
//- To the system "deferred" and "pass" are identical, so no separate range to check
//-	ESSNumProtocols,		// policyPass
	ESSDbgMarkHeap,			// policyVIDandNetworkControl
	ESSInvalidFunction		// fail (to KMaxTInt) 
	};

/** Index numbers into SocketServerElements[] */	
const TInt policyNetworkControl = 0;
const TInt policyPass = 1;
const TInt policyDeferred = policyPass;
const TInt policyVIDandNetworkControl = 2;
const TInt policyCustom = CPolicyServer::ECustomCheck;

/** Mapping of IPCs to policy elements */
const TUint8 SocketServerElementsIndex[SocketServerRangeCount] = 
	{
	policyNetworkControl,		/** ESSInstallExtension */
	policyCustom,				/** ECNControl */
	policyDeferred,				/** ESoCreate */
	policyVIDandNetworkControl,	/** ESSDbgMarkHeap */
 	CPolicyServer::ENotSupported/** ESSInvalidFunction to KMaxTInt */
	};

/** Individual policy elements */	
const CPolicyServer::TPolicyElement SocketServerElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient },
	{ _INIT_SECURITY_POLICY_PASS },
	{ _INIT_SECURITY_POLICY_V1(KSymbianVID,ECapabilityNetworkControl), CPolicyServer::EFailClient }
	};

/** Main policy */
const CPolicyServer::TPolicy SocketServerPolicy =
	{
	CPolicyServer::EAlwaysPass, 	/** Specifies all connect attempts should pass */
	SocketServerRangeCount,					
	SocketServerRanges,
	SocketServerElementsIndex,
	SocketServerElements
	};

/** Connection Server Security Policy Definitions */
const TUint ConnectionServerRangeCount = 8;

/** Definition of IPC ranges that each cover the same policy */
const TInt ConnectionServerRanges[ConnectionServerRangeCount] = 
	{
	ECMCreate,
	ECMAttachToTierManager + 1,

	ECMAccessPointStatusQuery_DoThenGetResultOrSize,
	ECMAccessPointStatusQuery_Cancel + 1,

	ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize,
	ECMAccessPointNotification_Cancel + 1,
	
	ECMApiExtBindIface,
	ECMApiExtIfaceClose + 1
	};

/** Index numbers into ConnectionServerElements[] */	
const TInt csPolicyNetworkServices = 0;
//const TInt csPolicyVIDandNetworkServices = 1;

/** Mapping of IPCs to policy elements */
const TUint8 ConnectionServerElementsIndex[ConnectionServerRangeCount] = 
	{
	csPolicyNetworkServices,	/** ECMCreate */
								/** ECMClose */
								/** ECMAttachToTierManager */
	CPolicyServer::ENotSupported,   /**  gap */
	csPolicyNetworkServices,	/** ECMAccessPointStatusQuery_DoThenGetResultOrSize */
								/** ECMAccessPointStatusQuery_GetResult	*/
								/** ECMAccessPointStatusQuery_Cancel */
	CPolicyServer::ENotSupported,   /**  gap */
	csPolicyNetworkServices,	/** ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize */
								/** ECMAccessPointNotification_AwaitThenGetResultOrSize */
								/** ECMAccessPointNotification_GetResult */
								/** ECMAccessPointNotification_Cancel */
	CPolicyServer::ENotSupported ,  /** gap */
	csPolicyNetworkServices,	/** ECMApiExtBindIface */
								/** ECMApiExtBindSendReceive */
	CPolicyServer::ENotSupported   /** _last_ + 1 to KMaxTInt */
	};

/** Individual policy elements */	
const CPolicyServer::TPolicyElement ConnectionServerElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices), CPolicyServer::EFailClient },
//	{ _INIT_SECURITY_POLICY_V1(KSymbianVID,ECapabilityNetworkServices), CPolicyServer::EFailClient }
	};

/** Main policy */
const CPolicyServer::TPolicy ConnectionServerPolicy =
	{
	CPolicyServer::EAlwaysPass, 	/** Specifies all connect attempts should pass */
	ConnectionServerRangeCount,					
	ConnectionServerRanges,
	ConnectionServerElementsIndex,
	ConnectionServerElements
	};

}

CPolicyServer::TCustomResult
CSocketServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
/**
Implement custom security checking for IPCs marked with TSpecialCase::ECustomCheck

Note that this only polices certain RConnection::Control(KCOLConnection) options that
are implemented within ESOCK.  NIFMAN will police the remainder of the options.

*/
	{
	// ECNControl is all we are expecting
	if(aMsg.Function() != ECNControl)
		{
		return EFail;
		}

   TInt optionLevel = aMsg.Int0();

	if (optionLevel == KCOLConnection)
		{
      TInt optionName = aMsg.Int1();

		switch(optionName)
			{
			case KCoEnumerateConnectionClients:
			case KCoGetConnectionClientInfo:
			case KCoEnumerateConnectionSockets:
			case KCoGetConnectionSocketInfo:
				{
				return (ESock::SocketServerElements[ESock::policyNetworkControl].iPolicy.CheckPolicy(aMsg)) ? EPass : EFail;
				}
			default:
				return EPass;
			}
		}

	return EPass;
	}

