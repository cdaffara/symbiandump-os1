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
 @file ESockDebugMessages.cpp
*/

#include "ESockDebugMessages.h"

#include <ss_glob.h>
#include <comms-infras/ss_thread.h>
#include <ss_std.h>
#include <comms-infras/netmessages.h>



namespace ESockDebug
{

namespace MemoryDebug
{

enum
	{
	ECheckFailNextMsg = 0,
	};

EXPORT_C TInt TCheckFailNextMsg::DispatchL(CSockSession* /*aHostSession*/)
	{
	return KErrNotSupported;
	}

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(TCheckFailNextMsg, KImplementationUid, ECheckFailNextMsg)
END_ATTRIBUTE_TABLE()

DEFINE_MVIP_CTR(TCheckFailNextMsg);

const TImplementationProxy ImplementationTable[] = 
	{
	MVIP_CTR_ENTRY(ECheckFailNextMsg, TCheckFailNextMsg),
	};

EXPORT_C TInt ImplementationProxyTableSize()
	{
	return (sizeof(ImplementationTable) / sizeof(ImplementationTable[0]));
	};

EXPORT_C const TImplementationProxy* ImplementationProxyTable()
	{
	return ImplementationTable;
	};

}	// namespace ESockDebug::MemoryDebug



}	// namespace ESockDebug

