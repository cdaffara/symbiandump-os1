// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <es_ini.h>
#include <flow.h>

#include "iface.h"
#include "context.h"
#include "async_request.h"
#include "tc.h"
#include "guqos_log.h"
#include "guqos.h"

#ifdef _LOG
// Ugly use of function from another module (context.cpp), but this debugging only!
extern void LogPacketFilter(const TPacketFilter& aFilter);
#endif

CFlowData* CFlowData::NewL(CFlowContext& aHandle, CNif& aNif, CFlowData *aNext)
	{
	CFlowData* flowData = new (ELeave) CFlowData(aHandle, aNif);
	CleanupStack::PushL(flowData);
	flowData->ConstructL();
	CleanupStack::Pop();
	flowData->iNext = aNext;
	return flowData;
	}

CFlowData* CFlowData::Find(const CFlowContext* aHandle, CFlowData *aList)
	{
	while (aList)
		{
		if (&aList->iFlowContext == aHandle)
			break;
		aList = aList->iNext;
		}
	return aList;
	}

CFlowData* CFlowData::Remove(const CFlowContext* aHandle, CFlowData **aList)
	{
	CFlowData* p = NULL;
	for ( ; (p = *aList) != NULL; aList = &p->iNext)
		{
		if (&p->iFlowContext == aHandle)
			{
			*aList = p->iNext;
			break;
			}
		}
	return p;
	}

void CFlowData::SetContext(CPdpContext *aContext)
	/**
	* Associate the flow with a new context (or none).
	*
	* Move the flow from one context to another. The
	* internal CClose reuqest is not triggered. (It's
	* only used in destructor).
	*
	* @param aContext The new context (or NULL)
	*/
	{
	if (iContext == aContext)
		{
		// Already attache to this context (or NONE) -- nothing to do.
		return;
		}

	if (iContext)
		{
		// Remove from previous context.
		iLink.Deque();
		iContext->FlowDetached();
		}
	iContext = aContext;
	if (iContext)
		{
		// Add to new context.
		iContext->Flows().AddLast(*this);
		iContext->FlowAttached();
		}
	MEventInterface *notifier = Nif().Module().QoSEvent();
	if (notifier)
		{
		// Determine the blocked or unblocked status for the flow
		// and notify upwards accordingly...

		// Throw away "const". The MEventInterface is misdeclared to
		// use non-const reference parameter, when const reference
		// would be the correct way...
		if (iContext == NULL || iContext->IsFlowBlocked())
			notifier->BlockFlow((CFlowContext &)iFlowContext);
		else
			notifier->UnBlockFlow((CFlowContext &)iFlowContext);
		}
	}


CFlowData::CFlowData(CFlowContext& aHandle, CNif& aNif) : 
	iNif(aNif), iFlowContext(aHandle)
	{
	
	iContext = NULL;

	// Initialize constant filter values from TPacketHead.
	const TPacketHead& h = aHandle.iHead;
	Mem::Copy(iPacketFilter.iSrcAddr, h.ip6.DstAddr().u.iAddr8, sizeof(iPacketFilter.iSrcAddr));
	Mem::Fill(iPacketFilter.iSrcAddrSubnetMask, sizeof(iPacketFilter.iSrcAddrSubnetMask), 0xFF);
	iPacketFilter.iProtocolNumberOrNextHeader = h.iProtocol;
	iPacketFilter.iSrcPortMin = h.iDstPort;
	iPacketFilter.iSrcPortMax = h.iDstPort;
	iPacketFilter.iDestPortMin = h.iSrcPort;
	iPacketFilter.iDestPortMax = h.iSrcPort;
	iPacketFilter.iTOSorTrafficClass = 0;
	LOG(Log::Printf(_L("new\tflow[%u] size=%d"), (TInt)this, sizeof(*this)));
	LOG(LogPacketFilter(iPacketFilter));

	// The filter is for the *INCOMING* traffic! Predicting the values
	// of the following fields from the *OUTGOING* flow would be
	// dubious!!

	/* iPacketFilter.iIPSecSPI = ??? */
	/* iPacketFilter.iTOSorTrafficClass = h.ip6.TrafficClass() ? */
	/* iPacketFilter.iFlowLabel = h.ip6.FlowLabel() ? */
	}

CFlowData::~CFlowData()
	{
	if (iContext)
		{
		// The flow is still attached to a contex.
		iLink.Deque();
		iContext->FlowDetached();
		}
	LOG(Log::Printf(_L("~\tflow[%u]"), (TInt)this));
	}

void CFlowData::ConstructL()
	{
	}

TInt CFlowData::Send(RMBufChain& aPacket, RMBufSendInfo& aInfo)
	{
	// Note: the fact that we are here, means that the CFlowContex in the aInfo
	// is same as CFlowContext associated with CFlowData. ASSERT is only a
	// remainder of this and never expect to fire.
	ASSERT(aInfo.iFlow.FlowContext() == &iFlowContext);
	if (iContext)
		{
		aInfo.iDstAddr.SetPort(iContext->ContextId());
		// Note: this assumes that the Send below does to come back
		// and delete structures from under us! If this becomes
		// possible, then at least CFlowData and possibly CPdpContext
		// need to be protected against destruction...
		const TInt ret = aInfo.iFlow.FlowContext()->Send(aPacket);
		if (ret <= 0)
			{
			// Packet accepted, but cannot send any more packets
			// to this context, until it is unblocked.
			LOG(Log::Printf(_L("Send\tflow[%u] returns %d, blocking PDP context id = %d"), (TInt)this, ret, iContext->ContextId()));
			iContext->Block();
			}
		}
	else
		{
		LOG(Log::Printf(_L("Send\tflow[%u] has no PDP context -- packet dropped"), (TInt)this));
		aInfo.iFlow.Close();
		aPacket.Free();
		}
	// Always eat the packet!
	return 1;
	}

