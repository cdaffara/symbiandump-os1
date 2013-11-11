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
//

/**
 @file SS_MMNODE.CPP
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_corepractivities.h>

using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;
using namespace ESock;

EXPORT_C void ACFMMNodeBase::ConstructL(TInt aSize)
    {
    //Preallocate space for preallocated activities
    if (aSize==KUseDefaultPreallocatedSize)
    	{
    	//Nodes will usually use the default parameter (-1) and rely on KDefaultPreallocatedActivityBufferSize.
    	//We do not use KDefaultPreallocatedActivityBufferSize as the default argument to avoid publishing of
    	//this constant or KDefaultMaxPreallocatedActivityCount/KDefaultMaxPreallocatedActivitySize (either can be
    	//freely changed at any time).
    	aSize = KDefaultMaxPreallocatedActivityCount * KDefaultMaxPreallocatedActivitySize;
    	}
    AMMNodeBase::ConstructL(aSize);
	}

EXPORT_C void ACFMMNodeBase::HandlePassThroughMessage(TNodeContextBase& aContext)
    {
	MeshMachine::AMMNodeBase::HandlePassThroughMessage(aContext);
    }
EXPORT_C NetInterfaces::TInterfaceControl* ACFMMNodeBase::FetchNodeInterfaceControlL(TInt aInterfaceId)
    {
    LOG(ESockLog::Printf(KESockMeshMachine, _L8("ACFMMNodeBase::FetchNodeInterfaceControlL: aInterfaceId = %d"), aInterfaceId));
    FetchInterfaceL(aInterfaceId); //To verify we are the right control
    return this;
    }

EXPORT_C NetInterfaces::TInterfaceControl* ACFMMNodeIdBase::FetchNodeInterfaceControlL(TInt aInterfaceId)
    {
    LOG(ESockLog::Printf(KESockMeshMachine, _L8("ACFMMNodeIdBase::FetchNodeInterfaceControlL: aInterfaceId = %d"), aInterfaceId));
    FetchInterfaceL(aInterfaceId); //To verify we are the right control
    return this;
    }


