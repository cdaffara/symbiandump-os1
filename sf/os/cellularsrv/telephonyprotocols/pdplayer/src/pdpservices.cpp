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
#include "pdpservicesTraces.h"
#endif

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_corepractivities.h> // TODO is this needed?
#include <comms-infras/coretiermanagerstates.h> // TODO is this needed?
#include <comms-infras/coretiermanageractivities.h> // TODO is this needed?
#include "pdpservices.h"
#include "pdptiermanager.h"
#include <pcktcs.h>
#include <comms-infras/es_connectionserv.h>

using namespace Messages;
using namespace MeshMachine; // TODO is this needed?
using namespace ESock;
using namespace NetStateMachine; // TODO is this needed?
using namespace ConnectionServ;


//Constructor for CMBMSServiceRequest
CMBMSServiceRequest::CMBMSServiceRequest(
		CMBMSEngine& aMBMSEngine,
		const TNodeId& aMBMSTMCommsId,
		const Messages::TNodeSignal::TMessageId& aRequestType,
		CRefCountOwnedParameterBundle* aRequestBundleOwner,
		const XMBMSServiceQuerySet::TQueryType aQueryType
		)
	:CActive(EPriorityStandard),
	iMBMSEngine(aMBMSEngine),
	iMBMSTMCommsId(aMBMSTMCommsId),
	iRequestType(aRequestType),
	iObjectBundleOwner(aRequestBundleOwner),
	iQueryType(aQueryType)
	{
	CActiveScheduler::Add(this);
	}

/**
The NewL factory function for CMBMSServiceRequest.
@param aMBMSTMCommsId comms Id.
@param aClientId Node Channel Id.
@param aRequestType Message Id
@param aRequestBundleOwner Parameter Bundle.
@return CMBMSServiceRequest*
*/
CMBMSServiceRequest* CMBMSServiceRequest::NewL(
		CMBMSEngine& aMBMSEngine,
		const TNodeId& aMBMSTMCommsId,
		RNodeInterface* aOriginatorInterface,
		const TRuntimeCtxId& aOriginator,
		const Messages::TNodeSignal::TMessageId& aRequestType,
		CRefCountOwnedParameterBundle* aRequestBundleOwner,
		const XMBMSServiceQuerySet::TQueryType aQueryType
		)
	{
	CMBMSServiceRequest* self = new(ELeave) CMBMSServiceRequest(
													aMBMSEngine,
													aMBMSTMCommsId,
													aRequestType,
													aRequestBundleOwner,
													aQueryType
													);

	CleanupStack::PushL(self);
	self->ConstructL(aOriginatorInterface, aOriginator);
	CleanupStack::Pop(self);

	return self;
	}
/**
ConstructL for CMBMSEngine.
*/
void CMBMSServiceRequest::ConstructL(RNodeInterface* aOriginatorInterface, const TRuntimeCtxId& aOriginator)
	{

	if (iRequestOriginator.Open(*aOriginatorInterface, aOriginator) != KErrNone)
		{
		// Fix the code - looks like this originator already has an outstanding request
		User::Leave(KErrArgument);
		}
	}

/**
Destructor for CMBMSServiceRequest.
*/
CMBMSServiceRequest::~CMBMSServiceRequest()
	{
	Cancel();
	iObjectBundleOwner->Close();
	delete iRetrievePcktMbms;
	iRequestOriginator.Close();
	}

/**
This function is used to cancel the notification message sent by the client
@param aError const TInt
@return void
*/
void CMBMSServiceRequest::CancelMessage(const TInt aError)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_CANCELMESSAGE_1, "CMBMSServiceRequest[%08x]::CancelMessage(%d)", (TUint)this, aError);
	if (iRequestType == TCFTierStatusProvider::TTierNotificationRegistration::Id())
		{
		iRequestOriginator.PostMessage(
				iMBMSTMCommsId,
				TEBase::TError(TCFTierStatusProvider::TTierNotificationRegistration::Id(), aError).CRef()
				);
		}
	}

/**
This function is used to process the MBMS requests.The RunL() function of CMBMSServiceRequest is called
based on the state machines.
@param aError const TInt
@return void
*/
void CMBMSServiceRequest::StartRequest()
	{
	iScanEngineState = EChecking;

	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
This function is used to return the client id.
@param None
@return TCFNodeChannelId
*/
const TNodeId& CMBMSServiceRequest::GetClientId()
	{
	return iRequestOriginator.Node().RecipientId();
	}

/**
This is CActive overloaded function.Cancel all outstanding requests.
@param None
@return Void
*/
void CMBMSServiceRequest::DoCancel()
	{
     switch (iScanEngineState)
		{
		case ENotifyService:
		case ERemoveAllComplete:
			iMBMSEngine.GetRPacketService().CancelAsyncRequest(EPacketUpdateMbmsMonitorServiceList);
		break;
		case ERetrieveBearerAvailability:
			iMBMSEngine.GetRPacketService().CancelAsyncRequest(EPacketGetMbmsNetworkServiceStatus);
		break;
		case EStartMonitor:
			iMBMSEngine.GetRPacketService().CancelAsyncRequest(EPacketNotifyMbmsServiceAvailabilityChange);
		break;
		case EResultCount:
			if(iQueryType == XMBMSServiceQuerySet::ECountMonitorList)
			   {
			   iMBMSEngine.GetRPacketService().CancelAsyncRequest(EPacketEnumerateMbmsMonitorServiceList);
			   }
			else if(iQueryType == XMBMSServiceQuerySet::ECountActiveServiceList)
			   {
			   iMBMSEngine.GetRPacketService().CancelAsyncRequest(EPacketEnumerateMbmsActiveServiceList);
			   }
		break;
		case EGetMonitorList:
			if(iRetrievePcktMbms != NULL)
			iRetrievePcktMbms->Cancel();
		break;
		}
	}

/**
This is CActive overloaded function.
@param None
@return TInt
*/
TInt CMBMSServiceRequest::RunError(TInt aError)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNERROR_1, "CMBMSServiceRequest[%08x]::RunError(%d)", (TUint)this, aError);

	// Stop the ScanEngine components
	Cancel();

	if (iRequestType == TCFTierStatusProvider::TTierNotificationRegistration::Id())
		{
		iRequestOriginator.PostMessage(
						iMBMSTMCommsId,
						TEBase::TError(TCFTierStatusProvider::TTierNotificationRegistration::Id(), aError).CRef()
						);
		}
	return KErrNone;
	}

/**
This is CActive overloaded function.
@param None
@return Void
*/
void CMBMSServiceRequest::RunL()
	{
    TRequestStatus* status = &iStatus;
	if (iStatus == KErrNone)
		{
		switch(iScanEngineState)
			{
			case EChecking:
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_1, "CMBMSServiceRequest[%08x]::In EChecking State", (TUint)this);
				User::LeaveIfError(iMBMSEngine.GetRPacketService().GetStatus(iPsdStatus)); // get initial status

				//check the query type
				if(iQueryType == XMBMSServiceQuerySet::EBearerAvailability)
					iScanEngineState = EBearerAvailable;
				else if (iQueryType == XMBMSServiceQuerySet::EAddService)
					iScanEngineState = EServiceAdd;
				else if (iQueryType == XMBMSServiceQuerySet::ERemoveService)
					iScanEngineState = EServiceRemove;
				else if (iQueryType == XMBMSServiceQuerySet::ERemoveAllService)
					iScanEngineState = EServiceRemoveAll;
				else if (iQueryType == XMBMSServiceQuerySet::ECountMonitorList)
					iScanEngineState = EGetCountMonitorList;
				else if (iQueryType == XMBMSServiceQuerySet::ECountActiveServiceList)
					iScanEngineState = EGetCountServiceList;

				SetActive();
				User::RequestComplete(status, KErrNone);
			break;

			case EBearerAvailable:
				//get MBMS bearer availability.
				 OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_2, "CMBMSServiceRequest[%08x]::In EBearerAvailable State", (TUint)this);
				 iMBMSEngine.GetRPacketService().GetMbmsNetworkServiceStatus(iStatus,ETrue,iNetworkServiceStatus);
			 	 iScanEngineState = ERetrieveBearerAvailability;
				 SetActive();
			break;

		   	case ERetrieveBearerAvailability:
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_3, "CMBMSServiceRequest[%08x]::In ERetrieveBearerAvailability State", (TUint)this);
			    //create a parameter bundle and send the results to the client.
			    SendResultBundleL();

			    iScanEngineState = EBearerAvailable;
			    SetActive();
			    User::RequestComplete(status, KErrNone);
			break;

			case EServiceAdd:
				{
				CPcktMbmsMonitoredServiceList* mbmsServiceList = UpdateMonitorServiceListL();
			    CleanupStack::PushL(mbmsServiceList);
			    iMBMSEngine.GetRPacketService().UpdateMbmsMonitorServiceListL(iStatus,EAddEntries,mbmsServiceList);
			    CleanupStack::PopAndDestroy(mbmsServiceList);

				SetActive();
				iScanEngineState = ENotifyService;
				}
			break;

			case EServiceRemoveAll:
			    {
			    iMBMSEngine.GetRPacketService().UpdateMbmsMonitorServiceListL (iStatus,ERemoveAllEntries);

				SetActive();
				iScanEngineState = ERemoveAllComplete;
			    }
			break;

			case ERemoveAllComplete:
				SendResultBundleL();
			break;

			case EServiceRemove:
				{
				CPcktMbmsMonitoredServiceList* mbmsSeriveList = UpdateMonitorServiceListL();
			    CleanupStack::PushL(mbmsSeriveList);
			    iMBMSEngine.GetRPacketService().UpdateMbmsMonitorServiceListL (iStatus,ERemoveEntries,mbmsSeriveList);
			    CleanupStack::PopAndDestroy(mbmsSeriveList);

				SetActive();
				iScanEngineState = ENotifyService;
				}
			break;

			case EGetCountMonitorList:
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_4, "CMBMSServiceRequest[%08x]::In ECountActiveServiceList State", (TUint)this);
				iMBMSEngine.GetRPacketService().EnumerateMbmsMonitorServiceList(iStatus,iCount,iMaxCount);

				SetActive();
				iScanEngineState = EResultCount;
				}
			break;

			case EGetCountServiceList:
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_5, "CMBMSServiceRequest[%08x]::In ECountActiveServiceList State", (TUint)this);
				iMBMSEngine.GetRPacketService().EnumerateMbmsActiveServiceList(iStatus,iCount,iMaxCount);

				SetActive();
				iScanEngineState = EResultCount;
				}
			break;

			case EResultCount:
				SendResultBundleL();
			break;

			case ENotifyService:
				//notify for MBMS Services.
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_6, "CMBMSServiceRequest[%08x]::In ENotifyService State", (TUint)this);
				iMBMSEngine.GetRPacketService().NotifyMbmsServiceAvailabilityChange(iStatus);
			    SetActive();
				iScanEngineState = EStartMonitor;
			break;

			case EStartMonitor:
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_7, "CMBMSServiceRequest[%08x]::In EStartMonitor State", (TUint)this);
				iRetrievePcktMbms = CRetrievePcktMbmsMonitoredServices::NewL(iMBMSEngine.GetRPacketService());
			    iRetrievePcktMbms->Start(iStatus);

			    SetActive();
				iScanEngineState = EGetMonitorList;
			break;

			case EGetMonitorList:
				SendResultBundleL();

			    SetActive();
			    User::RequestComplete(status, KErrNone);
			    iScanEngineState = ENotifyService;
			break;

		  }
		}
	else if(((iStatus.Int() == KErrMbmsImpreciseServiceEntries))||((iStatus.Int() == KErrNotFound)))
		 {
		  OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_8, "CMBMSServiceRequest[%08x]::RunL() KErrMbmsImpreciseServiceEntries || KErrNotFound", (TUint)this);
		  CancelMessage(iStatus.Int());
		  iMBMSEngine.RemoveFromRequestListL(GetClientId());
		 }
	else
		{
		 OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_RUNL_9, "CMBMSServiceRequest[%08x]::RunL() ERROR: Incorrect status, Aborting", (TUint)this);
		 User::Leave(iStatus.Int());
		}

	}

/**
This function is used to update Mbms Monitored Service List
@param None
@return Void
*/
CPcktMbmsMonitoredServiceList* CMBMSServiceRequest::UpdateMonitorServiceListL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_UPDATEMONITORSERVICELISTL_1, "CMBMSServiceRequest[%08x]::In UpdateMonitorServiceListL()", (TUint)this);

	//Read the entries in the parameter bundle and pass them to Etel.
	CPcktMbmsMonitoredServiceList* serviceList= CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(serviceList);

	if((iScanEngineState == EServiceAdd) || (iScanEngineState == EServiceRemove))
	   {
		const ConnectionServ::CConnectionServParameterBundle* objectBundle1 = static_cast<const CConnectionServParameterBundle*>(iObjectBundleOwner->Ptr());
	    ConnectionServ::CConnectionServParameterBundle* objectBundle = const_cast<CConnectionServParameterBundle *>(objectBundle1);

	    TUint numObjectPSCs = objectBundle->CountParamSetContainers();

		for(TUint i = 0; i < numObjectPSCs; i++)
			{
			CParameterSetContainer* objectPSC = objectBundle->GetParamSetContainer(i);
			XMBMSServiceParameterSet* objectMPS = XMBMSServiceParameterSet::FindInParamSetContainer(*objectPSC);
			RPacketService::TMbmsServiceAvailabilityV1 mbmsService;
			mbmsService.iTmgi.SetServiceId(objectMPS->GetChannelInfo()->GetTmgi().GetServiceId());
			mbmsService.iTmgi.SetMCC(objectMPS->GetChannelInfo()->GetTmgi().GetMCC());

			mbmsService.iTmgi.SetMNC(objectMPS->GetChannelInfo()->GetTmgi().GetMNC());
			mbmsService.iMbmsServiceMode = objectMPS->GetServiceMode();
			mbmsService.iMbmsAccessBearer = objectMPS->GetChannelInfo()->GetScope();
			serviceList->AddEntryL(mbmsService);
			}
	   }
	CleanupStack::Pop(serviceList);
	return serviceList;
	}

/**
This function is used to send result parameter bundle to the client
@param None
@return Void
*/
void CMBMSServiceRequest::SendResultBundleL()
	{
	CConnectionServParameterBundle* returnBundle = NULL;
	if(iScanEngineState == EGetMonitorList)
	   returnBundle = PrepareMonitorResultBundleL();
	else if(iScanEngineState == ERetrieveBearerAvailability)
	   returnBundle = PrepareBearerResultBundleL();
	else if (iScanEngineState == ERemoveAllComplete)
	   returnBundle = PrepareRemoveAllBundleL();
	else if (iScanEngineState == EResultCount)
	   returnBundle = PrepareCountBundleL();

	CleanupStack::PushL(returnBundle);// Ownership of the bundle passes to the Server
	CRefCountOwnedParameterBundle* returnBundleOwner = new (ELeave) CRefCountOwnedParameterBundle(returnBundle);
	returnBundleOwner->Open(); // Open the initial reference
	CleanupClosePushL(*returnBundleOwner);

	if (iRequestType == TCFTierStatusProvider::TTierNotificationRegistration::Id())
		{
		iRequestOriginator.PostMessage(
						iMBMSTMCommsId,
						TCFTierStatusProvider::TTierNotification(returnBundleOwner).CRef()
						);
		}
	CleanupStack::Pop(returnBundleOwner);
	CleanupStack::Pop(returnBundle);
	}

/**
This function is used to prepare result bundle.
@param None
@return CConnectionServParameterBundle*
*/
CConnectionServParameterBundle* CMBMSServiceRequest::PrepareMonitorResultBundleL() const
	{
	CConnectionServParameterBundle* returnBundle = CConnectionServParameterBundle::NewL();
	CleanupStack::PushL(returnBundle);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_PREPAREMONITORRESULTBUNDLEL_1, "CMBMSServiceRequest[%08x]::In PrepareMonitorResultBundleL function", (TUint)this);
	//read the MBMS Service list from Etel after receiving the MBMS Service Availability Notification.

	CPcktMbmsMonitoredServiceList* serviceList = iRetrievePcktMbms->RetrieveListL();
	TInt count = serviceList->Enumerate();
	CleanupStack::PushL(serviceList);

	//Read the monitor list from  Etel and send the list to client as parameter bundle.
    for(TUint i = 0; i < count; i++)
		{
		RPacketService::TMbmsServiceAvailabilityV1 serviceAvailability;
		CParameterSetContainer* returnPSC = CParameterSetContainer::NewL(*returnBundle);
		CleanupStack::PushL(returnPSC);
		serviceAvailability = serviceList->GetEntryL(i);

		TTmgi tmgi;
		tmgi.SetMCC(serviceAvailability.iTmgi.GetMCC());
		tmgi.SetMNC(serviceAvailability.iTmgi.GetMNC());
		tmgi.SetServiceId(serviceAvailability.iTmgi.GetServiceId());
												 			
		XMBMSServiceParameterSet* returnPS = XMBMSServiceParameterSet::NewL(*returnPSC);
		TMBMSChannelInfoV1* serviceInfo = returnPS->GetChannelInfo();

		serviceInfo->SetTmgi(tmgi);
		serviceInfo->SetScope(serviceAvailability.iMbmsAccessBearer);
		
	    returnPS->SetServiceMode(serviceAvailability.iMbmsServiceMode);
	    returnPS->SetMBMSServiceAvailability(serviceAvailability.iMbmsAvailabilityStatus);

	    CleanupStack::Pop(returnPSC);
	    }

	CleanupStack::PopAndDestroy(serviceList);
	CleanupStack::Pop(returnBundle);

	return returnBundle;
	}

/**
This function is used to prepare result bundle containing bearer availability.
@param None
@return CConnectionServParameterBundle*
*/
CConnectionServParameterBundle* CMBMSServiceRequest::PrepareBearerResultBundleL() const
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_PREPAREBEARERRESULTBUNDLEL_1, "CMBMSServiceRequest[%08x]::In PrepareBearerResultBundleL function", (TUint)this);

	CConnectionServParameterBundle* returnBundle = CConnectionServParameterBundle::NewL();
	CleanupStack::PushL(returnBundle);

	CParameterSetContainer* returnPSC = CParameterSetContainer::NewL(*returnBundle);
	CleanupStack::PushL(returnPSC);

	XMBMSServiceQuerySet* returnQuery = XMBMSServiceQuerySet::NewL(*returnPSC);
	returnQuery->SetMBMSBearerAvailability(iNetworkServiceStatus);

	CleanupStack::Pop(returnPSC);
	CleanupStack::Pop(returnBundle);

	return returnBundle;
	}

/**
This function is used to prepare result bundle containing current and max count of entries
in Monitor/Serice list table.
@param None
@return CConnectionServParameterBundle*
*/
CConnectionServParameterBundle* CMBMSServiceRequest::PrepareCountBundleL() const
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_PREPARECOUNTBUNDLEL_1, "CMBMSServiceRequest[%08x]::In PrepareCountBundleL function", (TUint)this);

	CConnectionServParameterBundle* returnBundle = CConnectionServParameterBundle::NewL();
	CleanupStack::PushL(returnBundle);

	CParameterSetContainer* returnPSC = CParameterSetContainer::NewL(*returnBundle);
	CleanupStack::PushL(returnPSC);

	XMBMSServiceQuerySet* returnQuery = XMBMSServiceQuerySet::NewL(*returnPSC);
	returnQuery->SetListCount(iCount);
	returnQuery->SetListMaxCount(iMaxCount);

	CleanupStack::Pop(returnPSC);
	CleanupStack::Pop(returnBundle);

	return returnBundle;
	}

/**
This function creates an empty bundle.
@param None
@return CConnectionServParameterBundle*
*/
CConnectionServParameterBundle* CMBMSServiceRequest::PrepareRemoveAllBundleL() const
	{
	//pass empty bundle
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSSERVICEREQUEST_PREPAREREMOVEALLBUNDLEL_1, "CMBMSServiceRequest[%08x]::In PrepareRemoveAllBundleL function", (TUint)this);

	CConnectionServParameterBundle* returnBundle = CConnectionServParameterBundle::NewL();
	return returnBundle;
	}
