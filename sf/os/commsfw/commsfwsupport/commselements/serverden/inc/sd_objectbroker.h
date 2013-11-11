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
 @internalComponent
*/

#ifndef SYMBAN_DEN_OBJECT_BROKER_H
#define SYMBAN_DEN_OBJECT_BROKER_H

#include <elements/nm_node.h>
#include <elements/cftransport.h>

namespace Den
{

//
//CCommonObjectBroker
class CCommonObjectBroker : public CBase,
                            public Messages::ANodeIdBase
/**
@internalComponent
*/
    {
    // Only the main thread is intended to host the global object broker
	friend class CCommonPitBoss;

public:
	IMPORT_C ~CCommonObjectBroker();
	IMPORT_C void CleanupDeadWorker();
	IMPORT_C void UpdateWorkerInfo(CommsFW::TWorkerId aWorkerId, const Messages::TClientType& aClientType, const Messages::TNodeId& aFactoryId);

	const Messages::TNodeId& Id () const
		{
		return ANodeId::Id ();			
		}
protected:
    IMPORT_C void ForwardMesasge(const Messages::TRuntimeCtxId& aSender, const Messages::TSignalBase& aMessage, const Messages::TClientType& aClientType);

protected:
	IMPORT_C CCommonObjectBroker();
    };

} //namespace Den

#endif
//SYMBAN_DEN_OBJECT_BROKER_H


