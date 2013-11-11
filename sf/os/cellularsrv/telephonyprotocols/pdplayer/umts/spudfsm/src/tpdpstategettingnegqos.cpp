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
// Retrieving Negotiated QoS state
// 
//

/**
 @file 
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstategettingnegqosTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"

TInt TPdpStateGettingNegQoS::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
    {
	switch (aOperation)
	    {
    	case SpudMan::EGetNegQoS:
    		EtelDriverInput(aFsm, EtelDriver::ENegQoSGet);
    		return KErrNone;
    		
    	case PdpFsm::EPdpNegQoSRetrieved:
    		aFsm.ChangeStateToOpen();
    		SpudManNotify(aFsm, KGetNegQoSEvent, KErrNone);
    		return KErrNone;
    		
    	case PdpFsm::EPdpNegQoSRetrievedFailed:
    		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATEGETTINGNEGQOS_INPUT_1, "*** PDP NEGOTIATED QOS RETRIEVE FAILED ***");
    		// As far as everybody is concerned, the context was activated:
    		// This makes it eligible for certain events. 
    		// We have to move to Open state, so that that we can handle any operations
    		// on the activated PDP context correctly.
    		aFsm.ChangeStateToOpen(); 
    		SpudManNotify(aFsm, KGetNegQoSEvent, aErrorCode);
    		return KErrNone;
    	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
    }
