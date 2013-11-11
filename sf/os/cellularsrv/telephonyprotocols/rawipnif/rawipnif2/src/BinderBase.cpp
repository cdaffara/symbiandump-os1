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
#include "RawIP2Flow.h"
#include "BcaController.h"

CBinderBase::CBinderBase(CRawIP2Flow& aFlow)
/**
 * Constructor
 *
 * @param aNifMain A pointer to CRawIPFlow
 */
	:iFlow(&aFlow),
	iStarted(EFalse)
	{	
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	iIPTagHeader = new (ELeave) CIPTagHeader();
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	}

CBinderBase::~CBinderBase()
/**
 * Destructor
 */
	{	
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	delete iIPTagHeader;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	}

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
void CBinderBase::SetType(TUint16 aType)
{
/**
 *	Used to specify the type of the IP header.
 */
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBINDERBASE_SETTYPE_1, "CBinderBase::SetType");
	
	iIPTagHeader->SetType(aType);	
}
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS

void CBinderBase::SetSender(BasebandChannelAdaptation2::MLowerDataSender* aLowerDataSender)
	{
	iLowerDataSender = aLowerDataSender;
	}

// from ESock::MLowerControl
TInt CBinderBase::BlockFlow(MLowerControl::TBlockOption aOption)
	{
	
	if(iFlow->GetBcaController())
		{
		if(iFlow->GetBcaController()->Bca())
			{
			if(aOption != MLowerControl::EEnableAllBinders)
				{ // unblock flow
				iFlow->GetBcaController()->Bca()->SetFlowControl(BasebandChannelAdaptation2::MBca2::EUnblockFlow);
				}
			else
				{ //block flow
				iFlow->GetBcaController()->Bca()->SetFlowControl(BasebandChannelAdaptation2::MBca2::EBlockFlow);
				}
			return KErrNone;
			}
		}
	// MBca not ready
	return KErrNotReady;
	}
	
void CBinderBase::ChangeFlow(CRawIP2Flow& aNewFlow)
    {
    ASSERT(iFlow);
    iFlow = &aNewFlow;
    }

ESock::MLowerDataSender* CBinderBase::Bind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl)
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

void CBinderBase::Unbind(ESock::MUpperDataReceiver* aUpperReceiver, ESock::MUpperControl* aUpperControl)
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

	if (!iStarted)
		{
		GetFlow().LinkLayerUp();
		iStarted = ETrue;
		}

	// Default implementation.
	// Uses iProtocol instead aProtocol.
	iUpperControl->StartSending();
	}

void CBinderBase::Error(TInt aErr)
/**
 * Indicates to the protocol layer that a fatal error has occured in the BCA.
 *
 * @param aProtocol A pointer to a protocol
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBINDERBASE_ERROR_1, "CBinderBase:\tError()");

	// Default implementation.
	// Uses iProtocol instead aProtocol.
	iUpperControl->Error(aErr);
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
