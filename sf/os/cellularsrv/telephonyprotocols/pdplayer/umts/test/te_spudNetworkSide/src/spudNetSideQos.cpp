// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements QoS utilities for te_spudNetworkTest.
// 
//

/**
 @file
 @internalComponent
*/


#include "spudNetSideQos.h"


#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//------------------------------------------------------
//	QoSEventToText
//------------------------------------------------------
TPtrC QoSEventToText(TQoSEvent aEvent)
{
	switch(aEvent)
    {
		case EQoSEventFailure:
		    return _L("EQoSEventFailure");
		case EQoSEventConfirm:
		    return _L("EQoSEventConfirm");
		case EQoSEventAdapt:
		    return _L("EQoSEventAdapt");
		case EQoSEventChannel:
		    return _L("EQoSEventChannel");
		case EQoSEventJoin:
		    return _L("EQoSEventJoin");
		case EQoSEventLeave:
		    return _L("EQoSEventLeave");
		case EQoSEventAddPolicy:
		    return _L("EQoSEventAddPolicy");
		case EQoSEventGetPolicy:
    	    return _L("EQoSEventGetPolicy");
		case EQoSEventDeletePolicy:
		    return _L("EQoSEventDeletePolicy");
		case EQoSEventLoadPolicyFile:
		    return _L("EQoSEventLoadPolicyFile");
		case EQoSEventUnloadPolicyFile:
		    return _L("EQoSEventUnloadPolicyFile");
		default:
			return _L("Unknown QoS event");
	}
}
#endif
