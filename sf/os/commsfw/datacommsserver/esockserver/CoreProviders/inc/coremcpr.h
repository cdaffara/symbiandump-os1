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
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_COREMCPR_H
#define SYMBIAN_COREMCPR_H

#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/coremcpractivities.h>

namespace MCprStates
{
	class TRequestCommsBinder;
    class TSendProvision;
    class TSelectNextLayer;
    class TAwaitingSelectComplete;
    class TSendFinalSelectComplete;
    class TAwaitingSelectNextLayer;
    class TAwaitingSelectNextLayerSuper;
}

//
//CMetaConnectionProvider
class CCoreMetaConnectionProvider : public ESock::CMetaConnectionProviderBase
    {
	friend class MCprStates::TRequestCommsBinder;
    friend class MCprStates::TSendProvision;
    friend class MCprStates::TSelectNextLayer;
    friend class MCprStates::TAwaitingSelectComplete;
    friend class MCprStates::TSendFinalSelectComplete;
    friend class MCprStates::TAwaitingSelectNextLayer;
    friend class MCprStates::TAwaitingSelectNextLayerSuper;

public:
	IMPORT_C virtual ~CCoreMetaConnectionProvider();
	IMPORT_C virtual void StartAvailabilityMonitoringL(const Messages::TNodeCtxId& aAvailabilityActivity); //Default implementation, usually empty on the mid-tier MCprs
	IMPORT_C virtual void CancelAvailabilityMonitoring(); //Default implementation, usually empty on the mid-tier MCprs
	

protected:
    IMPORT_C CCoreMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                         const ESock::TProviderInfo& aProviderInfo,
                                         const MeshMachine::TNodeActivityMap& aActivityMap);

    IMPORT_C void Received(MeshMachine::TNodeContextBase& aContext);
    IMPORT_C void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	TUint iIsSelectionCompleted : 1;
    };

#endif
//SYMBIAN_COREMCPR_H

