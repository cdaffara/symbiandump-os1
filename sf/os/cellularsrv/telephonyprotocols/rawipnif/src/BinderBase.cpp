// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements the base class for the protocol interface classes.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "BinderBaseTraces.h"
#endif

#include <nifmbuf.h>
#include "BinderBase.h"
#include "RawIPFlow.h"

using namespace ESock;

CBinderBase::CBinderBase(CRawIPFlow& aFlow)
/**
 * Constructor
 *
 * @param aNifMain A pointer to CRawIPFlow
 */
	:iFlow(&aFlow)
	{
	}

CBinderBase::~CBinderBase()
/**
 * Destructor
 */
	{
	}

// from ESock::MLowerControl
TInt CBinderBase::BlockFlow(MLowerControl::TBlockOption /*aOption*/)
	{
	return KErrNotSupported;
	}
	
void CBinderBase::ChangeFlow(CRawIPFlow& aNewFlow)
    {
    ASSERT(iFlow);
    iFlow = &aNewFlow;
    }

MLowerDataSender* CBinderBase::Bind(MUpperDataReceiver* aUpperReceiver, MUpperControl* aUpperControl)
/**
 * Binds TCP/IP protocol to Flow
 *
 * @param aUpperReceiver A pointer to Upper layer Receive class
 * @param aUpperControl A pointer to Upper layer control class
 */
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBINDERBASE_BIND_1, "CBinderBase:\tBind()");

	iUpperReceiver = aUpperReceiver;
	iUpperControl = aUpperControl;
	return this;
	}

void CBinderBase::Unbind(MUpperDataReceiver* aUpperReceiver, MUpperControl* aUpperControl)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBINDERBASE_UNBIND_1, "CBinderBase:\tUnbind()");
	
#ifndef _DEBUG
	(void) aUpperReceiver;
	(void) aUpperControl;
#endif

	ASSERT(aUpperReceiver == iUpperReceiver);
	ASSERT(aUpperControl == iUpperControl);
	iUpperReceiver = NULL;
	iUpperControl = NULL;
	}

void CBinderBase::StartSending()
/**
 * Indicates to the protocol layer that the NIF is ready to send packets.
 *
 * @param aProtocol A pointer to a protocol
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBINDERBASE_STARTSENDING_1, "CBinderBase:\tStartSending()");

	// Default implementation.
	// Uses iProtocol instead aProtocol.
	iUpperControl->StartSending();
	}

void CBinderBase::UpdateContextConfigL(const TPacketDataConfigBase& /*aConfig*/)
/**
 * Method overidden by UpdateContextConfig on IPv4ProtocolIf class
 *
 * @param aConfig Not used
 */
	{
	// Default implementation does nothing.
	}

void CBinderBase::UpdateConnectionSpeed(TUint /*aConnectionSpeed*/)
/**
 * Method overidden by UpdateConnectionSpeed on IPv4Binder class
 *
 * @param aConnectionSpeed Not used
 */
	{
	// Default implementation does nothing.
	}
