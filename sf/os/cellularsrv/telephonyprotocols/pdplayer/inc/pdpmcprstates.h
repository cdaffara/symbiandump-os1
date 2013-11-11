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
// PDP MCPR States
// 
//

/**
 @file
 @internalComponent
*/
#ifndef PDPMCPRSTATES_H
#define PDPMCPRSTATES_H

#include <comms-infras/coremcprstates.h>
#include "PDPMessageDefn.h"

class CPdpMetaConnectionProvider;

namespace PdpMCprStates
{
typedef MeshMachine::TNodeContext<CPdpMetaConnectionProvider, MCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TProcessErrorRecoveryReq, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessErrorRecoveryReq )

	
}

#endif // PDPMCPRSTATES_H
