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
// Creating secondary context state
// 
//

/**
 @file 
 @internalComponent
*/
 


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstatecreatingSecondaryTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "PDPFSM.h"
#include "cpdpfsm.h"

TInt TPdpStateCreatingSecondary::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::E2ryPdpContextCreated:
		aFsm.ChangeStateToCreatedSecondary();
		SpudManNotify(aFsm, KSecondaryContextCreated, KErrNone);
		return KErrNone;
	case PdpFsm::E2ryPdpContextCreatedFailed:
		iErrorCode = aErrorCode;
		EtelDriverInput(aFsm, EtelDriver::EContextDelete);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATECREATINGSECONDARY_INPUT_1, "**** SECONDARY PDP CONTEXT CREATE FAILED ****");
		return KErrNone;

	case PdpFsm::EContextDeletedFailed:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATECREATINGSECONDARY_INPUT_2, "**** CONTEXT DELETE FAILED ****");
		// fall through
	case PdpFsm::EContextDeleted:
		aFsm.ChangeStateToInitialised();
		SpudManNotify(aFsm, KSecondaryContextCreated, iErrorCode);
		iErrorCode = KErrNone;
		return KErrNone;
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}

