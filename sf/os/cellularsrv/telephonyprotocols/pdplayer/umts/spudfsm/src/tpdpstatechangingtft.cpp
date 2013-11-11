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
// Changing TFT state
// 
//

/**
 @file 
 @internalComponent
*/
 


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstatechangingtftTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"

TInt TPdpStateChangingTFT::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::ETftChanged:
		aFsm.ChangeStateToOpen();
		SpudManNotify (aFsm, KContextTFTModifiedEvent, KErrNone);
		return KErrNone;
	case PdpFsm::ETftChangedFailed:
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATECHANGINGTFT_INPUT_1, "*** TFT CHANGE FAILED ***");
		aFsm.ChangeStateToOpen();
		SpudManNotify (aFsm, KContextTFTModifiedEvent, aErrorCode);
		return KErrNone;
		
	// no default
	}

	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}

