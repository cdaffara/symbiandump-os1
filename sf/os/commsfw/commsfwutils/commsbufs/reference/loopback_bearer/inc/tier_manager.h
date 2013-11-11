/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef __LEGACY_LOOPBACK_TIER_MANAGER_H__
#define __LEGACY_LOOPBACK_TIER_MANAGER_H__

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <coretiermanagerstates.h>
#include <coretiermanager.h>
#else
#include <comms-infras/coretiermanagerstates.h>
#include <comms-infras/coretiermanager.h>
#endif

#include <elements/nm_signatures.h>

const TUid KUidConnPrefListTestingPubSub = {0x10285E24};

class CLegacyLoopBackTierManager : public CCoreTierManager
	{
public:
	static CLegacyLoopBackTierManager* NewL(ESock::CTierManagerFactoryBase& aFactory);
	~CLegacyLoopBackTierManager();

protected:
	CLegacyLoopBackTierManager(ESock::CTierManagerFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	virtual ESock::MProviderSelector* DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences);
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	};

namespace LegacyLoopbackTMStates
	{

	typedef MeshMachine::TNodeContext<CLegacyLoopBackTierManager,TMStates::TContext> TContext;

	DECLARE_SMELEMENT_HEADER( TSelectionTest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSelectionTest )
	}


#endif // __LEGACY_LOOPBACK_TIER_MANAGER_H__

