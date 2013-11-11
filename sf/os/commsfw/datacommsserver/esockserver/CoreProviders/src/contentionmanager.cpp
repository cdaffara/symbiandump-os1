// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/contentionmanager.h>
#include <comms-infras/ss_metaconnprov.h>
using namespace MeshMachine;
using namespace ESock;
using namespace Messages;

//
//CContentionManager

EXPORT_C CContentionManager::CContentionManager(const CTierManagerBase& aTierManager):
	iTierManager(aTierManager),iHaveFreeResources(ETrue)
	{
	}

EXPORT_C CContentionManager::~CContentionManager()
	{
	iContentionRequests.ResetAndDestroy();
	}

EXPORT_C TBool CContentionManager::HandleContentionL(CMetaConnectionProviderBase* aMcpr, TNodeId& aPendingCprId, TUint aPriority)
	{
	if (CheckContentionCondition(aPriority))
		{

		AddNewContentionRequestItemL(aMcpr, aPendingCprId, aPriority);
		return ETrue;
		}
	return EFalse;
	}


TBool CContentionManager::CheckContentionCondition(TUint aPriority) const
	{
	const RPointerArray<CMetaConnectionProviderBase>& metaConnectionProviders = iTierManager.GetMetaConnectionProviders();
	for (TInt i = 0; i<metaConnectionProviders.Count(); i++)
		{
		TClientIter<TDefaultClientMatchPolicy> iter = metaConnectionProviders[i]->GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
		RCFNodePriorityInterface* dataClient;
		while (dataClient = static_cast<RCFNodePriorityInterface*>(iter++), dataClient != NULL)
			{
			if ((dataClient->Flags()&TCFClientType::EStarted) &&
				(dataClient->Priority() > aPriority) &&
				(iStoppingCprId != dataClient->RecipientId()) )
				{ // We have CPR started with priority lower than preepmt CPR's priority.
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// Creates new contention request, adds it into requests array and starts contention resolving if necessary.
void CContentionManager::AddNewContentionRequestItemL(CMetaConnectionProviderBase* aMcpr, const TNodeId& aPendingCprId, TUint aPriority)
	{
	TContentionRequestItem *newRequest = new (ELeave) TContentionRequestItem;
	newRequest->iMcpr = aMcpr;
	newRequest->iPendingCprId = aPendingCprId;
	newRequest->iPriority = aPriority;
	CleanupStack::PushL(newRequest);
	iContentionRequests.AppendL(newRequest);
	CleanupStack::Pop(newRequest);
	if (iContentionRequests.Count() == 1 && !iResolvingContention)
		// If it is the first item and we are not already waiting for contention resolving
		{
		// start contention resolving
		ResolveContention();
		}

	}
TInt CContentionManager::FindStartedClientWithMinimalPriority(TNodeId& aCprId, TUint& aMinimalPriority) const
	{
	TUint minimalPriority = 0;
	TNodeId cprId;
	TInt mcprIndex = KErrNotFound;
	const RPointerArray<CMetaConnectionProviderBase>& metaConnectionProviders = iTierManager.GetMetaConnectionProviders();
	for (TInt i = 0; i<metaConnectionProviders.Count(); i++)
		{
		TClientIter<TDefaultClientMatchPolicy> iter = metaConnectionProviders[i]->GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
		RCFNodePriorityInterface* dataClient;
		while (dataClient = static_cast<RCFNodePriorityInterface*>(iter++), dataClient != NULL)
			{
			if (dataClient->Flags()&TCFClientType::EStarted && dataClient->Priority() > minimalPriority)
				{
				minimalPriority = dataClient->Priority();
				cprId = dataClient->RecipientId();
				mcprIndex = i;
				}
			}
		}
	if (mcprIndex == KErrNotFound)
		{
		return KErrNotFound;
		}
	aCprId = cprId;
	aMinimalPriority = minimalPriority;
	return mcprIndex;
	}

TInt  CContentionManager::GetMostPrioritizedContentionRequestIndex()
	{
	TInt index = KErrNotFound;
	TInt maximumPriority = KMaxTUint;
	for (TInt i=0; i < iContentionRequests.Count() ; i++)
		{
		if (iContentionRequests[i]->iPriority <= maximumPriority)
			{
			maximumPriority = iContentionRequests[i]->iPriority;
			index = i;
			}
		}
	return index;
	}

void CContentionManager::RejectAllContentionRequests()
	{
	for (TInt i = iContentionRequests.Count()-1; i >= 0; i--)
		{
		ContentionResolved(*iContentionRequests[i], EFalse);
		RemoveContentionRequest(i);
		}
	}

void CContentionManager::UpdateContentionRequestsPriorities()
	{
	TContentionRequestItem* currentRequest;
	for (TInt i=iContentionRequests.Count()-1; i >= 0 ; i--)
		{
		currentRequest = iContentionRequests[i];
		TClientIter<TDefaultClientMatchPolicy> iter = currentRequest->iMcpr->GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
		RCFNodePriorityInterface* dataClient = NULL;
		TBool iCprIsFound = EFalse;

		while (dataClient = static_cast<RCFNodePriorityInterface*>(iter++), dataClient != NULL)
			{
			if (currentRequest->iPendingCprId == dataClient->RecipientId())
				{
				currentRequest->iPriority = dataClient->Priority();
				iCprIsFound = ETrue;
				break;
				}
			}
		if (!iCprIsFound)
			// This is invalid request, probably cpr has already been unjoined/deleted, just remove it.
			{
			RemoveContentionRequest(i);
			}
		}
	}

void CContentionManager::RejectRequestsWithPriorityLowerThan(TUint aPriority)
	{
	for (TInt i = iContentionRequests.Count()-1; i >= 0; i--)
		{
		if (iContentionRequests[i]->iPriority > aPriority)
			{
			ContentionResolved(*iContentionRequests[i], EFalse);
			RemoveContentionRequest(i);
			}
		}
	}

void CContentionManager::SendAvailabilityNotifications(const TAvailabilityStatus& aStatus) const
	{
	const RPointerArray<CMetaConnectionProviderBase>& metaConnectionProviders = iTierManager.GetMetaConnectionProviders();
	for (TInt i = 0; i<metaConnectionProviders.Count(); i++)
		{
		TBool sendNotification = ETrue;
		TClientIter<TDefaultClientMatchPolicy> iter = metaConnectionProviders[i]->GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
		RCFNodePriorityInterface* dataClient;

		for (TInt j = 0; j < iContentionRequests.Count(); j++)
			{
			if (iContentionRequests[j]->iMcpr == metaConnectionProviders[i])
				{
				sendNotification = EFalse;
				break;
				}
			}

		if (!sendNotification)
			{
			continue;
			}

		while (dataClient = static_cast<RCFNodePriorityInterface*>(iter++), dataClient != NULL)
			{
			if (dataClient->Flags()&TCFClientType::EStarted)
				{
				sendNotification = EFalse;
				break;
				}
			}
		if (sendNotification)
			{
			ReportContentionAvailabilityStatus(*metaConnectionProviders[i], aStatus);
			}

		}
	}

void CContentionManager::ResolveContention()
	{
	// Resolve contentions one by one and not simultaneously.
	__ASSERT_DEBUG(!iResolvingContention, User::Invariant());
	__ASSERT_DEBUG(iStoppingCprId == TNodeId::NullId(), User::Invariant());
	TUint minimalPriority = 0;

	if (iContentionRequests.Count() == 0) // nothing to resolve
		{
		return;
		}

	TInt mcprIndex = FindStartedClientWithMinimalPriority(iStoppingCprId, minimalPriority);
	if (mcprIndex == KErrNotFound)
		{ // There are no connections to preemt. Reject all contention requests.
		RejectAllContentionRequests();
		return;
		}

	// Update priorities for contention requests. They can change.
	UpdateContentionRequestsPriorities();

	// Reject all contention requests with priority lower then stoppingCprId priority;
	RejectRequestsWithPriorityLowerThan(minimalPriority);

	// Check if we still have contention requests
	if (iContentionRequests.Count()>0)
		{
		iResolvingContention = ETrue;
		iHaveFreeResources = EFalse;
		// then tell to MCPR to stop CPR with minimal priority
		const RPointerArray<CMetaConnectionProviderBase>& metaConnectionProviders = iTierManager.GetMetaConnectionProviders();
		ContentionOccured(*metaConnectionProviders[mcprIndex]);
		TAvailabilityStatus availabilityStatus(TAvailabilityStatus::EMinAvailabilityScore);
		SendAvailabilityNotifications(availabilityStatus);
		ReportContentionAvailabilityStatus(*metaConnectionProviders[mcprIndex], availabilityStatus);
		}
	}

// Resources have been freed. Find the most prioritised contention request and tell to
// its MCPR that contention has been resolved
EXPORT_C void CContentionManager::ContentionResolved()
	{
	iResolvingContention = EFalse;
	iStoppingCprId = TNodeId::NullId();

	// Update priorities for contention requests. They can change.
	UpdateContentionRequestsPriorities();

	TInt index = GetMostPrioritizedContentionRequestIndex();
	if (index == KErrNotFound)
		// There are not contention requests
		{
		TAvailabilityStatus availabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore);
		SendAvailabilityNotifications(availabilityStatus);
		iHaveFreeResources = ETrue;
		return;
		}

	ContentionResolved(*iContentionRequests[index], ETrue);
	TAvailabilityStatus availabilityStatus(TAvailabilityStatus::EMaxAvailabilityScore);
	ReportContentionAvailabilityStatus(*iContentionRequests[index]->iMcpr, availabilityStatus);
	RemoveContentionRequest(index);
	// Resolve next contention if any
	ResolveContention();
	}

void CContentionManager::RemoveContentionRequest(TInt aIndex)
	{
	delete iContentionRequests[aIndex];
	iContentionRequests.Remove(aIndex);
	}

EXPORT_C TBool CContentionManager::IsUnavailableDueToContention(const CMetaConnectionProviderBase* aMetaConnectionProvider) const
	{
	TClientIter<TDefaultClientMatchPolicy> iter = aMetaConnectionProvider->GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
	RCFNodePriorityInterface* dataClient = NULL;
	// first check if MCPR has CPR's started.
	while (dataClient = static_cast<RCFNodePriorityInterface*>(iter++), dataClient != NULL)
			{
			if (dataClient->Flags()&TCFClientType::EStarted)
				{
				// if it has than it can't be unavailable
				return EFalse;
				}
			}

	//check if MCPR is in ContentionRequest list
	for (TInt i = 0; i < iContentionRequests.Count(); i++)
			{
			if (iContentionRequests[i]->iMcpr == aMetaConnectionProvider)
				{
				return EFalse;
				}
			}

	// return true if we do not have free contention resources, MCPR is stopped and it is not in ContentionRequests
	return !iHaveFreeResources;
	}

void CContentionManager::ContentionResolved(const TContentionRequestItem& /*aContentionRequest*/, TBool /*aResult*/)
	{
	}

void CContentionManager::ContentionOccured(ESock::CMetaConnectionProviderBase& /*aMcpr*/)
	{
	}

void CContentionManager::ReportContentionAvailabilityStatus(ESock::CMetaConnectionProviderBase& /*aMcpr*/, const ESock::TAvailabilityStatus& /*aStatus*/) const
	{
	}



