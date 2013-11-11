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
// QoS change notifier implementation
// 
//

/**
 @file 
 @internalComponent
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cqoschangenotifierTraces.h"
#endif

#include "cqoschangenotifier.h"
#include <pcktcs.h>


/** c'tor 

@param aId - pdp context id
@param aPacketQoS - etel packet QoS 
@param aPdpFsmInterface - pdp fsm interface
*/
CQoSChangeNotifier::CQoSChangeNotifier(TContextId aId, 
									RPacketQoS& aPacketQoS, 
									CPdpFsmInterface& aPdpFsmInterface)
: CEtelDriverNotifier(aPdpFsmInterface),
  iId(aId),
  iPacketQoS(aPacketQoS),
  iProfilePckg(iProfileBuffer)
  
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_CTOR_1, "CQoSChangeNotifier::CQoSChangeNotifier()");
	}
	
CQoSChangeNotifier::~CQoSChangeNotifier()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_DTOR_1, "CQoSChangeNotifier::~CQoSChangeNotifier()");
	Cancel();
	}
	
/** starts notification */
void CQoSChangeNotifier::Start()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_START_1, "CQoSChangeNotifier::Start()");
	if (!IsActive())
		{ 
		iPacketQoS.NotifyProfileChanged(iStatus, iProfilePckg); 
		SetActive();
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_START_2, "CQoSChangeNotifier::Start(), iPacketQoS.NotifyProfileChanged()");
		}
	}
	
/** stops notification */	
void CQoSChangeNotifier::DoCancel()
	{
	if (IsActive())
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_DOCANCEL_1, "CQoSChangeNotifier::DoCancel EPacketQoSNotifyProfileChanged");
		iPacketQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
		}
	}

/** notifies pdp fsm 

@param aStatus - request status
*/
void CQoSChangeNotifier::Notify(const TRequestStatus& aStatus)
	{
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_NOTIFY_1, "Qos Change Notification for Context [%S]", iPdpFsmInterface.GetContextName(iId));

    if(aStatus == KErrNone)
		{

#ifdef SYMBIAN_NETWORKING_UMTSR5
        iPdpFsmInterface.Set(iId, iProfileBuffer.NegotiatedQoSR5());

#else
// !SYMBIAN_NETWORKING_UMTSR5

        iPdpFsmInterface.Set(iId, iProfileBuffer.NegotiatedQoSR99_R4());
#endif
// SYMBIAN_NETWORKING_UMTSR5

		iPdpFsmInterface.Input(iId, PdpFsm::EQoSProfileChangeNetwork);
		}
	else
		{ 
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CQOSCHANGENOTIFIER_NOTIFY_2, "CQoSChangeNotifier::Notify(), error: %d", aStatus.Int());
		ASSERT(aStatus == KErrCancel); 
		}
	}
