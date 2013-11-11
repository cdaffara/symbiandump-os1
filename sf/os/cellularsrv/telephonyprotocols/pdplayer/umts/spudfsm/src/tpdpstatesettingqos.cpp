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
// Setting QoS state
// 
//

/**
 @file 
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstatesettingqosTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"

TInt TPdpStateSettingQoS::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::EQoSSet:
		aFsm.ChangeStateToCreatedSecondary();
		SpudManNotify(aFsm, KContextQoSSetEvent, KErrNone);
		return KErrNone;
	case PdpFsm::EQoSSetFailed:
		aFsm.ChangeStateToCreatedSecondary();
		SpudManNotify(aFsm, KContextQoSSetEvent, aErrorCode);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATESETTINGQOS_INPUT_1, "*** QOS SET FAILED ***()");
		return KErrNone;
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}
