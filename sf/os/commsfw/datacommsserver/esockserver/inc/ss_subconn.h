// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SS_SUBCONN_H
#define SS_SUBCONN_H

#include <ss_std.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_mmnode_subsess.h>
#include <cs_subconevents.h>

using namespace CorePanics;

const int KMaxUidFilterListLen = 10;

namespace SubConnActivities
{
	class CRejoin;
	class CBuildStack;
	class CSetParameters;
}

namespace SubConnStates
{
	class TJoinCPR;
	class TRequestServiceProvider;
	class TJoinTheOtherOwner;
	class TLeaveCPR;
	class TSendParamsToServiceProvider;
	class TStoreParams;
	class TNoTagOrParamsPresent;
	class TSendParamRequest;
	class TWriteSubConnParamsLength;
	class TWriteSubConnParams;
	class TFillInEvent;
	class TNoTagOrWaitForIncoming;
	class TEnqueueEvent;
	class TNoTagOrActiveWhenEventEnqued;
	class TStoreNewParams;
	class TSendBuildStack;
	class TRequestServiceProviderFromCPR;
	class TNoTagOrAttachToDefaultOrWaitForIncoming;
}

namespace ESock
{

class ASubConnectionLegacy;

NONSHARABLE_CLASS(CSubConnection) : public CMMSockSubSession, private ASubSessionPlatsecApiExt,
                                    public ITFHIERARCHY_LINK_1(CSubConnection, MeshMachine::AMMNodeBase, MPlatsecApiExt)

/**
Handle subconnection-related asynchronous client requests that may not complete immediately
@internalComponent
*/
	{
	friend class SubConnStates::TJoinCPR;
	friend class SubConnStates::TRequestServiceProvider;
	friend class SubConnStates::TJoinTheOtherOwner;
	friend class SubConnStates::TLeaveCPR;
	friend class SubConnStates::TSendParamsToServiceProvider;
	friend class SubConnStates::TStoreParams;
	friend class SubConnStates::TSendParamRequest;
	friend class SubConnStates::TNoTagOrParamsPresent;
	friend class SubConnStates::TWriteSubConnParamsLength;
	friend class SubConnStates::TWriteSubConnParams;
	friend class SubConnStates::TFillInEvent;
	friend class SubConnStates::TNoTagOrWaitForIncoming;
	friend class SubConnStates::TEnqueueEvent;
	friend class SubConnStates::TNoTagOrActiveWhenEventEnqued;
	friend class SubConnStates::TStoreNewParams;
	friend class SubConnStates::TSendBuildStack;
	friend class SubConnStates::TRequestServiceProviderFromCPR;
	friend class SubConnStates::TNoTagOrAttachToDefaultOrWaitForIncoming;
	friend class SubConnActivities::CRejoin;
	friend class SubConnActivities::CBuildStack;
	friend class SubConnActivities::CSetParameters;
	friend class AConnectionLegacy;
#ifdef __X86GCC__
	// gcc-mingw does not support template function instantiations as friends, only the
	// generic template.
	template<class NODE>
	friend  NODE* mcfnode_cast(Messages::ANode* aNode);
	template<class NODE>
	friend  NODE& mcfnode_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
    friend CSubConnection* mcfnode_cast<CSubConnection>(Messages::ANode* aNode);
    friend CSubConnection& mcfnode_cast<CSubConnection>(Messages::ANode& aNode);
#endif

    static const TInt KMaxEventQueueLength = 8;

public:
	typedef ITFHIERARCHY_LINK_1(CSubConnection, MeshMachine::AMMNodeBase, MPlatsecApiExt) TIfStaticFetcherNearestInHierarchy;

public:
    static CSubConnection* NewL(CConnection& aConnection, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
    virtual ~CSubConnection();

    //-----------------------------------
    //from TInterfaceControl
    //-----------------------------------
    void ReturnInterfacePtrL(MPlatsecApiExt*& aInterface)
        {
        aInterface = this;
        }

protected:
    CSubConnection(CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId);
    void ConstructL(CConnection& aConnection);
    CConnection& ControlProvider();

    //-----------------------------------
    //from CMMSockSubSession
    //-----------------------------------
    void Received(MeshMachine::TNodeContextBase& aContext);
    void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);


    //-----------------------------------
    //from CSockSubSession
    //-----------------------------------
protected:
    void ProcessMessageL();
    virtual void FinalCompleteAllBlockedMessages(TInt aResult);

public:
  	virtual Den::TSubSessInfo Type() const
  		{
  		return TCFSubSessInfo(TCFSubSessInfo::ESubConnection);
  		}

inline RSubConnection::TSubConnType SubConnType() const
	{
	return iSubConnType;
	}

public:
	IMPORT_C RCFParameterFamilyBundleC& GetOrCreateParameterBundleL();
	IMPORT_C RCFParameterFamilyBundleC& CreateParameterBundleL();

private:
  	RCFParameterFamilyBundleC iParameterBundle;
  	RSubConnection::TSubConnType iSubConnType;
    TNotificationQueue<CRefCountOwnedSubConNotification*, KMaxEventQueueLength> iEventQueue;
	};

} // namespace Esock

#endif
// SS_SUBCONN_H

