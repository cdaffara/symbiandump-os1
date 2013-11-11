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
// ESock external utils 
// 
//

/**
 @file
*/

#include <ss_pman.h>
#include <ss_std.h>
#include <comms-infras/ss_log.h>

EXPORT_C CProtocolBase* SocketServExt::FindAndLoadProtocolL(const TDesC& aName, TProtocolType aType)
/**
Find a protocol by name and load it.
Caller is responsible for ultimately deleting the protocol

*/
	{
	return ProtocolManager::FindAndLoadProtocolL(aName, aType);
	}

EXPORT_C CProtocolBase* SocketServExt::FindAndLoadProtocolL(TUint aAddrFamily,TUint aSockType,TUint aProtocol)
/**
Find a protocol by address family, socket type and protocol ID and load it.
Caller is responsible for ultimately deleting the protocol
*/
	{
	return ProtocolManager::FindAndLoadProtocolL(aAddrFamily, aSockType, aProtocol);
	}

EXPORT_C void SocketServExt::InstallExtensionL(const TDesC& aDllName, const TDesC& aArgs)
/**
Installs an Esock extension dll

*/
	{
	SocketServer::InstallExtensionL(aDllName, aArgs);
	}

EXPORT_C void SocketServExt::OpenSession()
  	{
	// Before 9.1 this provided extensions (ie NifMan) the ability to defer spontaneous shutdown. Now obsolete but retained for compatibility
	}

EXPORT_C void SocketServExt::CloseSession()
	{
	// Before 9.1 this provided extensions (ie NifMan) the ability to defer spontaneous shutdown. Now obsolete but retained for compatibility
	}

EXPORT_C void SocketServExt::InstallSchedulerWaitHook(TCallBack* /*aCall*/)
	{
//	CSocketScheduler::SetWaitHook(aCall);
	}

EXPORT_C CSocketServExtBase::CSocketServExtBase()
	{
	__DECLARE_NAME(_S("CSocketServExtBase"));
	}

EXPORT_C CSocketServExtBase::~CSocketServExtBase()
	{
//	if (iExtRef)
//		delete iExtRef;
	}

CSocketServExtRef::CSocketServExtRef()
	{
	__DECLARE_NAME(_S("CSocketServExtRef"));
	iExtLink.iPrev = &iExtLink;
	iExtLink.iNext = &iExtLink;
	}

CSocketServExtRef::~CSocketServExtRef()
	{
	iExtLink.Deque();
	}

static void CloseLibrary(TAny* aLib)
	{
	
	((RLibrary*)aLib)->Close();
	}

void CSocketServExtRef::InstallL(const TDesC& aDllName, const TDesC& aArgs)
/**
installs an extension
*/
	{
	LOG(TBuf8<64> buf8);
	LOG(buf8.Copy(aDllName));
	LOG(ESockLog::Printf(_L8("Loading extension '%S'"), &buf8));
	User::LeaveIfError(iLibrary.Load(aDllName));
	CleanupStack::PushL(TCleanupItem(CloseLibrary, &iLibrary));
	TSocketServExtInstallFn entry = (TSocketServExtInstallFn)iLibrary.Lookup(1);
	CSocketServExtBase* sx = entry();
	User::LeaveIfNull(sx);
	CleanupStack::PushL(sx);
	sx->InstallL(aArgs);
	iExtension = sx;
	sx->iExtRef = this;
	CleanupStack::Pop(2);
	}

void CSocketServExtRef::Remove()
	{
	if (iExtension)
		iExtension->Remove();
	}

void CSocketServExtRef::Close()
	{
	if (iExtension)
		delete iExtension;
	iLibrary.Close();
	}

