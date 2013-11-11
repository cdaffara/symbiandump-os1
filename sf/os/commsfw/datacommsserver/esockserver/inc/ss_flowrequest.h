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

/**
 @file
 @internalTechnology
 @released
*/

#if !defined(__SS_FLOWREQUEST_H__)
#define __SS_FLOWREQUEST_H__

#include <comms-infras/ss_nodemessages_internal.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <comms-infras/ss_common.h>
#include <cs_subconparams.h>

#include <elements/nm_messages_internal.h>



class CSockSession;
namespace ESock
{

//
//CFlowRequest
NONSHARABLE_CLASS(CFlowRequest) : public CBase,
	                              public ACFMMNodeIdBase,
                                  public ASubSessionPlatsecApiExt,
	                              public ITFHIERARCHY_LINK_1(CFlowRequest, MeshMachine::AMMNodeBase, MPlatsecApiExt)

	{
public:
	typedef ITFHIERARCHY_LINK_1(CFlowRequest, MeshMachine::AMMNodeBase, MPlatsecApiExt) TIfStaticFetcherNearestInHierarchy;

public:
	static CFlowRequest* NewL(TSubSessionUniqueId aSubSessionUniqueId);

    virtual ~CFlowRequest();
    void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	void ReturnInterfacePtrL(MPlatsecApiExt*& aInterface);

protected:
	void Received(MeshMachine::TNodeContextBase& aContext);

public:
	CFlowRequest(TSubSessionUniqueId aSubSessionUniqueId);
	RCFParameterFamilyBundleC iFlowParameters;
	};

//
//CImplicitFlowRequest - specialised for implicit
NONSHARABLE_CLASS(CImplicitFlowRequest) : public CFlowRequest
	{
public:
	~CImplicitFlowRequest();

	static CImplicitFlowRequest* NewL(TSubSessionUniqueId aSubSessionUniqueId, const Messages::TNodeId& aTierManagerFC, TUid aTierId);

public:
	Messages::TNodeId iTierManagerFC;
	Messages::TNodeId iCSR;
	TUid iTierId;
	
protected:
	CImplicitFlowRequest(TSubSessionUniqueId aSubSessionUniqueId, const Messages::TNodeId& aTierManagerFC, TUid aTierId);
	};

//
//TCFFlowRequestBase
struct TCFFlowRequestBase : public Messages::TSelfDispatcherAndErrorHandler
	{
protected:
	TCFFlowRequestBase(TSubSessionUniqueId aSubSessionUniqueId);
	TCFFlowRequestBase(TSubSessionUniqueId aSubSessionUniqueId, TFlowParams& aFlowParams) :
		iSubSessionUniqueId(aSubSessionUniqueId),
		iFlowParams(aFlowParams)
		{}
	TCFFlowRequestBase(TFlowParams& aFlowParams) :
		iFlowParams(aFlowParams)
		{}
	TCFFlowRequestBase()
		{
		}

protected:
	DATA_VTABLE

protected:
	TSubSessionUniqueId iSubSessionUniqueId;
	TFlowParams iFlowParams;
	};

//
//TCFImplicitFlowRequest
NONSHARABLE_STRUCT(TCFImplicitFlowRequest) : public TCFFlowRequestBase
	{
public:
	explicit TCFImplicitFlowRequest(
		TSubSessionUniqueId aSubSessionUniqueId,
		TFlowParams& aFlowParams)
		:	TCFFlowRequestBase(aSubSessionUniqueId, aFlowParams)
		{
		}
	TCFImplicitFlowRequest(TFlowParams& aFlowParams)
		: TCFFlowRequestBase(aFlowParams)
		{
		}
protected:
	void StartL(const Messages::TNodeId& aSender, const Messages::ANode& aItf);
	void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	TCFImplicitFlowRequest()
	:	TCFFlowRequestBase()
		{
		}
public:
	DATA_VTABLE
    DECLARE_MVIP_CTR(TCFImplicitFlowRequest)
	};

//
//TCFConnFlowRequest
NONSHARABLE_STRUCT(TCFConnFlowRequest) : public TCFFlowRequestBase
	{
public:
	explicit TCFConnFlowRequest(
		TSubSessionUniqueId aSubSessionUniqueId,
		CSockSession& aSession,
		TUint aHandle,
		TFlowParams& aFlowParams
		)
	:	TCFFlowRequestBase(aSubSessionUniqueId, aFlowParams),
		iSession(&aSession),
		iHandle(aHandle)
		{
		}

	explicit TCFConnFlowRequest(TSubSessionUniqueId aSubSessionUniqueId, CSockSession& aSession, TUint aHandle)
	:	TCFFlowRequestBase(aSubSessionUniqueId),
		iSession(&aSession),
		iHandle(aHandle)
		{
		}

protected:
	void StartL(const Messages::TNodeId& aSender);
	void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	TCFConnFlowRequest()
	:	TCFFlowRequestBase()
		{
		}

public:
    CSockSession* iSession;
    TUint iHandle;
public:
	DATA_VTABLE
    DECLARE_MVIP_CTR(TCFConnFlowRequest)
	};

//
//TCFSubConnFlowRequest
NONSHARABLE_STRUCT(TCFSubConnFlowRequest) : public TCFFlowRequestBase
	{
public:
	explicit TCFSubConnFlowRequest(TSubSessionUniqueId aSubSessionUniqueId, CSockSession& aSession, TUint aHandle,
		TFlowParams& aFlowParams)
	:	TCFFlowRequestBase(aSubSessionUniqueId, aFlowParams),
		iSession(&aSession),
		iHandle(aHandle)
		{
		}

	explicit TCFSubConnFlowRequest(TSubSessionUniqueId aSubSessionUniqueId, CSockSession& aSession, TUint aHandle)
	:	TCFFlowRequestBase(aSubSessionUniqueId),
		iSession(&aSession),
		iHandle(aHandle)
		{
		}

protected:
	void StartL(const Messages::TNodeId& aSender);
	void DispatchL(const Messages::TRuntimeCtxId& aSender, const Messages::TRuntimeCtxId& aRecipient);
	TCFSubConnFlowRequest()
	:	TCFFlowRequestBase()
		{
		}

public:
    CSockSession* iSession;
    TUint iHandle;

public:
	DATA_VTABLE
    DECLARE_MVIP_CTR(TCFSubConnFlowRequest)
	};


// aggregate class for ipc subsessions which request flows
NONSHARABLE_CLASS(AIPCFlowRequester)
	{
public:
	AIPCFlowRequester() : iIsFlowRequestPending(EFalse) {}

	void SetFlowRequestPending(TBool aPending)
		{
		iIsFlowRequestPending = aPending;
		}

	TBool FlowRequestPending()
		{
		return iIsFlowRequestPending;
		}

	void AdoptFlowRequestMessage(const RMessage2& aFlowRequestMessage)
		{
		iFlowRequestMessage.Adopt(static_cast<Den::RSafeMessage&>(const_cast<RMessage2&>(aFlowRequestMessage)));
		}

	void CompleteFlowRequestMessage(TInt err)
		{
		iFlowRequestMessage.Complete(err);
		iFlowRequestMessage.ResetHandle();
		}

protected:
	Den::RSafeMessage iFlowRequestMessage;
	TBool iIsFlowRequestPending;
	};

} // namespace ESock

#endif
// __SS_FLOWREQUEST_H__


