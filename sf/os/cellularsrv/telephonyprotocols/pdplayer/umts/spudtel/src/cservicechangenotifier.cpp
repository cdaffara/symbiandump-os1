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
// implementation for service change notifier
// 
//

/**
 @file 
 @internalComponent
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cservicechangenotifierTraces.h"
#endif

#include <e32def.h>

#include "cservicechangenotifier.h"
#include "PDPFSM.h"
#include "pdpfsmnmspace.h"

#include <pcktcs.h>

/** c'tor

@param aPacketService - reference to packet service
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CServiceChangeNotifier::CServiceChangeNotifier(RPacketService& aPacketService,
										CPdpFsmInterface& aPdpFsmInterface)
: CEtelDriverNotifier(aPdpFsmInterface),
  iPacketService(aPacketService),
  iServiceStatus(RPacketService::EStatusUnattached)
	{
	}

CServiceChangeNotifier::~CServiceChangeNotifier()
	{
	Cancel();
	}

/** initiates notification */
void CServiceChangeNotifier::Start()
	{
	if (!IsActive())
		{ 
		iPacketService.NotifyStatusChange(iStatus, iServiceStatus); 
		SetActive();
		}
	}

/** notifies pdp fsm 

@param aStatus - request status
*/
void CServiceChangeNotifier::Notify(const TRequestStatus& aStatus)
	{
	if(aStatus == KErrNone)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSERVICECHANGENOTIFIER_NOTIFY_1, "FSM set ServiceStatus");
		iPdpFsmInterface.Set(iServiceStatus);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSERVICECHANGENOTIFIER_NOTIFY_2, "FSM input EServiceStatusChangeNetwork");
		iPdpFsmInterface.Input(KAllContexts, 
								PdpFsm::EServiceStatusChangeNetwork);
		}
	else
		{ 
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSERVICECHANGENOTIFIER_NOTIFY_3, "CServiceChangeNotifier::Notify(), error: %d", aStatus.Int());
		ASSERT(aStatus == KErrCancel); 
		}
	}

/** cancels notification */	
void CServiceChangeNotifier::DoCancel()
	{
	if(IsActive())
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSERVICECHANGENOTIFIER_DOCANCEL_1, "CServiceChangeNotifier::DoCancel EPacketNotifyStatusChange");
		iPacketService.CancelAsyncRequest(EPacketNotifyStatusChange); 
		}
	}
