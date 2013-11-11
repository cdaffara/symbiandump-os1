// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Activating secondary context state
// 
//

/**
 @file 
 @internalComponent
*/
 


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstateactivatingsecondaryTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"

TInt TPdpStateActivatingSecondary::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::EPdpActivated:
		// When a secondary context is activated, the context status must be updated before sending the activated event
		// Normally this change in status is updated by a notify context status change event
		aFsm.Set(RPacketContext::EStatusActive);
		aFsm.ChangeStateToGettingNegQoS();  // Context was activated, but we don't necessarily have negotiated QoS yet; 
                       					    // a TSY might wait for a successfull activation to report it to Etel. 
		SpudManNotify (aFsm, KContextActivateEvent, KErrNone);
		return KErrNone;
	case PdpFsm::EPdpActivatedFailed:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATEACTIVATINGSECONDARY_INPUT_1, "*** PDP SECONDARY CONTEXT ACTIVATION FAILED ***");
		aFsm.ChangeStateToCreatedSecondary();
		SpudManNotify (aFsm, KContextActivateEvent, aErrorCode);
		return KErrNone;
	// no default
	}

	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}
