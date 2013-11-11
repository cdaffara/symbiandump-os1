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
// Activating mbms context state
// 
//

/**
 @file 
 @internalComponent
*/
 


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstateactivatingmbmsTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"

TInt TPdpStateActivatingMbms::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::EPdpActivated:
		// When a mbms context is activated, the context status must be updated before sending the activated event
		// Normally this change in status is updated by a notify context status change event
		aFsm.Set(RPacketContext::EStatusActive);
		aFsm.ChangeStateToCreatedMbms();//there is no Qos to be set
		SpudManNotify (aFsm, KContextActivateEvent, KErrNone);
		return KErrNone;
	case PdpFsm::EPdpActivatedFailed:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATEACTIVATINGMBMS_INPUT_1, "*** PDP MBMS CONTEXT ACTIVATED EVENT FAILED ***");
		aFsm.ChangeStateToCreatedMbms();
		SpudManNotify (aFsm, KContextActivateEvent, aErrorCode);
		return KErrNone;
	// no default
	}

	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}
