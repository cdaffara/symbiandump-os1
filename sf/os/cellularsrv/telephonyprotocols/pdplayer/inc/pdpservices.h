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

#ifndef SYMBIAN_PDPSERVICES_H
#define SYMBIAN_PDPSERVICES_H

#include "mbmsengine.h"
#include <comms-infras/coretiermanager.h>
#include <networking/mbmsparams.h>
#include <pcktretrieve.h>

using namespace ESock;
using namespace ConnectionServ;

class CMBMSEngine;

/* This class is used to send and retrieve MBMS parameters from the client and Etel.
 The MBMS parameters are fetched from the bundle and sent to Etel inorder to process the MBMS requests.
 Different State Changes are maintained in this class to carry out the MBMS requests.*/
class CMBMSServiceRequest: public CActive
{
public:
	static CMBMSServiceRequest* NewL(
					CMBMSEngine& aMBMSEngine,
					const Messages::TNodeId& aMBMSTMCommsId,
					Messages::RNodeInterface* aOriginatorInterface,
					const Messages::TRuntimeCtxId& aOriginator,
					const Messages::TNodeSignal::TMessageId& aRequestType,
					ESock::CRefCountOwnedParameterBundle* aRequestBundleOwner,
					const XMBMSServiceQuerySet::TQueryType aQueryType
					);

	~CMBMSServiceRequest();

	// From CActive
	virtual void	RunL();
	virtual void	DoCancel();
	virtual TInt	RunError(TInt aError);

	//This function is used to cancel the current outstanding requests.
	void CancelMessage(const TInt aError);
	//This function is used to get the client ID.
	const Messages::TNodeId& GetClientId();
	//This function is used to start MBMS requests.
	void StartRequest();

private:
	CMBMSServiceRequest(
			CMBMSEngine& aMBMSEngine,
			const Messages::TNodeId& aMBMSTMCommsId,
			const Messages::TNodeSignal::TMessageId& aRequestType,
			ESock::CRefCountOwnedParameterBundle* aRequestBundleOwner,
			const XMBMSServiceQuerySet::TQueryType aQueryType
			);

	void ConstructL(Messages::RNodeInterface* aOriginatorInterface, const Messages::TRuntimeCtxId& aOriginator);

	//This function is used to update the service list which is passed to Etel.
	CPcktMbmsMonitoredServiceList* UpdateMonitorServiceListL();
	//This function is used to prepare result bundle.
	CConnectionServParameterBundle* PrepareMonitorResultBundleL() const;
	//This function is used to prepare result bundle containing bearer availability.
	CConnectionServParameterBundle* PrepareBearerResultBundleL() const;
	//This function is used to prepare empty bundle.
	CConnectionServParameterBundle* PrepareRemoveAllBundleL() const;
	/*This function is used to prepare  bundle containing current and max count of service/monitor
	 list table.*/
	CConnectionServParameterBundle* PrepareCountBundleL() const;
	//This function is used to send result bundle to the client.
	void SendResultBundleL();

private:
	// Required for setting and controlling the StateChanges within the engine
	enum TScanEngineState
	 {
	 EInitialising = 0,
	 EChecking,
	 EBearerAvailable,
	 ERetrieveBearerAvailability,
	 EServiceAdd,
	 ENotifyService,
	 EServiceRemove,
	 EServiceRemoveAll,
	 ERemoveAllComplete,
	 EStartMonitor,
	 EGetMonitorList,
	 EResultCount,
	 EGetCountMonitorList,
	 EGetCountServiceList
	 };

	RPacketService::TStatus iPsdStatus;

    //to maintain different states in state machine
	TScanEngineState iScanEngineState;

	//required by CMBMSServiceRequest.
	CMBMSEngine& iMBMSEngine;

	// Needed for NotificationRequest Messages
	Messages::TNodeId iMBMSTMCommsId;
	Messages::RRequestOriginator iRequestOriginator;
	Messages::TNodeSignal::TMessageId iRequestType;

	//parameter bundle
	ESock::CRefCountOwnedParameterBundle* iObjectBundleOwner;

	TMbmsNetworkServiceStatus iNetworkServiceStatus;
	//operation type on MBMS Monitor List table
	TMbmsAction iMbmsAction;

	CRetrievePcktMbmsMonitoredServices*	iRetrievePcktMbms;
	//MBMS query set by the client
	XMBMSServiceQuerySet::TQueryType iQueryType;

	//to maintain current and max count of entries in monitor/service list table.
	TInt iCount;
	TInt iMaxCount;

};
#endif // SYMBIAN_PDPSERVICES_H

