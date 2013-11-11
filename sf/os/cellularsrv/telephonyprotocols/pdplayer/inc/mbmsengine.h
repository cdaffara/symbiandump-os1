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
 @file
 @internalComponent
*/

#ifndef MBMSENGINE_H
#define MBMSENGINE_H

#include <networking/mbmsparams.h>
#include <etel.h>
#include <pcktlist.h>
#include <comms-infras/coretiermanager.h>
#include "pdptiermanager.h"

using namespace ESock;
using namespace ConnectionServ;

class CMBMSServiceRequest;

class CMBMSEngine: public CActive
{
public:
	static CMBMSEngine* NewL(const Messages::TNodeId& aMbmsTMCommsId, MPacketServiceNotifier& aPacketServiceNotifier);

	~CMBMSEngine();

	// From CActive
	virtual void	RunL();
	virtual void	DoCancel();
	virtual TInt	RunError(TInt aError);

	void CheckPendingRequests();
	void RemoveFromRequestListL(const Messages::TRuntimeCtxId& aNodeCtxId);
	void CancelAndRemoveFromRequestListL(const Messages::TRuntimeCtxId& aNodeCtxId);
	void AddToRequestListL(
			Messages::RNodeInterface* aNodeInterface,
			const Messages::TRuntimeCtxId& aNodeCtxId,
			const Messages::TNodeSignal::TMessageId& aRequestType,
			ESock::CRefCountOwnedParameterBundle* aRequestBundleOwner
			);
 	RPacketService& GetRPacketService();

private:
	CMBMSEngine(const Messages::TNodeId& aMbmsTMCommsId, MPacketServiceNotifier& aPacketServiceNotifier);
	void ConstructL();
	TBool InitialisePhoneL();
	void DeleteIndexElement(TUint aIndex);
	TUint GetRequestElementL(const Messages::TRuntimeCtxId& aNodeCtxId);

private:
	enum TPhoneState
	   {
	 	EInitialising = 0,
	 	ESetAttachMode,
	 	EAttachModeComplete
	   };
	//tel server
	RTelServer iTelServer;
	//phone
	RPhone iPhone;
	RPhone::TStatus iPhoneStatus;
	RPacketService iPacketService;

	RTelServer::TPhoneInfo iPhoneInfo;

	RPointerArray<CMBMSServiceRequest> iMBMSRequestList;

	Messages::TNodeId	iMbmsTMCommsId;

	//to maintain different states in state machine
	TPhoneState iPhoneState;
	//to check for any outstanding requests
	TBool iPendingRequests;
	MPacketServiceNotifier& iPacketServiceNotifier;	

};
#endif // MBMSENGINE_H

