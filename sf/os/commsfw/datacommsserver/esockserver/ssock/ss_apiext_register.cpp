// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ss_apiext_register.h"
#include <ecom/ecom.h>
#include <comms-infras/ss_thread.h>
#include <ss_glob.h>
#include <comms-infras/ss_api_ext.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockspxtrg, "ESockSSockspxtrg");
#endif

using namespace ESock;
using namespace Messages;

TCommsApiExtTable::TCommsApiExtTable(TInt aExtensionId, const Meta::STypeId& aMsgImplTid)
:	iExtensionId(aExtensionId),
	iMsgImplTid(aMsgImplTid),
	iMsgPluginInfo(NULL)
	{
	}

//static
void TCommsApiExtTable::DeregisterInterfaceOnCleanup(TAny* aTid)
	{
	Meta::STypeId* tid = static_cast<Meta::STypeId*>(aTid);
	TlsGlobals::Get().DeregisterInterface(tid->iUid);
	}

void TCommsApiExtTable::AddClientL(TSubSessionUniqueId aClientId, CGlobals& aGlobals)
	{
	__ASSERT_DEBUG(iMsgImplTid.iUid.iUid!=0, User::Panic(KSpecAssert_ESockSSockspxtrg, 1));
	User::LeaveIfError(iClients.FindInOrder(aClientId)!=KErrNotFound? KErrAlreadyExists : KErrNone);

	//For the first client we need to register messages
	TBool isFirst = iClients.Count()==0;
	if (isFirst)
		{
		__ASSERT_DEBUG(iMsgPluginInfo==NULL, User::Panic(KSpecAssert_ESockSSockspxtrg, 2));
		iMsgPluginInfo = CExtItfMsgPluginInfo::NewInstanceL(iMsgImplTid);
		CleanupStack::PushL(iMsgPluginInfo);

		aGlobals.RegisterInterfaceL(iMsgImplTid.iUid, iMsgPluginInfo->ImplementationProxyTableSize(), iMsgPluginInfo->ImplementationProxy());
		CleanupStack::PushL(TCleanupItem(&TCommsApiExtTable::DeregisterInterfaceOnCleanup, (TAny*)&iMsgImplTid));
		}

	iClients.InsertInOrderL(aClientId);

	if (isFirst)
		{
		CleanupStack::Pop(); //DeregisterInterfaceOnCleanup
		CleanupStack::Pop(iMsgPluginInfo);
		}
	}

//If aClientId==0 we will remove all interfaces for all clients!
TBool TCommsApiExtTable::RemoveClient(TSubSessionUniqueId aClientId, CGlobals& aGlobals)
	{
	TInt index = KErrNotFound;
	if (aClientId!=0)
		{
		index = iClients.FindInOrder(aClientId);
		if(index!=KErrNotFound)
			{
			iClients.Remove(index);
			}
		}
	else
		{
		iClients.Reset();
		}

	//If no clients, deregister messages
	if (iClients.Count()==0)
		{
		iClients.Close();
		aGlobals.DeregisterInterface(iMsgImplTid.iUid);
		delete iMsgPluginInfo;
		iMsgPluginInfo = NULL;
		index = 0;
		}

	if (index != KErrNotFound && index < iClients.Count())
		{
		return iClients[index];
		}

	return EFalse;
	}


TCommsApiExtRegister::TCommsApiExtRegister(CGlobals& aGlobals)
:	iGlobals(aGlobals)
	{
	}

TCommsApiExtRegister::~TCommsApiExtRegister()
	{
	//We should not have anything to cleanup here, if we do, some interface hasn't been closed properly
	__ASSERT_DEBUG(iApiExtList.Count()==0, User::Panic(KSpecAssert_ESockSSockspxtrg, 3)); //Panic in debug
	for (TInt i = iApiExtList.Count() - 1; i >= 0; --i)
		{
		TCommsApiExtTable& ref = iApiExtList[i];
		if (!ref.RemoveClient(0, iGlobals))
			{
			iApiExtList.Remove(i);
			}
		}
	iApiExtList.Close();
	}

TInt TCommsApiExtRegister::FindApiExtClientsTable(TInt aExtItfId)
	{
	TCommsApiExtTable entry(aExtItfId, Meta::STypeId::CreateSTypeId());
	return iApiExtList.FindInUnsignedKeyOrder(entry);
	}

void TCommsApiExtRegister::RegisterInterfaceL(TInt aExtItfId, const Meta::STypeId& aMsgImplTid, TSubSessionUniqueId aClientId)
	{
	TInt index = FindApiExtClientsTable(aExtItfId);
	if (index!=KErrNotFound)
		{
		TCommsApiExtTable& ref = iApiExtList[index];
		ref.AddClientL(aClientId, iGlobals);
		}
	else
		{
		TCommsApiExtTable entry(aExtItfId, aMsgImplTid);
		entry.AddClientL(aClientId, iGlobals);
		if (iApiExtList.InsertInUnsignedKeyOrder(entry)!=KErrNone)
			{
			entry.RemoveClient(aClientId, iGlobals);
			}
		}
	}

TSubSessionUniqueId TCommsApiExtRegister::FirstClient(TInt aExtItfId)
	{
	TCommsApiExtTable entry(aExtItfId, Meta::STypeId::CreateSTypeId());
	TInt index = iApiExtList.FindInUnsignedKeyOrder(entry);
	TSubSessionUniqueId firstClient = 0;
	if (index!=KErrNotFound)
		{
		TCommsApiExtTable& ref = iApiExtList[index];
		__ASSERT_DEBUG(ref.iClients.Count(), User::Panic(KSpecAssert_ESockSSockspxtrg, 4)); //Would not be here otherwise
		firstClient = ref.iClients[0];
		}
	return firstClient;
	}

//If aClientId==0 we will deregister this itf for all all clients!
TSubSessionUniqueId TCommsApiExtRegister::DeRegisterInterface(TInt aExtItfId, TSubSessionUniqueId aClientId)
	{
	TCommsApiExtTable entry(aExtItfId, Meta::STypeId::CreateSTypeId());
	TInt index = iApiExtList.FindInUnsignedKeyOrder(entry);
	TSubSessionUniqueId nextClient = 0;
	if (index!=KErrNotFound)
		{
		TCommsApiExtTable& ref = iApiExtList[index];
		nextClient = ref.RemoveClient(aClientId, iGlobals);
		if (nextClient==0)
			{
			iApiExtList.Remove(index);
			}
		}
	return nextClient;
	}


