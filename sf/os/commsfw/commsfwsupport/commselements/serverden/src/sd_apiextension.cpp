// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SS_API_EXT.CPP
// TODO PREQ1116 disabled to get catchup to build #define SYMBIAN_NETWORKING_UPS
// 
//

/**
 @file
 @released
*/

#include "sd_apiextension.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemSvrDenApiXC, "ElemSvrDenApiXC");
#endif

using namespace Elements;
using namespace Den;
using namespace Messages;
using namespace Meta;

//
// CExtItfMsgPluginInfo
//

EXPORT_C CExtItfMsgPluginInfo* CExtItfMsgPluginInfo::NewInstanceL(const STypeId& aTypeId)
	{
	TUid destroyUid;
	CExtItfMsgPluginInfo* obj = reinterpret_cast<CExtItfMsgPluginInfo*>(REComSession::CreateImplementationL(aTypeId.iUid, destroyUid, (TAny*)aTypeId.iType));
	obj->iDestroyUid = destroyUid;
	return obj;
	}

EXPORT_C CExtItfMsgPluginInfo::~CExtItfMsgPluginInfo()
	{
	__ASSERT_DEBUG(iDestroyUid.iUid!=0, User::Panic(KSpecAssert_ElemSvrDenApiXC, 1));
	REComSession::DestroyedImplementation(iDestroyUid);
	}


//
// AIpcExtensionInterfaceBase
//
EXPORT_C AIpcExtensionInterfaceBase::AIpcExtensionInterfaceBase()
:	iMsgPluginInfo(NULL)
	{
	}

void AIpcExtensionInterfaceBase::RegisterMessagesL()
	{
	//RegisterInterfaceL is reference counted, make sure you always call
	//it from RegisterMessagesL to avoid premature deregistration scenarios.
	Meta::STypeId msgTid = MsgImplTid();
	CExtItfMsgPluginInfo* msgPluginInfo = iMsgPluginInfo;
	if (iMsgPluginInfo==NULL)
		{
		__ASSERT_DEBUG(msgTid.iUid.iUid!=0, User::Panic(KSpecAssert_ElemSvrDenApiXC, 2));
		msgPluginInfo = static_cast<CExtItfMsgPluginInfo*>(CExtItfMsgPluginInfo::NewInstanceL(msgTid));
		CleanupStack::PushL(msgPluginInfo);
		}

	const TImplementationProxy* implProxyTable = msgPluginInfo->ImplementationProxy();
	TlsGlobals::Get().RegisterInterfaceL(msgTid.iUid, msgPluginInfo->ImplementationProxyTableSize(), implProxyTable);
	if (iMsgPluginInfo==NULL)
		{
		CleanupStack::Pop(msgPluginInfo);
		iMsgPluginInfo = msgPluginInfo; //Set only when all done
		}
	}

void AIpcExtensionInterfaceBase::DeregisterMessages()
	{
	TlsGlobals::Get().DeregisterInterface(MsgImplTid().iUid);
	delete iMsgPluginInfo;
	iMsgPluginInfo = NULL;
	}


