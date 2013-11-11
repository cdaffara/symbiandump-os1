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

#ifndef SYMBIAN_PDPTIERMANAGER_H
#define SYMBIAN_PDPTIERMANAGER_H

#include <comms-infras/coretiermanager.h>
#include <comms-infras/contentionmanager.h>
#include <etelpckt.h>
#include <pdp_contention.h>
using namespace ESock;

class CMBMSEngine;
class CContextStatusMonitor;
class CContextTypeChecker;


class MPacketServiceNotifier
	{
public:	
	virtual void PacketServiceAttachedCallbackL() = 0;
	};


//PDP Tier Manager
class CPDPTierManager : public CCoreTierManager, public MPacketServiceNotifier
	{
public:
	static CPDPTierManager* NewL(ESock::CTierManagerFactoryBase& aFactory);
	~CPDPTierManager();

public:
	void PacketServiceAttachedCallbackL();
	TBool IsUnavailableDueToContention(const ESock::CMetaConnectionProviderBase* aMetaConnectionProvider) const;

protected:
    CPDPTierManager(ESock::CTierManagerFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);

    virtual ESock::MProviderSelector* DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences);
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	TBool HandleContentionL(ESock::CMetaConnectionProviderBase* aMcpr, Messages::TNodeId& aPendingCprId, TUint aPriority);
private:
    void ConstructL();

private:
	CMBMSEngine* iMBMSEngine;
#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
	CPdpContentionManager* iPdpContentionManager;
#endif
	
	};



#endif // SYMBIAN_PDPTIERMANAGER_H

