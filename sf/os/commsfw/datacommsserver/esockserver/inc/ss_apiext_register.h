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

/**
 @file
 @internalTechnology
*/

#if !defined(SS_APIEXT_REGISTER_H_INCLUDED)
#define SS_APIEXT_REGISTER_H_INCLUDED

#include <comms-infras/api_ext_list.h>
#include <comms-infras/api_ext_msg.h>
#include <elements/nm_common.h>

class CSockManData;

namespace ESock
{

class CExtItfMsgPluginInfo;
class TCommsApiExtRegister;

/**
	@internalTechnology

	Utility class
*/
class TCommsApiExtTable
	{
	friend class TCommsApiExtRegister;

private:
	explicit TCommsApiExtTable(TInt aExtensionId, const Meta::STypeId& aMsgImplTid);

	void AddClientL(TSubSessionUniqueId aClientId, Messages::CGlobals& aSockManData);
	TBool RemoveClient(TSubSessionUniqueId aClientId, Messages::CGlobals& aSockManData);
	static void DeregisterInterfaceOnCleanup(TAny* aTid);

private:
	const TInt iExtensionId;
	const Meta::STypeId iMsgImplTid;
	CExtItfMsgPluginInfo* iMsgPluginInfo;
	RArray<TSubSessionUniqueId> iClients; //Extensions opened by this client
	};

/**
	@internalTechnology

	Utility class
*/
class TCommsApiExtRegister
	{
public:
	explicit TCommsApiExtRegister(Messages::CGlobals& aSockManData);
	~TCommsApiExtRegister();
	void RegisterInterfaceL(TInt aExtItfId, const Meta::STypeId& aMsgImplTid, TSubSessionUniqueId aClientId);
	//Returns next client on this interface if any
	TSubSessionUniqueId FirstClient(TInt aExtItfId);
	TSubSessionUniqueId DeRegisterInterface(TInt aExtItfId, TSubSessionUniqueId aClientId);

private:
	TInt FindApiExtClientsTable(TInt aExtItfId);

private:
	Messages::CGlobals& iGlobals;
	RArray<TCommsApiExtTable> iApiExtList;
	};

} // namespace ESock

#endif // SS_APIEXT_REGISTER_H_INCLUDED

