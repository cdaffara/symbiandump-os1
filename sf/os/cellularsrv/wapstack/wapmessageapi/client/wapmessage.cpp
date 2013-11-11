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


#include <wapmessage.h>
#include <in_iface.h>
#include <ecom/ecom.h>

/** The WAP Messaging API.  Four interfaces are defined that provide bound and fully-specified versions of WDP and Connectionless Push.
	An instantiation of each may be obtained using the CreateImplementationL() function, and must be released using Release() when no
	longer required.  Release() causes the instantiation to be deleted.
*/

// Use SWS as default WAP stack
//  If undefined th3em NWSS stack will be used as default
#define WAPUSE_SWS // since 8.0

#ifndef WAPUSE_SWS
// Symbian OS 7.0
_LIT8(KDefBoundWdpCue, "wdp/bound");
_LIT8(KDefFullSpecWdpCue, "wdp/fullyspec");
_LIT8(KDefBoundCLPushCue, "clpush/bound");
_LIT8(KDefFullSpecCLPushCue, "clpush/fullyspec");

_LIT8(KAltBoundWdpCue, "swswdp/bound");
_LIT8(KAltFullSpecWdpCue, "swswdp/fullyspec");
_LIT8(KAltBoundCLPushCue, "swsclpush/bound");
_LIT8(KAltFullSpecCLPushCue, "swsclpush/fullyspec");

#else
// Symbian OS 8.0
_LIT8(KAltBoundWdpCue, "wdp/bound");
_LIT8(KAltFullSpecWdpCue, "wdp/fullyspec");
_LIT8(KAltBoundCLPushCue, "clpush/bound");
_LIT8(KAltFullSpecCLPushCue, "clpush/fullyspec");

_LIT8(KDefBoundWdpCue, "swswdp/bound");
_LIT8(KDefFullSpecWdpCue, "swswdp/fullyspec");
_LIT8(KDefBoundCLPushCue, "swsclpush/bound");
_LIT8(KDefFullSpecCLPushCue, "swsclpush/fullyspec");
#endif

_LIT8(KBoundCLWSPCue, "swsclwsp/bound");
_LIT8(KFullSpecCLWSPCue, "swsclwsp/fullyspec");

/** Bound WDP
*/

EXPORT_C 
CWapBoundDatagramService* CWapBoundDatagramService::NewL()
	/** Allocates and creates a new CWapBoundDatagramService object. 
	* 
	* @return	A new CWapBoundDatagramService object.
	* @leave	System wide error codes
	*/
	{
	// CWapBoundDatagramService ECOM Interface UID = 101F4471
	const TUid KUidBoundWDPInterface = {0x101F4471};

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KDefBoundWdpCue);
	resolverParams.SetWildcardMatch(ETrue);

	// Get the instantiation
	TInt trapValue(0); 
	TAny* ptr = NULL;
	TRAP(trapValue, ptr = REComSession::CreateImplementationL(KUidBoundWDPInterface,
								 _FOFF(CWapBoundDatagramService, iDtor_ID_Key),
								 resolverParams));
	if(trapValue)
	{
		resolverParams.SetDataType(KAltBoundWdpCue);
		// Trying to load old wapmessage plug-in
		ptr = REComSession::CreateImplementationL(KUidBoundWDPInterface,
								 _FOFF(CWapBoundDatagramService, iDtor_ID_Key),
								 resolverParams);
	}

	return REINTERPRET_CAST(CWapBoundDatagramService*, ptr);
	}

EXPORT_C 
CWapBoundDatagramService* CWapBoundDatagramService::NewL(const TUid& aImplementation)
	/** Allocates and creates a new CWapBoundDatagramService object. 
	* 
	* @param aImplementation
	* @return					A new CWapBoundDatagramService object.
	* @leave					System wide error codes
	*/
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementation,
								 _FOFF(CWapBoundDatagramService, iDtor_ID_Key));

	return	REINTERPRET_CAST(CWapBoundDatagramService*, ptr);
	}

EXPORT_C 
CWapBoundDatagramService::~CWapBoundDatagramService()
	/** Destructor */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C 
CWapBoundDatagramService::CWapBoundDatagramService()
	{
	}

EXPORT_C 
void CWapBoundDatagramService::ConstructL()
	{
	}
	
/** Fully-specified WDP
*/

EXPORT_C 
CWapFullySpecDatagramService* CWapFullySpecDatagramService::NewL()
	/** Allocates and creates a new CWapFullySpecDatagramService object. 
	* 
	* @return	A new CWapFullySpecDatagramService object.
	* @leave	System wide error codes.
	*/
	{
	// MWapFullySpecDatagramService ECOM Interface UID = 101F4473
	const TUid KUidFullySpecDatagramInterface = {0x101F4473};

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KDefFullSpecWdpCue);
	resolverParams.SetWildcardMatch(ETrue);

	// Get the instantiation
	TInt trapValue(0); 
	TAny* ptr = NULL;
	TRAP(trapValue, ptr = REComSession::CreateImplementationL(KUidFullySpecDatagramInterface,
								 _FOFF(CWapFullySpecDatagramService, iDtor_ID_Key),
								 resolverParams));
	if(trapValue)
	{
		// Trying to load old wapmessage plug-in
		resolverParams.SetDataType(KAltFullSpecWdpCue);
		ptr = REComSession::CreateImplementationL(KUidFullySpecDatagramInterface,
								 _FOFF(CWapFullySpecDatagramService, iDtor_ID_Key),
								 resolverParams);
	}

	return REINTERPRET_CAST(CWapFullySpecDatagramService*, ptr);
	}

EXPORT_C 
CWapFullySpecDatagramService* CWapFullySpecDatagramService::NewL(const TUid& aImplementation)
	/** Allocates and creates a new CWapFullySpecDatagramService object. 
	* 
	* @param aImplementation	
	* @return					A new CWapFullySpecDatagramService object.
	* @leave					System wide error codes.
	*/
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementation,
								 _FOFF(CWapFullySpecDatagramService, iDtor_ID_Key));

	return	REINTERPRET_CAST(CWapFullySpecDatagramService*, ptr);
	}

EXPORT_C 
CWapFullySpecDatagramService::~CWapFullySpecDatagramService()
	/** Destructor */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C 
CWapFullySpecDatagramService::CWapFullySpecDatagramService()
	{
	}

EXPORT_C 
void CWapFullySpecDatagramService::ConstructL()
	{
	}

/** Bound Connection-less Push
*/

EXPORT_C 
CWapBoundCLPushService* CWapBoundCLPushService::NewL()
	/** Allocates and creates a new CWapBoundCLPushService object. 
	*
	* @return	A new <code>CWapBoundCLPushService</code> object.
	* @leave	System wide error codes.
	*/
	{
	// MWapBoundCLPushService ECOM Interface UID = 101F4475
	const TUid KUidBoundCLPushInterface = {0x101F4475};

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KDefBoundCLPushCue);
	resolverParams.SetWildcardMatch(ETrue);

	// Get the instantiation
	TInt trapValue(0); 
	TAny* ptr = NULL;
	TRAP(trapValue, ptr = REComSession::CreateImplementationL(KUidBoundCLPushInterface,
								 _FOFF(CWapBoundCLPushService, iDtor_ID_Key),
								 resolverParams));
	if(trapValue)
	{
		// Trying to load old wapmessage plug-in
		resolverParams.SetDataType(KAltBoundCLPushCue);
		ptr = REComSession::CreateImplementationL(KUidBoundCLPushInterface,
								 _FOFF(CWapBoundCLPushService, iDtor_ID_Key),
								 resolverParams);
	}
	return REINTERPRET_CAST(CWapBoundCLPushService*, ptr);
	}

EXPORT_C 
CWapBoundCLPushService* CWapBoundCLPushService::NewL(const TUid& aImplementation)
	/** Allocates and creates a new CWapBoundCLPushService object. 
	*
	* @param aImplementation	
	* @return					A new CWapBoundCLPushService object.
	* @leave					System wide error codes.
	*/
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementation,
								 _FOFF(CWapBoundCLPushService, iDtor_ID_Key));

	return	REINTERPRET_CAST(CWapBoundCLPushService*, ptr);
	}

EXPORT_C 
CWapBoundCLPushService::~CWapBoundCLPushService()
	/** Destructor */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C 
CWapBoundCLPushService::CWapBoundCLPushService()
	{
	}

EXPORT_C 
void CWapBoundCLPushService::ConstructL()
	{
	}

/** Fully-specified Connection-less Push
*/

EXPORT_C 
CWapFullySpecCLPushService* CWapFullySpecCLPushService::NewL()
	/** Allocates and creates a new CWapFullySpecCLPushService object. 
	*
	* @return	A new CWapFullySpecCLPushService object.
	* @leave	System wide error codes.
	*/
	{
	// MWapFullySpecCLPushService ECOM Interface UID = 101F4477
	const TUid KUidFullySpecCLPushInterface = {0x101F4477};

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KDefFullSpecCLPushCue);
	resolverParams.SetWildcardMatch(ETrue);

	// Get the instantiation
	TInt trapValue(0); 
	TAny* ptr = NULL;
	TRAP(trapValue, ptr = REComSession::CreateImplementationL(KUidFullySpecCLPushInterface,
								 _FOFF(CWapFullySpecCLPushService, iDtor_ID_Key),
								 resolverParams));
	if(trapValue)
	{
		// Trying to load old wapmessage plug-in
		resolverParams.SetDataType(KAltFullSpecCLPushCue);
		ptr = REComSession::CreateImplementationL(KUidFullySpecCLPushInterface,
								 _FOFF(CWapFullySpecCLPushService, iDtor_ID_Key),
								 resolverParams);
	}

	return REINTERPRET_CAST(CWapFullySpecCLPushService*, ptr);
	}

EXPORT_C 
CWapFullySpecCLPushService* CWapFullySpecCLPushService::NewL(const TUid& aImplementation)
	/** Allocates and creates a new CWapFullySpecCLPushService object. 
	*
	* @param aImplementation
	* @return					A new CWapFullySpecCLPushService object.
	* @leave					System wide error codes.
	*/
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementation,
								 _FOFF(CWapFullySpecCLPushService, iDtor_ID_Key));

	return	REINTERPRET_CAST(CWapFullySpecCLPushService*, ptr);
	}

EXPORT_C 
CWapFullySpecCLPushService::~CWapFullySpecCLPushService()
	/** Destructor */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C 
CWapFullySpecCLPushService::CWapFullySpecCLPushService()
	{
	}

EXPORT_C 
void CWapFullySpecCLPushService::ConstructL()
	{
	}

/** Bound WSP
	New interface and implementation
*/

EXPORT_C 
CWapBoundCLWSPService* CWapBoundCLWSPService::NewL()
	/** Allocates and creates a new CWapBoundCLWSPService object. 
	*
	* @return					A new CWapBoundCLWSPService object.
	* @leave					System wide error codes.
	*/
	{
	// CWapBoundWSPService ECOM Interface UID = 101FBB3B
	const TUid KUidBoundCLWSPInterface = {0x101FBB3B};

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KBoundCLWSPCue);
	resolverParams.SetWildcardMatch(ETrue);

	// Get the instantiation
	TAny* ptr = REComSession::CreateImplementationL(KUidBoundCLWSPInterface,
								 _FOFF(CWapBoundCLWSPService, iDtor_ID_Key),
								 resolverParams);

	return REINTERPRET_CAST(CWapBoundCLWSPService*, ptr);
	}

EXPORT_C 
CWapBoundCLWSPService* CWapBoundCLWSPService::NewL(const TUid& aImplementation)
	/** Allocates and creates a new CWapBoundCLWSPService object. 
	*
	* @param aImplementation
	* @return					A new CWapBoundCLWSPService object.
	* @leave					System wide error codes.
	*/
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementation,
								 _FOFF(CWapBoundCLWSPService, iDtor_ID_Key));

	return	REINTERPRET_CAST(CWapBoundCLWSPService*, ptr);
	}

EXPORT_C 
CWapBoundCLWSPService::~CWapBoundCLWSPService()
	/** Destructor */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C 
CWapBoundCLWSPService::CWapBoundCLWSPService()
	{
	}

EXPORT_C 
void CWapBoundCLWSPService::ConstructL()
	{
	}

/** Fully-specified Connection-less WSP
	New interface and implementation
*/

EXPORT_C 
CWapFullySpecCLWSPService* CWapFullySpecCLWSPService::NewL()
	/** Allocates and creates a new CWapFullySpecCLWSPService object. 
	*
	* @return					A new CWapFullySpecCLWSPService object.
	* @leave					System wide error codes.
	*/
	{
	// CWapFullySpecCLWSPService ECOM Interface UID = 101FBB3D
	const TUid KUidFullySpecCLWSPInterface = {0x101FBB3D};

	// Set resolving parameters
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KFullSpecCLWSPCue);
	resolverParams.SetWildcardMatch(ETrue);

	// Get the instantiation
	TAny* ptr = REComSession::CreateImplementationL(KUidFullySpecCLWSPInterface,
								 _FOFF(CWapFullySpecCLWSPService, iDtor_ID_Key),
								 resolverParams);

	return REINTERPRET_CAST(CWapFullySpecCLWSPService*, ptr);
	}

EXPORT_C 
CWapFullySpecCLWSPService* CWapFullySpecCLWSPService::NewL(const TUid& aImplementation)
	/** Allocates and creates a new CWapFullySpecCLWSPService object. 
	*
	* @param aImplementation
	* @return					A new CWapFullySpecCLWSPService object.
	* @leave					System wide error codes.
	*/
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementation,
								 _FOFF(CWapFullySpecCLWSPService, iDtor_ID_Key));

	return	REINTERPRET_CAST(CWapFullySpecCLWSPService*, ptr);
	}

EXPORT_C 
CWapFullySpecCLWSPService::~CWapFullySpecCLWSPService()
	/** Destructor */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C 
CWapFullySpecCLWSPService::CWapFullySpecCLWSPService()
	{
	}

EXPORT_C 
void CWapFullySpecCLWSPService::ConstructL()
	{
	}

   
EXPORT_C TInt CWapMessageUtils::GetLocalAddressesL(RArray<Wap::TAddressInfo>& aAddressInfo)
	/** Gets a list of all the available network interface addresses.
	*
	* @param aAddressInfo	On return, an array of the available network interface addresses.
	* @return				KErrNone if successful, KErrNotFound if there are no addresses available. */
	{
	Wap::TAddressInfo addrInfo;

	// Check if the esock ini file has been parsed
//	User::LeaveIfError(Nifman::CheckIniConfig());

	// The list of interfaces is available from a RSocket.GetOpt
	TAutoClose<RSocketServ> sockServer;
	User::LeaveIfError(sockServer.iObj.Connect());
	sockServer.PushL();

	TAutoClose<RSocket> sock;
	User::LeaveIfError(sock.iObj.Open(sockServer.iObj, _L("udp")));
	sock.PushL();

	User::LeaveIfError(sock.iObj.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));

	TPckgBuf<TSoInetInterfaceInfo> info, next;

	TInt res=sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, info);
	if(res!=KErrNone)
		User::Leave(res);

	TInt validAddr = KErrNotFound;

	while(res==KErrNone)
		{
		res=sock.iObj.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, next);

		// Only consider the address is its up, and is point to point.
		// (TODO-would it be right to include pending state addresses too????)
		if ( (info().iState == EIfUp) &&
			 (info().iFeatures & KIfIsPointToPoint) )
			{
			// Just want the interface name
			// = info().iName;
			// and the address
			// = info().iAddress;
			validAddr = KErrNone;	   
			
			addrInfo.iAddress = info().iAddress;
			addrInfo.iName = info().iName;
			aAddressInfo.Append(addrInfo);
			}
		}

	sock.Pop();
	sockServer.Pop();

	return validAddr;
	}
