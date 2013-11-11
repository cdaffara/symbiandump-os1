// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "susutilserver.h"
#include "susutilsession.h"
#include "susutilclisrv.h"
#include "suspluginloader.h"
#include "suspluginframe.h"
#include "suspanic.h"
#include "ssmdebug.h"


// ------------------- Policy Server Security Setup ----------------------

const TUint  KRangeCount = 3;
const TInt   KRanges[KRangeCount] =
	{
	0,
	KSusUtilServFirstUsedOpCode,	//range: KFirstUsedSsmOpCode...(EEndOfSusUtilOpCodes-1) inclusive
#ifdef _DEBUG
	EEndOfSusUtilOpCodes+1	// this op-code is used to shutdown server in debug builds
#else
	EEndOfSusUtilOpCodes
#endif //_DEBUG
	};

/**
 Maps to index 0 in the array KPolicyElements
 */
const TInt KCapabilitiesInSsmServer = 0;

/**
 Specifies the appropriate action to take for each range in KRanges.
 The n:th element of KElementsIndex specifies the appropriate action to take for the n:th range in KRanges.
 */
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::ENotSupported,
	KCapabilitiesInSsmServer,
	CPolicyServer::ENotSupported,
	};

/**
 Array containing the different security checks performed by this server
 */
const CPolicyServer::TPolicyElement KPolicyElements[] =
	{
	{_INIT_SECURITY_POLICY_C6(ECapabilityDiskAdmin, ECapabilityPowerMgmt, ECapabilityProtServ, ECapabilitySwEvent, ECapabilityWriteDeviceData, ECapabilityReadDeviceData), CPolicyServer::EFailClient} //lint !e778 suppress Constant expression evaluates to 0 in operation '+'
	};

/**
 Setup a security policy which require all caps used by the SsmServer for all requests
 including creating a connection. We don't yet have a usecase for anyone else than
 SsmServer using SusUtilServer. The caller's SID will also be matched against SsmServer's
 SID in each ServiceL call.
 */
const CPolicyServer::TPolicy KSusUtilServerPolicy =
	{
	KCapabilitiesInSsmServer,	// map connection attempts as well to index [0] in KPolicyElements[]
	KRangeCount,
	KRanges,
	KElementsIndex,
	KPolicyElements
	};

// ------------------- SusUtilServer Implementation ----------------------

/**
 */
CSusUtilServer::CSusUtilServer(TSecureId aClientSecureId)
	: CPolicyServer(EPriorityStandard, KSusUtilServerPolicy), iClientSecureId(aClientSecureId)
	{
	} //lint !e1746 suppress parameter 'aClientSecureId' could be made const reference

/**
 */
CSusUtilServer::~CSusUtilServer()
	{
	const TInt ignore = User::SetCritical(User::ENotCritical);
	iLoadedPlugins.ResetAndDestroy();
	} //lint !e529 suppress Symbol 'ignore' not subsequently referenced

/**
 Factory method used by production code.
 */
CSusUtilServer* CSusUtilServer::NewLC( )
	{
	TSecureId uidOfSysStateMgr(0x2000D75B); //The SID of the only process that will allowed to use this server.
	return CSusUtilServer::NewLC(KSusUtilServerName, uidOfSysStateMgr);
	}

/**
 Factory method to be used by testcode only.

 @param aServerName	A non-production server-name is required if the real server was started
 					during TechView startup.
 @param aClientSecureId The SID of the only process that will allowed to use this server.
 @internalComponent
 @released
 */
CSusUtilServer* CSusUtilServer::NewLC(const TDesC& aServerName, TSecureId aClientSecureId)
	{
	CSusUtilServer* server = new(ELeave) CSusUtilServer(aClientSecureId);
	CleanupStack::PushL(server);
	server->ConstructL(aServerName);
	return server;
	} //lint !e1746 suppress parameter 'aClientSecureId' could be made const reference

/**
 Second-phase construct and start this server. Will rename current thread to
 KSusUtilServerName and set it to ESystemCritical.
 @internalComponent
 @released
 */
void CSusUtilServer::ConstructL(const TDesC& aServerName)
	{
	__ASSERT_ALWAYS( KErrNone == User::SetCritical(User::ESystemCritical),
			User::Panic(KPanicSsmSus, EUtilServerError1));

	__ASSERT_ALWAYS( KErrNone == User::RenameThread(KSusUtilServerName),
			User::Panic(KPanicSsmSus, EUtilServerError2));

	const TInt err = Start(aServerName);
	SSMLOGLEAVEIFERROR( err );
	}

/**
 Create a new session. Boilerplate code for Symbian Client-Server framework.
 @internalComponent
 @released
 */
CSession2* CSusUtilServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	// Check we're the right version
	TVersion v(KSusUtilServMajorVersionNumber, KSusUtilServMinorVersionNumber,
			KSusUtilServBuildVersionNumber);

	if ( !User::QueryVersionSupported(v,aVersion) )
		{
		User::Leave (KErrNotSupported );
		}

	// version supported, go ahead
	return CSusUtilSession::NewL ( );
	}

/**
 The SID of the only process that will allowed to use this server.
 @internalComponent
 @released
 */
TSecureId CSusUtilServer::ClientSecureId() const
	{
	return iClientSecureId;
	}

/**
 Attempt to load the DLL file named in aSupInfo. If successful, instantiate, initialize and start
 the contained uitility plugin using the ordinal function specified in aSupInfo and the MSsmUtility
 interface.
 @internalComponent
 @released
 */
void CSusUtilServer::LoadUtilityPluginL(TSsmSupInfo& aSupInfo)
	{
	RLibrary lib;
	SusPluginLoader::LoadDllFileLC(lib, aSupInfo); // open handle on CleanupStack

	if (KErrNotFound != IndexOf(lib, aSupInfo.NewLOrdinal()))
		{
		//will leave in release builds as well
		SSMLOGLEAVE(KErrAlreadyExists); //lint !e527 Unreachable
		}

	const TInt newL = aSupInfo.NewLOrdinal();
	CSusPluginFrame* plugin = SusPluginLoader::CreatePluginLC(lib, newL);
	iLoadedPlugins.AppendL(plugin);
	CleanupStack::Pop(plugin);
	plugin->SetLibrary(lib); // takes ownership of open library handle

	CleanupStack::Pop(&lib);
	}
 
/**
 Check if a Utility Plugin is already loaded.

 @param 	A RLibrary based on the DLL file which we want to investigate if its already
 			included in iLoadedPlugins.
 @return 	The index of any CSusPluginFrame in iLoadedPlugins which is based on the same DLL file
 			as given in parameter aLibrary.
 @internalComponent
 @released
 */
TInt CSusUtilServer::IndexOf(const RLibrary& aLibrary, TInt aNewLOrdinal) const
	{
	const TFileName nameToLookFor = aLibrary.FileName();
	const TInt count = iLoadedPlugins.Count();
	for(TInt i=0; i<count; i++)
		{
		if(0 == nameToLookFor.CompareF(iLoadedPlugins[i]->FileName()) && aNewLOrdinal == iLoadedPlugins[i]->NewLOrdinal())
			{
			return i;
			}
		}
	return KErrNotFound;
	}

/**
 @return 	0 or a positive integer if a plugin was released and deleted, otherwise KErrNotFound
 @internalComponent
 @released
 */
TInt CSusUtilServer::UnLoadUtilityPluginL(TSsmSupInfo& aSupInfo)
	{
	RLibrary lib;
	SusPluginLoader::LoadDllFileLC(lib, aSupInfo); // open handle on CleanupStack
	const TInt index = IndexOf(lib, aSupInfo.NewLOrdinal());
	if( index > KErrNotFound )
		{
		CSusPluginFrame* frame = iLoadedPlugins[index];
		iLoadedPlugins.Remove(index);
		delete frame;
		}
	CleanupStack::PopAndDestroy(&lib);
	return index;
	}

#ifdef _DEBUG
// Used for testing memory leaks on server.
void CSusUtilServer::CompressPluginArray()
	{
	iLoadedPlugins.Compress();
	}
#endif
