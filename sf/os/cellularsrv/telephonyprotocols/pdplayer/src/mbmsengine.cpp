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

#include "mbmsengine.h"
#include "pdpservices.h"
#include <commsdattypesv1_1.h>

using namespace ESock;
using namespace ConnectionServ;
using namespace CommsDat;
using namespace Messages;

//This function retrieves the phone information.
void GetPhoneInfoL(RTelServer& aTelServer, const TDesC& aLoadedTsyName, RTelServer::TPhoneInfo& aInfo);

//Constructor for CMBMSEngine
CMBMSEngine::CMBMSEngine(const TNodeId& aMbmsTMCommsId, MPacketServiceNotifier& aPacketServiceNotifier)
	:CActive(EPriorityStandard),
	iMbmsTMCommsId(aMbmsTMCommsId),
	iPacketServiceNotifier(aPacketServiceNotifier)
	{
	CActiveScheduler::Add(this);
	}

/**
The NewL factory function for CMBMSEngine.
@param aMBMSTMCommsId comms Id.
@param aClientId Node Channel Id.
@param aRequestType Message Id
@param aRequestBundleOwner Parameter Bundle.
@return CMBMSEngine*
*/
CMBMSEngine* CMBMSEngine::NewL(const TNodeId& aMbmsTMCommsId, MPacketServiceNotifier& aPacketServiceNotifier)
	{
	CMBMSEngine* self = new(ELeave) CMBMSEngine(aMbmsTMCommsId, aPacketServiceNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor for CMBMSEngine.
*/
CMBMSEngine::~CMBMSEngine()
	{
	iPacketService.Close();
	iPhone.Close();
	iTelServer.Close();

	iMBMSRequestList.ResetAndDestroy();
	}
/**
ConstructL for CMBMSEngine.
*/
void CMBMSEngine::ConstructL()
	{
	InitialisePhoneL();
	iPendingRequests = EFalse;
	}
/**
This function is used to find the MBMS request from the list based on the client Id.
@param aClientId The reference to TCFNodeChannelId
@return TUint
*/
TUint CMBMSEngine::GetRequestElementL(const TRuntimeCtxId& aNodeCtxId)
	{
	TBool findMBMSRequest = EFalse;
	TInt index = 0;
	for (;index < iMBMSRequestList.Count(); index++)
		{
		if(aNodeCtxId == iMBMSRequestList[index]->GetClientId())
			{
			findMBMSRequest = ETrue;
			break;
		    }
		}
	if(!findMBMSRequest)
	   User::Leave(KErrNotFound);

	return index;
	}

/**
This function is used to remove requests from the list.
@param aClientId The  reference to TCFNodeChannelId
@return void
*/
void CMBMSEngine::RemoveFromRequestListL(const Messages::TRuntimeCtxId& aNodeCtxId)
	{
	TUint index = GetRequestElementL(aNodeCtxId);
	DeleteIndexElement(index);
	}
/**
This function is used to delete the request object fron the indexed list.
@param aIndex TUint
@return void
*/
void CMBMSEngine::DeleteIndexElement(TUint aIndex)
	{
	delete iMBMSRequestList[aIndex];
	iMBMSRequestList[aIndex] = NULL;
	iMBMSRequestList.Remove(aIndex);
    iMBMSRequestList.Compress();

    //check for any pending requests
	CheckPendingRequests();
	}

/**
This function is used to cancel any outstanding request and remove it from the list.
@param aClientId The  reference to TCFNodeChannelId
@return void
*/
void CMBMSEngine::CancelAndRemoveFromRequestListL(const Messages::TRuntimeCtxId& aNodeCtxId)
	{
	TUint index = GetRequestElementL(aNodeCtxId);
	iMBMSRequestList[index]->CancelMessage(KErrCancel);
	DeleteIndexElement(index);
	}

/**
This function is used to check whether any outstanding requests are there or not.
In case if any outstanding requests are there,active the requests at the head of the list.
@return void
*/
void CMBMSEngine::CheckPendingRequests()
	{
	if((iMBMSRequestList.Count() > 0) && (!iMBMSRequestList[0]->IsActive()))
	 	{
		//activate the requests at the head of the list.
		iMBMSRequestList[0]->StartRequest();
		}
	}

/**
This function is used to check whether the MBMS query is valid or not.If valid,
create an object of CMBMSServiceRequest and store in a list for later use.
@param aCFMessage The  reference to Messages::TSignatureBase
@return void
*/
void CMBMSEngine::AddToRequestListL(
		Messages::RNodeInterface* aNodeInterface,
		const Messages::TRuntimeCtxId& aNodeCtxId,
		const Messages::TNodeSignal::TMessageId& aRequestType,
		CRefCountOwnedParameterBundle* aRequestBundleOwner
			)
	{
	TUint mbmsQueryCount = 0;
	//the below enums are initialised to EBearerAvailability to avoid Armv5 warnings.
	XMBMSServiceQuerySet::TQueryType previousQuery = XMBMSServiceQuerySet::EBearerAvailability ;
	XMBMSServiceQuerySet::TQueryType currentQuery  = XMBMSServiceQuerySet::EBearerAvailability;

	const ConnectionServ::CConnectionServParameterBundle* mbmsParamsBundle1 = static_cast<const CConnectionServParameterBundle*>(aRequestBundleOwner->Ptr());
	if(!mbmsParamsBundle1)
	   User::Leave(KErrNotFound);

	ConnectionServ::CConnectionServParameterBundle* mbmsParamsBundle = const_cast<CConnectionServParameterBundle *>(mbmsParamsBundle1);

    //get the cout of Containers
	TUint containerCount = mbmsParamsBundle->CountParamSetContainers();

	//leave if Container is empty
	if (containerCount == 0)
		{
		User::Leave(KErrArgument);
		}

	TBool checkMBMSQuery = EFalse;
	for(TUint i = 0; i < containerCount; i++)
		{
		CParameterSetContainer* objectPSC = mbmsParamsBundle->GetParamSetContainer(i);
		XMBMSServiceQuerySet* queryMBMS = XMBMSServiceQuerySet::FindInParamSetContainer(*objectPSC);

		//pick only MBMS Query Types.
		if(!queryMBMS)
		   continue;

		if(queryMBMS)
		   checkMBMSQuery = ETrue;

		//leave if there are more than one type of MBMS query in the bundle.
		currentQuery = queryMBMS->GetQueryType();
		//leave if query is not set.
		if(currentQuery<0)
		  User::Leave(KErrArgument);

		//check whether the bundle has got only MBMS queries.
		if(mbmsQueryCount>0)
		  {
		  if(currentQuery != previousQuery)
		   	{
		   	User::Leave(KErrArgument);
		   	}
		  }
		previousQuery = currentQuery;
		mbmsQueryCount++;
		}
	//leave if no MBMS query found.
	if(!checkMBMSQuery)
	  User::Leave(KErrArgument);

	//store the CMBMSServiceRequest in a list.
	CMBMSServiceRequest* newServiceRequest = CMBMSServiceRequest::NewL(
			*this,
			iMbmsTMCommsId,
			aNodeInterface,
			aNodeCtxId,
			aRequestType,
			aRequestBundleOwner,
			currentQuery
			);
	CleanupStack::PushL(newServiceRequest);
	iMBMSRequestList.AppendL(newServiceRequest);
	CleanupStack::Pop(newServiceRequest);

	//start the requests if none of the requests are pending.
	CheckPendingRequests();
	}
/**
Function to initialize the phone settings
@return TBool
*/
TBool CMBMSEngine::InitialisePhoneL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* dbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* dbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
    CleanupStack::PushL(dbSession);

	CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
    TRAPD(err, globalSettingsRecord.LoadL(*dbSession));
    if(err != KErrNone)
      {
      User::Leave(KErrNotFound);
      }

    CCDModemBearerRecord *modemBearerRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
    CleanupStack::PushL(modemBearerRecord);

    modemBearerRecord->SetRecordId(((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iModemForPhoneServicesAndSMS);
    TRAPD(err1,modemBearerRecord->LoadL(*dbSession));

    if(err1 != KErrNone)
      {
      User::Leave(KErrNotFound);
      }

    TName tsyName;
    tsyName = modemBearerRecord->iTsyName;
    //leave if not able to read Tsy name.
    if(tsyName.Length() == 0)
	  {
      User::Leave(KErrNotFound);
	  }

    CleanupStack::PopAndDestroy(modemBearerRecord);
    CleanupStack::PopAndDestroy(dbSession);

    User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.LoadPhoneModule(tsyName));
	User::LeaveIfError(iTelServer.SetExtendedErrorGranularity(RTelServer::EErrorExtended));

	//Open telephony server
	GetPhoneInfoL(iTelServer,tsyName,iPhoneInfo);

	//Open phone
	User::LeaveIfError(iPhone.Open(iTelServer,iPhoneInfo.iName));
	//Get phone status
	User::LeaveIfError(iPhone.GetStatus(iPhoneStatus));

	SetActive();
	iPhoneState = EInitialising;

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);

	return ETrue;
	}

/**
This is CActive overloaded function.Cancel all outstanding requests.
@param None
@return Void
*/
void CMBMSEngine::DoCancel()
	{
    iPhone.InitialiseCancel();
	}

/**
This is CActive overloaded function.
@param None
@return TInt
*/
TInt CMBMSEngine::RunError(TInt /*aError*/)
	{
	Cancel();
	return KErrNone;
	}

/**
This is CActive overloaded function.
@param None
@return Void
*/
void CMBMSEngine::RunL()
	{
	TRequestStatus* status = &iStatus;
	if (iStatus == KErrNone)
		{
		switch(iPhoneState)
			{
			case EInitialising:
				if(iPhoneStatus.iMode==RPhone::EModeUnknown)
					{
					iPhone.Initialise(iStatus);
					}
				else
					{
					User::RequestComplete(status, KErrNone);
					}
				SetActive();
				iPhoneState = ESetAttachMode;
			break;
	       	case ESetAttachMode:
				//Open packet service
				User::LeaveIfError(iPacketService.Open(iPhone));

				//Put phone is attachwhenpossible mode.
				//In this mode, the phone will try to attach to the gprs network whenever it can.
				iPacketService.SetAttachMode(iStatus, RPacketService::EAttachWhenNeeded);
				iPhoneState = EAttachModeComplete;
				SetActive();
			break;
			case EAttachModeComplete:
			iPacketServiceNotifier.PacketServiceAttachedCallbackL();
			break;
			}
		}
	}

/**
Function to retrieve the RPacketService Instance
@return RPacketService&
*/
RPacketService& CMBMSEngine::GetRPacketService()
	{
	return iPacketService;
	}
