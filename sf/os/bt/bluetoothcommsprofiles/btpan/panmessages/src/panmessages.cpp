// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "panmessages.h"
#include <es_sock.h>
#include <ss_glob.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/metatype.h>
#include <elements/nm_common.h>

// PAN Message _Signatures_ (NOT messages)
enum EMessageTypeId 
	{
	ESigInternalSocket              = 0,
	ESigBool                        = 1,
	ESigSetupConnectionResponse     = 2,
	ESigUuidUuid                    = 3,
	ESigPanRolePanRole              = 4,
	};


EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigInternalSocket, KPanMessagesImplementationUid, ESigInternalSocket)
	REGISTER_ATTRIBUTE(TSigInternalSocket, iSocket, TMeta<RInternalSocket>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigBool, KPanMessagesImplementationUid, ESigInternalSocket)
	REGISTER_ATTRIBUTE(TSigBool, iValue, TMeta<TBool>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigSetupConnectionResponse, KPanMessagesImplementationUid, ESigInternalSocket)
	REGISTER_ATTRIBUTE(TSigSetupConnectionResponse, iSetupResponse, TMeta<TBnepSetupConnectionResponseMessage>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigUuidUuid, KPanMessagesImplementationUid, ESigUuidUuid)
	REGISTER_ATTRIBUTE(TSigUuidUuid, iRequestedLocalRole, TMeta<TUUID>)
	REGISTER_ATTRIBUTE(TSigUuidUuid, iRequestedRemoteRole, TMeta<TUUID>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TSigPanRolePanRole, KPanMessagesImplementationUid, ESigPanRolePanRole)
	REGISTER_ATTRIBUTE(TSigPanRolePanRole, iRequestedLocalRole, TMeta<TBluetoothPanRole>)
	REGISTER_ATTRIBUTE(TSigPanRolePanRole, iRequestedRemoteRole, TMeta<TBluetoothPanRole>)
END_ATTRIBUTE_TABLE_BASE(TSignatureBase, 0)


DEFINE_MVIP_CTR(TSigInternalSocket);
DEFINE_MVIP_CTR(TSigBool);
DEFINE_MVIP_CTR(TSigSetupConnectionResponse);
DEFINE_MVIP_CTR(TSigUuidUuid);
DEFINE_MVIP_CTR(TSigPanRolePanRole);


const TImplementationProxy SignatureImplementationTable[] = 
	{
	//NOTE: Entries must be sorted for the binary search to work efficiently!
	MVIP_CTR_ENTRY(ESigInternalSocket, TSigInternalSocket),     // 0
	MVIP_CTR_ENTRY(ESigBool, TSigBool),                         // 1
	MVIP_CTR_ENTRY(ESigSetupConnectionResponse, TSigSetupConnectionResponse),   // 2
	MVIP_CTR_ENTRY(ESigUuidUuid, TSigUuidUuid),                 // 3
	MVIP_CTR_ENTRY(ESigPanRolePanRole, TSigPanRolePanRole),     // 4
	};


EXPORT_C void TPanMessage::RegisterL()
	{
	Messages::TlsGlobals::Get().RegisterInterfaceL(TUid::Uid(KPanMessagesImplementationUid), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	}

EXPORT_C void TPanMessage::DeRegister()
	{
	Messages::TlsGlobals::Get().DeregisterInterface(TUid::Uid(KPanMessagesImplementationUid));	
	}

