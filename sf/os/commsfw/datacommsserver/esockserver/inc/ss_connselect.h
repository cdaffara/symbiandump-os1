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

#if !defined(__SS_CONNSELECT_H__)
#define __SS_CONNSELECT_H__

#include <ss_select.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <comms-infras/ss_nodemessages_serviceprovider.h>

namespace ESock
{

class CSelectionRequest : public CBase,
                          public Messages::ASimpleNodeIdBase,
	                      public MPlatsecApiExt,
						  protected NetInterfaces::TInterfaceControl,
						  public ITFHIERARCHY_1(CSelectionRequest,MPlatsecApiExt)

/**
Connection selection request.

@internalTechnology
*/
	{
public:
	typedef ITFHIERARCHY_1(CSelectionRequest,MPlatsecApiExt) TIfStaticFetcherNearestInHierarchy;

public:
	static CSelectionRequest* NewL(const Messages::TRuntimeCtxId& aRequestingClient, const Messages::TNodeId& aTierManagerId);
	virtual ~CSelectionRequest();

	void Select(const TSelectionPrefs& aSelectionPreferences);
	void SelectConnPrefList(const RConnPrefList& aConnPrefList);
	void Provision(const Meta::SMetaData* aProvisionConfig);
	void Cancel(); //Cancels all active requests

	void JoinComplete(const Messages::TNodeId& aSender, TCFPeer::TJoinComplete& aCFMessage);
    void CommsBinderResponse(const TCFServiceProvider::TCommsBinderResponse& aMsg);
	void SelectComplete(const Messages::TNodeId& aSenderId, const Messages::TNodeId& aMcprId, const TProviderInfo& aMcprInfo);

	// from MPlatsecApiExt
	virtual TInt SecureId(TSecureId& aResult) const;
    virtual TInt VendorId(TVendorId& aResult) const;
    virtual TBool HasCapability(const TCapability aCapability) const;
    virtual TInt CheckPolicy(const TSecurityPolicy& aPolicy) const;

	void ReturnInterfacePtrL(MPlatsecApiExt*& aInterface);
	virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);

protected:
	explicit CSelectionRequest(const Messages::TRuntimeCtxId& aRequestingClient, const Messages::TNodeId& aTierManagerId);
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	TInt FindActiveRequest(const Messages::TRuntimeCtxId& aMcprId);
	void HandleProviderSelection(const Messages::TNodeId& aMcprId);
	void ProviderSelectionFinished(const Messages::TRuntimeCtxId& aMcprId); //Our selection request (SP or SNL) has been fully served -> SelectComplete(NULL).
	void SelectionFinished(); //The whole selection has finished.
	void PostError(const Messages::TNodeSignal::TMessageId& aMessageId, TInt aError);
	void InitialiseDestroy();

private:
	void SelectionError(const Messages::TRuntimeCtxId& aSenderId, TInt aError); //Selection error
	void CommsBinderRequestError(const Messages::TRuntimeCtxId& aSenderId, Messages::TEBase::TError& aCFMessage); //CommsBinderRequest returned error

private:
	enum {EIdle = 0x00, ESelecting = 0x01, ECancelling = 0x02, ERequestingCommsBinder = 0x04}; //Selection Status
	TInt iOriginalError;
	TSelectionPrefs iSelectionPreferences;
	Messages::TNodeId iTierManagerId;
	Messages::TNodeId iTopMcprId;
	TProviderInfo iTopMcprInfo;
	Messages::RNodeInterface     iRequestingNode;
    Messages::RRequestOriginator iRequest;
	RArray<Messages::RNodeInterface> iActiveRequests; //List of all service providers (mcprs) currently processing our selection requests
	TUint iSelectionStatus : 4;
	TUint iDestroying      : 1;
	TUint iJoined          : 1;
	ASubSessionPlatsecApiExt* iPlatsecApiExt;
    const Meta::SMetaData* iProvision;
	RConnPrefList iConnPrefList;
	};

} //namespace ESock


#endif //__SS_CONNSELECT_H__

