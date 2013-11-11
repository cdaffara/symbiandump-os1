// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// PDP Connection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PPDCPR_H
#define SYMBIAN_PDPCPR_H

#include <comms-infras/corecpr.h>
#include <comms-infras/corecprstates.h>
#include "pdpcpravailabilitylistener.h"
#include "MPDPCPRBearerObserver.h"

class CPDPConnectionProvider;
class CPDPConnectionProviderFactory;

namespace PDPCprActivities
    {
    DECLARE_ACTIVITY_MAP(activityMap)
    }


#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
namespace PDPCprStates
{
typedef MeshMachine::TNodeContext<CPDPConnectionProvider, CprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TUpdateBundle, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, PDPCprStates::TContext)
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TUpdateBundle)

DECLARE_AGGREGATED_TRANSITION2(
   TUpdateBundleAndRespondWithRetrievedParams,
   TUpdateBundle,
   PRStates::TRespondWithRetrievedParams
   )
}
#endif


class CPDPConnectionProvider : public CCoreConnectionProvider, MPDPCPRBearerObserver
/** PDP connection provider

@internalTechnology
*/
    {
	friend class CPDPConnectionProviderFactory;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	friend class PDPCprStates::TUpdateBundle;
#endif

public:
    typedef CPDPConnectionProviderFactory FactoryType;
	
	static CPDPConnectionProvider* NewL(ESock::CConnectionProviderFactoryBase& aFactory);

	// from MPDPCPRBearerObserver
	virtual void BearerChangeDetectedL();

private:	
    CPDPConnectionProvider(ESock::CConnectionProviderFactoryBase& aFactory);
   
protected:
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    void StartListener();
    void StopListener();
    void UpdateBearer();
    TUint32 Bearer(TUint aDynamicCaps, RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode);
#endif
    
    virtual ~CPDPConnectionProvider();
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	void ConstructL();
private:
	CPDPCPRDynamicCapsListener* iDynamicCapsEventListener;
	CPDPCPRNetworkModeListener* iNetworkModeEventListener;
	TUint32 iBearerType;
	TBool iBearerSet;
    };

#endif //SYMBIAN_PDPCPR_H
