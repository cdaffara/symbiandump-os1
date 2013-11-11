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

/**
 @file
 @internalTechnology
*/

#include "sd_apiextension.h"
#include "sd_apiextensionrouting.h"

using namespace Den;

static const TUint32 KDenRoutingImplementationUid = 0x12348866; //TODO: allocate an UID

enum
	{
	EIpcExtensionRoutingMessageBase = 1,
	EIpcExtensionRoutingMessageFromSession,
	EIpcExtensionRoutingMessageFromSubSession
	};

//
//TApiExtRoutingMessageBase

EXPORT_START_ATTRIBUTE_TABLE_NO_FN(TIpcExtensionRoutingMessageBase, 0, EIpcExtensionRoutingMessageBase)
END_ATTRIBUTE_TABLE_BASE( TSignalBase, ESignalBase )

EXPORT_C void TIpcExtensionRoutingMessageBase::RegisterMessagesAtDestinationL(AIpcExtensionInterfaceBase& aInterface)
	{
	aInterface.RegisterMessagesL();
	}

EXPORT_C void TIpcExtensionRoutingMessageBase::DeregisterMessagesAtDestination(AIpcExtensionInterfaceBase& aInterface)
	{
	aInterface.DeregisterMessages();
	}

//
//TApiExtRoutingMessageFromSession

EXPORT_START_ATTRIBUTE_TABLE_NO_FN(TIpcExtensionRoutingMessageFromSession, KDenRoutingImplementationUid, EIpcExtensionRoutingMessageFromSession)
END_ATTRIBUTE_TABLE_BASE( TIpcExtensionRoutingMessageBase, EIpcExtensionRoutingMessageBase )


//
//TApiExtRoutingMessageFromSubSession

EXPORT_START_ATTRIBUTE_TABLE_NO_FN(TIpcExtensionRoutingMessageFromSubSession, KDenRoutingImplementationUid, EIpcExtensionRoutingMessageFromSubSession)
END_ATTRIBUTE_TABLE_BASE( TIpcExtensionRoutingMessageBase, EIpcExtensionRoutingMessageBase )



