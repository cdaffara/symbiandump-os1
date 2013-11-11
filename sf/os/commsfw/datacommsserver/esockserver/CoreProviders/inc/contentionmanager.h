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

/**
 @file contentionmanager.h
 @internalTechnology
*/

#ifndef SYMBIAN_CONTENTIONMANAGER_H
#define SYMBIAN_CONTENTIONMANAGER_H

#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/es_availability.h>

struct TContentionRequestItem
	{
	ESock::CMetaConnectionProviderBase* iMcpr;
	Messages::TNodeId iPendingCprId;
	TUint iPriority;
	};

class MContentionObserver
	{
public:
	virtual void ContentionResolved() = 0;
	};


//
// CContentionManager
// Base class for resolving contention situations.
// Resolves contention situations one by one.
// Every network layer can have its own contention manager that should be
// inherited from this base class.

class CContentionManager : public CBase, public MContentionObserver
	{
	public:
		IMPORT_C CContentionManager(const ESock::CTierManagerBase& aTierManager);
		IMPORT_C ~CContentionManager();

	public:
		IMPORT_C TBool HandleContentionL(ESock::CMetaConnectionProviderBase* aMcpr, Messages::TNodeId& aPendingCprId, TUint aPriority);
		IMPORT_C TBool IsUnavailableDueToContention(const ESock::CMetaConnectionProviderBase* aMetaConnectionProvider) const;

	protected:
		// from MContentionObserver
		// Is called when resource (e.g. PDP context) has been freed
		IMPORT_C virtual void ContentionResolved();

	private:
		virtual void ContentionResolved(const TContentionRequestItem& aContentionRequest, TBool aResult);
		virtual void ContentionOccured(ESock::CMetaConnectionProviderBase& aMcpr);
		virtual void ReportContentionAvailabilityStatus(ESock::CMetaConnectionProviderBase& aMcpr, const ESock::TAvailabilityStatus& aStatus) const;
		
	private:
		void  AddNewContentionRequestItemL(ESock::CMetaConnectionProviderBase* aMcpr, const Messages::TNodeId& aPendingCprId, TUint aPriority);
		TBool CheckContentionCondition(TUint aPriority) const;
		TInt  FindStartedClientWithMinimalPriority(Messages::TNodeId& aCprId, TUint& aMinimalPriority) const;
		TInt  GetMostPrioritizedContentionRequestIndex();
		void  RejectAllContentionRequests();
		void  RejectRequestsWithPriorityLowerThan(TUint aPriority);
		void  ResolveContention();
		void  UpdateContentionRequestsPriorities();
		void  RemoveContentionRequest(TInt aIndex);
		void  SendAvailabilityNotifications(const ESock::TAvailabilityStatus& aStatus) const;

	private:
		const ESock::CTierManagerBase& iTierManager;
		RPointerArray<TContentionRequestItem> iContentionRequests;
		Messages::TNodeId iStoppingCprId;
		TBool iResolvingContention;
		TBool iHaveFreeResources;
	};

#endif
// SYMBIAN_CONTENTIONMANAGER_H

