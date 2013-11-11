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
*/

#include <comms-infras/mobilitymessages_internal.h>

#include <comms-infras/es_mobility_apiext.h>
#include <comms-infras/mobilitymessagesecom.h>
#include <comms-infras/mobilitymessages.h>



using namespace ESock;


/**
Default empty constructor, and is present only to support virtual
function table export.

@publishedPartner
@released
*/
EXPORT_C RCommsMobilityApiExt::RCommsMobilityApiExt()
	{
	}

/**
Closes the extension after it has been used. It is not possible to use 
an extension after it has been closed.

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::Close()
	{
	RCommsApiExtensionBase::Close();
	}

/**
This API is used to register (subscribe) for Mobility notifications from the Implementation. 

@param aResponseEventBuf	Will be filled with a corresponding response message before aStatus is completed.
@param aReqStatus 			Returns the result code after the asynchronous call completes.

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::RegisterForMobilityNotification(TDes8& aResponseEventBuf, TRequestStatus& aStatus)
	{
	TMobilitySubscriptionMsg msg;
	SendRequest(msg, aResponseEventBuf, aStatus);
	}

/**
Cancels previously requested subscription for Mobility notifications.

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::CancelMobilityNotification()
	{
	TMobilitySubscriptionCancelMsg msg;	
	SendMessage(msg);
	}

/**
Instructs the Implementation that it should migrate the Client to the newly 
available preferred carrier (or, in case of the current carrier becoming unavailable, 
to the less preferred but available carrier).

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::MigrateToPreferredCarrier()
	{
	TMigrateToPreferredCarrier msg;
	SendMessage(msg);
	}

/**
Instructs the Implementation that it should not migrate the Client to the newly 
available preferred carrier and that the Client wishes to continue using the current carrier.

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::IgnorePreferredCarrier()
	{
	TIgnorePreferredCarrier msg;
	SendMessage(msg);
	}

/**
Instructs the Implementation that the Client wishes to use the new carrier it has been migrated to.

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::NewCarrierAccepted()
	{
	TNewCarrierAccepted msg;
	SendMessage(msg);
	}

/**
Instructs the Implementation that the Client wishes to reject the new carrier it has been 
migrated to and be offered another preferred carrier unless nothing else is available. 
In case nothing else is available the request completes with an error.

@publishedPartner
@released
*/
EXPORT_C void RCommsMobilityApiExt::NewCarrierRejected()
	{
	TNewCarrierRejected msg;
	SendMessage(msg);
	}



void CActiveCommsMobilityApiExt::DoCancel()
    {
    iExtApi.CancelMobilityNotification();
    iProtocol.Error(KErrCancel);
    }

void CActiveCommsMobilityApiExt::RunL()
    {
    if(iStatus.Int() == KErrNone)
        {
        CCommsApiExtRespMsg* msg = CCommsApiExtRespMsg::NewL(iResponseBuf);

        // Re-register for the next message.
        iExtApi.RegisterForMobilityNotification(iResponseBuf,iStatus);
        SetActive();

        //check types
        STypeId preferredCarrierAvailableTid = STypeId::CreateSTypeId(KMobilityMessagesImplementationUid,EPreferredCarrierAvailable);
        STypeId newCarrierActiveTid = STypeId::CreateSTypeId(KMobilityMessagesImplementationUid,ENewCarrierActive);

        if (msg->IsTypeOf(preferredCarrierAvailableTid))
           {
           CPreferredCarrierAvailable& pca = reinterpret_cast<CPreferredCarrierAvailable&>(*msg);
           iProtocol.PreferredCarrierAvailable(pca.OldAPInfo(),pca.NewAPInfo(),pca.IsUpgrade(),pca.IsSeamless());
           }
        else if (msg->IsTypeOf(newCarrierActiveTid))
           {
           CNewCarrierActive& nca = reinterpret_cast<CNewCarrierActive&>(*msg);
           iProtocol.NewCarrierActive(nca.NewAPInfo(),nca.IsSeamless());
           }
        delete msg;
        }
	else
		{
        iProtocol.Error(iStatus.Int());
		}
    }

TInt CActiveCommsMobilityApiExt::RunError(TInt aError)
    {
    //The notification has not been sent to the client because of KErrNoMemory
    //We may need to provide an inplace construction for the messages to exclude this possibility.
    return aError;
    }

/**
This call is used to construct the Mobility extension, initialised with the MMobilityProtocolResp 
implementation provided as a parameter. MMobilityProtocolResp implementation is used when a Mobility 
notification arrives to the Client and is being dispatched on one of the call back APIs of MMobilityProtocolResp.

@param aExtensionProvider	The corresponding RCommsSubSession under which the extension is to be opened
							(in case of Mobility this is the RConnection or RCommsManager instance).
@param aProtocol			Handles the response communication between the implementation and the client.

@publishedPartner
@released
*/
EXPORT_C CActiveCommsMobilityApiExt* CActiveCommsMobilityApiExt::NewL(RCommsSubSession& aExtensionProvider,MMobilityProtocolResp& aProtocol)
    {
    CActiveCommsMobilityApiExt* self = new (ELeave) CActiveCommsMobilityApiExt(aProtocol);
    CleanupStack::PushL(self);
    self->ConstructL(aExtensionProvider);
    CleanupStack::Pop(self);
    return self;
    }

/**
Cancels the outstanding notification request and deletes the Mobility extension.

@publishedPartner
@released
*/
EXPORT_C CActiveCommsMobilityApiExt::~CActiveCommsMobilityApiExt()
	{
	Cancel();
	iExtApi.Close();
	iResponseBuf.Close();
	}

void CActiveCommsMobilityApiExt::ConstructL(RCommsSubSession& aExtensionProvider)
    {
    User::LeaveIfError(iExtApi.Open(aExtensionProvider));
    iResponseBuf.CreateL(KMaxExtApiIpcMsgLength);
    iExtApi.RegisterForMobilityNotification(iResponseBuf,iStatus);
    SetActive();
    }

CActiveCommsMobilityApiExt::CActiveCommsMobilityApiExt(MMobilityProtocolResp& aProtocol)
:   CActive(CActive::EPriorityUserInput),
    iProtocol(aProtocol)
	{
	CActiveScheduler::Add(this);
	}

/**
Instructs the Implementation that it should migrate the Client to the newly 
available preferred carrier (or, in case of the current carrier becoming 
unavailable, to the less preferred but available carrier).

@publishedPartner
@released
*/
EXPORT_C void CActiveCommsMobilityApiExt::MigrateToPreferredCarrier()
	{
	iExtApi.MigrateToPreferredCarrier();
	}

/**
Instructs the Implementation that it should not migrate the Client to the 
newly available preferred carrier and that the Client wishes to continue using 
the current carrier.

@publishedPartner
@released
*/
EXPORT_C void CActiveCommsMobilityApiExt::IgnorePreferredCarrier()
	{
    iExtApi.IgnorePreferredCarrier();
	}

/**
Instructs the Implementation that the Client wishes to use the new carrier it has been migrated to.

@publishedPartner
@released
*/
EXPORT_C void CActiveCommsMobilityApiExt::NewCarrierAccepted()
	{
    iExtApi.NewCarrierAccepted();
	}

/**
Instruct the Implementation that the Client wishes to reject the new carrier 
it has been migrated to be offered another preferred carrier unless nothing else is available. 
In case nothing else is available the request completes with an error.

@publishedPartner
@released
*/
EXPORT_C void CActiveCommsMobilityApiExt::NewCarrierRejected()
	{
    iExtApi.NewCarrierRejected();
	}

