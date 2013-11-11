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
// implementation of base spud etel driver notifier
// 
//

/**
 @file 
 @internalComponent
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ceteldrivernotifierTraces.h"
#endif

#include <e32def.h>

#include "ceteldrivernotifier.h"
#include "PDPFSM.h"


/**
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CEtelDriverNotifier::CEtelDriverNotifier(CPdpFsmInterface& aPdpFsmInterface)
: CActive(EPriorityStandard),
  iPdpFsmInterface(aPdpFsmInterface)
	{
	CActiveScheduler::Add(this);
	}


void CEtelDriverNotifier::RunL()
	{
	// notify fsm in any case
	Notify(iStatus);
	
	if(iStatus == KErrNone)
		{
		// repost
		Start();
		}
	else // error
		{
		if(KErrCancel != iStatus.Int())
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERNOTIFIER_RUNL_1, "CEtelDriverNotifier::RunL() reported an error: %d", iStatus.Int());
			}
		}
	}
	
