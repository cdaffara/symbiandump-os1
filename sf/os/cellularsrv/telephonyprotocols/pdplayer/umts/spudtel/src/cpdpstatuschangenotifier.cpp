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
// PDP status change notifier
// 
//

/**
 @file 
 @internalComponent
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpdpstatuschangenotifierTraces.h"
#endif

#include <e32base.h>

#include "cpdpstatuschangenotifier.h"
#include "PDPFSM.h"
#include "pdpfsmnmspace.h"

#include <pcktcs.h>


/**
@param aId - pdp context id
@param aPacketContext - reference to etel packet context
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CPdpStatusChangeNotifier::CPdpStatusChangeNotifier(TContextId aId,
										RPacketContext& aPacketContext,
										CPdpFsmInterface& aPdpFsmInterface)
: CEtelDriverNotifier(aPdpFsmInterface),
  iId(aId),
  iPacketContext(aPacketContext),
  iContextStatus(RPacketContext::EStatusUnknown)
	{
	}

CPdpStatusChangeNotifier::~CPdpStatusChangeNotifier()
	{
	Cancel();
	}

/** initiates notification */
void CPdpStatusChangeNotifier::Start()
	{
	if (!IsActive() && (iContextStatus != RPacketContext::EStatusDeleted))
		{ 
		iPacketContext.NotifyStatusChange(iStatus, iContextStatus); 
		SetActive();
		}
	}

/** stops notification */	
void CPdpStatusChangeNotifier::DoCancel()
	{
	if(IsActive())
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSTATUSCHANGENOTIFIER_DOCANCEL_1, "CPdpStatusChangeNotifier::DoCancel EPacketContextNotifyStatusChange");
		iPacketContext.CancelAsyncRequest(EPacketContextNotifyStatusChange); 
		}
	}

/** notifies pdp fsm 

@param aStatus - request status
*/
void CPdpStatusChangeNotifier::Notify(const TRequestStatus& aStatus)
	{
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSTATUSCHANGENOTIFIER_NOTIFY_1, "Status Change Notification for Context [%S]", iPdpFsmInterface.GetContextName(iId));
    if(aStatus == KErrNone)
		{
		RPacketContext::TContextStatus aOldContextStatus;
		iPdpFsmInterface.Get(iId,aOldContextStatus);
		// only notify of change if it has actually changed
		if (iContextStatus != aOldContextStatus)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSTATUSCHANGENOTIFIER_NOTIFY_2, "Context status changed from [%d] to [%d]", aOldContextStatus, iContextStatus);
			iPdpFsmInterface.Set(iId, iContextStatus);
            TInt err = KErrNone;
            iPacketContext.GetLastErrorCause(err); // Ignore error return code.
			iPdpFsmInterface.Input(iId,  PdpFsm::EContextStatusChangeNetwork, err);
			}
		}
	else
		{ 
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPSTATUSCHANGENOTIFIER_NOTIFY_3, "CPdpStatusChangeNotifier::Notify(), error: %d", aStatus.Int());
		ASSERT(aStatus == KErrCancel); 
		}
	}
	
	
	
	
	
/**
@param aId - pdp context id
@param aPacketContext - reference to etel packet context
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CMbmsPdpStatusChangeNotifier::CMbmsPdpStatusChangeNotifier(TContextId aId,
										RPacketMbmsContext& aMbmsPacketContext,
										CPdpFsmInterface& aPdpFsmInterface)
: CEtelDriverNotifier(aPdpFsmInterface),
  iId(aId),
  iMbmsPacketContext(aMbmsPacketContext),
  iContextStatus(RPacketContext::EStatusUnknown)
	{
	}

CMbmsPdpStatusChangeNotifier::~CMbmsPdpStatusChangeNotifier()
	{
	Cancel();
	}

/** initiates notification */
void CMbmsPdpStatusChangeNotifier::Start()
	{
	if (!IsActive() && (iContextStatus != RPacketContext::EStatusDeleted))
		{ 
		iMbmsPacketContext.NotifyStatusChange(iStatus, iContextStatus); 
		SetActive();
		}
	}

/** stops notification */	
void CMbmsPdpStatusChangeNotifier::DoCancel()
	{
	if(IsActive())
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPSTATUSCHANGENOTIFIER_DOCANCEL_1, "CMbmsPdpStatusChangeNotifier::DoCancel EPacketContextNotifyStatusChange");
		iMbmsPacketContext.CancelAsyncRequest(EPacketContextNotifyStatusChange); 
		}
	}

/** notifies pdp fsm 

@param aStatus - request status
*/
void CMbmsPdpStatusChangeNotifier::Notify(const TRequestStatus& aStatus)
	{
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPSTATUSCHANGENOTIFIER_NOTIFY_1, "MBMS Status Change Notification for Context [%S]", iPdpFsmInterface.GetContextName(iId));

    if(aStatus == KErrNone)
		{
		RPacketContext::TContextStatus aOldContextStatus;
		iPdpFsmInterface.Get(iId,aOldContextStatus);
		// only notify of change if it has actually changed
		if (iContextStatus != aOldContextStatus)
			{
            OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPSTATUSCHANGENOTIFIER_NOTIFY_2, "Context status changed from [%d] to [%d]", aOldContextStatus, iContextStatus);
			iPdpFsmInterface.Set(iId, iContextStatus);
            TInt err = KErrNone;
            iMbmsPacketContext.GetLastErrorCause(err); // Ignore error return code.
			iPdpFsmInterface.Input(iId, PdpFsm::EContextStatusChangeNetwork, err);
			}
		}
	else
		{ 
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPSTATUSCHANGENOTIFIER_NOTIFY_3, "CMbmsPdpStatusChangeNotifier::Notify(), error: %d", aStatus.Int());
		ASSERT(aStatus == KErrCancel); 
		}
	}
	
