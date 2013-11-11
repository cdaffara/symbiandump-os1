// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pdptiermanagerTraces.h"
#endif

#include "pdptiermanager.h"
#include "mbmsengine.h"
#include "pdptiermanagerselector.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanageractivities.h>
#include <pcktcs.h>

#ifdef SYMBIAN_TRACE_ENABLE
#define KPDPTierMgrTag KESockMetaConnectionTag
#endif

#include "pdpmcpr.h"

using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace ConnectionServ;
using namespace NetStateMachine;
using namespace CommsDat;

namespace PDPTierManagerActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(stateMap)
ACTIVITY_MAP_END_BASE(TMActivities, coreTMActivities)
}


/**
The NewL factory function for PDPTierManager.
This function also acts as the single ECom entry point into this object.
@param aFactory The  reference to CTierManagerFactoryBase
@return CPDPTierManager*
*/
CPDPTierManager* CPDPTierManager::NewL(ESock::CTierManagerFactoryBase& aFactory)
	{
 	CPDPTierManager* self = new(ELeave)CPDPTierManager(aFactory, PDPTierManagerActivities::stateMap::Self());
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor for CPDPTierManager
*/
CPDPTierManager::CPDPTierManager(ESock::CTierManagerFactoryBase& aFactory,
								const MeshMachine::TNodeActivityMap& aActivityMap)
	:CCoreTierManager(aFactory,aActivityMap)
	{
	LOG_NODE_CREATE(KPDPTierMgrTag, CPDPTierManager);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_CONSTRUCTOR_1, "Constructor");
	}

/**
Descructor for CPDPTierManager
*/
CPDPTierManager::~CPDPTierManager()
	{
	LOG_NODE_DESTROY(KPDPTierMgrTag, CPDPTierManager);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_DESTRUCTOR_1, "Destructor");
#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
	delete iPdpContentionManager;
	iPdpContentionManager = NULL;
#endif
	
	if (iMBMSEngine)
		{
		iMBMSEngine->Cancel();
		delete iMBMSEngine;
		iMBMSEngine = NULL;
		}
	}

/**
Create selector for this Tier.
@return MProviderSelector*
*/
MProviderSelector* CPDPTierManager::DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_DOCREATEPROVIDERSELECTORL_1, "CPdpTierManager[%08x]::DoSelectProvider()", (TUint)this);
	return TPdpSelectorFactory::NewSelectorL(aSelectionPreferences);
	}

/**
ConstructL function of CPDPTierManager.
This function is used to retrieve tsy name.
@return void
*/
void CPDPTierManager::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_CONSTRUCTL_1, "ConstructL::In CPDPTierManager");
	iMBMSEngine = CMBMSEngine::NewL(NodeId(), *this);
	}

/**
 The PacketServiceAttachedCallbackL is a callback function.
 It's called by MBMSEngine when RPacketService has been attached.
*/
void CPDPTierManager::PacketServiceAttachedCallbackL()
	{
#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_PACKETSERVICEATTACHEDCALLBACKL_1, "Starting PDP Contention Manager");
	if (!iPdpContentionManager)
		{
		iPdpContentionManager = CPdpContentionManager::NewL(*this, iMBMSEngine->GetRPacketService());
		}
	static_cast<CPdpContentionManager*>(iPdpContentionManager)->StartMonitoringL();
#endif
	}

/**
The ReceivedL function for PDPTierManager.
The messages from RConnectionServ are received in ReceivedL function.
@param aCFMessage The  reference to Messages::TSignatureBase
@return TInt*
*/
void CPDPTierManager::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_RECEIVEDL_1, "Realm Id [%u] Message Id [%u]", aMessage.MessageId().Realm(), aMessage.MessageId().MessageId());
	//TODO I think that the generic TCancel handling will do this - so perhaps remove this and CancelAndRemoveFromRequestList!
	if (aMessage.IsMessage<TEBase::TCancel>())
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_RECEIVEDL_2, "Received Message Type: TEBase::TCancel");
		TRAPD(err, iMBMSEngine->CancelAndRemoveFromRequestListL(aSender));
		if (err == KErrNone)
			{
			aMessage.ClearMessageId();
			}
		else if (err != KErrNotFound)
			{
			User::Leave(err);
			}
		// err == KErrNotFound
		// Probably this is not cancel for MBMS request but cancel for PDP availability notification request
		// It will be processed by core TM then.
		}

	if (aMessage.IsMessage<TCFTierStatusProvider::TTierNotificationRegistration>())
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_RECEIVEDL_3, "Received Message Type: TCFTierStatusProvider::TTierNotificationRegistration");
		TCFTierStatusProvider::TTierNotificationRegistration* msgTNR = message_cast<TCFTierStatusProvider::TTierNotificationRegistration>(&aMessage);

		const Messages::TNodeSignal::TMessageId requestType = TCFTierStatusProvider::TTierNotificationRegistration::Id();
		CRefCountOwnedParameterBundle* requestBundleOwner = msgTNR->iBundle;

		TRAPD(err, iMBMSEngine->AddToRequestListL(FindClient(aSender), aSender, requestType, requestBundleOwner));
		if (err == KErrNone)
			{
			aMessage.ClearMessageId();
			}
		else if (err != KErrArgument)
			{
			User::Leave(err);
			}
		// err == KErrArgument
		// Probably this is not MBMS notification request but PDP availability notification request
		// It will be processed by core TM then.
		}

	TNodeContext<CPDPTierManager> ctx(*this, aMessage, aSender, aRecipient);
	CCoreTierManager::Received(ctx);
	User::LeaveIfError(ctx.iReturn);
	}

#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT

TBool CPDPTierManager::HandleContentionL(ESock::CMetaConnectionProviderBase* aMcpr, Messages::TNodeId& aPendingCprId, TUint aPriority)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPTIERMANAGER_HANDLECONTENTIONL_1, "Handle Contention Request");
	User::LeaveIfNull(iMBMSEngine);
	if (!iPdpContentionManager)
		{
		iPdpContentionManager = CPdpContentionManager::NewL(*this, iMBMSEngine->GetRPacketService());
		}
	return iPdpContentionManager->HandleContentionL(aMcpr, aPendingCprId, aPriority);
	}

TBool CPDPTierManager::IsUnavailableDueToContention(const ESock::CMetaConnectionProviderBase* aMetaConnectionProvider) const
    {
    TBool result(EFalse);
    
    if (!iPdpContentionManager)
        {
        result = EFalse;
        }
    else
        {
        result = iPdpContentionManager->IsUnavailableDueToContention(aMetaConnectionProvider);
        }
    return result;
    }

#else

/* these are inherited from the tier manager base class in esock so they must be implemented
 * even if this feature is disabled.  If it is disabled, then contention resolution will always
 * be false.  Note: this functionality is done at a higher layer so it is unlikely that this feature
 * will make (i.e. this code is depreciated).
 */

TBool CPDPTierManager::HandleContentionL(ESock::CMetaConnectionProviderBase* /*aMcpr */, Messages::TNodeId& /*aPendingCprId */, TUint /* aPriority */)
    {
    return EFalse;
    }


TBool CPDPTierManager::IsUnavailableDueToContention(const ESock::CMetaConnectionProviderBase* /* aMetaConnectionProvider */) const
    {
    return EFalse;
    }

#endif

